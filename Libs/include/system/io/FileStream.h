#if !defined(__IZANAGI_IO_FILE_STREAM_H__)
#define __IZANAGI_IO_FILE_STREAM_H__

#include "izDefs.h"

namespace izanagi
{
    /**
    * 超簡易ファイル入力
    */
    class CFileStream
    {
    protected:
        CFileStream() {}
        ~CFileStream() {}

        NO_COPIABLE(CFileStream);

    protected:
        void OnOpen(FILE_HANDLE* file, const char* path, const char* mode);
        void OnClose(FILE_HANDLE file);
        IZ_INT OnSeek(FILE_HANDLE file, IZ_INT offset, IZ_INT origin);
        IZ_LONG GetLength(FILE_HANDLE file);
        IZ_INT OnRead(void* buf, IZ_UINT size, IZ_UINT count, FILE_HANDLE file);
        
        IZ_UINT Tell(FILE_HANDLE file);
        IZ_UINT OnWrite(const void* src, size_t size, size_t count, FILE_HANDLE file);
        IZ_INT Print(FILE_HANDLE file, const char* str);
    };
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_IO_FILE_STREAM_H__)
