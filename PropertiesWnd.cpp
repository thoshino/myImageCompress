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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "myImageCompress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
	m_propImageType = NULL;
	m_propImageQuality = NULL;
	m_propImageKeepCrossways = NULL;
	m_propImageExtensionRate = NULL;
	m_propImageLimitWidth = NULL;
	m_propImageLimitHeight = NULL;
	m_propImageLimitRate = NULL;
	m_propImageDeleteData = NULL;
	m_propImageSeparate = NULL;
	m_propImageKeepItWhenSizeBelow = NULL;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar メッセージ ハンドラー

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

//	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

//	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), rectClient.Height() -(m_nComboHeight), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// コンボ ボックスの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("プロパティ グリッドを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	InitPropList();

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	//m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ロックされています*/);
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ロックされました*/);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	//m_wndToolBar.SetOwner(this);

	//// すべてのコマンドが、親フレーム経由ではなくこのコントロール経由で渡されます:
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: ここにコマンド ハンドラー コードを追加します
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: ここにコマンド ハンドラー コードを追加します
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO:ここにコマンド更新 UI ハンドラー コードを追加します
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	//編集してもボールド表示しないようにします。
	m_wndPropList.MarkModifiedProperties(FALSE);

	CMFCPropertyGridProperty* pProp = NULL;
	{
		CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("converted setting"));

		m_propImageType = new CMFCPropertyGridProperty(_T("image type"), _T("jpg"), _T("choose which types of images are converting to"));
		m_propImageType->AddOption(_T("jpg"));
		m_propImageType->AddOption(_T("tiff"));
		m_propImageType->AddOption(_T("gif"));
		m_propImageType->AddOption(_T("png"));
		m_propImageType->AddOption(_T("bmp"));
		m_propImageType->AddOption(_T("emf"));
		m_propImageType->AllowEdit(FALSE);
		pGroup->AddSubItem(m_propImageType);


		m_propImageQuality = new CMFCPropertyGridProperty(_T("image quality"), (_variant_t)60l, _T("image quality"));
		m_propImageQuality->EnableSpinControl(TRUE, 0, 100);
		pGroup->AddSubItem(m_propImageQuality);

		m_propImageDeleteData = new CMFCPropertyGridProperty(_T("Delete orignal data."), (_variant_t)true, _T("画像の縦横比率を維持します。"));
		pGroup->AddSubItem(m_propImageDeleteData);

		m_propImageKeepCrossways = new CMFCPropertyGridProperty(_T("縦横比を維持"), (_variant_t)true, _T("画像の縦横比率を維持します。"));
		pGroup->AddSubItem(m_propImageKeepCrossways);

		m_propImageKeepItWhenSizeBelow = new CMFCPropertyGridProperty(_T("維持データサイズ"), (_variant_t)60L, _T("元データの画像データサイズが指定サイズKB以下なら、変換処理しません。。"));
		m_propImageKeepItWhenSizeBelow->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageKeepItWhenSizeBelow);

		m_propImageExtensionRate = new CMFCPropertyGridProperty(_T("縮小・拡大比率"), (_variant_t)100L, _T("画像の縮小・拡大比率を指定します。100を指定している場合、拡大縮小はしません。"));
		m_propImageExtensionRate->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageExtensionRate);

		m_propImageLimitWidth = new CMFCPropertyGridProperty(_T("横の長さ"), (_variant_t)0L, _T("画像の横の長さが指定値より大きい場合、この値縮小します。"));
		m_propImageLimitWidth->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitWidth);

		m_propImageLimitHeight = new CMFCPropertyGridProperty(_T("縦の長さ"), (_variant_t)1000L, _T("画像の縦の長さが指定値より大きい場合、この値縮小します。"));
		m_propImageLimitHeight->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitHeight);

		m_propImageLimitRate = new CMFCPropertyGridProperty(_T("縦横縮小閾値"), (_variant_t)100L, _T("縦横のサイズ縮小の長さが、指定％未満の範囲なら縮小処理は行いません。"));
		m_propImageLimitRate->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitRate);

		m_propImageSeparate = new CMFCPropertyGridProperty(_T("Gif分割"), (_variant_t)true, _T("アニメーションGif、マルチページTiffをそれぞれのフレームに分割して保存します。"));
		pGroup->AddSubItem(m_propImageSeparate);
		
		m_wndPropList.AddProperty(pGroup);
	}
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}
