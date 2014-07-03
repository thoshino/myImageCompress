// ���� MFC �T���v�� �\�[�X �R�[�h�ł́AMFC Microsoft Office Fluent ���[�U�[ �C���^�[�t�F�C�X 
// ("Fluent UI") �̎g�p���@�������܂��B���̃R�[�h�́AMFC C++ ���C�u���� �\�t�g�E�F�A�� 
// ��������Ă��� Microsoft Foundation Class ���t�@�����X����ъ֘A�d�q�h�L�������g��
// �⊮���邽�߂̎Q�l�����Ƃ��Ē񋟂���܂��B
// Fluent UI �𕡐��A�g�p�A�܂��͔z�z���邽�߂̃��C�Z���X�����͌ʂɗp�ӂ���Ă��܂��B
// Fluent UI ���C�Z���X �v���O�����̏ڍׂɂ��ẮAWeb �T�C�g
// http://go.microsoft.com/fwlink/?LinkId=238214 ���Q�Ƃ��Ă��������B
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : CMainFrame �N���X�̎���
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

// CMainFrame �R���X�g���N�V����/�f�X�g���N�V����

CMainFrame::CMainFrame()
{
	// TODO: �����o�[�������R�[�h�������ɒǉ����Ă��������B
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

	// �t���[���̃N���C�A���g�̈�S�̂��߂�r���[���쐬���܂��B
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
		return -1;      // �쐬�ł��Ȃ��ꍇ
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
	//�v���O���X�͈̔͂�ݒ肵�܂��B
	m_wndProgress->SetRange(0, 10000);
	// Visual Studio 2005 �X�^�C���̃h�b�L���O �E�B���h�E�����L���ɂ��܂�
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 �X�^�C���̃h�b�L���O �E�B���h�E�̎�����\�������L���ɂ��܂�
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// �L���v�V���� �o�[���쐬���܂�:
	if (!CreateCaptionBar())
	{
		return -1;      //�쐬�ł��Ȃ��ꍇ
	}

	// �h�b�L���O �E�B���h�E���쐬���܂�
	if (!CreateDockingWindows())
	{
		return -1;
	}

	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// �Œ�l�Ɋ�Â��ăr�W���A�� �}�l�[�W���[�� visual �X�^�C����ݒ肵�܂�
	OnApplicationLook(theApp.m_nAppLook);

	//���W�X�g����������擾���܂��B
	//�v���p�e�B�������W�X�g���ɕۑ����܂��B
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
	//�ϐ��̓��e���v���p�e�B���ɔ��f���܂��B
	setClassVariablesToProperties();

	//���X�g�r���[�p�̃C���[�W���X�g���쐬���܂��B
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
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// �v���p�e�B �E�B���h�E���쐬���܂�
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("�v���p�e�B �E�B���h�E���쐬�ł��܂���ł���\n");
		return FALSE; // �쐬�ł��܂���ł���
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

// CMainFrame ���b�Z�[�W �n���h���[

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// �r���[ �E�B���h�E�Ƀt�H�[�J�X��^���܂��B
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// �r���[�ɍŏ��ɃR�}���h����������@���^���܂��B
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ȊO�̏ꍇ�́A����̏������s���܂��B
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

	//�v���p�e�B�������W�X�g���ɕۑ����܂��B
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


	//����ۑ����܂��B
	CFrameWndEx::OnDestroy();
}
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CRect	rect;
	GetClientRect(&rect);
	//�X�e�[�^�X�o�[�̃v���O���X�o�[�T�C�Y�𒲐����܂��B
	if (m_wndProgress != NULL){
		m_wndProgress->setWidth(rect.Width() - 30);
	}

	CFrameWndEx::OnSize(nType, cx, cy);
}

