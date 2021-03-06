#include "math/MathPlane.h"
#include "math/MathRay.h"
#include "math/MathCVector4.h"

namespace izanagi
{
namespace math
{
    // 平面上でレイと交差する点を取得
    IZ_BOOL CPlane::GetIntersectPoint(
        const CPlane& plane,
        const SRay& ray,
        SVector4& refPtr,
        IZ_FLOAT* retRayCoefficient/*= IZ_NULL*/)
    {
        return plane.GetIntersectPoint(ray, refPtr, retRayCoefficient);
    }

    CPlane::CPlane()
    {
        a = b = c = d = padding = 0.0f;
    }

    CPlane::CPlane(const SVector4& normal, const SVector4& ptr)
    {
        Set(normal, ptr);
    }

    CPlane::CPlane(const SPlane& rhs)
    {
        *this = rhs;
    }

    CPlane::CPlane(IZ_FLOAT _a, IZ_FLOAT _b, IZ_FLOAT _c, IZ_FLOAT _d)
    {
        a = _a;
        b = _b;
        c = _c;
        d = _d;
    }

    const CPlane& CPlane::operator=(const SPlane& rhs)
    {
        Set(rhs.nml, rhs.pt);
        return *this;
    }

    // 平面を設定.
    void CPlane::Set(const SVector4& normal, const SVector4& ptr)
    {
        SVector4::Copy(nml, normal);

        nml.w = 0.0f;
        SVector4::Normalize(nml, nml);

        d = -(a * ptr.x + b * ptr.y + c * ptr.z);

        pt.Set(ptr.x, ptr.y, ptr.z);
    }

    // 平面を設定.
    void CPlane::Set(const SVector4& normal, IZ_FLOAT _d)
    {
        SVector4::Copy(nml, normal);

        nml.w = 0.0f;
        SVector4::Normalize(nml, nml);

        d = _d;

        // 平面の基準位置を計算する
        // => 原点からの法線方向へのレイとの交点を計算する
        CRay ray(
            CVector4(0.0f, 0.0f, 0.0f),
            nml);
        GetIntersectPoint(ray, this->pt);
    }

    // 原点からの距離を取得.
    IZ_FLOAT CPlane::GetDistance() const
    {
        return ::fabs(d);
    }

    // 平面上に存在する点かどうか.
    IZ_BOOL CPlane::IsOnPlane(const SVector4& ptr)
    {
        IZ_FLOAT tmp = a * ptr.x + b * ptr.y + c * ptr.z;
        return CMath::IsNearyEqualZero(tmp + d);
    }

    // 4x4行列による変換.
    void CPlane::Transform(const SMatrix44& mtx)
    {
        // TODO
        // Matrixにスケールが含まれていた場合
        // 一度Normalizeする？

        SMatrix44::ApplyXYZ(nml, nml, mtx);
    }

    // 平面をあらわすベクトルを取得
    void CPlane::GetPlaneVector(SVector4& plane)
    {
        plane.Set(a, b, c, d);
    }

    // レイと交差する点を取得
    IZ_BOOL CPlane::GetIntersectPoint(
        const SRay& ray,
        SVector4& refPtr,
        IZ_FLOAT* retRayCoefficient/*= IZ_NULL*/) const
    {
        // NOTE
        // Plane : L = <N, D>
        // Ray   : P(t) = Q + Vt
        //
        // N・P(t) + D = 0
        // <=> N・Q + (N・V)t + D = 0
        // <=> t = -(N・Q + D) / N・V
        // <=> t = -L・Q / L・V
        //
        // Q = (Qx, Qy, Qz, 1) => 位置ベクトル
        // V = (Vx, Vy, Vz, 0) => 方向ベクトル
        // L = (Nx, Ny, Nz, D)
        // なので、
        // N・Q + D = Nx * Qx + Ny * Qy + Nz * Qz + D * 1 = L・Q
        // N・V + 0 = Nx * Vx + Ny * Vy + Nz * Vz + D * 0 = L・V

        CVector4 plane(a, b, c, d);

        // L・V
        IZ_FLOAT d = SVector4::Dot(plane, ray.v);
        if (CMath::IsNearyEqualZero(d))
        {
            return IZ_FALSE;
        }

        // L・Q
        IZ_FLOAT t = SVector4::Dot(plane, ray.p);

        // t = -L・Q / L・V 
        t = -t / d;

        if (retRayCoefficient != IZ_NULL) {
            *retRayCoefficient = t;
        }

        refPtr.x = ray.p.x + t * ray.v.x;
        refPtr.y = ray.p.y + t * ray.v.y;
        refPtr.z = ray.p.z + t * ray.v.z;
        refPtr.w = 1.0f;

        return IZ_TRUE;
    }

    // 線分と交差する点を取得.
    IZ_BOOL CPlane::GetIntersectPoint(
        const SVector4& from,
        const SVector4& to,
        SVector4& refPtr,
        IZ_FLOAT* retRayCoefficient/*= IZ_NULL*/) const
    {
        if ((IsPositive(from) && !IsPositive(to))
            || (!IsPositive(from) && IsPositive(to)))
        {
            // 二つの点は面の正負のそれぞれにないといけない
            CRay ray(
                from,
                CVector4(to, from, CVector4::INIT_SUB));

            IZ_BOOL ret = GetIntersectPoint(ray, refPtr, retRayCoefficient);
            return ret;
        }

        return IZ_FALSE;
    }

    // レイと交差するかどうか
    IZ_BOOL CPlane::IsIntersect(const SRay& ray) const
    {
        CVector4 plane(a, b, c, d);

        IZ_FLOAT d = SVector4::Dot(plane, ray.v);

        if (CMath::IsNearyEqualZero(d))
        {
            return IZ_FALSE;
        }

        return IZ_TRUE;
    }

    // 面の正側（法線の向き側）に点があるかどうか.
    IZ_BOOL CPlane::IsPositive(const SVector4& ptr) const
    {
#if 0
        // 原点からの距離
        IZ_FLOAT len = SVector4::Length(ptr);

        // 平面まで届いていない
        if (len <= ::fabs(d))
        {
            return IZ_FALSE;
        }

        // 原点からの方向ベクトル
        CVector4 dir(ptr.x, ptr.y, ptr.z, 0.0f);

        // NOTE
        //   | /       |
        //   |/        |
        //---+----  ---+---
        //            /
        //           /
        //
        // 面の法線となす角が90度より小さければ面の正の側
        // => 内積の値がプラスであれば法線の向き側に点がある

        IZ_FLOAT dot = SVector4::Dot(nml, dir);

        return (dot >= 0.0f);
#else
        CVector4 n(a, b, c, d);
        IZ_FLOAT dot = SVector4::Dot(n, ptr);
        dot = (CMath::IsNearyEqualZero(dot) ? 0.0f : dot);
        return (dot >= 0.0f);
#endif
    }
}   // namespace math
}   // namespace izanagi
