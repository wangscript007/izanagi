#include "Option.h"
#include "izToolKit.h"

COption::COption()
{
}

COption::~COption()
{
}

// 解析
IZ_BOOL COption::Analysis(int argc, char* argv[])
{
	IZ_BOOL result = IZ_TRUE;

	for (IZ_INT i = 1; i < argc; i++) {
		std::string opt(argv[i]);

		if (i < argc - 1) {
			if (result = (opt == "-i")) {
				in = argv[++i];
			}
			else if (result = (opt == "-o")) {
				out = argv[++i];
			}
			else if (result = (opt == "-f")) {
				std::string subOpt(argv[++i]);

				result = AnalysisModelType(subOpt);
			}
		}

		if (!result) {
			// TODO
			printf("無効なオプションです[%s]\n\n", opt.c_str());
			//ASSERT(FALSE);
			return IZ_FALSE;
		}
	}

	IZ_BOOL ret = AnalysisInternal();

	return ret;
}

static char BUF[MAX_PATH];

IZ_BOOL COption::AnalysisInternal()
{
	if (out.empty()) {
		// 出力ファイルが空なので、入力ファイルから作成する

		// 拡張子を除いた入力ファイル名を取得
		IZ_BOOL result = izanagi::tool::CFileUtility::RemoveExtension(
							BUF,
							sizeof(BUF),
							in.c_str());
		VRETURN(result);

		out.format("%s.mtrl", BUF);
	}

	if (!IsValidModelType()) {
		// 入力ファイルの拡張子から判定する

		// 拡張子を取得
		IZ_PCSTR ext = izanagi::tool::CFileUtility::GetExtension(
						BUF,
						sizeof(BUF),
						in.c_str());
		VRETURN(ext != IZ_NULL);

		std::string strExt(ext);

		AnalysisModelType(strExt);
	}

	return IZ_TRUE;
}

// 出力ファイル名から拡張子を取り除く
IZ_BOOL COption::RemoveExtFromExportFileName()
{
	IZ_ASSERT(!out.empty());


	// 拡張子を除いた入力ファイル名を取得
	IZ_BOOL result = izanagi::tool::CFileUtility::RemoveExtension(
						BUF,
						sizeof(BUF),
						out.c_str());
	VRETURN(result);

	out.format("%s", BUF);

	return IZ_TRUE;
}

IZ_BOOL COption::IsValid()
{
	VRETURN(!in.empty());
	VRETURN(!out.empty());
	VRETURN(IsValidModelType());

	return IZ_TRUE;
}