//��ʍX�V���s���A�|�X�g�p���b�Z�[�W
LRESULT	CMainFrame::OnChangeState(WPARAM wParam, LPARAM lParam)
{
	UpdateStatusData* updateStatusData = reinterpret_cast<UpdateStatusData*>(wParam);
	if (NULL == updateStatusData){
		//�f�[�^���Ȃ��B
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
	//�v���p�e�B�������W�X�g���ɕۑ����܂��B
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
//���݂̃N���X���ϐ����v���p�e�B���ɐݒ肵�܂��B
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
//���݂̐i�s�󋵁A���b�Z�[�W���X�V���܂��B
void	CMainFrame::updateStatus(const bool bFinished, const int nProgress, LPCWSTR pszAddText, const int nIcon)
{
	UpdateStatusData*	updateStatusData = new UpdateStatusData;
	if (NULL == updateStatusData){
		//�������s��
		_ASSERT(false);
		return;
	}
	updateStatusData->bFinished = bFinished;
	updateStatusData->nProgress = nProgress;
	updateStatusData->strAddText = pszAddText;
	updateStatusData->nIcon = nIcon;

	if (0 == PostMessage(WM_UPDATE_PROGRESS, reinterpret_cast<WPARAM>(updateStatusData))){
		//���s
		_ASSERT(false);
		delete updateStatusData, updateStatusData = NULL;
	}

}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	do{
		if (isInProgress()){
			//�������Ȃ̂ŁA�I���܂ł��߁B
			break;
		}
		//�v���p�e�B����ϐ��Ɋi�[���܂��B
		setPropertiesToClassVariables();
		//�v���O���X�o�[�����������܂��B
		m_wndProgress->SetPos(0);

		UINT count;	//Drop���ꂽ�t�@�C����
		count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
		//�h���b�v���������O�̏ꍇ�͏I��
		if (count == 0){
			break;
		}
		setInProgress(true);
		setCancel(false);
		MyThreadParam*	myThreadParam = new MyThreadParam;
		myThreadParam->pThis = this;

		UINT i;
		TCHAR	filePath[1024] = { '\0' };
		//�t�@�C���A�t�H���_�ꗗ���擾���܂��B
		for (i = 0; i < count; i++){
			DragQueryFile(hDropInfo, i, filePath, _countof(filePath));
			myThreadParam->files.push_back(filePath);
		}
		//�񓯊����̓X���b�h���J�n�����܂��B
		CWinThread*	pThread = AfxBeginThread(_ImageTransformerThread,
			myThreadParam,
			THREAD_PRIORITY_NORMAL,
			0,
			CREATE_SUSPENDED);
		//�X���b�h�J�n
		pThread->ResumeThread();
	} while (false);

	DragFinish(hDropInfo);
}
//�ϐ��̎w��ɂ��摜�f�[�^��ϊ����܂��B
void	CMainFrame::convertImageDependentOnClassVariables(MyFileInfo& fileInfo)
{
	Bitmap* bitmap = NULL;
	DWORD dwReturn = ERROR_SUCCESS;
	CString strError;

	do{
		if (m_nImageKeepItWhenSizeBelow > static_cast<const long>(fileInfo.dwFileSize / 1024)){
			//�t�@�C���T�C�Y���\���菬�����̂ŁA�ϊ������I��
			break;
		}
		dwReturn = CMyImageTransformer::LoadBitmapFromFile(fileInfo.strPath, bitmap);
		if (NULL == bitmap){
			//�摜�f�[�^�ł͖����B
			strError = TEXT("�ǂݍ��ݎ��s.");
			break;
		}

		int nFrameCount = 1;

		GUID   pageGuid = FrameDimensionTime;
		if (m_bImageSeparate){
			//�摜���ɂ���t���[�������擾���܂��B
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

		//臒l�p�̃T�C�Y���v�Z���܂��B
		long nImageLimitWidthRate = m_nImageLimitWidth;
		long nImageLimitHeightRate = m_nImageLimitHeight;
		if (m_nImageLimitRate > 100){
			nImageLimitWidthRate = m_nImageLimitWidth*m_nImageLimitRate / 100;
			nImageLimitHeightRate = m_nImageLimitHeight*m_nImageLimitRate / 100;
		}

		int n;
		CString	strFormat(TEXT("image/"));
		strFormat += m_strImageType;
		//�t�@�C�������擾���܂��B
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

			//�ϊ����K�v���m�F���܂��B�g���q���ϊ���Ɠ������m�F���܂��B
			if (0!=m_strImageType.CompareNoCase(&wszExt[1])){
				//�g���q���قȂ�B
				bNeedConvert = true;
			}
			else{
				//�g���q�������Ȃ̂ŁA�폜���K�v
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
					//�摜�f�[�^���g��E�k�����܂��B
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
					//�����t���[���̏ꍇ�A�����t�@�C���������܂��B
					WCHAR	wszTemp[16];
					::swprintf_s(wszTemp, OLESTR("%04x."), n);
					strSaveToPath += wszTemp;
					bSameFile = false;
				}
				strSaveToPath += m_strImageType;
				//����ۑ��ŁA�ۑ��悪�����t�@�C���̏ꍇ OR �폜�\��̏ꍇ�A�ǂݎ�葮������菜���܂��B
				if (n == 0 && (bSameFile || m_bImageDeleteData)){
					//�����t�@�C���̏ꍇ�A�ۑ��O�ɓǂݎ�葮��������ꍇ�A�ǂݎ�葮������菜���܂��B
					DWORD dwAttr = ::GetFileAttributes(fileInfo.strPath);
					if (dwAttr&FILE_ATTRIBUTE_READONLY){
						//�ǂݎ�葮�����폜���܂��B
						dwAttr &= ~FILE_ATTRIBUTE_READONLY;
						SetFileAttributes(fileInfo.strPath, dwAttr);
					}
				}
				//�摜��ۑ����܂��B
				dwReturn = CMyImageTransformer::saveImage(image, strFormat, strSaveToPath, m_nImageQuality);
				if (image != bitmap){
					//�폜���܂��B
					delete image, image=NULL;
				}
				if (dwReturn != ERROR_SUCCESS){
					//�ۑ��Ɏ��s
					strError = TEXT("�ۑ��G���[");
					break;
				}
			}
			bitmap->SelectActiveFrame(&pageGuid, n + 1);
		}
		//�f�[�^�폜���܂��B�������A���f�[�^�Ɣ�ׂđ傫���Ȃ��Ă���ꍇ���f�[�^���c���āA�쐬�����f�[�^���폜���܂��B
		if (dwReturn == ERROR_SUCCESS&&!bSameFile&&m_bImageDeleteData){
			DWORD dwNewImageSize = myGetFileSize(strSaveToPath);
			if (dwNewImageSize <= fileInfo.dwFileSize){
				//���f�[�^���폜���܂��B
				if (0 == ::DeleteFile(fileInfo.strPath)){
					dwReturn = ::GetLastError();
					strError = OLESTR("���f�[�^�폜�G���[");
				}
			}
			else{
				//�����f�[�^�̂ق����������̂ŁA�V�K�쐬�f�[�^���폜���܂��B
				if (0 == ::DeleteFile(strSaveToPath)){
				}
			}
		}


	} while (false);
	//�������������
	if (NULL != bitmap){
		delete bitmap, bitmap = NULL;
	}
	//����ǉ�
	if (ERROR_SUCCESS != dwReturn || strError.GetLength()>0){
		WCHAR wszTemp[1024] = { '\0' };
		int nIcon = ERROR_SUCCESS != dwReturn ? 0 : 2;
		//
		::swprintf_s(wszTemp, TEXT("%s Error=%d file=%s"), static_cast<LPCWSTR>(strError), dwReturn, static_cast<LPCWSTR>(fileInfo.strPath));
		updateStatus(false, 0, wszTemp, nIcon);
	}
}

