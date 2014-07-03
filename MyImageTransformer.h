#pragma once
class CMyImageTransformer
{
public:
	static const DWORD ConvertStatusToWin32Error(const Status status);
	//!	@brief	GDIPlus �� Image �I�u�W�F�N�g ��Method Save �ɂ�
	//!	���p����AEncode CLSID ���擾���܂��B
	//!	�w��摜�C���[�W�����񂩂�ACLSID ���擾���A����𗘗p���邱�ƂŁA
	//!	�C�ӂ̌`���ŉ摜�C���[�W��ۑ��ł��܂��B
	//!	@param[in]	pszFormat	�摜�`�� �� image/jpeg
	//!	@param[out]	Clsid		�Ή������摜�C���[�W��CLSID���ԓ�����܂��B
	//!	@retval		ERROR_SUCCESS		����
	//!	@retval		ERROR_SUCCESS�ȊO	�G���[���� Win32API �G���[�ԍ����Ԃ���܂��B
	static const DWORD	GetEncoderClsid(LPCWSTR pszFormat, CLSID& Clsid);
	//!	@brie	�A�v���P�[�V�������x����肾���ǁA�����ɋL��
	//!	�C���[�W���i�[����Ă���t�H���_���擾���܂��B
	static CString&	GetImagesFolder(void);
	//!	@brief	���\�[�X����摜�C���[�W�����[�h���܂��B
	//!	@param[in]	hModule		���\�[�X���W���[���̃n���h��
	//!	@param[in]	pszCategory	���\�[�X�J�e�S��
	//!	@param[in]	nID			���\�[�XID
	//!	@param[out]	btDst		���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
	//!	@retval		ERROR_SUCCESS	����
	//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
	static const DWORD	LoadBitmapFromJpgResource(HMODULE hModule, const UINT nID, LPCWSTR pszCategory, CBitmap& btDst);
	//!	@brief	�w��t�@�C�����C���[�W�f�[�^�Ƃ��ēǂݍ��݂܂��B
	//!	@param[in]	pszFile	�t�@�C����
	//!	@param[out]	btDst	���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
	//!	@retval		ERROR_SUCCESS	����
	//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
	static const DWORD	LoadBitmapFromFile(LPCWSTR pszFile, CBitmap& btDst);
	//!	@brief	�w��t�@�C�����C���[�W�f�[�^�Ƃ��ēǂݍ��݂܂��B
	//!	@param[in]	pszFile	�t�@�C����
	//!	@param[out]	btDst	���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
	//!	@retval		ERROR_SUCCESS	����
	//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
	static const DWORD	LoadBitmapFromFile(LPCWSTR pszFile, Bitmap*& pBitmap);
	//!	@brief	�w��T�C�Y��Bitmap���T�C�Y�ύX���܂��B
	//!			����Bitmap���w��T�C�Y�����̏ꍇ�A�T�C�Y�ύX�����R�s�[�����l��ԓ����܂��B
	//!			���̏c���䗦�͕ێ����A�T�C�Y�ύX���܂��B
	//!	@param[in]	srcBitmap		��Bitmap
	//!	@param[in]	width			�k����̕�
	//!	@param[in]	height			�k����̍�
	//!	@param[out]	distBitmap		�V�K�쐬����Bitmap�N���X
	//!	@retval		ERROR_SUCCESS	����
	//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
	static const DWORD	CreateAdjustThumbnailImage(Image* srcBitmap, const int width, const int height, Image*& distBitmap);

	//!	@brief	Bitmap�`��ɍ��킹�ă��[�W�����쐬���s���܂��B
	static HRGN CreateRgnFromBitmap(HBITMAP hBitmap, DWORD dwTransColor);

