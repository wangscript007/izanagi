// Phong Shader

#define USE_REFLECTION

struct SVSInput {
    float4 vPos     : POSITION;
    float3 vNormal  : NORMAL;
    float4 vColor   : COLOR;
};

struct SPSInput {
    float4 vPos         : POSITION;
    float3 vNormal      : TEXCOORD0;    // 法線
    float4 ambient      : COLOR0;
    float4 viewSpace    : TEXCOORD1;
};

#define SVSOutput        SPSInput

struct SPSOutput {
    float4 albedo   : COLOR0;
    float4 normal   : COLOR1;
    float4 depth    : COLOR2;
};

/////////////////////////////////////////////////////////////

float4x4 g_mL2W;
float4x4 g_mW2V;
float4x4 g_mW2C;
float4x4 g_mV2C;

float g_farClip;

// マテリアル
float4 g_vMtrlAmbient;

// ライト
float4 g_vLitAmbientColor;

/////////////////////////////////////////////////////////////

SVSOutput mainVSGeometryPass(SVSInput In)
{
    SVSOutput Out = (SVSOutput)0;

    Out.vPos = mul(In.vPos, g_mL2W);

    Out.viewSpace = mul(Out.vPos, g_mW2V).z;
    Out.viewSpace /= g_farClip;

    Out.vPos = mul(Out.vPos, g_mW2C);

    Out.vNormal = mul(In.vNormal, (float3x3)g_mL2W);
    Out.vNormal = normalize(Out.vNormal);

    // Ambient
    Out.ambient = g_vMtrlAmbient * g_vLitAmbientColor;

    return Out;
}

SPSOutput mainPSGeometryPass(SPSInput In)
{
    SPSOutput Out = (SPSOutput)0;

    Out.normal.rgb = In.vNormal * 0.5f + 0.5f;  // normalize [-1, 1] ->[0, 1]
    Out.normal.a = 1.0f;

    Out.depth = In.viewSpace;
    Out.depth.a = 1.0f;

    Out.albedo = In.ambient;
    Out.albedo.a = 1.0f;

    return Out;
}

/////////////////////////////////////////////////////////////

struct SVSInputSSAO {
    float4 vPos     : POSITION;
    float2 vUV      : TEXCOORD0;
};

struct SPSInputSSAO {
    float4 vPos     : POSITION;
    float2 vUV      : TEXCOORD0;
};

#define SVSOutputSSAO   SPSInputSSAO

texture texNormal;
texture texDepth;

sampler sTexNormal = sampler_state
{
    Texture = texNormal;
};

sampler sTexDepth = sampler_state
{
    Texture = texDepth;
};

float4x4 g_mC2V;
float4x4 g_mV2W;

#define SAMPLE_NUM  (32)
float4 samples[SAMPLE_NUM];

SVSOutputSSAO mainVS_SSAO(SVSInputSSAO sIn)
{
    SVSOutputSSAO sOut;

    // 頂点位置
    sOut.vPos = sIn.vPos;

    // [0, 1] -> [-1, 1]
    sOut.vPos.xy = sOut.vPos.xy * 2.0f - 1.0f;

    // さらにY座標は反転させる
    sOut.vPos.y *= -1.0f;

    // UV座標
    sOut.vUV = sIn.vUV;

    return sOut;
}

