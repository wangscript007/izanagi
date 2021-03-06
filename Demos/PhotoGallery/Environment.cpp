#include "Environment.h"
#include "Utility.h"
#include "Configure.h"

Environment Environment::s_Instance;

Environment& Environment::Instance()
{
    return s_Instance;
}

Environment::Environment()
{
    m_PointLight.vPos.Set(0.0f, 40.0f, -Configure::InnerRadius * 0.5f);
    m_PointLight.color.Set(1.0f, 1.0f, 1.0f, 1.0f);
    m_PointLight.attn.Set(0.0f, 0.0f, 0.0005f, 0.0f);

    // Ambient Light Color
    m_Ambient.color.Set(0.75f, 0.75f, 0.75f);

    // Parallel Light Color
    m_ParallelLight.color.Set(0.8f, 0.8f, 0.8f);

    // Parallel Light Direction
    m_ParallelLight.vDir.Set(
#if 0
        -0.679090f,
        -0.701356f,
        -0.216650f);
    izanagi::math::SVector4::Normalize(m_ParallelLight.vDir, m_ParallelLight.vDir);
#else
         0.370910f,
        -0.151356f,
         0.303350f);
#endif
}

Environment::~Environment()
{
}

void Environment::SetPointLightParam(izanagi::shader::CShaderBasic* shader)
{
    Utility::SetShaderParam(
        shader,
        "g_PointLightPos",
        (void*)&m_PointLight.vPos,
        sizeof(m_PointLight.vPos));
    Utility::SetShaderParam(
        shader,
        "g_PointLight",
        (void*)&m_PointLight.attn,
        sizeof(m_PointLight.attn));
    Utility::SetShaderParam(
        shader,
        "g_PointLightColor",
        (void*)&m_PointLight.color,
        sizeof(m_PointLight.color));
}

void Environment::SetParallelLightParam(
    const izanagi::CCamera& camera,
    const izanagi::math::SMatrix44& mtxL2W,
    izanagi::shader::CShaderBasic* shader)
{
    // ライトの方向をローカル座標に変換する

    // ライトの方向はワールド座標なので World -> Localマトリクスを計算する
    izanagi::math::SMatrix44 mtxW2L;
    izanagi::math::SMatrix44::Inverse(mtxW2L, mtxL2W);

    // World -> Local
    izanagi::math::SVector4 parallelLightLocalDir;
    izanagi::math::SMatrix44::ApplyXYZ(
        parallelLightLocalDir,
        m_ParallelLight.vDir,
        mtxW2L);

    Utility::SetShaderParam(
        shader,
        "g_vLitParallelDir",
        (void*)&parallelLightLocalDir,
        sizeof(parallelLightLocalDir));

    // L2V = L2W * W2V の逆行列を計算する
    izanagi::math::SMatrix44 mtxV2L;
    izanagi::math::SMatrix44::Mul(mtxV2L, mtxL2W, camera.GetParam().mtxW2V);
    izanagi::math::SMatrix44::Inverse(mtxV2L, mtxV2L);

    // ビュー座標系における視点は常に原点
    izanagi::math::CVector4 eyePos(0.0f, 0.0f, 0.0f, 1.0f);

    // 視点のローカル座標を計算する
    izanagi::math::SMatrix44::Apply(eyePos, eyePos, mtxV2L);

    Utility::SetShaderParam(
        shader,
        "g_vEye",
        (void*)&eyePos,
        sizeof(eyePos));

    Utility::SetShaderParam(shader, "g_vLitParallelColor", &m_ParallelLight.color, sizeof(m_ParallelLight.color));
}

void Environment::SetAmbientLightParam(izanagi::shader::CShaderBasic* shader)
{
    Utility::SetShaderParam(shader, "g_vLitAmbientColor", &m_Ambient.color, sizeof(m_Ambient.color));
}
