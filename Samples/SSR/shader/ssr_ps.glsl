#version 450
precision highp float;
precision highp int;

// NOTE
// http://roar11.com/2015/07/screen-space-glossy-reflections/
// http://casual-effects.blogspot.jp/2014/08/screen-space-ray-tracing.html
// https://github.com/kode80/kode80SSR/blob/master/Assets/Resources/Shaders/SSR.shader
// http://tips.hecomi.com/entry/2016/04/04/022550

// NOTE
// Perspective Projection and Texture Mapping
// http://15462.courses.cs.cmu.edu/fall2016content/lectures/07_texture/07_texture_slides.pdf

in vec4 varColor;
in vec3 varNormal;
in float linearDepth;

layout(location = 0) out vec4 outColor;

uniform sampler2D s0;   // color
uniform sampler2D s1;   // linear depth.

uniform mat4 mtxW2V;	// World to View(Camera) space.
uniform mat4 mtxV2C;	// View(Camera) to Clip space.

uniform mat4 mtxC2V;	// Clip to View(Camera) space.
uniform mat4 mtxV2W;	// View(Camera) to World space.

uniform vec4 camPos;	// Camera position (World space).
uniform float nearPlaneZ;

uniform float maxDistance;
uniform float zThickness;
uniform int maxSteps;
uniform float stride;

float squaredLength(vec2 a, vec2 b)
{
	a -= b;
	return dot(a, a);
}

bool intersectsDepthBuffer(float z, float minZ, float maxZ)
{
	/*
	* Based on how far away from the camera the depth is,
	* adding a bit of extra thickness can help improve some
	* artifacts. Driving this value up too high can cause
	* artifacts of its own.
	*/

	// 指定範囲内（レイの始点と終点）に z があれば、それはレイにヒットしたとみなせる.
	z += zThickness;
	return (maxZ >= z) && (minZ - zThickness <= z);
}

bool traceScreenSpaceRay(
	vec3 csOrig,
	vec3 csDir,
	float jitter,
	out vec2 hitPixel,
	out vec4 hitPoint)
{
	// Clip to the near plane.
	float rayLength = (csOrig.z + csDir.z * maxDistance) < nearPlaneZ
		? (nearPlaneZ - csOrig.z) / csDir.z
		: maxDistance;

	vec3 csEndPoint = csOrig + csDir * rayLength;

	// Project into homogeneous clip space.
	vec4 H0 = mtxV2C * vec4(csOrig, 1);
	vec4 H1 = mtxV2C * vec4(csEndPoint, 1);

	float k0 = 1.0 / H0.w;
	float k1 = 1.0 / H1.w;

	// The interpolated homogeneous version of the camera-space points.
	vec3 Q0 = csOrig * k0;
	vec3 Q1 = csEndPoint * k1;

	// Screen space point.
	vec2 P0 = H0.xy * k0;
	vec2 P1 = H1.xy * k1;

	// [-1, 1] -> [0, 1]
	P0 = P0 * 0.5 + 0.5;
	P1 = P1 * 0.5 + 0.5;

	ivec2 texsize = textureSize(s0, 0);

	P0 *= vec2(texsize.xy);
	P1 *= vec2(texsize.xy);

	// If the line is degenerate, make it cover at least one pixel to avoid handling zero-pixel extent as a special case later.
	// 2点間の距離がある程度離れるようにする.
	P1 += squaredLength(P0, P1) < 0.0001
		? vec2(0.01, 0.01)
		: vec2(0.0);
	vec2 delta = P1 - P0;

	// Permute so that the primary iteration is in x to collapse all quadrant-specific DDA cases later.
	bool permute = false;
	if (abs(delta.x) < abs(delta.y))
	{
		permute = true;

		delta = delta.yx;
		P0 = P0.yx;
		P1 = P1.yx;
	}

	float stepDir = sign(delta.x);
	float invdx = stepDir / delta.x;

	// Track the derivatives of Q and k.
	vec3 dQ = (Q1 - Q0) * invdx;
	float dk = (k1 - k0) * invdx;

	// y is slope.
	// slope = (y1 - y0) / (x1 - x0)
	vec2 dP = vec2(stepDir, delta.y * invdx);	

	// Adjust end condition for iteration direction
	float end = P1.x * stepDir;

	int stepCount = 0;

	float prevZMaxEstimate = csOrig.z;
	
	float rayZMin = prevZMaxEstimate;
	float rayZMax = prevZMaxEstimate;
	
	float sceneZMax = rayZMax + 100.0f;

	dP *= stride;
	dQ *= stride;
	dk *= stride;

	P0 += dP * jitter;
	Q0 += dQ * jitter;
	k0 += dk * jitter;

	vec4 PQk = vec4(P0, Q0.z, k0);
	vec4 dPQk = vec4(dP, dQ.z, dk);
	vec3 Q = Q0;

	bool isCaster = true;

	for (;
		((PQk.x * stepDir) <= end)	// 終点に到達したか.
		&& (stepCount < maxSteps)	// 最大処理数に到達したか.
		//&& !intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax)	// レイが衝突したか.
		&& (sceneZMax != 0.0);	// 何もないところに到達してないか.
		++stepCount)
	{
		// 前回のZの最大値が次の最小値になる.
		rayZMin = prevZMaxEstimate;

		// 次のZの最大値を計算する.
		// ただし、1/2 pixel分 余裕を持たせる.
		// Qはw成分で除算されていて、そこに1/wで除算するので、元（View座標系）に戻ることになる.
		rayZMax = (PQk.z + dPQk.z * 0.5) / (PQk.w + dPQk.w * 0.5);

		// 次に向けて最大値を保持.
		prevZMaxEstimate = rayZMax;

		if (rayZMin > rayZMax) {
			// 念のため.
			float tmp = rayZMin;
			rayZMin = rayZMax;
			rayZMax = tmp;
		}

		hitPixel = permute ? PQk.yx : PQk.xy;

		// シーン内の現時点での深度値を取得.
		sceneZMax = texelFetch(s1, ivec2(hitPixel), 0).r;

		isCaster = (sceneZMax > 0);
		sceneZMax = abs(sceneZMax);

		if (intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax) && isCaster) {
			break;
		}

		PQk += dPQk;
	}

	// Advance Q based on the number of steps
	Q.xy += dQ.xy * stepCount;

	// Qはw成分で除算されていて、そこに1 / wで除算するので、元（View座標系）に戻ることになる.
	hitPoint.xyz = Q * (1.0f / PQk.w);

	if (sceneZMax <= 0) {
		return false;
	}
	return intersectsDepthBuffer(sceneZMax, rayZMin, rayZMax);
}

