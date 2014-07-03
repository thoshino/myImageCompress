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
// CResourceViewBar ���b�Z�[�W �n���h���[

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

	// �R���{ �{�b�N�X�̍쐬:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�v���p�e�B �O���b�h���쐬�ł��܂���ł���\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}

	InitPropList();

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	//m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ���b�N����Ă��܂�*/);
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ���b�N����܂���*/);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	//m_wndToolBar.SetOwner(this);

	//// ���ׂẴR�}���h���A�e�t���[���o�R�ł͂Ȃ����̃R���g���[���o�R�œn����܂�:
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
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	//�ҏW���Ă��{�[���h�\�����Ȃ��悤�ɂ��܂��B
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

		m_propImageDeleteData = new CMFCPropertyGridProperty(_T("Delete orignal data."), (_variant_t)true, _T("�摜�̏c���䗦���ێ����܂��B"));
		pGroup->AddSubItem(m_propImageDeleteData);

		m_propImageKeepCrossways = new CMFCPropertyGridProperty(_T("�c������ێ�"), (_variant_t)true, _T("�摜�̏c���䗦���ێ����܂��B"));
		pGroup->AddSubItem(m_propImageKeepCrossways);

		m_propImageKeepItWhenSizeBelow = new CMFCPropertyGridProperty(_T("�ێ��f�[�^�T�C�Y"), (_variant_t)60L, _T("���f�[�^�̉摜�f�[�^�T�C�Y���w��T�C�YKB�ȉ��Ȃ�A�ϊ��������܂���B�B"));
		m_propImageKeepItWhenSizeBelow->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageKeepItWhenSizeBelow);

		m_propImageExtensionRate = new CMFCPropertyGridProperty(_T("�k���E�g��䗦"), (_variant_t)100L, _T("�摜�̏k���E�g��䗦���w�肵�܂��B100���w�肵�Ă���ꍇ�A�g��k���͂��܂���B"));
		m_propImageExtensionRate->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageExtensionRate);

		m_propImageLimitWidth = new CMFCPropertyGridProperty(_T("���̒���"), (_variant_t)0L, _T("�摜�̉��̒������w��l���傫���ꍇ�A���̒l�k�����܂��B"));
		m_propImageLimitWidth->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitWidth);

		m_propImageLimitHeight = new CMFCPropertyGridProperty(_T("�c�̒���"), (_variant_t)1000L, _T("�摜�̏c�̒������w��l���傫���ꍇ�A���̒l�k�����܂��B"));
		m_propImageLimitHeight->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitHeight);

		m_propImageLimitRate = new CMFCPropertyGridProperty(_T("�c���k��臒l"), (_variant_t)100L, _T("�c���̃T�C�Y�k���̒������A�w�聓�����͈̔͂Ȃ�k�������͍s���܂���B"));
		m_propImageLimitRate->EnableSpinControl(TRUE, 0, MAXLONG);
		pGroup->AddSubItem(m_propImageLimitRate);

		m_propImageSeparate = new CMFCPropertyGridProperty(_T("Gif����"), (_variant_t)true, _T("�A�j���[�V����Gif�A�}���`�y�[�WTiff�����ꂼ��̃t���[���ɕ������ĕۑ����܂��B"));
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
