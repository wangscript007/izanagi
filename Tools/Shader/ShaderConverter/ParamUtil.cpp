#include "izToolKit.h"
#include "ParamUtil.h"
#include "ShaderConverterUtil.h"
#include "DataBuffer.h"
#include "ShaderConfig.h"

namespace {
#if 0
    void _SetString(IZ_PCSTR& target, CGannotation ann)
    {
        IZ_PCSTR str = cgGetStringAnnotationValue(ann);
        IZ_UINT pos = CStringChunk::GetInstance().Register(str);
        target = *(IZ_PCSTR*)(&pos);
    }
#else
    void _SetString(IZ_UINT16& target, CGannotation ann)
    {
        IZ_PCSTR str = cgGetStringAnnotationValue(ann);
        IZ_UINT pos = CStringChunk::GetInstance().Register(str);
        target = pos;
    }
#endif

    // UIName
    BOOL _SetValueUIName(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        _SetString(sAnn.UIName, ann);
        sAnn.hasUIName = TRUE;
        return TRUE;
    }

    // UIHelp
    BOOL _SetValueUIHelp(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        _SetString(sAnn.UIHelp, ann);
        sAnn.hasUIHelp = TRUE;
        return TRUE;
    }

    // UIWidget
    BOOL _SetValueUIWidget(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        _SetString(sAnn.UIWidget, ann);
        sAnn.hasUIWidget = TRUE;
        return TRUE;
    }

    // UIMax
    BOOL _SetValueUIMax(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        int num = 0;
        const float* p = cgGetFloatAnnotationValues(ann, &num);
        memcpy(sAnn.paramMax, p, sizeof(IZ_FLOAT) * num);

        sAnn.hasUIMax = TRUE;
        return TRUE;
    }

    // UIMin
    BOOL _SetValueUIMin(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        int num = 0;
        const float* p = cgGetFloatAnnotationValues(ann, &num);
        memcpy(sAnn.paramMin, p, sizeof(IZ_FLOAT) * num);

        sAnn.hasUIMin = TRUE;
        return TRUE;
    }

    // UIStep
    BOOL _SetValueUIStep(izanagi::shader::S_SHD_PARAM_ANN& sAnn, CGannotation ann)
    {
        int num = 0;
        const float* p = cgGetFloatAnnotationValues(ann, &num);
        memcpy(sAnn.paramStep, p, sizeof(IZ_FLOAT) * num);

        sAnn.hasUIStep = TRUE;
        return TRUE;
    }

    // 各種アノテーションをセット
    BOOL _SetAnnValue(
        izanagi::shader::S_SHD_PARAM_ANN& sAnn, 
        CGannotation ann)
    {
        static struct {
            const char* Name;
            BOOL (*Func)(izanagi::shader::S_SHD_PARAM_ANN&, CGannotation);
        } AnnTable[] = {
            {"UIName", _SetValueUIName},
            {"UIHelp", _SetValueUIHelp},
            {"UIWidget", _SetValueUIWidget},
            {"UIMax", _SetValueUIMax},
            {"UIMin", _SetValueUIMin},
            {"UIStep", _SetValueUIStep},
        };

        BOOL ret = TRUE;

        izanagi::tool::CString str(cgGetAnnotationName(ann));

        for (UINT i = 0; i < COUNTOF(AnnTable); i++) {
            if (str == AnnTable[i].Name) {
                ret = (*AnnTable[i].Func)(sAnn, ann);
                break;
            }
        }

        return ret;
    }
}   // namespace

BOOL CParamUtil::SetAnnValue(
    izanagi::shader::S_SHD_PARAM_ANN& sAnn,
    CGparameter param)
{
    CGannotation ann = ::cgGetFirstParameterAnnotation(param);

    while (ann != NULL) {
        VRETURN(_SetAnnValue(sAnn, ann));
        ann = ::cgGetNextAnnotation(ann);
    }

    return TRUE;
}

namespace {
    inline BOOL _GetBoolValue(
        CGparameter param,
        LPCSTR lpszName)
    {
        CGannotation ann = ::cgGetFirstParameterAnnotation(param);

        while (ann != NULL) {
            izanagi::tool::CString str(::cgGetAnnotationName(ann));

            if (str == "DoNotStrip") {
                int num = 0;
                const CGbool* p = ::cgGetBoolAnnotationValues(ann, &num);

                if (num > 0) {
                    return *p;
                }
            }

            ann = ::cgGetNextAnnotation(ann);
        }

        return FALSE;
    }

    inline BOOL _IsPublic(CGparameter param)
    {
        return _GetBoolValue(param, "Public");
    }

