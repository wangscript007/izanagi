#include "collision/ColAABB.h"

namespace izanagi
{
namespace col
{
    AABB::AABB(
        const math::SVector4& minPtr,
        const math::SVector4& maxPtr)
    {
        initialize(minPtr, maxPtr);
    }

    AABB::AABB(
        const math::SVector4& center,
        IZ_FLOAT lengthX,
        IZ_FLOAT lengthY,
        IZ_FLOAT lengthZ)
    {
        initialize(center, lengthX, lengthY, lengthZ);
    }

    AABB::AABB(const AABB& rhs)
    {
        *this = rhs;
    }

    const AABB& AABB::operator = (const AABB& rhs)
    {
        m_min.Set(rhs.m_min);
        m_size.Set(rhs.m_size);
        return *this;
    }

    void AABB::initialize(
        const math::SVector4& minPtr,
        const math::SVector4& maxPtr)
    {
        m_min.Set(minPtr);

        m_size.x = maxPtr.x - minPtr.x;
        m_size.y = maxPtr.y - minPtr.y;
        m_size.z = maxPtr.z - minPtr.z;
    }

    void AABB::initialize(
        const math::SVector4& center,
        IZ_FLOAT lengthX,
        IZ_FLOAT lengthY,
        IZ_FLOAT lengthZ)
    {
        m_min.Set(center);

        m_min.x -= lengthX * 0.5f;
        m_min.y -= lengthY * 0.5f;
        m_min.z -= lengthZ * 0.5f;

        m_size.x = lengthX;
        m_size.y = lengthY;
        m_size.z = lengthZ;
    }

    void AABB::apply(const math::SMatrix44& mtx)
    {
        // AABB�Ȃ̂ŁA��]�͖���.
        const auto& translate = mtx.v[3];

        math::SVector4::AddXYZ(m_min, m_min, translate);

        // TODO
        // �X�P�[��...
    }

    IZ_FLOAT AABB::computeRadius(const math::SVector4& normal) const
    {
        // �{�b�N�X���\������3�����v�Z.
        math::CVector4 x;
        x.x = m_size.x;

        math::CVector4 y;
        y.y = m_size.y;

        math::CVector4 z;
        z.z = m_size.z;

        // �������a���v�Z.
        IZ_FLOAT radius = math::CMath::Absf(math::SVector4::Dot(x, normal));
        radius += math::CMath::Absf(math::SVector4::Dot(y, normal));
        radius += math::CMath::Absf(math::SVector4::Dot(z, normal));
        radius *= 0.5f;

        return radius;
    }

    IZ_FLOAT AABB::computeRadius(
        const math::SMatrix44& mtxW2V,
        const math::SVector4& normal) const
    {
        // �{�b�N�X���\������3�����v�Z.
        math::CVector4 x;
        x.x = m_size.x;

        math::CVector4 y;
        y.y = m_size.y;

        math::CVector4 z;
        z.z = m_size.z;

        // View���W�n�ɕϊ�.
        math::SMatrix44::ApplyXYZ(x, x, mtxW2V);
        math::SMatrix44::ApplyXYZ(y, y, mtxW2V);
        math::SMatrix44::ApplyXYZ(z, z, mtxW2V);

        // �������a���v�Z.
        IZ_FLOAT radius = math::CMath::Absf(math::SVector4::Dot(x, normal));
        radius += math::CMath::Absf(math::SVector4::Dot(y, normal));
        radius += math::CMath::Absf(math::SVector4::Dot(z, normal));
        radius *= 0.5f;

        return radius;
    }

    const math::SVector4 AABB::getCenter() const
    {
        math::SVector4 center;

        center.Set(m_min);

        center.x += m_size.x * 0.5f;
        center.y += m_size.y * 0.5f;
        center.z += m_size.z * 0.5f;

        return center;
    }

    // �ŏ����W���擾.
    const math::SVector4 AABB::getMin() const
    {
        return m_min;
    }

    // �ő���W���擾.
    const math::SVector4 AABB::getMax() const
    {
        math::SVector4 ret;

        ret.Set(m_min);

        ret.x += m_size.x;
        ret.y += m_size.y;
        ret.z += m_size.z;

        return ret;
    }

    void AABB::makeCubic()
    {
        // �ő�̃T�C�Y���擾.
        IZ_FLOAT maxSize = IZ_MAX(m_size.x, m_size.y);
        maxSize = IZ_MAX(maxSize, m_size.z);

        // �S�������T�C�Y�ɂ���.
        m_size.x = maxSize;
        m_size.y = maxSize;
        m_size.z = maxSize;
    }
}   // namespace math
}   // namespace izanagi