#if !defined(__IZANAGI_RESOURCE_ARC_FORMAT_H__)
#define __IZANAGI_RESOURCE_ARC_FORMAT_H__

#include "izDefs.h"

// ARC = ARChive

namespace izanagi
{
namespace resource
{
    // ARCファイルバージョン
    enum E_ARC_VERSION {
        E_ARC_VERSION_0010 = IZ_CC4('0', '0', '1', '0'),                // バージョン0010

        E_ARC_VERSION_CURRENT = E_ARC_VERSION_0010,     // 現在のバージョン
        E_ARC_VERSION_FORCE_INT32 = 0x7FFFFFFF,         // enum のサイズを IZ_INT と同じにするため.
    };

    enum {
        // "ARC "
        ARC_MAGIC_NUMBER = IZ_CC4('A', 'R', 'C', ' '),
    };

    enum {
        // ファイル名最大長
        ARC_FILE_NAME_MAX = 256,
    };

    // NOTE
    // フォーマット
    // +--------------------+
    // |   ファイルヘッダ   |
    // +--------------------+
    // |    ファイル情報    +-- numFiles 個
    // +--------------------+
    // |   文字列バッファ   |
    // +--------------------+
    // |   ファイルデータ   |
    // +--------------------+

    /**
    */
    struct S_ARC_HEADER {
        IZ_UINT magic;
        IZ_UINT version;

        IZ_UINT sizeFile;           // size of ARC
        IZ_UINT sizeHeader;         // size of header

        IZ_UINT numFiles;           // num of files in ARC

        IZ_UINT maxFileSize;

        IZ_UINT sizeName;           // size of name
        IZ_UINT sizeFileHeader;     // size of file list
        IZ_UINT sizeFileData;       // size of file data

        IZ_UINT posName;            // position of name
        IZ_UINT posFileHeader;      // position of file headers
        IZ_UINT posFileData;        // position of file data
    };

    /**
    */
    struct S_ARC_FILE_HEADER {
        IZ_UINT sizeName;           // size of name.
        IZ_UINT posName;            // position of name.

        IZ_UINT size;   ///< file size.

        IZ_UINT pos;    ///< position of file in ARC.

        IZ_UINT key;    ///< key of file in ARC.

        IZ_UINT sizeCompressed; ///< compressed file size. If sizeCompressed is 0, not compressed.
        IZ_UINT crc32;

        struct {
            IZ_UINT isCompressed    : 1;    // whether file is compressed.
            IZ_UINT isEncrypted     : 1;    // whether file is encrypted.
        };
    };
}   // namespace resource
}   // namespace izanagi

// ARCフォーマットかどうか
#define IS_ARC_FORMAT(f)            ((f) == izanagi::resource::ARC_MAGIC_NUMBER)

// 最新バージョンかどうか
#define IS_CURRENT_ARC_VERSION(v)   ((v) == izanagi::resource::E_ARC_VERSION_CURRENT)

#endif  // #if !defined(__IZANAGI_RESOURCE_ARC_FORMAT_H__)
