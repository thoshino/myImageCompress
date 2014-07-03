#include "stdafx.h"
#include "MyImageTransformer.h"

//!	@brief	GDIPlus の Image オブジェクト のMethod Save にて
//!	利用する、Encode CLSID を取得します。
//!	指定画像イメージ文字列から、CLSID を取得し、これを利用することで、
//!	任意の形式で画像イメージを保存できます。
//!	@param[in]	pszFormat	画像形式 例 image/jpeg
//!	@param[out]	Clsid		対応した画像イメージのCLSIDが返答されます。
//!	@retval		ERROR_SUCCESS		成功
//!	@retval		ERROR_SUCCESS以外	エラー発生 Win32API エラー番号が返されます。
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
	
	//キャッシュ化します。
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
//!	@brief	リソースから画像イメージをロードします。
//!	@param[in]	hModule		リソースモジュールのハンドル
//!	@param[in]	pszCategory	リソースカテゴリ
//!	@param[in]	nID			リソースID
//!	@param[out]	btDst		ロードされた画像イメージを格納する CBitmap クラス
//!	@retval		ERROR_SUCCESS	成功
//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
const DWORD	CMyImageTransformer::LoadBitmapFromJpgResource(HMODULE hModule, const UINT nID, LPCWSTR pszCategory, CBitmap& btDst)
{
	//背景用ビットマップをロードします。
	DWORD	dwReturn(ERROR_SUCCESS);
	while (true){
		//リソースにアクセスします。
		HRSRC	hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(nID), pszCategory);
		if (hRsrc == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		//リソースデータを取得します。
		HGLOBAL	hData = ::LoadResource(hModule, hRsrc);
		if (hData == NULL){
			dwReturn = ::GetLastError();
			_ASSERT(false);
			break;
		}
		//ロックしデータを取得します。
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
			// CreateStreamOnHGlobal関数の2番目の引数が!FALSEなので関数が成功したならhImageはストリームのリリース時に後始末される
			// ここでは失敗しているので明示的に削除する
			::GlobalFree(hImage);
			dwReturn = hResult;
			_ASSERT(false);
			break;
		}
		//イメージをロードします。
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
//!	@brief	指定ファイルをイメージデータとして読み込みます。
//!	@param[in]	pszFile	ファイル名
//!	@param[out]	btDst	ロードされた画像イメージを格納する CBitmap クラス
//!	@retval		ERROR_SUCCESS	成功
//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
const DWORD	CMyImageTransformer::LoadBitmapFromFile(LPCWSTR pszFile, CBitmap & btDst)
{
	//イメージをロードします。
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

//!	@brief	指定ファイルをイメージデータとして読み込みます。
//!	@param[in]	pszFile	ファイル名
//!	@param[out]	btDst	ロードされた画像イメージを格納する CBitmap クラス
//!	@retval		ERROR_SUCCESS	成功
//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
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
		//ファイルサイズを取得します。
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		//メモリを確保します。
		hImage = ::GlobalAlloc(GPTR, dwSize);
		if (NULL == hImage){
			dwReturn = ::GetLastError();
			break;
		}
		DWORD dwNumberOfBytesRead = 0;
		::ReadFile(hFile, hImage, dwSize, &dwNumberOfBytesRead, NULL);
		if (dwNumberOfBytesRead != dwSize){
			//読み込めなった。
			dwReturn = ::GetLastError();
			break;
		}
		CComPtr<IStream>	stream;
		HRESULT				hResult = ::CreateStreamOnHGlobal(hImage, true, &stream.p);
		if (stream.p == NULL){
			dwReturn = hResult;
			break;
		}
		//イメージをロードします。
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
//!	@brief	指定サイズにBitmapをサイズ変更します。
//!			元のBitmapが指定サイズ未満の場合、サイズ変更せずコピーした値を返答します。
//!			元の縦横比率は保持しつつ、サイズ変更します。
//!	@param[in]	srcBitmap		元Bitmap
//!	@param[in]	width			縮小後の幅
//!	@param[in]	height			縮小後の高
//!	@param[out]	distBitmap		新規作成したBitmapクラス
//!	@retval		ERROR_SUCCESS	成功
//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
const DWORD	CMyImageTransformer::CreateAdjustThumbnailImage(Image* srcBitmap, const int width, const int height, Image*& distBitmap)
{
	//幅、高 等々を取得します。
	if (NULL == srcBitmap){
		return ERROR_INVALID_PARAMETER;
	}
	//現在のウインドウサイズに調整するため、画像データのサイズを取得します。
	int	nImageWidht = srcBitmap->GetWidth();
	int	nImageHeigth = srcBitmap->GetHeight();
	///拡大・縮小比率を算出します。
	const int	nRateWidth = (width * 100) / nImageWidht;
	const int	nRateHeigth = (height * 100) / nImageHeigth;
	int	nRate = 100;
	//縮小倍率の計算用 小さいレートに合わせます。
	if (nRateWidth>nRateHeigth){
		nRate = nRateHeigth;
	}
	else{
		nRate = nRateWidth;
	}
	if (nRate>100){
		//元の画像データから拡大してしまう場合、元画像のまま返答します。
		distBitmap = srcBitmap->GetThumbnailImage(nImageWidht, nImageHeigth);
	}
	else{
		nImageWidht = nImageWidht*nRate / 100;
		nImageHeigth = nImageHeigth*nRate / 100;
		//サムネイルイメージを取得します。
		distBitmap = srcBitmap->GetThumbnailImage(nImageWidht, nImageHeigth);
		Status sts = srcBitmap->GetLastStatus();
		if (Ok != sts){
			//作成に失敗。
			delete distBitmap, distBitmap = NULL;
			return ConvertStatusToWin32Error(sts);
		}
	}
	return ERROR_SUCCESS;
}


// ------------------------------------------------------------------
// ビットマップ形状に合わせてリージョンを作成します。（マスクカラー値部分を切り抜きます。）
// ------------------------------------------------------------------
HRGN CMyImageTransformer::CreateRgnFromBitmap(HBITMAP hBitmap, DWORD dwTransColor)
{
	// 画面と同じデバイスコンテキストを作成する
	HDC hDC = CreateCompatibleDC(NULL);
	if (!hDC){
		return NULL;
	}
	// ビットマップを取得する
	BITMAP bm = { '\0' };
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	// 最大要素数で動的配列を作る
	LPCOLORREF pScanData = new COLORREF[bm.bmWidth];
	// リージョンハンドル
	HRGN hRgn = ::CreateRectRgn(0, 0, bm.bmWidth, bm.bmHeight);

	if (pScanData){
		// ビットマップ情報を作る
		BITMAPINFOHEADER bi = { '\0' };
		ZeroMemory(&bi, sizeof(BITMAPINFOHEADER));
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = bm.bmWidth;
		bi.biHeight = bm.bmHeight;
		bi.biPlanes = 1;
		bi.biBitCount = 32;                          // 扱うのは 32bit カラー
		bi.biCompression = BI_RGB;
		for (int y = 0; y < bm.bmHeight; y++){

			// 一行分画像データを DIB で取り出す（bmpなので下から取り出す）
			GetDIBits(hDC, hBitmap, y, 1, pScanData, (LPBITMAPINFO)&bi, DIB_RGB_COLORS);

			// 一行分の透明／非透明の変化点を調査する
			for (int x = 0; x < bm.bmWidth; x++){
				if (pScanData[x] == dwTransColor){                  // 透明色でなければ
					HRGN DiffRgn = CreateRectRgn(x, y, x + 1, y + 1);
					_ASSERT(DiffRgn != NULL);
					// 黒じゃない部分を取り除く
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
//!	@brief	指定画像のマスクカラー部分を削除したリージョンを作成し指定ウインドウへ適用します。
//!	@param[in]	hWnd			リージョンを適用するウインドウ
//!	@param[in]	hBitmap			マスクカラーが含まれたビットマップ情報
//!	@param[in]	clrMaskColor	マスクカラー
//!	@retval		ERROR_SUCCESS	成功
//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
const DWORD	CMyImageTransformer::SetWindowMask(HWND hWnd, HBITMAP hBitmap, COLORREF clrMaskColor)
{
	//透過処理するためWindowリージョンを作成します。
	DWORD		dwReturn = ERROR_SUCCESS;
	HRGN	hRgn = CMyImageTransformer::CreateRgnFromBitmap(hBitmap, clrMaskColor);
	if (NULL == hRgn){
		dwReturn = ::GetLastError();
		_ASSERT(false);
	}
	//透過処理としてリージョン設定を行います。
	if (0 == SetWindowRgn(hWnd, hRgn, TRUE)){
		dwReturn = ::GetLastError();
		_ASSERT(false);
	}
	//リージョンは不要となったので削除します。
	if (NULL != hRgn){
		::DeleteObject(hRgn);
	}
	return dwReturn;
}
//!	@brief	指定Bitmapを指定DCへ描画します。
//const DWORD	CMyImageTransformer::DrawImage(HDC hDc,HBITMAP hBitmap,const RECT& rect)
//{
//	//指定Bitmapを描画します。
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
//!	@brief	指定Bitmapを指定DCへ描画します。
const DWORD	CMyImageTransformer::DrawImage(HDC hDc, Image* pBitmap, const RECT& rect, const float alpha)
{
	if (NULL == pBitmap || NULL == hDc){
		//処理なし。
		return ERROR_SUCCESS;
	}
	//描画用のグラフィックオブジェクトを作成します。
	Graphics	graphics(hDc);
	Status	status = graphics.GetLastStatus();
	if (Ok != status){
		_ASSERT(false);
		return ERROR_INVALID_PARAMETER;
	}
	return DrawImage(graphics, pBitmap, rect, alpha);
}
//!	@brief	指定Bitmapを指定 Graphics へ描画します。
const DWORD	CMyImageTransformer::DrawImage(Graphics& graphics, Image* pBitmap, const RECT& rect, const float alpha)
{
	//画像を描画します。
	Rect	destRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	Status	status;
	if (1.0f>alpha){
		//alpha 指定があるため、透過処理描画を行います。
		ColorMatrix	colorMatrix = { '\0' };
		//透過処理用の alpha 値を設定します。
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
		//通常描画
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
//!	@brief	指定Bitmapを指定 Graphics へ描画します。
//const DWORD	CMyImageTransformer::DrawImage(Graphics& graphics,Image* pBitmap,const int nLeft,const int nTop)
//{
//	//画像を描画します。
//	Status	status	=status=graphics.DrawImage(pBitmap,nLeft,nTop);
//	if(Ok!=status){
//		_ASSERT(false);
//		return ERROR_INVALID_PARAMETER;
//	}
//
//	return ERROR_SUCCESS;
//}
//!	@brief	指定Bitmapを指定DCへ描画します。
//const DWORD	CMyImageTransformer::DrawImage(HDC hDc,Image* pBitmap,const int nLeft,const int nTop)
//{
//	if(NULL==pBitmap){
//		//処理なし。
//		return ERROR_SUCCESS;
//	}
//	//描画用のグラフィックオブジェクトを作成します。
//	Graphics	graphics(hDc);
//	Status	status	=	graphics.GetLastStatus();
//	if(Ok!=status){
//		_ASSERT(false);
//		return ERROR_INVALID_PARAMETER;
//	}
//	return DrawImage(graphics,pBitmap,nLeft,nTop);
//}
//----------------------------------------------------------------
// 関数名　　CreateImageMask
// 説明　　　イメージとマスクを作成する
// 引数    　(IN)HDC hDC            デバイスコンテキスト    
//    　(IN)HBITMAP hSrcBitmapDC        元のhBmp
//    　(IN)COLORREF crTransParentPixel    透明色にする色
// 戻値    　TRUE:成功    FALSE:失敗
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
//!	@brief	背景に指定したいウインドウを透過処理したBitmapを返答します。
//!	指定された画面を 40% 程度輝度を落として自身の背景イメージとして利用します。
//!	@param[in]	hWnd			画像イメージウインドウ
//!	@param[out]	imgTransparent	戻り値 作成した画像データ
//!	@param[in]	rate			輝度レベル 1 以上にすると明るい。1未満にすると暗くなる。
//!	@retval	ERROR_SUCCESS		成功
//!	@retval	ERROR_SUCCESS以外	失敗
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

//!	@brief	背景に指定したいウインドウを透過処理したBitmapを返答します。
//!	指定された画面を 40% 程度輝度を落として自身の背景イメージとして利用します。
//!	@param[in]	hWnd			画像イメージウインドウ
//!	@param[out]	imgTransparent	戻り値 作成した画像データ
//!	@param[in]	rate			輝度レベル 1 以上にすると明るい。1未満にすると暗くなる。
//!	@retval	ERROR_SUCCESS		成功
//!	@retval	ERROR_SUCCESS以外	失敗
const DWORD	CMyImageTransformer::CreateFilterImage(HWND hWnd, CBitmap& imgTransparent, const float rate, const FILTER_ID filterID)
{
	if (imgTransparent.GetSafeHandle() != NULL){
		imgTransparent.DeleteObject();
	}

	RECT	rect = { '\0' };
	//領域を取得します。
	::GetWindowRect(hWnd, &rect);

	BITMAPINFO	bitmapInfo = { '\0' };
	LPBYTE		pBitmapData = NULL;

	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = rect.right - rect.left;
	bitmapInfo.bmiHeader.biHeight = rect.bottom - rect.top;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;		//24 bit
	bitmapInfo.bmiHeader.biCompression = BI_RGB;	//非圧縮
	CClientDC	cdc(NULL);
	HBITMAP	hBitmap = CreateDIBSection(cdc, &bitmapInfo, DIB_RGB_COLORS, (VOID**)&pBitmapData, NULL, 0);
	if (hBitmap == NULL){
		//エラー
		DWORD	dwError = ::GetLastError();
		_ASSERT(false);
		return dwError;
	}

	CDC			memDc;
	memDc.CreateCompatibleDC(&cdc);

	//作成したBitmapを割り当てます。
	HBITMAP		hOldBitmap = (HBITMAP)memDc.SelectObject(imgTransparent);
	//指定された画面をBitmap上に書き込みます。
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
	//!	明度を rate レベルに下げる
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
			//輝度調整
			switch (filterID){
			case BRIGHTNESS:
				blue = (const WORD)(b*rate);
				green = (const WORD)(g*rate);
				red = (const WORD)(r*rate);
				break;
			case CONTRAST:
				//コントラスト
				blue = (b - 128)*rate + 128;
				green = (g - 128)*rate + 128;
				red = (r - 128)*rate + 128;
				break;
			case GRAYSCALE:
				//グレースケール
				blue = (b * 114) / 1000;
				green = (g * 587) / 1000;
				red = (r * 299) / 1000;
				blue = green = red = (blue + green + red)*rate;
				break;
			case SEPIA:
				//セピア
				//最初にグレースケール
				blue = (b * 114) / 1000;
				green = (g * 587) / 1000;
				red = (r * 299) / 1000;
				blue = green = red = blue + green + red;
				//そしてセピア設定
				blue = (blue * 145) / 255;
				green = (green * 200) / 255;
				red = (red * 240) / 255;
				break;
			case GAMMA:
				//ガンマ補正（上手くいかない・・）
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

//!	@brief	指定ファイルにBitmapデータを保存します。
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

	//役者がそろったので、データ保存を開始します。
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
			//１ページ目
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
