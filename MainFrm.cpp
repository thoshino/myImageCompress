// この MFC サンプル ソース コードでは、MFC Microsoft Office Fluent ユーザー インターフェイス 
// ("Fluent UI") の使用方法を示します。このコードは、MFC C++ ライブラリ ソフトウェアに 
// 同梱されている Microsoft Foundation Class リファレンスおよび関連電子ドキュメントを
// 補完するための参考資料として提供されます。
// Fluent UI を複製、使用、または配布するためのライセンス条項は個別に用意されています。
// Fluent UI ライセンス プログラムの詳細については、Web サイト
// http://go.microsoft.com/fwlink/?LinkId=238214 を参照してください。
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "myImageCompress.h"

#include "MainFrm.h"
#include "MyImageTransformer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

#define	WM_UPDATE_PROGRESS	(WM_USER+0x01)

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_UPDATE_PROGRESS,&CMainFrame::OnChangeState)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewProperty)
	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewProperty)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CANCEL, &CMainFrame::OnUpdateCaptionBarCancel)
	ON_COMMAND(ID_TOOLS_CANCEL, &CMainFrame::OnCaptionBarCancel)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame()
{
	// TODO: メンバー初期化コードをここに追加してください。
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);
	m_wndProgress = NULL;
	m_inProgress = false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// フレームのクライアント領域全体を占めるビューを作成します。
	if (!m_wndView.Create(AFX_WS_DEFAULT_VIEW | LVS_REPORT | LVS_SHOWSELALWAYS, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Error \n");
		return -1;
	}
	m_wndView.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_wndView.InsertColumn(0, TEXT("log"), LVCFMT_LEFT,500);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Error \n");
		return -1;      // 作成できない場合
	}

	CRect	rect;
	GetClientRect(&rect);

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndProgress = new CMyMFCRibbonProgressBar();
	m_wndStatusBar.AddElement(m_wndProgress, TEXT(""));
	//プログレスの範囲を設定します。
	m_wndProgress->SetRange(0, 10000);
	// Visual Studio 2005 スタイルのドッキング ウィンドウ動作を有効にします
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 スタイルのドッキング ウィンドウの自動非表示動作を有効にします
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// キャプション バーを作成します:
	if (!CreateCaptionBar())
	{
		return -1;      //作成できない場合
	}

	// ドッキング ウィンドウを作成します
	if (!CreateDockingWindows())
	{
		return -1;
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 固定値に基づいてビジュアル マネージャーと visual スタイルを設定します
	OnApplicationLook(theApp.m_nAppLook);

	//レジストリから情報を取得します。
	//プロパティ情報をレジストリに保存します。
	m_strImageType=theApp.GetProfileString(OLESTR("converted setting"), OLESTR("imageType"), OLESTR("jpg"));
	m_nImageQuality=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageQuality"), 60);
	m_bImageKeepCrossways=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageKeepCrossways"), true)?true:false;
	m_nImageExtensionRate=	theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageExtensionRate"), 100);
	m_nImageLimitWidth=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitWidth"), 0);
	m_nImageLimitHeight=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitHeight"), 1000);
	m_nImageLimitRate=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitRate"), 100);
	m_bImageDeleteData = theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageDeleteData"), true) ? true : false;
	m_bImageSeparate=theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageSeparate"), true)?true:false;
	m_nImageKeepItWhenSizeBelow = theApp.GetProfileInt(OLESTR("converted setting"), OLESTR("ImageKeepItWhenSizeBelow"),60);
	//変数の内容をプロパティ情報に反映します。
	setClassVariablesToProperties();

	//リストビュー用のイメージリストを作成します。
	m_imgView.Create(16, 16, ILC_COLOR32 | ILC_MASK, 3, 0);
	m_imgView.Add(::LoadIcon(NULL, IDI_ERROR));
	m_imgView.Add(::LoadIcon(NULL, IDI_WARNING));
	m_imgView.Add(::LoadIcon(NULL, IDI_INFORMATION));
	m_wndView.SetImageList(&m_imgView, LVSIL_SMALL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// プロパティ ウィンドウを作成します
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("プロパティ ウィンドウを作成できませんでした\n");
		return FALSE; // 作成できませんでした
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		return FALSE;
	}
	m_wndCaptionBar.SetButton(TEXT("Cancel"), ID_TOOLS_CANCEL, CMFCCaptionBar::ALIGN_RIGHT,FALSE);
	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);

	return TRUE;
}