//�摜�ϊ�������p�̊֐�
const DWORD CMainFrame::FindAllFiles(vector<MyFileInfo>& files, const int progressStart, const int progressEnd)
{
	DWORD dwReturn(ERROR_SUCCESS);
	size_t n;
	size_t count = files.size();
	if (count == 0){
		//�f�[�^�������̂ŏI��
		return ERROR_SUCCESS;
	}
	int perProgress = (progressEnd - progressStart) / count;
	perProgress = perProgress == 0 ? 1 : perProgress;
	int nowProgress = progressStart;
	int nextProgress = progressStart;
	for (n = 0; n < count; n++){
		if (isCancel()){
			//�L�����Z�����ꂽ�̂Ŕ����܂��B
			dwReturn = ERROR_CANCELLED;
			break;
		}
		MyFileInfo& fileInfo = files[n];
		vector<MyFileInfo>	subFiles;
		DWORD dwFileAttr = ::GetFileAttributes(fileInfo.strPath);
		nextProgress += perProgress;
		if (FILE_ATTRIBUTE_DIRECTORY&dwFileAttr){
			//�w��p�X���f�B���N�g�� �f�B���N�g������S�Č������܂��B
			do{
				CString	strFindFile = fileInfo.strPath;
				strFindFile += OLESTR("\\*.*");
				WIN32_FIND_DATA	findData = { '\0' };
				HANDLE			hFindHandle = INVALID_HANDLE_VALUE;
				//�t�@�C���������s���܂��B
				hFindHandle = ::FindFirstFile(strFindFile, &findData);
				if (INVALID_HANDLE_VALUE == hFindHandle){
					//err
					dwReturn = ::GetLastError();
					if (ERROR_FILE_NOT_FOUND != dwReturn){
						//�t�@�C�����Ȃ��ꍇ�ȊO�͂���
					}
					break;
				}
				do{
					if ((::wcscmp(findData.cFileName, TEXT(".")) != 0)&&
						(::wcscmp(findData.cFileName, TEXT("..")) != 0)){
						//���������t�@�C�����̊��S�p�X���擾���܂��B
						MyFileInfo	subFileInfo;
						subFileInfo.strPath = fileInfo.strPath;
						subFileInfo.strPath += OLESTR("\\");
						subFileInfo.strPath += findData.cFileName;
						subFileInfo.dwFileSize = findData.nFileSizeLow;
						subFiles.push_back(subFileInfo);
					}
					//���̃t�@�C�����������܂��B
					if (::FindNextFile(hFindHandle, &findData) == 0){
						dwReturn = ::GetLastError();
						if (ERROR_NO_MORE_FILES == dwReturn){
							//�t�@�C�������݂��܂���B
							dwReturn = ERROR_SUCCESS;
							break;
						}
						else{
							//�G���[
							break;
						}
					}
				} while (true);
				if (INVALID_HANDLE_VALUE != hFindHandle){
					::FindClose(hFindHandle);
					hFindHandle = NULL;
				}
			} while (false);
			//�ċA�I�Ɋ֐����Ăяo���܂��B
			FindAllFiles(subFiles, nowProgress, nextProgress);
		}
		else{
			//�摜�f�[�^��ϊ����܂��B
			convertImageDependentOnClassVariables(fileInfo);
		}
		nowProgress = nextProgress;
		updateStatus(false, nextProgress, TEXT(""), 0);
	}
	return 0;
}

//!	@brief	�w��p�X�̃t�@�C���T�C�Y���擾���܂��B
const DWORD	CMainFrame::myGetFileSize(LPCWSTR pszPaht)
{
	HANDLE hFile = ::CreateFile(pszPaht, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile){
		return 0;
	}
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	//�t�@�C���T�C�Y���擾�����̂ŏI�����܂��B
	::CloseHandle(hFile);
	return dwSize;
}

//�摜�ϊ��p�X���b�h
const DWORD	CMainFrame::ImageTransformerThread(MyThreadParam* myThreadParam)
{
	updateStatus(false, 0, OLESTR("�����J�n"), 2);
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
	updateStatus(true, 0, OLESTR("�����I��"), 2);

	//�摜�ϊ������I���ł��B
	setInProgress(false);
	return 0;
}
UINT	AFX_CDECL CMainFrame::_ImageTransformerThread(LPVOID pParam)
{
	MyThreadParam*	myThreadParam = reinterpret_cast<MyThreadParam*>(pParam);
	if (NULL == myThreadParam){
		//����Ȃ͂��͖����B
		return 0;
	}
	return myThreadParam->pThis->ImageTransformerThread(myThreadParam);
}

