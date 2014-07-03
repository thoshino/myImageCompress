#pragma once
class CMyImageTransformer
{
public:
	static const DWORD ConvertStatusToWin32Error(const Status status);
	//!	@brief	GDIPlus の Image オブジェクト のMethod Save にて
	//!	利用する、Encode CLSID を取得します。
	//!	指定画像イメージ文字列から、CLSID を取得し、これを利用することで、
	//!	任意の形式で画像イメージを保存できます。
	//!	@param[in]	pszFormat	画像形式 例 image/jpeg
	//!	@param[out]	Clsid		対応した画像イメージのCLSIDが返答されます。
	//!	@retval		ERROR_SUCCESS		成功
	//!	@retval		ERROR_SUCCESS以外	エラー発生 Win32API エラー番号が返されます。
	static const DWORD	GetEncoderClsid(LPCWSTR pszFormat, CLSID& Clsid);
	//!	@brie	アプリケーションレベル寄りだけど、ここに記載
	//!	イメージが格納されているフォルダを取得します。
	static CString&	GetImagesFolder(void);
	//!	@brief	リソースから画像イメージをロードします。
	//!	@param[in]	hModule		リソースモジュールのハンドル
	//!	@param[in]	pszCategory	リソースカテゴリ
	//!	@param[in]	nID			リソースID
	//!	@param[out]	btDst		ロードされた画像イメージを格納する CBitmap クラス
	//!	@retval		ERROR_SUCCESS	成功
	//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
	static const DWORD	LoadBitmapFromJpgResource(HMODULE hModule, const UINT nID, LPCWSTR pszCategory, CBitmap& btDst);
	//!	@brief	指定ファイルをイメージデータとして読み込みます。
	//!	@param[in]	pszFile	ファイル名
	//!	@param[out]	btDst	ロードされた画像イメージを格納する CBitmap クラス
	//!	@retval		ERROR_SUCCESS	成功
	//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
	static const DWORD	LoadBitmapFromFile(LPCWSTR pszFile, CBitmap& btDst);
	//!	@brief	指定ファイルをイメージデータとして読み込みます。
	//!	@param[in]	pszFile	ファイル名
	//!	@param[out]	btDst	ロードされた画像イメージを格納する CBitmap クラス
	//!	@retval		ERROR_SUCCESS	成功
	//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
	static const DWORD	LoadBitmapFromFile(LPCWSTR pszFile, Bitmap*& pBitmap);
	//!	@brief	指定サイズにBitmapをサイズ変更します。
	//!			元のBitmapが指定サイズ未満の場合、サイズ変更せずコピーした値を返答します。
	//!			元の縦横比率は保持しつつ、サイズ変更します。
	//!	@param[in]	srcBitmap		元Bitmap
	//!	@param[in]	width			縮小後の幅
	//!	@param[in]	height			縮小後の高
	//!	@param[out]	distBitmap		新規作成したBitmapクラス
	//!	@retval		ERROR_SUCCESS	成功
	//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
	static const DWORD	CreateAdjustThumbnailImage(Image* srcBitmap, const int width, const int height, Image*& distBitmap);

	//!	@brief	Bitmap形状に合わせてリージョン作成を行います。
	static HRGN CreateRgnFromBitmap(HBITMAP hBitmap, DWORD dwTransColor);

	//!	@brief	指定画像のマスクカラー部分を削除したリージョンを作成し指定ウインドウへ適用します。
	//!	@param[in]	hWnd			リージョンを適用するウインドウ
	//!	@param[in]	hBitmap			マスクカラーが含まれたビットマップ情報
	//!	@param[in]	clrMaskColor	マスクカラー
	//!	@retval		ERROR_SUCCESS	成功
	//!	@retval		ERROR_SUCCESS以外 エラー Win32API エラー番号
	static const DWORD	SetWindowMask(HWND hWnd, HBITMAP hBitmap, COLORREF clrMaskColor);
	//!	@brief	指定Bitmapを指定DCへ描画します。
	//static const DWORD	DrawImage(HDC hDc,HBITMAP hBitmap,const RECT& rect);
	//!	@brief	指定Bitmapを指定DCへ描画します。
	//!	@param[in]	hDc		描画先のデバイスコンテキスト
	//!	@param[in]	pBitmap	描画したい Image クラス
	//!	@param[in]	rect	描画領域
	//!	@param[in]	alpha	描画時のα値 1 の場合無し
	static const DWORD	DrawImage(HDC hDc, Image* pBitmap, const RECT& rect, const float alpha = 1);
	//!	@brief	指定Bitmapを指定DCへ描画します。
	//static const DWORD	DrawImage(HDC hDc,Image* pBitmap,const int nLeft,const int nTop);
	//!	@brief	指定Bitmapを指定 Graphics へ描画します。
	//!	@param[in]	graphics	描画先のデバイスコンテキスト
	//!	@param[in]	pBitmap		描画したい Image クラス
	//!	@param[in]	rect		描画領域
	//!	@param[in]	alpha		描画時のα値 1 の場合無し
	static const DWORD	DrawImage(Graphics& graphics, Image* pBitmap, const RECT& rect, const float alpha = 1);
	//!	@brief	指定Bitmapを指定 Graphics へ描画します。
	//static const DWORD	DrawImage(Graphics& graphics,Image* pBitmap,const int nLeft,const int nTop);
	static HBITMAP	CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);

	//!	@brief	背景に指定したいウインドウを透過処理したBitmapを返答します。
	//!	指定された画面を 40% 程度輝度を落として自身の背景イメージとして利用します。
	//!	@param[in]	hWnd			画像イメージウインドウ
	//!	@param[out]	imgTransparent	戻り値 作成した画像データ
	//!	@param[in]	rate			輝度レベル 1 以上にすると明るい。1未満にすると暗くなる。
	//!	@retval	ERROR_SUCCESS		成功
	//!	@retval	ERROR_SUCCESS以外	失敗
	enum FILTER_ID{
		BRIGHTNESS,
		CONTRAST,
		GRAYSCALE,
		SEPIA,
		GAMMA,
	};
	//!	@brief	背景に指定したいウインドウを透過処理したBitmapを返答します。
	//!	指定された画面を 40% 程度輝度を落として自身の背景イメージとして利用します。
	//!	@param[in]	hWnd			画像イメージウインドウ
	//!	@param[out]	imgTransparent	戻り値 作成した画像データ
	//!	@param[in]	rate			輝度レベル 1 以上にすると明るい。1未満にすると暗くなる。
	//!	@retval	ERROR_SUCCESS		成功
	//!	@retval	ERROR_SUCCESS以外	失敗
	static const DWORD	CreateFilterImage(HWND hWnd, CBitmap& imgTransparent, const float rate, const FILTER_ID filterID = BRIGHTNESS);
	static const DWORD	CreateFilterImage(HWND hWnd, Bitmap*& imgTransparent, const float rate, const FILTER_ID filterID = BRIGHTNESS);
	//!	@brief	指定ファイルにBitmapデータを保存します。
	static const DWORD	SaveBitmapToFile(LPBITMAPINFOHEADER pBI, LPCWSTR pszFile);

	static const DWORD  saveMultiPageTiff(vector<Image*>& vecImages, LPCWSTR lpszFileName, const ULONG quality);
	static const DWORD  saveImage(Image* image,LPCWSTR pszType,LPCWSTR pszFileName, const ULONG quality);

};

