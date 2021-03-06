#if !defined(__IZANAGI_ANM_TWEENER_UTILITY_H__)
#define __IZANAGI_ANM_TWEENER_UTILITY_H__

#include "izDefs.h"
#include "izStd.h"

namespace izanagi {
namespace animation {

    enum E_ANM_TWEENER_TRANSITION
    {
        E_ANM_TWEENER_TRANSITION_SHIFT = 0,
        E_ANM_TWEENER_TRANSITION_MASK  = 0x0000ffff,

        E_ANM_TWEENER_TRANSITION_BEGIN = 0,

        E_ANM_TWEENER_TRANSITION_BACK = E_ANM_TWEENER_TRANSITION_BEGIN,
        E_ANM_TWEENER_TRANSITION_BOUNCE,
        E_ANM_TWEENER_TRANSITION_CIRC,
        E_ANM_TWEENER_TRANSITION_CUBIC,
        E_ANM_TWEENER_TRANSITION_ELASTIC,
        E_ANM_TWEENER_TRANSITION_EXPO,
        E_ANM_TWEENER_TRANSITION_QUAD,
        E_ANM_TWEENER_TRANSITION_QUART,
        E_ANM_TWEENER_TRANSITION_QUINT,
        E_ANM_TWEENER_TRANSITION_SINE,
        E_ANM_TWEENER_TRANSITION_LINEAR,

        E_ANM_TWEENER_TRANSITION_END = E_ANM_TWEENER_TRANSITION_LINEAR,

        E_ANM_TWEENER_TRANSITION_NUM,
        E_ANM_TWEENER_TRANSITION_INT32 = 0x7fffffff,
    };

    enum E_ANM_TWEENER_EQUATION
    {
        E_ANM_TWEENER_EQUATION_SHIFT  = 16,
        E_ANM_TWEENER_EQUATION_MASK   = 0x7fff0000,

        E_ANM_TWEENER_EQUATION_EASE_IN     = 1 << E_ANM_TWEENER_EQUATION_SHIFT,
        E_ANM_TWEENER_EQUATION_EASE_OUT    = 2 << E_ANM_TWEENER_EQUATION_SHIFT,
        E_ANM_TWEENER_EQUATION_EASE_IN_OUT = 3 << E_ANM_TWEENER_EQUATION_SHIFT,
    };

    /**
    */
    enum E_ANM_TWEENER_MODE
    {
        E_ANM_TWEENER_MODE_BACK_EASE_IN     = E_ANM_TWEENER_TRANSITION_BACK | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_BACK_EASE_OUT    = E_ANM_TWEENER_TRANSITION_BACK | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_BACK_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_BACK | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_BOUNCE_EASE_IN     = E_ANM_TWEENER_TRANSITION_BOUNCE | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_BOUNCE_EASE_OUT    = E_ANM_TWEENER_TRANSITION_BOUNCE | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_BOUNCE_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_BOUNCE | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_CIRC_EASE_IN     = E_ANM_TWEENER_TRANSITION_CIRC | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_CIRC_EASE_OUT    = E_ANM_TWEENER_TRANSITION_CIRC | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_CIRC_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_CIRC | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_CUBIC_EASE_IN     = E_ANM_TWEENER_TRANSITION_CUBIC | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_CUBIC_EASE_OUT    = E_ANM_TWEENER_TRANSITION_CUBIC | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_CUBIC_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_CUBIC | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_ELASTIC_EASE_IN     = E_ANM_TWEENER_TRANSITION_ELASTIC | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_ELASTIC_EASE_OUT    = E_ANM_TWEENER_TRANSITION_ELASTIC | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_ELASTIC_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_ELASTIC | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_EXPO_EASE_IN     = E_ANM_TWEENER_TRANSITION_EXPO | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_EXPO_EASE_OUT    = E_ANM_TWEENER_TRANSITION_EXPO | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_EXPO_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_EXPO | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_QUAD_EASE_IN     = E_ANM_TWEENER_TRANSITION_QUAD | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_QUAD_EASE_OUT    = E_ANM_TWEENER_TRANSITION_QUAD | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_QUAD_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_QUAD | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_QUART_EASE_IN     = E_ANM_TWEENER_TRANSITION_QUART | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_QUART_EASE_OUT    = E_ANM_TWEENER_TRANSITION_QUART | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_QUART_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_QUART | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_QUINT_EASE_IN     = E_ANM_TWEENER_TRANSITION_QUINT | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_QUINT_EASE_OUT    = E_ANM_TWEENER_TRANSITION_QUINT | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_QUINT_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_QUINT | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_SINE_EASE_IN     = E_ANM_TWEENER_TRANSITION_SINE | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_SINE_EASE_OUT    = E_ANM_TWEENER_TRANSITION_SINE | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_SINE_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_SINE | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,

        E_ANM_TWEENER_MODE_LINEAR_EASE_IN     = E_ANM_TWEENER_TRANSITION_LINEAR | E_ANM_TWEENER_EQUATION_EASE_IN,
        E_ANM_TWEENER_MODE_LINEAR_EASE_OUT    = E_ANM_TWEENER_TRANSITION_LINEAR | E_ANM_TWEENER_EQUATION_EASE_OUT,
        E_ANM_TWEENER_MODE_LINEAR_EASE_IN_OUT = E_ANM_TWEENER_TRANSITION_LINEAR | E_ANM_TWEENER_EQUATION_EASE_IN_OUT,
    };

    class CTimeline;

    /**
    */
    class CTweener {
    public:
        CTweener()
        {
            Init(E_ANM_TWEENER_MODE_LINEAR_EASE_IN, 0.0f, 0.0f);
        }

        CTweener(
            E_ANM_TWEENER_MODE mode,
            IZ_FLOAT start,
            IZ_FLOAT end)
        {
            Init(mode, start, end);
        }

        ~CTweener() {}

        NO_COPIABLE(CTweener);

    public:
        IZ_FLOAT GetValue(const CTimeline& timeline);

        IZ_FLOAT GetValue(IZ_FLOAT time, IZ_FLOAT duration);

        void Init(
            E_ANM_TWEENER_MODE mode,
            IZ_FLOAT start,
            IZ_FLOAT end)
        {
            SetRange(start, end);
            SetMode(mode);
        }

        void SetRange(
            IZ_FLOAT start,
            IZ_FLOAT end)
        {
            m_Start = start;
            m_End = end;
        }

        IZ_FLOAT GetStart() const { return m_Start; }
        IZ_FLOAT GetEnd() const { return m_End; }

        void SetMode(E_ANM_TWEENER_MODE mode);
        E_ANM_TWEENER_MODE GetMode() const;

    protected:
        E_ANM_TWEENER_TRANSITION m_Transition;
        E_ANM_TWEENER_EQUATION m_Equation;

        IZ_FLOAT m_Start;
        IZ_FLOAT m_End;
    };

}   // namespace animation
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_ANM_TWEENER_UTILITY_H__)
