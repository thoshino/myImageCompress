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

// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once
#include "PropertiesWnd.h"

//ステータスバーのプログレスバーを画面サイズに合わせてサイズ変更するため、
//CMFCRibbonProgressBar を拡張しました。
//クラス変数 m_nWidth を変更するのみの拡張
class CMyMFCRibbonProgressBar : public CMFCRibbonProgressBar
{
public:
	void	setWidth(const int w){ m_nWidth = w; };
};

//!	このアプリのメインロジックとか色々処理している中核クラス
class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 実装
public:
	virtual ~CMainFrame();

protected:
	//スレッド間のパラメータ
	typedef	struct	MyThreadParam{
		CMainFrame*			pThis;
		vector<CString>		files;		//結果出力用CSVファイルのハンドル
	} MyThreadParam;

	typedef struct MyFileInfo{
		CString		strPath;
		DWORD		dwFileSize;
	}MyFileInfo;

protected:  // コントロール バー用メンバー
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;
	CMyMFCRibbonProgressBar*	m_wndProgress;	//進行状況のプログレスバー
	CListCtrl			m_wndView;		//エラー結果の一覧
	CImageList			m_imgView;	//イメージリスト

	CCriticalSection	m_locks;			//スレッド変数ロック用
	bool				m_inProgress;		//処理中フラグ
	bool				m_cancel;			//キャンセルフラグ
	//m_inProgress の setter/getter
	const bool		isInProgress(){
		CSingleLock	autolock(&m_locks, TRUE);
		return m_inProgress;
	}
	void			setInProgress(const bool inProgress){
		CSingleLock	autolock(&m_locks, TRUE);
		m_inProgress = inProgress;
	}
	//m_cancel の setter/getter
	const bool		isCancel(){
		CSingleLock	autolock(&m_locks, TRUE);
		return m_cancel;
	}
	void			setCancel(const bool cancel){
		CSingleLock	autolock(&m_locks, TRUE);
		m_cancel = cancel;
	}

	//データ変換のパラメータ
	CString	m_strImageType;
	long	m_nImageQuality;
	bool	m_bImageKeepCrossways;
	long	m_nImageExtensionRate;
	long	m_nImageLimitWidth;
	long	m_nImageLimitHeight;
	long	m_nImageLimitRate;
	bool	m_bImageDeleteData;
	bool	m_bImageSeparate;
	long	m_nImageKeepItWhenSizeBelow;

// 生成された、メッセージ割り当て関数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewProperty(CCmdUI* pCmdUI);
	afx_msg void OnViewProperty();
	afx_msg void OnUpdateCaptionBarCancel(CCmdUI* pCmdUI);
	afx_msg void OnCaptionBarCancel();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//画面の進行状況を変更します。
	afx_msg LRESULT OnChangeState(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();

	//状況通知用 ポスト用データ
	typedef struct UpdateStatusData{
		bool	bFinished;
		int		nProgress;
		CString	strAddText;
		int		nIcon;
	} UpdateStatusData;
	//現在の進行状況、メッセージを更新します。
	void		updateStatus(const bool bFinished, const int nProgress, LPCWSTR pszAddText,const int nIcon);
	//現在のプロパティ情報をクラス内変数に設定します。
	void		setPropertiesToClassVariables(void);
	//現在のクラス内変数をプロパティ情報に設定します。
	void		setClassVariablesToProperties(void);

	//変数の指定により画像データを変換します。
	void		convertImageDependentOnClassVariables(MyFileInfo& fileInfo);
	//!	@brief	指定パスのファイルサイズを取得します。
	const DWORD	myGetFileSize(LPCWSTR pszPaht);

	//画像変換処理専用の関数
	const DWORD FindAllFiles(vector<MyFileInfo>& files, const int progressStart, const int progressEnd);
	//画像変換用スレッド
	const DWORD	ImageTransformerThread(MyThreadParam* myThreadParam);
	static UINT	AFX_CDECL _ImageTransformerThread(LPVOID pParam);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};


