// Basic Shader

struct SVSInput {
    float4 vPos        : POSITION;
    float2 vUV        : TEXCOORD0;
    float4 vColor    : COLOR;
};

struct SPSInput {
    float4 vPos        : POSITION;
    float2 vUV        : TEXCOORD2;
    float4 vColor    : COLOR;
};

#define SVSOutput        SPSInput

struct SVSInput_NoTex {
    float4 vPos        : POSITION;
    float4 vColor    : COLOR;
};

struct SPSInput_NoTex {
    float4 vPos        : POSITION;
    float4 vColor    : COLOR;
};

#define SVSOutput_NoTex    SPSInput_NoTex

/////////////////////////////////////////////////////////////

float4x4 g_mL2W;
float4x4 g_mW2C;

texture tex;

sampler sTex = sampler_state
{
    Texture = tex;
};

/////////////////////////////////////////////////////////////

SVSOutput mainVS(SVSInput In)
{
    SVSOutput Out = (SVSOutput)0;

    Out.vPos = mul(In.vPos, g_mL2W);
    Out.vPos = mul(Out.vPos, g_mW2C);

    Out.vUV = In.vUV;
    Out.vColor = In.vColor;
    
    return Out;
}

float4 mainPS(SPSInput In) : COLOR
{
    float4 vOut = tex2D(sTex, In.vUV);

    vOut.rgb *= In.vColor.rgb;
    vOut.rgb *= In.vColor.a;

    return vOut;
}

////////////////////////////////////////////////

SVSOutput_NoTex mainVS_NoTex(SVSInput_NoTex In)
{
    SVSOutput_NoTex Out = (SVSOutput_NoTex)0;

    Out.vPos = mul(In.vPos, g_mL2W);
    Out.vPos = mul(Out.vPos, g_mW2C);

    Out.vColor = In.vColor;

    return Out;
}

float4 mainPS_NoTex(SPSInput_NoTex In)    : COLOR
{
    float4 vOut = In.vColor;

    return vOut;
}

/////////////////////////////////////////////////////////////

technique BasicShader
{
    pass P0
    {
        VertexShader = compile vs_2_0 mainVS();
        PixelShader = compile ps_2_0 mainPS();
    }
}

technique BasicShader_NoTex
{
    pass P0
    {
        VertexShader = compile vs_2_0 mainVS_NoTex();
        PixelShader = compile ps_2_0 mainPS_NoTex();
    }
}
