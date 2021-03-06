#if !defined(__IZANAGI_MATH_INTERPOLATER_H__)
#define __IZANAGI_MATH_INTERPOLATER_H__

#include "izDefs.h"
#include "MathCVector4.h"

namespace izanagi {
namespace math {
    /**
     */
    class CMathCurve
    {
    private:
        CMathCurve();
        ~CMathCurve();

    public:
        /** ベジェ曲線.
         */
        static IZ_FLOAT ComputeBezier(
            IZ_FLOAT t,
            IZ_FLOAT x0,
            IZ_FLOAT x1,
            IZ_FLOAT x2,
            IZ_FLOAT x3);

        /** ベジェ曲線.
         */
        static IZ_FLOAT ComputeBezier(
            IZ_FLOAT t,
            const SVector4& p);

        /** Hermite曲線.
         */
        static IZ_FLOAT ComputeHermite(
            IZ_FLOAT t,
            IZ_FLOAT x0,
            IZ_FLOAT x1,
            IZ_FLOAT v0,
            IZ_FLOAT v1);

        /** Hermite曲線.
         */
        static IZ_FLOAT ComputeHermite(
            IZ_FLOAT t,
            const SVector4& p);

        /** Catmull-Rom曲線.
         */
        static IZ_FLOAT ComputeCatmullRom(
            IZ_FLOAT t,
            IZ_FLOAT x0,
            IZ_FLOAT x1,
            IZ_FLOAT x2,
            IZ_FLOAT x3);

        /** Catmull-Rom曲線.
         */
        static IZ_FLOAT ComputeCatmullRom(
            IZ_FLOAT t,
            const SVector4& p);
    };
}   // namespace math {
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_MATH_INTERPOLATER_H__)
