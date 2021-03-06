#if !defined(__STATE_A_H__)
#define __STATE_A_H__

#include "StateBase.h"

class CStateA : public CStateBase {
public:
    CStateA() {}
    virtual ~CStateA() {}

public:
    // 更新.
    virtual IZ_BOOL Update(
        IZ_FLOAT time,
        izanagi::graph::CGraphicsDevice* device);

    virtual IZ_UINT GetIndex();

    // For Windows

    // キー押下
    virtual IZ_BOOL OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key);
};

#endif    // #if !defined(__STATE_A_H__)
