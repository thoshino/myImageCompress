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

// MainFrm.h : CMainFrame �N���X�̃C���^�[�t�F�C�X
//

#pragma once
#include "PropertiesWnd.h"

//�X�e�[�^�X�o�[�̃v���O���X�o�[����ʃT�C�Y�ɍ��킹�ăT�C�Y�ύX���邽�߁A
//CMFCRibbonProgressBar ���g�����܂����B
//�N���X�ϐ� m_nWidth ��ύX����݂̂̊g��
class CMyMFCRibbonProgressBar : public CMFCRibbonProgressBar
{
public:
	void	setWidth(const int w){ m_nWidth = w; };
};

//!	���̃A�v���̃��C�����W�b�N�Ƃ��F�X�������Ă��钆�j�N���X
class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// ����
public:

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ����
public:
	virtual ~CMainFrame();

protected:
	//�X���b�h�Ԃ̃p�����[�^
	typedef	struct	MyThreadParam{
		CMainFrame*			pThis;
		vector<CString>		files;		//���ʏo�͗pCSV�t�@�C���̃n���h��
	} MyThreadParam;

	typedef struct MyFileInfo{
		CString		strPath;
		DWORD		dwFileSize;
	}MyFileInfo;

protected:  // �R���g���[�� �o�[�p�����o�[
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;
	CMyMFCRibbonProgressBar*	m_wndProgress;	//�i�s�󋵂̃v���O���X�o�[
	CListCtrl			m_wndView;		//�G���[���ʂ̈ꗗ
	CImageList			m_imgView;	//�C���[�W���X�g

	CCriticalSection	m_locks;			//�X���b�h�ϐ����b�N�p
	bool				m_inProgress;		//�������t���O
	bool				m_cancel;			//�L�����Z���t���O
	//m_inProgress �� setter/getter
	const bool		isInProgress(){
		CSingleLock	autolock(&m_locks, TRUE);
		return m_inProgress;
	}
	void			setInProgress(const bool inProgress){
		CSingleLock	autolock(&m_locks, TRUE);
		m_inProgress = inProgress;
	}
	//m_cancel �� setter/getter
	const bool		isCancel(){
		CSingleLock	autolock(&m_locks, TRUE);
		return m_cancel;
	}
	void			setCancel(const bool cancel){
		CSingleLock	autolock(&m_locks, TRUE);
		m_cancel = cancel;
	}

	//�f�[�^�ϊ��̃p�����[�^
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

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	//��ʂ̐i�s�󋵂�ύX���܂��B
	afx_msg LRESULT OnChangeState(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();

	//�󋵒ʒm�p �|�X�g�p�f�[�^
	typedef struct UpdateStatusData{
		bool	bFinished;
		int		nProgress;
		CString	strAddText;
		int		nIcon;
	} UpdateStatusData;
	//���݂̐i�s�󋵁A���b�Z�[�W���X�V���܂��B
	void		updateStatus(const bool bFinished, const int nProgress, LPCWSTR pszAddText,const int nIcon);
	//���݂̃v���p�e�B�����N���X���ϐ��ɐݒ肵�܂��B
	void		setPropertiesToClassVariables(void);
	//���݂̃N���X���ϐ����v���p�e�B���ɐݒ肵�܂��B
	void		setClassVariablesToProperties(void);

	//�ϐ��̎w��ɂ��摜�f�[�^��ϊ����܂��B
	void		convertImageDependentOnClassVariables(MyFileInfo& fileInfo);
	//!	@brief	�w��p�X�̃t�@�C���T�C�Y���擾���܂��B
	const DWORD	myGetFileSize(LPCWSTR pszPaht);

	//�摜�ϊ�������p�̊֐�
	const DWORD FindAllFiles(vector<MyFileInfo>& files, const int progressStart, const int progressEnd);
	//�摜�ϊ��p�X���b�h
	const DWORD	ImageTransformerThread(MyThreadParam* myThreadParam);
	static UINT	AFX_CDECL _ImageTransformerThread(LPVOID pParam);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};


