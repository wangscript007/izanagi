#ifndef __RENDER2D_H__
#define __RENDER2D_H__

extern "C" {
    int Hoge();

    int Loop();

    void SetRootPath(const char* path);

    typedef void (*Callback)();
    void SetCallback(Callback func);
}

#endif