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
    float4 position     : TEXCOORD1;
};

#define SVSOutput        SPSInput

struct SPSOutput {
    float4 ambient  : COLOR0;
    float4 normal   : COLOR1;
    float4 depth    : COLOR2;
    float4 position : COLOR3;
};

/////////////////////////////////////////////////////////////

float4x4 g_mL2W;
float4x4 g_mW2V;
float4x4 g_mV2C;

// マテリアル
float4 g_vMtrlAmbient;

// ライト
float4 g_vLitAmbientColor;

/////////////////////////////////////////////////////////////

SVSOutput mainVS(SVSInput In)
{
    SVSOutput Out = (SVSOutput)0;
    
    Out.vPos = mul(In.vPos, g_mL2W);
    Out.vPos = mul(Out.vPos, g_mW2V);
    Out.vPos = mul(Out.vPos, g_mV2C);
    
    Out.vNormal = normalize(mul(In.vNormal, (float3x3)g_mL2W));

    // Ambient
    Out.ambient = g_vMtrlAmbient * g_vLitAmbientColor;

    Out.position = mul(In.vPos, g_mL2W);
    
    return Out;
}

SPSOutput mainPS(SPSInput In)
{
    SPSOutput vOut = (SPSOutput)0;

    vOut.ambient = In.ambient;
    vOut.ambient.a = 1.0f;
    
    vOut.normal.xyz = normalize(In.vNormal);
    vOut.normal.xyz = 0.5f * vOut.normal.xyz + 0.5f;
    vOut.normal.a = 1.0f;
    
    float4 view = mul(In.position, g_mW2V);
    float4 projected = mul(view, g_mV2C);
    float d = projected.z / projected.w;
    vOut.depth = float4(d, d, d, 1.0f);

    vOut.position = In.position;
    
    return vOut;
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

texture texAmbient;
texture texNormal;
texture texPosition;
texture texDepth;

sampler sTexAmbient = sampler_state
{
    Texture = texAmbient;
};

sampler sTexNormal = sampler_state
{
    Texture = texNormal;
};

sampler sTexDepth = sampler_state
{
    Texture = texDepth;
};

sampler sTexPosition = sampler_state
{
    Texture = texPosition;
};

#define SAMPLE_NUM  (32)
float4 samples[SAMPLE_NUM];

SPSInputSSAO mainVS_SSAO(SVSInputSSAO sIn)
{
    SPSInputSSAO sOut;

    // 頂点位置
    sOut.vPos = sIn.vPos;

    // [0.0f, 0.0f] - [1.0f, 1.0f] -> [-1.0f, -1.0f] - [1.0f, 1.0f]
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

    float4 ambient = tex2D(sTexAmbient, sIn.vUV);

    // 実際の位置での法線
    float3 normal = tex2D(sTexNormal, sIn.vUV).rgb;
    normal = normalize(2.0f * normal - 1.0f);

    // 実際の位置での深度
    float z = tex2D(sTexDepth, sIn.vUV).r;

    // 頂点位置
    float4 position = tex2D(sTexPosition, sIn.vUV);

    float bl = 0;

    for (int i = 0; i < SAMPLE_NUM; i++) {
        float4 ray = samples[i];

        // NOTE
        // 法線と逆方向を向いているときは反転することで法線方向の半円内におさまるようにする
        ray.xyz *= sign(dot(ray.xyz, normal));

        // サンプリング点の位置ずらした位置の計算
        float4 pos = mul(position + ray, g_mW2V);
        pos = mul(pos, g_mV2C);

        // サンプリング点の位置ずらした位置を2D上のUVに変換
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
        float depth = tex2D(sTexDepth, uv).r;

        // 正：遮蔽するピクセルが現在のピクセルより前 -> 現在のピクセルは遮蔽される
        // 負：遮蔽するピクセルが現在のピクセルより奥 -> 現在のピクセルは遮蔽されない
        float depthDiff = z - depth;

        // step(falloff, depthDiff) -> falloff より小さければ 0.0、大きければ 1.0
        //  -> ある程度以上深度が離れていない場合は遮蔽されていることとする
        bl += step(falloff, depthDiff) * nmlDiff;
    }

    // サンプリング点の合計なので平均を計算する
    bl /= (float)SAMPLE_NUM;

    // blが1.0に近いほど遮蔽されるので
    float a = 1.0f - bl;

    float4 Out = ambient * (float4)a;
    Out.a = 1.0f;

    return Out;
}

float4 mainPS_Ambient(SPSInputSSAO sIn) : COLOR0
{
    float4 ambient = tex2D(sTexAmbient, sIn.vUV);
    ambient.a = 1.0f;
    return ambient;
}

/////////////////////////////////////////////////////////////

technique RenderToMRT
{
    pass P0
    {
        AlphaBlendEnable = false;
        VertexShader = compile vs_3_0 mainVS();
        PixelShader = compile ps_3_0 mainPS();
    }
}

technique RenderSSAO
{
    pass P0
    {
        VertexShader = compile vs_3_0 mainVS_SSAO();
        PixelShader = compile ps_3_0 mainPS_SSAO();
    }
}

technique RenderNoSSAO
{
    pass P0
    {
        VertexShader = compile vs_3_0 mainVS_SSAO();
        PixelShader = compile ps_3_0 mainPS_Ambient();
    }
}