// CMainFrame メッセージ ハンドラー

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ビュー ウィンドウにフォーカスを与えます。
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ビューに最初にコマンドを処理する機会を与えます。
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// それ以外の場合は、既定の処理を行います。
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}
void CMainFrame::OnUpdateViewProperty(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndProperties.IsVisible());
}
void CMainFrame::OnViewProperty()
{
	m_wndProperties.ShowWindow(m_wndProperties.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}


void CMainFrame::OnUpdateCaptionBarCancel(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnCaptionBarCancel()
{
	setCancel(true);
}
void CMainFrame::OnDestroy()
{
	setPropertiesToClassVariables();

	//プロパティ情報をレジストリに保存します。
	theApp.WriteProfileString(OLESTR("converted setting"), OLESTR("imageType"), m_strImageType);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageQuality"), m_nImageQuality);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageKeepCrossways"), m_bImageKeepCrossways);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageExtensionRate"), m_nImageExtensionRate);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitWidth"), m_nImageLimitWidth);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitHeight"), m_nImageLimitHeight);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageLimitRate"), m_nImageLimitRate);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageDeleteData"), m_bImageDeleteData);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageSeparate"), m_bImageSeparate);
	theApp.WriteProfileInt(OLESTR("converted setting"), OLESTR("ImageKeepItWhenSizeBelow"), m_nImageKeepItWhenSizeBelow);


	//情報を保存します。
	CFrameWndEx::OnDestroy();
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CRect	rect;
	GetClientRect(&rect);
	//ステータスバーのプログレスバーサイズを調整します。
	if (m_wndProgress != NULL){
		m_wndProgress->setWidth(rect.Width() - 30);
	}

	CFrameWndEx::OnSize(nType, cx, cy);
}