void main()
{
	// Normal is transformed in view space.
	vec3 normal = normalize(varNormal);

	ivec2 texsize = textureSize(s0, 0);

#if 1
	// Screen coordinate.
	vec4 pos = vec4(gl_FragCoord.xy / texsize, 0, 1);

	// [0, 1] -> [-1, 1]
	pos.xy = pos.xy * 2.0 - 1.0;

	// Screen-space -> Clip-space
	pos.xy *= linearDepth;

	// Clip-space -> View-space
	pos = mtxC2V * pos;
	pos.z = linearDepth;

	// Compute ray direction.
	// From ray origin(0,0,0) to view position.
	vec3 rayDir = normalize(pos.xyz);

	// Compute reflection vector.
	vec3 refDir = normalize(reflect(rayDir, normal));
	vec3 refOrg = pos.xyz;
#else
	// Ray origin is camera origin.
	vec3 rayOrg = camPos.xyz;

	// Screen coordinate.
	vec4 pos = vec4(gl_FragCoord.xy / texsize, 0, 1);

	// [0, 1] -> [-1, 1]
	pos.xy = pos.xy * 2.0 - 1.0;

	// Screen-space -> Clip-space
	pos.xy *= linearDepth;

	// Clip-space -> View-space
	pos = mtxC2V * pos;
	pos.z = linearDepth;

	// View-space -> World-space.
	vec3 worldPos = (mtxV2W * vec4(pos.xyz, 1)).xyz;

	// Compute ray direction.
	// From ray origin to world position.
	vec3 rayDir = normalize(worldPos - rayOrg);

	// Compute reflection vector.
	vec3 refDir = normalize(reflect(rayDir, normal));

	// Reflection vector origin is world position.
	vec3 refOrg = worldPos;

	// Transform to view coordinate.
	refOrg = (mtxW2V * vec4(refOrg, 1)).xyz;
	refDir = normalize((mtxW2V * vec4(refDir, 0)).xyz);
#endif

	vec2 hitPixel = vec2(0, 0);
	vec4 hitPoint = vec4(0, 0, 0, 0);

	float c = (gl_FragCoord.x + gl_FragCoord.y) * 0.25;
	float jitter = stride > 1.0 ? mod(c, 1.0) : 0.0;

	// Trace screen space ray.
	bool isIntersect = traceScreenSpaceRay(refOrg, refDir, jitter, hitPixel, hitPoint);

	vec2 uv = hitPixel / texsize.xy;

	if (uv.x > 1.0 || uv.x < 0.0f || uv.y > 1.0 || uv.y < 0.0) {
		isIntersect = false;
	}

	if (isIntersect) {
		outColor = varColor * texture(s0, uv);
	}
	else {
		outColor = varColor;
	}
}