    inline BOOL _DoNotStrip(CGparameter param)
    {
        return _GetBoolValue(param, "DoNotStrip");
    }
}

BOOL CParamUtil::DoNotStrip(CGparameter param)
{
    return _DoNotStrip(param);
}

namespace {
    inline CGtype _GetCGType(CGparameter param)
    {
        CGtype ret = ::cgGetParameterType(param);

        if (ret == CG_ARRAY) {
            ret = ::cgGetArrayType(param);
        }

        return ret;
    }
}   // namespace

BOOL CParamUtil::SetDescValue(
    const SShaderConfig& config,
    izanagi::shader::S_SHD_PARAMETER& sDesc,
    CGparameter param)
{
    CGparameterclass classParam = ::cgGetParameterClass(param);
    CGtype typeParam = _GetCGType(param);

    sDesc.Class = CShaderConvUtil::CgParamClassToIzanagiShaderParamClass(classParam);
    sDesc.Type = CShaderConvUtil::CgParamTypeToIzanagiShaderParamType(typeParam);

    sDesc.originalType = sDesc.Type;

    sDesc.Rows = ::cgGetParameterRows(param);
    sDesc.Columns = ::cgGetParameterColumns(param);

    // 配列の要素数を取得
    // Dimensionは０固定
    sDesc.Elements = ::cgGetArraySize(param, 0);

#if 0
    // For debug...
    auto name = ::cgGetParameterName(param);
    printf("Parameter[%s]\n", name);

#define _SHD_PARAM_TYPE(type)   {izanagi::shader::E_SHADER_PARAMETER_TYPE_##type, #type}

    struct {
        izanagi::shader::E_SHADER_PARAMETER_TYPE type;
        const char* name;
    } types[] = {
        _SHD_PARAM_TYPE(FLOAT),
        _SHD_PARAM_TYPE(FLOAT1),
        _SHD_PARAM_TYPE(FLOAT4),
        _SHD_PARAM_TYPE(FLOAT4x4),
    };

    IZ_BOOL isMatch = IZ_FALSE;
    for (IZ_UINT i = 0; i < COUNTOF(types); i++) {
        if (types[i].type == sDesc.Type) {
            printf("    Type(%s)\n", types[i].name);
            isMatch = IZ_TRUE;
            break;
        }
    }
    if (isMatch) {
        printf("    Type(Unknown)\n");
    }

    printf("    Rows(%d) Columns(%d) Elements(%d)\n", sDesc.Rows, sDesc.Columns, sDesc.Elements);
#endif

    // TODO
    // 構造体は禁止なので計算しない
    sDesc.StructMembers = 0;

    UINT size = CShaderConvUtil::GetSizeFromCgType(typeParam);
    sDesc.Bytes = size * sDesc.Rows * sDesc.Columns;

    if (sDesc.Elements > 0) {
        sDesc.Bytes *= sDesc.Elements;
    }

    sDesc.hasAnn = HasAnn(param);
    sDesc.Public = _IsPublic(param);
    sDesc.DoNotStrip = _DoNotStrip(param);

    return TRUE;
}

BOOL CParamUtil::GetInitValue(
    izanagi::shader::S_SHD_PARAMETER& sDesc,
    CGparameter param)
{
    IZ_UINT nElements = sDesc.Rows * sDesc.Columns;
    nElements = (sDesc.Elements > 0 ? nElements * sDesc.Elements : nElements);

    IZ_UINT nParamSize = sDesc.Bytes;

    CGtype type = _GetCGType(param);

    int nValueNum = 0;
    std::vector<IZ_UINT8> tvBuf(nParamSize);

    if (CShaderConvUtil::IsIntType(type)) {
        // int
        nValueNum = cgGetParameterValueir(
                        param,
                        nElements,
                        (int*)&tvBuf[0]);
    }
    else if (CShaderConvUtil::IsFloatType(type)) {
        // float
        nValueNum = cgGetParameterValuefr(
                        param,
                        nElements,
                        (float*)&tvBuf[0]);

#if 0
        for (int i = 0; i < nValueNum; i++) {
            float xx = *(float*)&tvBuf[i * 4];
            printf("%f\n", xx);
        }
#endif
    }
    else {
        VRETURN(IZ_FALSE);
    }

    BOOL ret = (nValueNum > 0);
    VRETURN(ret);

    if (ret) {
        sDesc.hasDefaultValue = IZ_TRUE;
        sDesc.Pos = CDataBuffer::GetInstance().Register(
                        &tvBuf[0],
                        tvBuf.size());
    }

    return ret;
}