float4 mainPS_SSAO(SPSInputSSAO sIn) : COLOR0
{
    float falloff = 0.000002;

    // 実際の位置での法線
    float3 normal = tex2D(sTexNormal, sIn.vUV).rgb;
    normal = normalize(2.0f * normal - 1.0f);

    // Depth format is R32F.
    float centerDepth = tex2D(sTexDepth, sIn.vUV).r;

    // Convert depth [0, 1] -> [0, far] in view-space.
    float depth = centerDepth * g_farClip;

    float4 position = float4(sIn.vUV.x, sIn.vUV.y, 0.0f, 1.0f);

    // Convert from texcoord to clip-space.
    position.y = 1.0f - position.y;
    position.xy = position.xy * 2.0f - 1.0f;  // [0, 1] -> [-1, 1]

    // NOTE
    // P = (X, Y, Z, 1)
    // mtxV2C = W 0 0 0
    //          0 H 0 0
    //          0 0 A 1
    //          0 0 B 0
    // Pview * mtxV2C = (Xclip, Yclip, Zclip, Wclip) = (Xclip, Yclip, Zclip, Zview)
    //  Wclip = Zview = depth
    // X' = Xclip / Wclip = Xclip / Zview = Xclip / depth
    // Y' = Yclip / Wclip = Yclip / Zview = Yclip / depth
    //
    // X' * depth = Xclip
    // Xview = Xclip * mtxC2V

    position.xy *= depth;
    position = mul(position, g_mC2V);

    position.xy /= position.z;
    position.z = depth;

    position = mul(position, g_mV2W);

    float bl = 0;

    for (int i = 0; i < SAMPLE_NUM; i++) {
        float4 ray = samples[i];

        // NOTE
        // 法線と逆方向を向いているときは反転することで法線方向の半円内におさまるようにする
        ray.xyz *= sign(dot(ray.xyz, normal));

        // サンプリング点の位置ずらした位置の計算
        float4 pos = mul(position + ray, g_mW2C);
        //pos = mul(pos, g_mV2C);

        // サンプリング点のずらした位置を2D上のUVに変換
        float2 uv = float2(
            0.5f * (pos.x / pos.w) + 0.5f,
            -0.5f * (pos.y / pos.w) + 0.5f);

        // 遮蔽するピクセルの法線
        float3 ocNml = tex2D(sTexNormal, uv).xyz;
        ocNml = normalize(ocNml * 2.0f - 1.0f);

        // 遮蔽するピクセルと現在のピクセルでの法線の角度の差を計算
        // 同方向なら1.0、反対なら0.0
        // 同方向を向いていたら遮蔽されていない、角度が大きいほど遮蔽されている、とみなす
        //  -> 同方向を向いている = 同じ面上にある -> 遮蔽していない
        //  -> 角度が大きい = 同じ面上にない -> 遮蔽している
        float nmlDiff = 1.0 - dot(ocNml, normal);

        // 遮蔽するピクセルの深度
        float sampleDepth = tex2D(sTexDepth, uv).r;

        // 正：遮蔽するピクセルが現在のピクセルより前 -> 現在のピクセルは遮蔽される
        // 負：遮蔽するピクセルが現在のピクセルより奥 -> 現在のピクセルは遮蔽されない
        float depthDiff = centerDepth - sampleDepth;

        // TODO
        // 0.75 -> radius.
        float rangeCheck = smoothstep(0.0, 1.0, (0.75 / g_farClip) / abs(centerDepth - sampleDepth));

        // step(falloff, depthDiff) -> falloff より小さければ 0.0、大きければ 1.0
        //  -> ある程度以上深度が離れていない場合は遮蔽されていることとする
        bl += step(falloff, depthDiff) * nmlDiff * rangeCheck;
    }

    // サンプリング点の合計なので平均を計算する
    bl /= (float)SAMPLE_NUM;

    // blが1.0に近いほど遮蔽されるので
    float a = 1.0f - bl;

    float4 Out = a;
    Out.a = 1.0f;

    if (centerDepth >= 1.0f) {
        Out.a = 0.0f;
    }

    return Out;
}

/////////////////////////////////////////////////////////////

struct SVSInputFinal {
    float4 vPos     : POSITION;
    float2 vUV      : TEXCOORD0;
};

struct SPSInputFinal {
    float4 vPos     : POSITION;
    float2 vUV      : TEXCOORD0;
};

#define SVSOutputFinal   SPSInputFinal

texture texAlbedo;
texture texSSAO;

sampler sTexAlbedo = sampler_state
{
    Texture = texAlbedo;
};

sampler sTexSSAO = sampler_state
{
    Texture = texSSAO;
};

SVSOutputFinal mainVSFinalPass(SVSInputFinal sIn)
{
    SVSOutputFinal sOut;

    // 頂点位置
    sOut.vPos = sIn.vPos;

    // [0, 1] -> [-1, 1]
    sOut.vPos.xy = sOut.vPos.xy * 2.0f - 1.0f;

    // さらにY座標は反転させる
    sOut.vPos.y *= -1.0f;

    // UV座標
    sOut.vUV = sIn.vUV;

    return sOut;
}

float4 mainPSFinalPass(SPSInputSSAO sIn) : COLOR0
{
    float4 albedo = tex2D(sTexAlbedo, sIn.vUV);
    float4 ssao = tex2D(sTexSSAO, sIn.vUV);
    albedo *= ssao;
    return albedo;
}

/////////////////////////////////////////////////////////////

technique SSAO
{
    pass geometryPass
    {
        AlphaBlendEnable = false;
        VertexShader = compile vs_3_0 mainVSGeometryPass();
        PixelShader = compile ps_3_0 mainPSGeometryPass();
    }

    pass ssaoPass
    {
        VertexShader = compile vs_3_0 mainVS_SSAO();
        PixelShader = compile ps_3_0 mainPS_SSAO();
    }

    pass finalPass
    {
        VertexShader = compile vs_3_0 mainVSFinalPass();
        PixelShader = compile ps_3_0 mainPSFinalPass();
    }
}