//画面更新を行う、ポスト用メッセージ
LRESULT	CMainFrame::OnChangeState(WPARAM wParam, LPARAM lParam)
{
	UpdateStatusData* updateStatusData = reinterpret_cast<UpdateStatusData*>(wParam);
	if (NULL == updateStatusData){
		//データがない。
		return 0;
	}

	if (updateStatusData->nProgress > 0){
		m_wndProgress->SetPos(updateStatusData->nProgress);
	}
	if (updateStatusData->strAddText.GetLength() > 0){
		m_wndView.InsertItem(0, updateStatusData->strAddText, updateStatusData->nIcon);
	}

	delete updateStatusData, updateStatusData=NULL;

	return 0;
}
void CMainFrame::setPropertiesToClassVariables(void)
{
	//プロパティ情報をレジストリに保存します。
	m_strImageType = (LPCWSTR)(_bstr_t)(_variant_t)m_wndProperties.m_propImageType->GetValue();
	m_nImageQuality = (_variant_t)m_wndProperties.m_propImageQuality->GetValue();
	m_bImageKeepCrossways = (_variant_t)m_wndProperties.m_propImageKeepCrossways->GetValue();
	m_nImageExtensionRate = (_variant_t)m_wndProperties.m_propImageExtensionRate->GetValue();
	m_nImageLimitWidth = (_variant_t)m_wndProperties.m_propImageLimitWidth->GetValue();
	m_nImageLimitHeight = (_variant_t)m_wndProperties.m_propImageLimitHeight->GetValue();
	m_nImageLimitRate = (_variant_t)m_wndProperties.m_propImageLimitRate->GetValue();
	m_bImageDeleteData = (_variant_t)m_wndProperties.m_propImageDeleteData->GetValue();
	m_bImageSeparate = (_variant_t)m_wndProperties.m_propImageSeparate->GetValue();
	m_nImageKeepItWhenSizeBelow = (_variant_t)m_wndProperties.m_propImageKeepItWhenSizeBelow->GetValue();
}
//現在のクラス内変数をプロパティ情報に設定します。
void	CMainFrame::setClassVariablesToProperties(void)
{
	m_wndProperties.m_propImageType->SetValue(m_strImageType);
	m_wndProperties.m_propImageQuality->SetValue((_variant_t)m_nImageQuality);
	m_wndProperties.m_propImageKeepCrossways->SetValue((_variant_t)m_bImageKeepCrossways);
	m_wndProperties.m_propImageExtensionRate->SetValue((_variant_t)m_nImageExtensionRate);
	m_wndProperties.m_propImageLimitWidth->SetValue((_variant_t)m_nImageLimitWidth);
	m_wndProperties.m_propImageLimitHeight->SetValue((_variant_t)m_nImageLimitHeight);
	m_wndProperties.m_propImageLimitRate->SetValue((_variant_t)m_nImageLimitRate);
	m_wndProperties.m_propImageDeleteData->SetValue((_variant_t)m_bImageDeleteData);
	m_wndProperties.m_propImageSeparate->SetValue((_variant_t)m_bImageSeparate);
	m_wndProperties.m_propImageKeepItWhenSizeBelow->SetValue((_variant_t)m_nImageKeepItWhenSizeBelow);
}
//現在の進行状況、メッセージを更新します。
void	CMainFrame::updateStatus(const bool bFinished, const int nProgress, LPCWSTR pszAddText, const int nIcon)
{
	UpdateStatusData*	updateStatusData = new UpdateStatusData;
	if (NULL == updateStatusData){
		//メモリ不足
		_ASSERT(false);
		return;
	}
	updateStatusData->bFinished = bFinished;
	updateStatusData->nProgress = nProgress;
	updateStatusData->strAddText = pszAddText;
	updateStatusData->nIcon = nIcon;

	if (0 == PostMessage(WM_UPDATE_PROGRESS, reinterpret_cast<WPARAM>(updateStatusData))){
		//失敗
		_ASSERT(false);
		delete updateStatusData, updateStatusData = NULL;
	}

}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	do{
		if (isInProgress()){
			//処理中なので、終わるまでだめ。
			break;
		}
		//プロパティ情報を変数に格納します。
		setPropertiesToClassVariables();
		//プログレスバーを初期化します。
		m_wndProgress->SetPos(0);

		UINT count;	//Dropされたファイル数
		count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		//ドロップした数が０の場合は終了
		if (count == 0){
			break;
		}
		setInProgress(true);
		setCancel(false);
		MyThreadParam*	myThreadParam = new MyThreadParam;
		myThreadParam->pThis = this;

		UINT i;
		TCHAR	filePath[1024] = { '\0' };
		//ファイル、フォルダ一覧を取得します。
		for (i = 0; i < count; i++){
			DragQueryFile(hDropInfo, i, filePath, _countof(filePath));
			myThreadParam->files.push_back(filePath);
		}
		//非同期時はスレッドを開始させます。
		CWinThread*	pThread = AfxBeginThread(_ImageTransformerThread,
			myThreadParam,
			THREAD_PRIORITY_NORMAL,
			0,
			CREATE_SUSPENDED);
		//スレッド開始
		pThread->ResumeThread();
	} while (false);

	DragFinish(hDropInfo);
}
//変数の指定により画像データを変換します。
void	CMainFrame::convertImageDependentOnClassVariables(MyFileInfo& fileInfo)
{
	Bitmap* bitmap = NULL;
	DWORD dwReturn = ERROR_SUCCESS;
	CString strError;

	do{
		if (m_nImageKeepItWhenSizeBelow > static_cast<const long>(fileInfo.dwFileSize / 1024)){
			//ファイルサイズが予定より小さいので、変換せず終了
			break;
		}
		dwReturn = CMyImageTransformer::LoadBitmapFromFile(fileInfo.strPath, bitmap);
		if (NULL == bitmap){
			//画像データでは無い。
			strError = TEXT("読み込み失敗.");
			break;
		}

		int nFrameCount = 1;

		GUID   pageGuid = FrameDimensionTime;
		if (m_bImageSeparate){
			//画像内にあるフレーム数を取得します。
			UINT dimensionsCount = bitmap->GetFrameDimensionsCount();
			// Get the list of frame dimensions from the Image object.
			GUID* pDimensionIDs = new GUID[dimensionsCount];
			bitmap->GetFrameDimensionsList(pDimensionIDs, dimensionsCount);
			// Get the number of frames in the first dimension.
			if(dimensionsCount>0){
				pageGuid=pDimensionIDs[0];
				nFrameCount = bitmap->GetFrameCount(&pDimensionIDs[0]);
			}
			delete[] pDimensionIDs, pDimensionIDs = NULL;
		}

		//閾値用のサイズを計算します。
		long nImageLimitWidthRate = m_nImageLimitWidth;
		long nImageLimitHeightRate = m_nImageLimitHeight;
		if (m_nImageLimitRate > 100){
			nImageLimitWidthRate = m_nImageLimitWidth*m_nImageLimitRate / 100;
			nImageLimitHeightRate = m_nImageLimitHeight*m_nImageLimitRate / 100;
		}

		int n;
		CString	strFormat(TEXT("image/"));
		strFormat += m_strImageType;
		//ファイル名を取得します。
		WCHAR		wszDrive[_MAX_DRIVE] = { '\0' };
		WCHAR		wszDir[_MAX_DIR] = { '\0' };
		WCHAR		wszFname[_MAX_FNAME] = { '\0' };
		WCHAR		wszExt[_MAX_EXT] = { '\0' };

		::_wsplitpath_s(fileInfo.strPath, wszDrive, wszDir, wszFname, wszExt);

		bool bSameFile = false;
		CString	strSaveToPath;
		for (n = 0; n < nFrameCount; n++){

			int nRate = 10000;
			int nWidth = bitmap->GetWidth();
			int nHeigth = bitmap->GetHeight();
			bool bNeedConvert = false;

			//変換が必要か確認します。拡張子が変換後と同じか確認します。
			if (0!=m_strImageType.CompareNoCase(&wszExt[1])){
				//拡張子が異なる。
				bNeedConvert = true;
			}
			else{
				//拡張子が同じなので、削除が必要
				bSameFile = true;
			}
			if (m_nImageExtensionRate != 100){
				bNeedConvert = true;
				nRate = m_nImageExtensionRate*100;
			}
			if (m_nImageLimitWidth != 0 && nWidth > nImageLimitWidthRate){
				bNeedConvert = true;
				int nTempRate = ((m_nImageLimitWidth+1) * 10000) / nWidth;
				if (nTempRate<nRate){
					nRate = nTempRate;
				}
			}
			if (m_nImageLimitHeight != 0 && nHeigth > nImageLimitHeightRate){
				bNeedConvert = true;
				int nTempRate = ((m_nImageLimitHeight+1) * 10000) / nHeigth;
				if (nTempRate<nRate){
					nRate = nTempRate;
				}
			}
			if (m_nImageQuality < 100){
				bNeedConvert = true;
			}

			if (bNeedConvert){
				Gdiplus::Image*	image = bitmap;
				if (nRate != 10000){
					int nToWidth = (nWidth*nRate) / 10000;
					int nToHeight = (nHeigth*nRate) / 10000;
					//画像データを拡大・縮小します。
					Gdiplus::Graphics	graphicsMain((HWND)NULL);
					Gdiplus::Bitmap		bitmapMain(nWidth, nHeigth, &graphicsMain);
					Gdiplus::Graphics	graphicsThumbnail(&bitmapMain);
					graphicsThumbnail.DrawImage(bitmap, 0, 0,nToWidth, nToHeight);
					//image = bitmap->GetThumbnailImage((nWidth*nRate) / 10000, );
					image = bitmapMain.Clone(0, 0, nToWidth, nToHeight, NULL);
				}
				strSaveToPath = wszDrive;
				strSaveToPath += wszDir;
				strSaveToPath += wszFname;
				strSaveToPath += TEXT(".");
				if (nFrameCount > 1){
					//複数フレームの場合、分割ファイル名をつけます。
					WCHAR	wszTemp[16];
					::swprintf_s(wszTemp, OLESTR("%04x."), n);
					strSaveToPath += wszTemp;
					bSameFile = false;
				}
				strSaveToPath += m_strImageType;
				//初回保存で、保存先が同名ファイルの場合 OR 削除予定の場合、読み取り属性を取り除きます。
				if (n == 0 && (bSameFile || m_bImageDeleteData)){
					//同名ファイルの場合、保存前に読み取り属性がある場合、読み取り属性を取り除きます。
					DWORD dwAttr = ::GetFileAttributes(fileInfo.strPath);
					if (dwAttr&FILE_ATTRIBUTE_READONLY){
						//読み取り属性を削除します。
						dwAttr &= ~FILE_ATTRIBUTE_READONLY;
						SetFileAttributes(fileInfo.strPath, dwAttr);
					}
				}
				//画像を保存します。
				dwReturn = CMyImageTransformer::saveImage(image, strFormat, strSaveToPath, m_nImageQuality);
				if (image != bitmap){
					//削除します。
					delete image, image=NULL;
				}
				if (dwReturn != ERROR_SUCCESS){
					//保存に失敗
					strError = TEXT("保存エラー");
					break;
				}
			}
			bitmap->SelectActiveFrame(&pageGuid, n + 1);
		}
		//データ削除します。ただし、元データと比べて大きくなっている場合元データを残して、作成したデータを削除します。
		if (dwReturn == ERROR_SUCCESS&&!bSameFile&&m_bImageDeleteData){
			DWORD dwNewImageSize = myGetFileSize(strSaveToPath);
			if (dwNewImageSize <= fileInfo.dwFileSize){
				//元データを削除します。
				if (0 == ::DeleteFile(fileInfo.strPath)){
					dwReturn = ::GetLastError();
					strError = OLESTR("元データ削除エラー");
				}
			}
			else{
				//既存データのほうが小さいので、新規作成データを削除します。
				if (0 == ::DeleteFile(strSaveToPath)){
				}
			}
		}


	} while (false);
	//メモリから消去
	if (NULL != bitmap){
		delete bitmap, bitmap = NULL;
	}
	//情報を追加
	if (ERROR_SUCCESS != dwReturn || strError.GetLength()>0){
		WCHAR wszTemp[1024] = { '\0' };
		int nIcon = ERROR_SUCCESS != dwReturn ? 0 : 2;
		//
		::swprintf_s(wszTemp, TEXT("%s Error=%d file=%s"), static_cast<LPCWSTR>(strError), dwReturn, static_cast<LPCWSTR>(fileInfo.strPath));
		updateStatus(false, 0, wszTemp, nIcon);
	}
}

