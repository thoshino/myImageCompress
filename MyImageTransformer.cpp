#include "stdafx.h"
#include "MyImageTransformer.h"

//!	@brief	GDIPlus �� Image �I�u�W�F�N�g ��Method Save �ɂ�
//!	���p����AEncode CLSID ���擾���܂��B
//!	�w��摜�C���[�W�����񂩂�ACLSID ���擾���A����𗘗p���邱�ƂŁA
//!	�C�ӂ̌`���ŉ摜�C���[�W��ۑ��ł��܂��B
//!	@param[in]	pszFormat	�摜�`�� �� image/jpeg
//!	@param[out]	Clsid		�Ή������摜�C���[�W��CLSID���ԓ�����܂��B
//!	@retval		ERROR_SUCCESS		����
//!	@retval		ERROR_SUCCESS�ȊO	�G���[���� Win32API �G���[�ԍ����Ԃ���܂��B
const DWORD	CMyImageTransformer::GetEncoderClsid(LPCWSTR pszFormat, CLSID& Clsid)
{
	if (NULL == pszFormat){
		_ASSERT(false);
		return ERROR_INVALID_PARAMETER;
	}
	if (::wcscmp(pszFormat, OLESTR("image/jpg")) == 0){
		pszFormat = OLESTR("image/jpeg");
	}

	UINT	nNum = 0;
	UINT	nSize = 0;
	Status	status;
	DWORD	dwReturn = ERROR_FILE_NOT_FOUND;

	static map<CString, CLSID>	mapCache;

	map<CString, CLSID>::iterator	elm;
	
	//�L���b�V�������܂��B
	elm=mapCache.find(pszFormat);
	if (elm != mapCache.end()){
		Clsid = elm->second;
		return ERROR_SUCCESS;
	}

	status = ::GetImageEncodersSize(&nNum, &nSize);
	ImageCodecInfo*	icInfo = reinterpret_cast<ImageCodecInfo*>(new BYTE[nSize]);
	if (NULL == icInfo){
		_ASSERT(false);
		return ERROR_OUTOFMEMORY;
	}
	do{
		status = ::GetImageEncoders(nNum, nSize, icInfo);
		if (status != Ok){
			_ASSERT(false);
			dwReturn = status;
			break;
		}

		for (UINT nDx = 0; nDx < nNum; nDx++) {
			if (::wcscmp(icInfo[nDx].MimeType, pszFormat) == 0) {
				Clsid = icInfo[nDx].Clsid;
				mapCache.insert(pair<CString, CLSID>(pszFormat, icInfo[nDx].Clsid));
				dwReturn = ERROR_SUCCESS;
				break;
			}
		}
	} while (false);

	delete[] icInfo;

	return dwReturn;
}
//!	@brief	���\�[�X����摜�C���[�W�����[�h���܂��B
//!	@param[in]	hModule		���\�[�X���W���[���̃n���h��
//!	@param[in]	pszCategory	���\�[�X�J�e�S��
//!	@param[in]	nID			���\�[�XID
//!	@param[out]	btDst		���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
//!	@retval		ERROR_SUCCESS	����
//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
const DWORD	CMyImageTransformer::LoadBitmapFromJpgResource(HMODULE hModule, const UINT nID, LPCWSTR pszCategory, CBitmap& btDst)
{
	//�w�i�p�r�b�g�}�b�v�����[�h���܂��B
	DWORD	dwReturn(ERROR_SUCCESS);
	while (true){
		//���\�[�X�ɃA�N�Z�X���܂��B
		HRSRC	hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(nID), pszCategory);
		if (hRsrc == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		//���\�[�X�f�[�^���擾���܂��B
		HGLOBAL	hData = ::LoadResource(hModule, hRsrc);
		if (hData == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		//���b�N���f�[�^���擾���܂��B
		void*		lpData = ::LockResource(hData);
		if (lpData == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		DWORD				dwSize = ::SizeofResource(hModule, hRsrc);
		HGLOBAL			hImage = ::GlobalAlloc(GPTR, dwSize);
		if (hImage == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		::memmove(hImage, lpData, dwSize);
		CComPtr<IStream>	stream;
		HRESULT			hResult = ::CreateStreamOnHGlobal(hImage, true, &stream.p);
		if (stream.p == NULL){
			// CreateStreamOnHGlobal�֐���2�Ԗڂ̈�����!FALSE�Ȃ̂Ŋ֐������������Ȃ�hImage�̓X�g���[���̃����[�X���Ɍ�n�������
			// �����ł͎��s���Ă���̂Ŗ����I�ɍ폜����
			::GlobalFree(hImage);
			dwReturn = hResult;
			_ASSERT(false);
			break;
		}
		//�C���[�W�����[�h���܂��B
		Bitmap* image = new Bitmap(stream);
		HBITMAP hbmp = NULL;
		Color cl;
		image->GetPixel(0, 0, &cl);
		Status sts = image->GetHBITMAP(cl, &hbmp);
		if (Ok != sts){
			_ASSERT(false);
		}

		btDst.Attach(hbmp);
		delete image;

		break;
	}
	return dwReturn;
}
//!	@brief	�w��t�@�C�����C���[�W�f�[�^�Ƃ��ēǂݍ��݂܂��B
//!	@param[in]	pszFile	�t�@�C����
//!	@param[out]	btDst	���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
//!	@retval		ERROR_SUCCESS	����
//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
const DWORD	CMyImageTransformer::LoadBitmapFromFile(LPCWSTR pszFile, CBitmap & btDst)
{
	//�C���[�W�����[�h���܂��B
	Bitmap* image = new Bitmap(pszFile);
	Status sts = image->GetLastStatus();
	if (Ok != sts){
		delete image, image = NULL;
		return ERROR_INVALID_PARAMETER;
	}
	HBITMAP hbmp = NULL;
	Color cl;
	image->GetPixel(0, 0, &cl);
	sts = image->GetHBITMAP(cl, &hbmp);
	if (Ok != sts){
		_ASSERT(false);
	}
	btDst.Attach(hbmp);
	delete image;

	return sts;
}
const DWORD CMyImageTransformer::ConvertStatusToWin32Error(const Status status)
{
	typedef struct StatusToWin32Error
	{
		Status status;
		DWORD  win32error;
	} StatusToWin32Error;
	static const StatusToWin32Error	statusToWin32Error[] ={
	 { Ok, ERROR_SUCCESS },
	 { GenericError, ERROR_GENERIC_COMMAND_FAILED },
	 { InvalidParameter, ERROR_INVALID_PARAMETER },
	 { OutOfMemory, ERROR_OUTOFMEMORY },
	 { ObjectBusy, ERROR_PATH_BUSY },
	 { InsufficientBuffer, ERROR_INSUFFICIENT_BUFFER },
	 { NotImplemented, ERROR_CALL_NOT_IMPLEMENTED },
	 { Win32Error, WINCODEC_ERR_WIN32ERROR },
	 { WrongState, XACT_E_WRONGSTATE },
	 { Aborted, ERROR_OPERATION_ABORTED },
	 { FileNotFound, ERROR_FILE_NOT_FOUND },
	 { ValueOverflow, ERROR_BUFFER_OVERFLOW },
	 { AccessDenied, ERROR_ACCESS_DENIED },
	 { UnknownImageFormat, WINCODEC_ERR_UNKNOWNIMAGEFORMAT },
	 { FontFamilyNotFound, DWRITE_E_NOFONT },
	 { FontStyleNotFound, DWRITE_E_FILENOTFOUND },
//	 { NotTrueTypeFont, DWRITE_E_UNSUPPORTEDOPERATION },
	 { UnsupportedGdiplusVersion, ERROR_UNSUPPORTED_TYPE },
	 { GdiplusNotInitialized, PEERDIST_ERROR_NOT_INITIALIZED },
	 { PropertyNotFound, ERROR_UNKNOWN_PROPERTY },
	 { PropertyNotSupported, DNS_ERROR_INVALID_PROPERTY },
	};

	size_t n;
	for (n = 0; _countof(statusToWin32Error); n++){
		if (status == statusToWin32Error[n].status){
			return statusToWin32Error[n].win32error;
		}
	}
	return ERROR_UNKNOWN_FEATURE;

}

//!	@brief	�w��t�@�C�����C���[�W�f�[�^�Ƃ��ēǂݍ��݂܂��B
//!	@param[in]	pszFile	�t�@�C����
//!	@param[out]	btDst	���[�h���ꂽ�摜�C���[�W���i�[���� CBitmap �N���X
//!	@retval		ERROR_SUCCESS	����
//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
const DWORD	CMyImageTransformer::LoadBitmapFromFile(LPCWSTR pszFile, Bitmap*& pBitmap)
{
	if (NULL == pszFile || ::wcslen(pszFile)<1){
		return ERROR_INVALID_PARAMETER;
	}
	DWORD dwReturn(ERROR_SUCCESS);
	HANDLE	hFile=::CreateFile(pszFile, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile){
		dwReturn = ::GetLastError();
		return dwReturn;
	}
	HGLOBAL	hImage = NULL;
	do{
		//�t�@�C���T�C�Y���擾���܂��B
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		//���������m�ۂ��܂��B
		hImage = ::GlobalAlloc(GPTR, dwSize);
		if (NULL == hImage){
			dwReturn = ::GetLastError();
			break;
		}
		DWORD dwNumberOfBytesRead = 0;
		::ReadFile(hFile, hImage, dwSize, &dwNumberOfBytesRead, NULL);
		if (dwNumberOfBytesRead != dwSize){
			//�ǂݍ��߂Ȃ����B
			dwReturn = ::GetLastError();
			break;
		}
		CComPtr<IStream>	stream;
		HRESULT				hResult = ::CreateStreamOnHGlobal(hImage, true, &stream.p);
		if (stream.p == NULL){
			dwReturn = hResult;
			break;
		}
		//�C���[�W�����[�h���܂��B
		pBitmap = Bitmap::FromStream(stream);
		if (NULL == pBitmap){
			dwReturn=ERROR_OUTOFMEMORY;
			break;
		}
		Status sts = pBitmap->GetLastStatus();
		if (Ok != sts){
			delete pBitmap, pBitmap = NULL;
			dwReturn = ConvertStatusToWin32Error(sts);
			break;
		}
	} while (false);

	if (hFile != INVALID_HANDLE_VALUE){
		::CloseHandle(hFile);
	}

	return dwReturn;
}
//!	@brief	�w��T�C�Y��Bitmap���T�C�Y�ύX���܂��B
//!			����Bitmap���w��T�C�Y�����̏ꍇ�A�T�C�Y�ύX�����R�s�[�����l��ԓ����܂��B
//!			���̏c���䗦�͕ێ����A�T�C�Y�ύX���܂��B
//!	@param[in]	srcBitmap		��Bitmap
//!	@param[in]	width			�k����̕�
//!	@param[in]	height			�k����̍�
//!	@param[out]	distBitmap		�V�K�쐬����Bitmap�N���X
//!	@retval		ERROR_SUCCESS	����
//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
const DWORD	CMyImageTransformer::CreateAdjustThumbnailImage(Image* srcBitmap, const int width, const int height, Image*& distBitmap)
{
	//���A�� ���X���擾���܂��B
	if (NULL == srcBitmap){
		return ERROR_INVALID_PARAMETER;
	}
	//���݂̃E�C���h�E�T�C�Y�ɒ������邽�߁A�摜�f�[�^�̃T�C�Y���擾���܂��B
	int	nImageWidht = srcBitmap->GetWidth();
	int	nImageHeigth = srcBitmap->GetHeight();
	///�g��E�k���䗦���Z�o���܂��B
	const int	nRateWidth = (width * 100) / nImageWidht;
	const int	nRateHeigth = (height * 100) / nImageHeigth;
	int	nRate = 100;
	//�k���{���̌v�Z�p ���������[�g�ɍ��킹�܂��B
	if (nRateWidth>nRateHeigth){
		nRate = nRateHeigth;
	}
	else{
		nRate = nRateWidth;
	}
	if (nRate>100){
		//���̉摜�f�[�^����g�債�Ă��܂��ꍇ�A���摜�̂܂ܕԓ����܂��B
		distBitmap = srcBitmap->GetThumbnailImage(nImageWidht, nImageHeigth);
	}
	else{
		nImageWidht = nImageWidht*nRate / 100;
		nImageHeigth = nImageHeigth*nRate / 100;
		//�T���l�C���C���[�W���擾���܂��B
		distBitmap = srcBitmap->GetThumbnailImage(nImageWidht, nImageHeigth);
		Status sts = srcBitmap->GetLastStatus();
		if (Ok != sts){
			//�쐬�Ɏ��s�B
			delete distBitmap, distBitmap = NULL;
			return ConvertStatusToWin32Error(sts);
		}
	}
	return ERROR_SUCCESS;
}


// ------------------------------------------------------------------
// �r�b�g�}�b�v�`��ɍ��킹�ă��[�W�������쐬���܂��B�i�}�X�N�J���[�l������؂蔲���܂��B�j
// ------------------------------------------------------------------
HRGN CMyImageTransformer::CreateRgnFromBitmap(HBITMAP hBitmap, DWORD dwTransColor)
{
	// ��ʂƓ����f�o�C�X�R���e�L�X�g���쐬����
	HDC hDC = CreateCompatibleDC(NULL);
	if (!hDC){
		return NULL;
	}
	// �r�b�g�}�b�v���擾����
	BITMAP bm = { '\0' };
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	// �ő�v�f���œ��I�z������
	LPCOLORREF pScanData = new COLORREF[bm.bmWidth];
	// ���[�W�����n���h��
	HRGN hRgn = ::CreateRectRgn(0, 0, bm.bmWidth, bm.bmHeight);

	if (pScanData){
		// �r�b�g�}�b�v�������
		BITMAPINFOHEADER bi = { '\0' };
		ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bm.bmWidth;
		bi.biHeight = bm.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 32;                          // �����̂� 32bit �J���[
		bi.biCompression = BI_RGB;
		for (int y = 0; y < bm.bmHeight; y++){

			// ��s���摜�f�[�^�� DIB �Ŏ��o���ibmp�Ȃ̂ŉ�������o���j
			GetDIBits(hDC, hBitmap, y, 1, pScanData, (LPBITMAPINFO)&bi, DIB_RGB_COLORS);

			// ��s���̓����^�񓧖��̕ω��_�𒲍�����
			for (int x = 0; x < bm.bmWidth; x++){
				if (pScanData[x] == dwTransColor){                  // �����F�łȂ����
					HRGN DiffRgn = CreateRectRgn(x, y, x + 1, y + 1);
					_ASSERT(DiffRgn != NULL);
					// ������Ȃ���������菜��
					if (ERROR == CombineRgn(hRgn, hRgn, DiffRgn, RGN_DIFF)){
						_ASSERT(false);
					}
					if (NULL != DiffRgn){
						::DeleteObject(DiffRgn);
					}
				}
			}
		}
	}
	delete[] pScanData;  //
	DeleteDC(hDC);

	return hRgn;
}
//!	@brief	�w��摜�̃}�X�N�J���[�������폜�������[�W�������쐬���w��E�C���h�E�֓K�p���܂��B
//!	@param[in]	hWnd			���[�W������K�p����E�C���h�E
//!	@param[in]	hBitmap			�}�X�N�J���[���܂܂ꂽ�r�b�g�}�b�v���
//!	@param[in]	clrMaskColor	�}�X�N�J���[
//!	@retval		ERROR_SUCCESS	����
//!	@retval		ERROR_SUCCESS�ȊO �G���[ Win32API �G���[�ԍ�
const DWORD	CMyImageTransformer::SetWindowMask(HWND hWnd, HBITMAP hBitmap, COLORREF clrMaskColor)
{
	//���ߏ������邽��Window���[�W�������쐬���܂��B
	DWORD		dwReturn = ERROR_SUCCESS;
	HRGN	hRgn = CMyImageTransformer::CreateRgnFromBitmap(hBitmap, clrMaskColor);
	if (NULL == hRgn){
		dwReturn = ::GetLastError();
		_ASSERT(false);
	}
	//���ߏ����Ƃ��ă��[�W�����ݒ���s���܂��B
	if (0 == SetWindowRgn(hWnd, hRgn, TRUE)){
		dwReturn = ::GetLastError();
		_ASSERT(false);
	}
	//���[�W�����͕s�v�ƂȂ����̂ō폜���܂��B
	if (NULL != hRgn){
		::DeleteObject(hRgn);
	}
	return dwReturn;
}
//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
//const DWORD	CMyImageTransformer::DrawImage(HDC hDc,HBITMAP hBitmap,const RECT& rect)
//{
//	//�w��Bitmap��`�悵�܂��B
//	CDCHandle		cdc(hDc);
//	CBitmapHandle	img(hBitmap);
//	BITMAP			bm={'\0'} ;
//	CDC				cdcMem;
//
//	img.GetBitmap(bm);
//	cdcMem.CreateCompatibleDC(cdc);
//	HBITMAP	hOldBitmap	=	cdcMem.SelectBitmap(img);
//	//cdc.BitBlt(rect.left
//	//			,rect.top
//	//			,rect.right-rect.left
//	//			,rect.bottom-rect.top
//	//			,cdcMem,0,0,SRCCOPY);
//	cdc.StretchBlt(	rect.left
//					,rect.top
//					,rect.right-rect.left
//					,rect.bottom-rect.top
//					,cdcMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY|CAPTUREBLT);
//	cdcMem.SelectBitmap(hOldBitmap);
//
//	return ERROR_SUCCESS;
//}
//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
const DWORD	CMyImageTransformer::DrawImage(HDC hDc, Image* pBitmap, const RECT& rect, const float alpha)
{
	if (NULL == pBitmap || NULL == hDc){
		//�����Ȃ��B
		return ERROR_SUCCESS;
	}
	//�`��p�̃O���t�B�b�N�I�u�W�F�N�g���쐬���܂��B
	Graphics	graphics(hDc);
	Status	status = graphics.GetLastStatus();
	if (Ok != status){
		_ASSERT(false);
		return ERROR_INVALID_PARAMETER;
	}
	return DrawImage(graphics, pBitmap, rect, alpha);
}
//!	@brief	�w��Bitmap���w�� Graphics �֕`�悵�܂��B
const DWORD	CMyImageTransformer::DrawImage(Graphics& graphics, Image* pBitmap, const RECT& rect, const float alpha)
{
	//�摜��`�悵�܂��B
	Rect	destRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	Status	status;
	if (1.0f>alpha){
		//alpha �w�肪���邽�߁A���ߏ����`����s���܂��B
		ColorMatrix	colorMatrix = { '\0' };
		//���ߏ����p�� alpha �l��ݒ肵�܂��B
		colorMatrix.m[0][0] = 1;
		colorMatrix.m[1][1] = 1;
		colorMatrix.m[2][2] = 1;
		colorMatrix.m[3][3] = alpha;
		colorMatrix.m[4][4] = 1;
		ImageAttributes	ia;
		ia.SetColorMatrix(&colorMatrix);

		status = status = graphics.DrawImage(pBitmap
			, destRect
			, 0, 0
			, static_cast<const INT>(pBitmap->GetWidth())
			, static_cast<const INT>(pBitmap->GetHeight())
			, UnitPixel
			, &ia
			);
	}
	else{
		//�ʏ�`��
		status = status = graphics.DrawImage(pBitmap
			, destRect
			);
	}
	if (Ok != status){
		_ASSERT(false);
		return ERROR_INVALID_PARAMETER;
	}

	return ERROR_SUCCESS;
}
//!	@brief	�w��Bitmap���w�� Graphics �֕`�悵�܂��B
//const DWORD	CMyImageTransformer::DrawImage(Graphics& graphics,Image* pBitmap,const int nLeft,const int nTop)
//{
//	//�摜��`�悵�܂��B
//	Status	status	=status=graphics.DrawImage(pBitmap,nLeft,nTop);
//	if(Ok!=status){
//		_ASSERT(false);
//		return ERROR_INVALID_PARAMETER;
//	}
//
//	return ERROR_SUCCESS;
//}
//!	@brief	�w��Bitmap���w��DC�֕`�悵�܂��B
//const DWORD	CMyImageTransformer::DrawImage(HDC hDc,Image* pBitmap,const int nLeft,const int nTop)
//{
//	if(NULL==pBitmap){
//		//�����Ȃ��B
//		return ERROR_SUCCESS;
//	}
//	//�`��p�̃O���t�B�b�N�I�u�W�F�N�g���쐬���܂��B
//	Graphics	graphics(hDc);
//	Status	status	=	graphics.GetLastStatus();
//	if(Ok!=status){
//		_ASSERT(false);
//		return ERROR_INVALID_PARAMETER;
//	}
//	return DrawImage(graphics,pBitmap,nLeft,nTop);
//}
//----------------------------------------------------------------
// �֐����@�@CreateImageMask
// �����@�@�@�C���[�W�ƃ}�X�N���쐬����
// ����    �@(IN)HDC hDC            �f�o�C�X�R���e�L�X�g    
//    �@(IN)HBITMAP hSrcBitmapDC        ����hBmp
//    �@(IN)COLORREF crTransParentPixel    �����F�ɂ���F
// �ߒl    �@TRUE:����    FALSE:���s
HBITMAP	CMyImageTransformer::CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	// Create monochrome (1 bit) mask bitmap.  

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Get some HDCs that are compatible with the display driver

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	// Set the background colour of the colour image to the colour
	// you want to be transparent.
	SetBkColor(hdcMem, crTransparent);

	// Copy the bits from the colour image to the B+W mask... everything
	// with the background colour ends up white while everythig else ends up
	// black...Just what we wanted.
	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	// Take our new mask and use it to turn the transparent colour in our
	// original colour image to black so the transparency effect will
	// work right.
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	// Clean up.
	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}
//!	@brief	�w�i�Ɏw�肵�����E�C���h�E�𓧉ߏ�������Bitmap��ԓ����܂��B
//!	�w�肳�ꂽ��ʂ� 40% ���x�P�x�𗎂Ƃ��Ď��g�̔w�i�C���[�W�Ƃ��ė��p���܂��B
//!	@param[in]	hWnd			�摜�C���[�W�E�C���h�E
//!	@param[out]	imgTransparent	�߂�l �쐬�����摜�f�[�^
//!	@param[in]	rate			�P�x���x�� 1 �ȏ�ɂ���Ɩ��邢�B1�����ɂ���ƈÂ��Ȃ�B
//!	@retval	ERROR_SUCCESS		����
//!	@retval	ERROR_SUCCESS�ȊO	���s
const DWORD	CMyImageTransformer::CreateFilterImage(HWND hWnd, Bitmap*& imgTransparent, const float rate, const FILTER_ID filterID)
{
	CBitmap	bitmap;

	CreateFilterImage(hWnd, bitmap, rate, filterID);

	if (imgTransparent != NULL){
		delete imgTransparent, imgTransparent = NULL;
	}
	imgTransparent = new Bitmap((HBITMAP)bitmap.GetSafeHandle(), NULL);

	return ERROR_SUCCESS;
}

//!	@brief	�w�i�Ɏw�肵�����E�C���h�E�𓧉ߏ�������Bitmap��ԓ����܂��B
//!	�w�肳�ꂽ��ʂ� 40% ���x�P�x�𗎂Ƃ��Ď��g�̔w�i�C���[�W�Ƃ��ė��p���܂��B
//!	@param[in]	hWnd			�摜�C���[�W�E�C���h�E
//!	@param[out]	imgTransparent	�߂�l �쐬�����摜�f�[�^
//!	@param[in]	rate			�P�x���x�� 1 �ȏ�ɂ���Ɩ��邢�B1�����ɂ���ƈÂ��Ȃ�B
//!	@retval	ERROR_SUCCESS		����
//!	@retval	ERROR_SUCCESS�ȊO	���s
const DWORD	CMyImageTransformer::CreateFilterImage(HWND hWnd, CBitmap& imgTransparent, const float rate, const FILTER_ID filterID)
{
	if (imgTransparent.GetSafeHandle() != NULL){
		imgTransparent.DeleteObject();
	}

	RECT	rect = { '\0' };
	//�̈���擾���܂��B
	::GetWindowRect(hWnd, &rect);

	BITMAPINFO	bitmapInfo = { '\0' };
	LPBYTE		pBitmapData = NULL;

	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = rect.right - rect.left;
	bitmapInfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;		//24 bit
	bitmapInfo.bmiHeader.biCompression = BI_RGB;	//�񈳏k
	CClientDC	cdc(NULL);
	HBITMAP	hBitmap = CreateDIBSection(cdc, &bitmapInfo, DIB_RGB_COLORS, (VOID**)&pBitmapData, NULL, 0);
	if (hBitmap == NULL){
		//�G���[
		DWORD	dwError = ::GetLastError();
		_ASSERT(false);
		return dwError;
	}

	CDC			memDc;
	memDc.CreateCompatibleDC(&cdc);

	//�쐬����Bitmap�����蓖�Ă܂��B
	HBITMAP		hOldBitmap = (HBITMAP)memDc.SelectObject(imgTransparent);
	//�w�肳�ꂽ��ʂ�Bitmap��ɏ������݂܂��B
	if (0 == ::PrintWindow(hWnd, memDc, 0)){
		DWORD	dwError = ::GetLastError();
		_ASSERT(false);
		return dwError;
	}
	memDc.SelectObject(hOldBitmap);

	WORD	r, g, b;
	WORD	red, green, blue;
	int		nPosRGB;
	int		nPosGray;
	WORD	nMax = 255;
	int		gamma = 2;
	red = green = blue = 0;
	//!	���x�� rate ���x���ɉ�����
	for (LONG dy = 0; dy<bitmapInfo.bmiHeader.biHeight; dy++){
		for (LONG dx = 0; dx<bitmapInfo.bmiHeader.biWidth; dx++){
			nPosGray = (dy*bitmapInfo.bmiHeader.biWidth + dx);
			nPosRGB = nPosGray * 3;
			////pBitmapData[dx+dy*bitmapInfo.bmiHeader.biWidth]=0;
			////continue;
			b = pBitmapData[nPosRGB];
			g = pBitmapData[nPosRGB + 1];
			r = pBitmapData[nPosRGB + 2];
#pragma warning(disable:4244)
			//�P�x����
			switch (filterID){
			case BRIGHTNESS:
				blue = (const WORD)(b*rate);
				green = (const WORD)(g*rate);
				red = (const WORD)(r*rate);
				break;
			case CONTRAST:
				//�R���g���X�g
				blue = (b - 128)*rate + 128;
				green = (g - 128)*rate + 128;
				red = (r - 128)*rate + 128;
				break;
			case GRAYSCALE:
				//�O���[�X�P�[��
				blue = (b * 114) / 1000;
				green = (g * 587) / 1000;
				red = (r * 299) / 1000;
				blue = green = red = (blue + green + red)*rate;
				break;
			case SEPIA:
				//�Z�s�A
				//�ŏ��ɃO���[�X�P�[��
				blue = (b * 114) / 1000;
				green = (g * 587) / 1000;
				red = (r * 299) / 1000;
				blue = green = red = blue + green + red;
				//�����ăZ�s�A�ݒ�
				blue = (blue * 145) / 255;
				green = (green * 200) / 255;
				red = (red * 240) / 255;
				break;
			case GAMMA:
				//�K���}�␳�i��肭�����Ȃ��E�E�j
				blue = (((b * 100 / 255) ^ (1 / gamma)) * 255);
				green = (((g * 100 / 255) ^ (1 / gamma)) * 255);
				red = (((r * 100 / 255) ^ (1 / gamma)) * 255);
				break;
			}

#pragma warning( push ) 
			blue = blue>nMax ? nMax : blue;
			green = green>nMax ? nMax : green;
			red = red>nMax ? nMax : red;
			pBitmapData[nPosRGB] = blue;
			pBitmapData[nPosRGB + 1] = green;
			pBitmapData[nPosRGB + 2] = red;
#pragma warning( pop ) 
		}
	}

	return ERROR_SUCCESS;
}

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

//!	@brief	�w��t�@�C����Bitmap�f�[�^��ۑ����܂��B
//!	@param[in]	pBI	-	Pointer to DIB info structure
const DWORD	CMyImageTransformer::SaveBitmapToFile(LPBITMAPINFOHEADER pBI, LPCWSTR pszFilePath)
{
	// Header for Bitmap file
	BITMAPFILEHEADER	bmfHdr = { '\0' };
	DWORD				dwDIBSize = 0;

	if (pBI == NULL)
		return ERROR_INVALID_PARAMETER;

	if (sizeof(BITMAPINFOHEADER) != pBI->biSize){
		// It's an other-style DIB (save not supported)
		return ERROR_INVALID_DATA;
	}

	//Fill in the fields of the file header

	//Fill in file type (first 2 bytes must be "BM" for a bitmap) 
	bmfHdr.bfType = ((WORD)('M' << 8) | 'B');  // "BM"

	// Calculating the size of the DIB is a bit tricky (if we want to
	// do it right).  The easiest way to do this is to call GlobalSize()
	// on our global handle, but since the size of our global memory may have
	// been padded a few bytes, we may end up writing out a few too
	// many bytes to the file (which may cause problems with some apps).
	//
	// So, instead let's calculate the size manually (if we can)
	//
	// First, find size of header plus size of color table.  Since the
	// first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
	// the size of the structure, let's use this.
	// Partial Calculation
	DWORD	dibNumColors = 0;
	switch (pBI->biBitCount){
	case 1:
		dibNumColors = 2;
		break;
	case 4:
		dibNumColors = 16;
		break;
	case 8:
		dibNumColors = 256;
		break;
	}
	DWORD	dwPaletteSize(dibNumColors*sizeof(RGBQUAD));
	dwDIBSize = pBI->biSize + dwPaletteSize;

	// Now calculate the size of the image
	if ((pBI->biCompression == BI_RLE8) || (pBI->biCompression == BI_RLE4)){
		// It's an RLE bitmap, we can't calculate size, so trust the
		// biSizeImage field
		dwDIBSize += pBI->biSizeImage;
	}
	else{
		DWORD dwBmBitsSize;  // Size of Bitmap Bits only
		// It's not RLE, so size is Width (DWORD aligned) * Height

		dwBmBitsSize = WIDTHBYTES((pBI->biWidth)*((DWORD)pBI->biBitCount)) * pBI->biHeight;

		dwDIBSize += dwBmBitsSize;

		// Now, since we have calculated the correct size, why don't we
		// fill in the biSizeImage field (this will fix any .BMP files which
		// have this field incorrect).

		pBI->biSizeImage = dwBmBitsSize;
	}


	// Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)

	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	/*
	* Now, calculate the offset the actual bitmap bits will be in
	* the file -- It's the Bitmap file header plus the DIB header,
	* plus the size of the color table.
	*/
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+pBI->biSize + dwPaletteSize;

	//���҂���������̂ŁA�f�[�^�ۑ����J�n���܂��B
	DWORD		dwReturn(ERROR_SUCCESS);
	HANDLE		hFile = ::CreateFile(pszFilePath
		, GENERIC_WRITE
		, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
		, NULL
		, CREATE_ALWAYS
		, NULL
		, NULL
		);
	if (INVALID_HANDLE_VALUE == hFile){
		dwReturn = ::GetLastError();
		_ASSERT(false);
		return dwReturn;
	}
	DWORD	dwNumberOfBytesWritten = 0;
	do{
		if (0 == ::WriteFile(hFile, &bmfHdr, sizeof(BITMAPFILEHEADER), &dwNumberOfBytesWritten, NULL)){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		if (0 == ::WriteFile(hFile, pBI, dwDIBSize, &dwNumberOfBytesWritten, NULL)){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}

	} while (false);
	::CloseHandle(hFile), hFile = NULL;

	return dwReturn;
};
const DWORD  CMyImageTransformer::saveMultiPageTiff(vector<Image*>& vecImages, LPCWSTR lpszFileName, const ULONG quality)
{
	Status				stat;
	DWORD				dwReturn;

	EncoderParameters	encoderParameters[3] = { '\0' };

	ULONG				parameterValue;
	encoderParameters[0].Count = 2;
	encoderParameters[0].Parameter[0].Guid = EncoderSaveFlag;
	encoderParameters[0].Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters[0].Parameter[0].NumberOfValues = 1;
	encoderParameters[0].Parameter[0].Value = &parameterValue;

	ULONG				nCompression = EncoderValueCompressionCCITT4;
	encoderParameters[1].Count = 2;
	encoderParameters[1].Parameter[0].Guid = EncoderCompression;
	encoderParameters[1].Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters[1].Parameter[0].NumberOfValues = 1;
	encoderParameters[1].Parameter[0].Value = &nCompression;

	ULONG				nQuality = quality;
	encoderParameters[2].Count = 2;
	encoderParameters[2].Parameter[0].Guid = EncoderQuality;
	encoderParameters[2].Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters[2].Parameter[0].NumberOfValues = 1;
	encoderParameters[2].Parameter[0].Value = &nQuality;

	// Get the CLSID of the TIFF encoder.
	CLSID encoderClsid;
	dwReturn=GetEncoderClsid(TEXT("image/tiff"), encoderClsid);
	if (ERROR_SUCCESS != dwReturn){
		_ASSERT(false);
		return dwReturn;
	}

	Image*	pStartPage = NULL;
	for (size_t nDx = 0; nDx<vecImages.size(); nDx++){
		Image*	pImage = vecImages[nDx];
		if (NULL == pImage){
			_ASSERT(false);
			continue;
		}
		if (0 == nDx){
			//�P�y�[�W��
			// Save the first page (frame).
			parameterValue = EncoderValueMultiFrame;
			pStartPage = pImage->Clone();
			stat = pStartPage->Save(lpszFileName, &encoderClsid, encoderParameters);
		}
		else{
			// Save the second page (frame).
			parameterValue = EncoderValueFrameDimensionPage;
			stat = pStartPage->SaveAdd(pImage, encoderParameters);
		}
		if (stat != Ok){
			_ASSERT(false);
		}
	}
	_ASSERT(pStartPage != NULL);
	if (NULL != pStartPage){
		parameterValue = EncoderValueFlush;
		stat = pStartPage->SaveAdd(encoderParameters);
		if (stat != Ok){
			_ASSERT(false);
		}
	}

	return ERROR_SUCCESS;
}

const DWORD  CMyImageTransformer::saveImage(Image* image, LPCWSTR pszType, LPCWSTR pszFileName, const ULONG quality)
{
	Status				stat;
	DWORD				dwReturn;

	EncoderParameters	encoderParameters[1] = { '\0' };

	ULONG				nQuality = quality;
	encoderParameters[0].Count = 1;
	encoderParameters[0].Parameter[0].Guid = EncoderQuality;
	encoderParameters[0].Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters[0].Parameter[0].NumberOfValues = 1;
	encoderParameters[0].Parameter[0].Value = &nQuality;

	// Get the CLSID of the TIFF encoder.
	CLSID encoderClsid;
	dwReturn = GetEncoderClsid(pszType, encoderClsid);
	if (ERROR_SUCCESS != dwReturn){
		_ASSERT(false);
		return dwReturn;
	}

	stat = image->Save(pszFileName, &encoderClsid, encoderParameters);

	return CMyImageTransformer::ConvertStatusToWin32Error(stat);
}
