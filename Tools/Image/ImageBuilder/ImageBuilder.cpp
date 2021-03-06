// ImageBuilder.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#ifndef _WIN32_WINNT        // Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501 // これを Windows の他のバージョン向けに適切な値に変更してください。
#endif                      

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include "izToolKit.h"
#include "ImageBuilderImpl.h"
#include "AppError.h"
#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

namespace {
    inline void _DispUsage()
    {
        ::fprintf(
            stdout,
            "ImageBuilder Ver.0.01\n"
            "\n"
            "[USAGE]\n"
            "ImageBuilder.exe -i [xml] -o [out]\n"
            "\n"
            " -i [xml] : input xml file\n"
            " -o [out] : ouput file name\n"
            "            If name is not specified, this tool set name from input file name.\n"
            " -p [type] : specify graphics platform\n"
            "             dx9 : DirectX9\n"
            "             gles2 : OpenGLES2\n"
        );
    }

    void _PrintString(IZ_PCSTR pszStr)
    {
        ::fprintf(stderr, "%s\n", pszStr);
    }
}   // namespace

static IZ_CHAR BUF[MAX_PATH];

#ifdef VGOTO
#undef VGOTO
#endif

#define VGOTO(b)    if (!(b)) { IZ_ASSERT(FALSE); nRetCode = 1; goto __EXIT__; }

int main(int argc, char* argv[])
{
    int nRetCode = 0;

    if (argc == 1) {
        _DispUsage();
        return 1;
    }

    // エラーストリング表示用関数セット
    izanagi::tool::CException::SetPrintLogFunc(_PrintString);

    COption cOption;

    // ウインドウハンドル取得
    HWND hWnd = ::GetConsoleWindow();
    if (hWnd == NULL)
    {
        hWnd = ::FindWindow(0, 0);
    }
    VGOTO(hWnd != NULL);

    // グラフィックスデバイス作成
    izanagi::tool::CGraphicsDeviceLite* pDevice = IZ_NULL;
    pDevice = izanagi::tool::CGraphicsDeviceLite::CreateGraphicsDeviceLight(hWnd);
    VGOTO(pDevice != NULL);

    // オプション解析
    BOOL result = cOption.Analysis(argc, argv);
    VGOTO(result);
    VGOTO(cOption.IsValid());

    try {
        // 入力XMLのパス部分のみを取得
        izanagi::tool::CFileUtility::GetPathWithoutFileName(
            BUF,
            COUNTOF(BUF),
            cOption.in);

#if 0
        // 絶対パスに変更
        {
            izanagi::tool::CString tmp;
            tmp.Format("%s", BUF);
            BOOL result = (_fullpath(BUF, tmp, COUNTOF(BUF)) != NULL);
            VGOTO(result);
        }
#endif

        // XMLに記述されているイメージデータがXMLからの相対パスになるようにする
        CImageBuilder::GetInstance().SetBasePath(BUF);

        xercesc::XMLPlatformUtils::Initialize();

        xercesc::SAX2XMLReader* parser = xercesc::XMLReaderFactory::createXMLReader();
        
        parser->setContentHandler(&CImageBuilder::GetInstance());

        parser->parse(cOption.in);

        result = CImageBuilder::GetInstance().BuildIMG(
            cOption.type,
            cOption.out.c_str());
        VGOTO(result);
    }
    catch (izanagi::tool::CException* e) {
        e->PrintLog();
        VGOTO(FALSE);
    }
    catch (...) {
        ::fprintf(stderr, "Failed\n");
        VGOTO(FALSE);
    }

__EXIT__:
    SAFE_RELEASE(pDevice);

    return nRetCode;
}