//画像変換処理専用の関数
const DWORD CMainFrame::FindAllFiles(vector<MyFileInfo>& files, const int progressStart, const int progressEnd)
{
	DWORD dwReturn(ERROR_SUCCESS);
	size_t n;
	size_t count = files.size();
	if (count == 0){
		//データが無いので終了
		return ERROR_SUCCESS;
	}
	int perProgress = (progressEnd - progressStart) / count;
	perProgress = perProgress == 0 ? 1 : perProgress;
	int nowProgress = progressStart;
	int nextProgress = progressStart;
	for (n = 0; n < count; n++){
		if (isCancel()){
			//キャンセルされたので抜けます。
			dwReturn = ERROR_CANCELLED;
			break;
		}
		MyFileInfo& fileInfo = files[n];
		vector<MyFileInfo>	subFiles;
		DWORD dwFileAttr = ::GetFileAttributes(fileInfo.strPath);
		nextProgress += perProgress;
		if (FILE_ATTRIBUTE_DIRECTORY&dwFileAttr){
			//指定パスがディレクトリ ディレクトリ内を全て検査します。
			do{
				CString	strFindFile = fileInfo.strPath;
				strFindFile += OLESTR("\\*.*");
				WIN32_FIND_DATA	findData = { '\0' };
				HANDLE			hFindHandle = INVALID_HANDLE_VALUE;
				//ファイル検索を行います。
				hFindHandle = ::FindFirstFile(strFindFile, &findData);
				if (INVALID_HANDLE_VALUE == hFindHandle){
					//err
					dwReturn = ::GetLastError();
					if (ERROR_FILE_NOT_FOUND != dwReturn){
						//ファイルがない場合以外はここ
					}
					break;
				}
				do{
					if ((::wcscmp(findData.cFileName, TEXT(".")) != 0)&&
						(::wcscmp(findData.cFileName, TEXT("..")) != 0)){
						//見つかったファイル名の完全パスを取得します。
						MyFileInfo	subFileInfo;
						subFileInfo.strPath = fileInfo.strPath;
						subFileInfo.strPath += OLESTR("\\");
						subFileInfo.strPath += findData.cFileName;
						subFileInfo.dwFileSize = findData.nFileSizeLow;
						subFiles.push_back(subFileInfo);
					}
					//次のファイルを検査します。
					if (::FindNextFile(hFindHandle, &findData) == 0){
						dwReturn = ::GetLastError();
						if (ERROR_NO_MORE_FILES == dwReturn){
							//ファイルが存在しません。
							dwReturn = ERROR_SUCCESS;
							break;
						}
						else{
							//エラー
							break;
						}
					}
				} while (true);
				if (INVALID_HANDLE_VALUE != hFindHandle){
					::FindClose(hFindHandle);
					hFindHandle = NULL;
				}
			} while (false);
			//再帰的に関数を呼び出します。
			FindAllFiles(subFiles, nowProgress, nextProgress);
		}
		else{
			//画像データを変換します。
			convertImageDependentOnClassVariables(fileInfo);
		}
		nowProgress = nextProgress;
		updateStatus(false, nextProgress, TEXT(""), 0);
	}
	return 0;
}