	//!	@brief	�w��摜�̃}�X�N�J���[�������폜�������[�W�������쐬���w��E�C���h�E�֓K�p���܂��B
	//!	@param[in]	hWnd			���[�W������K�p����E�C���h�E
	//!	@param[in]	hBitmap			�}�X�N�J���[���܂܂ꂽ�r�b�g�}�b�v���
	//!	@param[in]	clrMaskColor	�}�X�N�J���[
	//!	@retval		ERROR_SUCCESS	����
	//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
	static const DWORD	SetWindowMask(HWND hWnd, HBITMAP hBitmap, COLORREF clrMaskColor);
	//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
	//static const DWORD	DrawImage(HDC hDc,HBITMAP hBitmap,const RECT& rect);
	//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
	//!	@param[in]	hDc		�`���̃f�o�C�X�R���e�L�X�g
	//!	@param[in]	pBitmap	�`�悵���� Image �N���X
	//!	@param[in]	rect	�`��̈�
	//!	@param[in]	alpha	�`�掞�̃��l 1 �̏ꍇ����
	static const DWORD	DrawImage(HDC hDc, Image* pBitmap, const RECT& rect, const float alpha = 1);
	//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
	//static const DWORD	DrawImage(HDC hDc,Image* pBitmap,const int nLeft,const int nTop);
	//!	@brief	�w��Bitmap���w�� Graphics �֕`�悵�܂��B
	//!	@param[in]	graphics	�`���̃f�o�C�X�R���e�L�X�g
	//!	@param[in]	pBitmap		�`�悵���� Image �N���X
	//!	@param[in]	rect		�`��̈�
	//!	@param[in]	alpha		�`�掞�̃��l 1 �̏ꍇ����
	static const DWORD	DrawImage(Graphics& graphics, Image* pBitmap, const RECT& rect, const float alpha = 1);
	//!	@brief	�w��Bitmap���w�� Graphics �֕`�悵�܂��B
	//static const DWORD	DrawImage(Graphics& graphics,Image* pBitmap,const int nLeft,const int nTop);
	static HBITMAP	CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

	//!	@brief	�w�i�Ɏw�肵�����E�C���h�E�𓧉ߏ�������Bitmap��ԓ����܂��B
	//!	�w�肳�ꂽ��ʂ� 40% ���x�P�x�𗎂Ƃ��Ď��g�̔w�i�C���[�W�Ƃ��ė��p���܂��B
	//!	@param[in]	hWnd			�摜�C���[�W�E�C���h�E
	//!	@param[out]	imgTransparent	�߂�l �쐬�����摜�f�[�^
	//!	@param[in]	rate			�P�x���x�� 1 �ȏ�ɂ���Ɩ��邢�B1�����ɂ���ƈÂ��Ȃ�B
	//!	@retval	ERROR_SUCCESS		����
	//!	@retval	ERROR_SUCCESS�ȊO	���s
	enum FILTER_ID{
		BRIGHTNESS,
		CONTRAST,
		GRAYSCALE,
		SEPIA,
		GAMMA,
	};
	//!	@brief	�w�i�Ɏw�肵�����E�C���h�E�𓧉ߏ�������Bitmap��ԓ����܂��B
	//!	�w�肳�ꂽ��ʂ� 40% ���x�P�x�𗎂Ƃ��Ď��g�̔w�i�C���[�W�Ƃ��ė��p���܂��B
	//!	@param[in]	hWnd			�摜�C���[�W�E�C���h�E
	//!	@param[out]	imgTransparent	�߂�l �쐬�����摜�f�[�^
	//!	@param[in]	rate			�P�x���x�� 1 �ȏ�ɂ���Ɩ��邢�B1�����ɂ���ƈÂ��Ȃ�B
	//!	@retval	ERROR_SUCCESS		����
	//!	@retval	ERROR_SUCCESS�ȊO	���s
	static const DWORD	CreateFilterImage(HWND hWnd, CBitmap& imgTransparent, const float rate, const FILTER_ID filterID = BRIGHTNESS);
	static const DWORD	CreateFilterImage(HWND hWnd, Bitmap*& imgTransparent, const float rate, const FILTER_ID filterID = BRIGHTNESS);
	//!	@brief	�w��t�@�C����Bitmap�f�[�^��ۑ����܂��B
	static const DWORD	SaveBitmapToFile(LPBITMAPINFOHEADER pBI, LPCWSTR pszFile);

	static const DWORD  saveMultiPageTiff(vector<Image*>& vecImages, LPCWSTR lpszFileName, const ULONG quality);
	static const DWORD  saveImage(Image* image,LPCWSTR pszType,LPCWSTR pszFileName, const ULONG quality);

};

