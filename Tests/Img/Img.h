﻿// Basic.h : Basic アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"       // メイン シンボル


// CImgApp:
// このクラスの実装については、Basic.cpp を参照してください。
//

class CImgApp : public CWinApp
{
public:
	CImgApp();


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CImgApp theApp;