//!	@brief	指定パスのファイルサイズを取得します。
const DWORD	CMainFrame::myGetFileSize(LPCWSTR pszPaht)
{
	HANDLE hFile = ::CreateFile(pszPaht, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile){
		return 0;
	}
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	//ファイルサイズを取得したので終了します。
	::CloseHandle(hFile);
	return dwSize;
}

//画像変換用スレッド
const DWORD	CMainFrame::ImageTransformerThread(MyThreadParam* myThreadParam)
{
	updateStatus(false, 0, OLESTR("処理開始"), 2);
	size_t n;
	vector<CString>& files = myThreadParam->files;
	int	progress = 0;
	int perPos = 10000 / files.size();
	for (n = 0; n < files.size(); n++){
		CString& strPath = files[n];
		vector<MyFileInfo>	subFiles;
		MyFileInfo			fileInfo;
		fileInfo.strPath = strPath;
		fileInfo.dwFileSize = myGetFileSize(strPath);
		subFiles.push_back(fileInfo);
		FindAllFiles(subFiles, progress, progress + perPos);
		progress += perPos;
		updateStatus(false, progress, TEXT(""), 0);
	}
	updateStatus(true, 0, OLESTR("処理終了"), 2);

	//画像変換処理終了です。
	setInProgress(false);
	return 0;
}
UINT	AFX_CDECL CMainFrame::_ImageTransformerThread(LPVOID pParam)
{
	MyThreadParam*	myThreadParam = reinterpret_cast<MyThreadParam*>(pParam);
	if (NULL == myThreadParam){
		//そんなはずは無い。
		return 0;
	}
	return myThreadParam->pThis->ImageTransformerThread(myThreadParam);
}

