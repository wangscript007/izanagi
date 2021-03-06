#if !defined(__MODEL_LIB_MSH_EXPORTER_H__)
#define __MODEL_LIB_MDL_EXPORTER_H__

#include "izToolKit.h"
#include "Importer.h"

class CMshExporter {
    static CMshExporter s_cInstance;

public:
    static CMshExporter& GetInstance() { return s_cInstance; }

private:
    CMshExporter();
    ~CMshExporter();

    NO_COPIABLE(CMshExporter);

public:
    IZ_BOOL Export(
        IZ_UINT maxJointMtxNum,
        IZ_PCSTR lpszOutFile,
        IImporter* pImporter);

    /** トライアングルリストで出力するかどうかを設定.
     */
    void SetIsExportTriList(IZ_BOOL flag);

private:
    izanagi::CFileOutputStream m_Out;
};

#endif  // #if !defined(__MODEL_LIB_MDL_EXPORTER_H__)
