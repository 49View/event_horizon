#include "../../../platform_util.h"

#include <regex>
#include <chrono>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#define stat _stat

#include "../../../util.h"

#include <windows.h>

bool isalnumCC(char c) {
	return isalnum(c);
}

std::string cacheFolder() {
	const char* homeDir = getenv( "TEMP" );
	return std::string( homeDir ) + "/";
}

const std::string userComputerName() {
    const char* cname = getenv( "COMPUTERNAME" );
    const char* uname = getenv( "USERNAME" );
    auto userComputerName = std::string( uname ) + "@" + std::string( cname );
    return userComputerName;
}

const std::string userName() {
    const char *uname = getenv( "USERNAME" );
    auto userName = std::string( uname );
    return userName;
}

const std::string computerName() {
    char cname[512];
    char *cnameEnv = getenv( "COMPUTERNAME" );
    return std::string( cname );
}

const std::string getUserDownloadFolder() {
    return "/Users/" + userName() + "/Downloads";
}

bool checkFileNameNotACopy( const std::string& filename )  {
    std::vector<std::regex> fcPatterns = { std::regex("\\s\\\\\\(\\d\\\\\\)"), std::regex("\\s\\d\\.\\w") };
    for ( const auto& wb1 : fcPatterns ) {
        std::smatch base_match;
        int expectedResults = 1;
        std::regex_search( filename, base_match, wb1 );
        auto nM = base_match.size();
        if ( nM == expectedResults ) {
            return true;
        }
    }
    return false;
}

void errhandler( const char *error ) {
	throw error;
}

//PBITMAPINFO CreateBitmapInfoStruct( HBITMAP hBmp ) {
//	BITMAP bmp;
//	PBITMAPINFO pbmi;
//	WORD    cClrBits;
//
//	// Retrieve the bitmap color format, width, and height.
//	if ( !GetObject( hBmp, sizeof( BITMAP ), (LPSTR)&bmp ) )
//		errhandler( "GetObject" );
//
//	// Convert the color format to a count of bits.
//	cClrBits = (WORD)( bmp.bmPlanes * bmp.bmBitsPixel );
//	if ( cClrBits == 1 )
//		cClrBits = 1;
//	else if ( cClrBits <= 4 )
//		cClrBits = 4;
//	else if ( cClrBits <= 8 )
//		cClrBits = 8;
//	else if ( cClrBits <= 16 )
//		cClrBits = 16;
//	else if ( cClrBits <= 24 )
//		cClrBits = 24;
//	else cClrBits = 32;
//
//	// Allocate memory for the BITMAPINFO structure. (This structure  contains a BITMAPINFOHEADER structure and an array of RGBQUAD data structures.)
//	if ( cClrBits < 24 )
//		pbmi = (PBITMAPINFO)LocalAlloc( LPTR, sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * ( 1LLU << cClrBits ) );
//	else // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel
//		pbmi = (PBITMAPINFO)LocalAlloc( LPTR, sizeof( BITMAPINFOHEADER ) );
//
//	// Initialize the fields in the BITMAPINFO structure.
//	pbmi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
//	pbmi->bmiHeader.biWidth = bmp.bmWidth;
//	pbmi->bmiHeader.biHeight = bmp.bmHeight;
//	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
//	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
//
//	if ( cClrBits < 24 )
//		pbmi->bmiHeader.biClrUsed = ( 1 << cClrBits );
//
//	// If the bitmap is not compressed, set the BI_RGB flag.
//	pbmi->bmiHeader.biCompression = BI_RGB;
//
//	// Compute the number of bytes in the array of color
//	// indices and store the result in biSizeImage.
//	// The width must be DWORD aligned unless the bitmap is RLE
//	// compressed.
//	pbmi->bmiHeader.biSizeImage = ( ( pbmi->bmiHeader.biWidth * cClrBits + 31 ) & ~31 ) / 8 * pbmi->bmiHeader.biHeight;
//	// Set biClrImportant to 0, indicating that all of the
//	// device colors are important.
//	pbmi->bmiHeader.biClrImportant = 0;
//	return pbmi;
//}
//
//typedef struct {
//	BYTE    rgbRed;
//	BYTE    rgbGreen;
//	BYTE    rgbBlue;
//	BYTE    rgbReserved;
//} RGB;

std::string return_current_time_and_date() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t( now );

	std::stringstream ss;
	tm* ltm = std::localtime( &in_time_t );
	ss << std::put_time( ltm, "%Y%m%d%_H%M%S" );
	return ss.str();
}


//bool writePNGFileFromBuffer( const std::string &filename, PBITMAPINFO pbi, HBITMAP hBMP ) {
//	//PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbi;
//
//	//png_structp png = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
//	//if ( !png )
//	//	return false;
//
//	//png_infop info = png_create_info_struct( png );
//	//if ( !info ) {
//	//	png_destroy_write_struct( &png, &info );
//	//	return false;
//	//}
//
//	//FILE *fp = fopen( filename.c_str(), "wb" );
//	//if ( !fp ) {
//	//	png_destroy_write_struct( &png, &info );
//	//	return false;
//	//}
//	//png_init_io( png, fp );
//	//png_set_IHDR( png, info, pbih->biWidth, pbih->biHeight, 8 /* depth */, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );
//	//png_colorp palette = (png_colorp)png_malloc( png, PNG_MAX_PALETTE_LENGTH * sizeof( png_color ) );
//	//if ( !palette ) {
//	//	fclose( fp );
//	//	png_destroy_write_struct( &png, &info );
//	//	return false;
//	//}
//	//png_set_PLTE( png, info, palette, PNG_MAX_PALETTE_LENGTH );
//	//png_write_info( png, info );
//	//png_set_packing( png );
//
//	//// Create a compatible DC which is used in a BitBlt from the window DC
//	//HDC hdcMemDC = CreateCompatibleDC( NULL );
//
//	//size_t byteSize = sizeof( png_bytep );
//	//png_bytepp rows = (png_bytepp)png_malloc( png, pbih->biHeight * byteSize );//
//
//	//const size_t maxWidth = 10000;
//	//const size_t maxHeight = 10000;
//
//	//LPBYTE lpBits[maxHeight];
//
//	//for ( size_t i = 0; i < pbih->biHeight; i++ ) {
//	//	RGBQUAD pixels[maxWidth];
//
//	//	// Retrieve the color table (RGBQUAD array) and the bits (array of palette indices) from the DIB.
//	//	if ( !GetDIBits( hdcMemDC, hBMP, static_cast<UINT>( i ), 1, &pixels, pbi, DIB_RGB_COLORS ) )
//	//		errhandler( "GetDIBits" );
//
//	//	RGB reversedPixels[maxWidth];
//
//	//	for ( size_t j = 0; j < pbih->biWidth; j++ ) {
//	//		reversedPixels[j].rgbBlue = pixels[j].rgbBlue;
//	//		reversedPixels[j].rgbGreen = pixels[j].rgbGreen;
//	//		reversedPixels[j].rgbRed = pixels[j].rgbRed;
//	//		reversedPixels[j].rgbReserved = pixels[j].rgbReserved;
//	//	}
//
//	//	lpBits[i] = (LPBYTE)GlobalAlloc( GMEM_FIXED, pbih->biWidth * 4 );
//
//	//	memcpy( lpBits[i], reversedPixels, pbih->biWidth * 4 );
//
//	//	rows[pbih->biHeight - 1 - i] = (png_bytep)( lpBits[i] );
//
//	//	/*LPBYTE lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biWidth * 4);
//	//	if (!GetDIBits(hdcMemDC, hBMP, i, 1, lpBits, pbi, DIB_RGB_COLORS))
//	//		errhandler("GetDIBits");
//
//	//	rows[pbih->biHeight - 1 - i] = (png_bytep)(lpBits);*/
//	//}
//
//	//png_write_image( png, rows );
//	//png_write_end( png, info );
//	//png_free( png, palette );
//	//png_destroy_write_struct( &png, &info );
//
//	//fclose( fp );
//	//delete[] rows;
//
//	//for ( size_t i = 0; i < pbih->biHeight; i++ )
//	//	GlobalFree( (HGLOBAL)(LPBYTE)lpBits[i] );
//
//	////delete[] lpBits;
//
//	//return true;
//	return false;
//}
//
//bool writePNGFileFromBuffer( const char *filename, unsigned char *pixels, int w, int h ) {
//	//png_structp png = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );//8
//	//if ( !png )
//	//	return false;
//
//	//png_infop info = png_create_info_struct( png );//7
//	//if ( !info ) {
//	//	png_destroy_write_struct( &png, &info );//
//	//	return false;
//	//}
//
//	//FILE *fp = fopen( filename, "wb" );
//	//if ( !fp ) {
//	//	png_destroy_write_struct( &png, &info );//
//	//	return false;
//	//}
//	//png_init_io( png, fp );//9
//	//png_set_IHDR( png, info, w, h, 8 /* depth */, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );//10
//	//png_colorp palette = (png_colorp)png_malloc( png, PNG_MAX_PALETTE_LENGTH * sizeof( png_color ) );//4
//	//if ( !palette ) {
//	//	fclose( fp );
//	//	png_destroy_write_struct( &png, &info );//
//	//	return false;
//	//}
//	//png_set_PLTE( png, info, palette, PNG_MAX_PALETTE_LENGTH );//12
//	//png_write_info( png, info );//1
//	//						  //png_set_packing(png);//5
//
//	//size_t byteSize = sizeof( png_bytep );
//	//png_bytepp rows = (png_bytepp)png_malloc( png, h * byteSize );//
//	//for ( int i = h - 1; i >= 0; i-- )
//	//	rows[h - 1 - i] = (png_bytep)( pixels + ( i * w * 4 ) );
//
//	//png_write_image( png, rows );//2
//	//png_write_end( png, info );//6
//	//png_free( png, palette );//11
//	//png_destroy_write_struct( &png, &info );//3
//
//	//fclose( fp );
//	//delete[] rows;
//	//return true;
//	return false;
//}
//
//void CreateBMPFile( LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP ) {
//	HANDLE hf;                  // file handle
//	BITMAPFILEHEADER hdr;       // bitmap file-header
//	PBITMAPINFOHEADER pbih;     // bitmap info-header
//	LPBYTE lpBits;              // memory pointer
//	DWORD dwTotal;              // total count of bytes
//	DWORD cb;                   // incremental count of bytes
//	BYTE *hp;                   // byte pointer
//	DWORD dwTmp;
//
//	pbih = (PBITMAPINFOHEADER)pbi;
//	lpBits = (LPBYTE)GlobalAlloc( GMEM_FIXED, pbih->biSizeImage );
//
//	if ( !lpBits )
//		errhandler( "GlobalAlloc" );
//
//	// Create a compatible DC which is used in a BitBlt from the window DC
//	HDC hdcMemDC = CreateCompatibleDC( NULL );
//
//	// Retrieve the color table (RGBQUAD array) and the bits (array of palette indices) from the DIB.
//	if ( !GetDIBits( hdcMemDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS ) )
//		errhandler( "GetDIBits" );
//
//	// Create the .BMP file.
//	hf = CreateFile( pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL );
//	if ( hf == INVALID_HANDLE_VALUE )
//		errhandler( "CreateFile" );
//
//	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
//	// Compute the size of the entire file.
//	hdr.bfSize = (DWORD)( sizeof( BITMAPFILEHEADER ) + pbih->biSize + pbih->biClrUsed * sizeof( RGBQUAD ) + pbih->biSizeImage );
//	hdr.bfReserved1 = 0;
//	hdr.bfReserved2 = 0;
//
//	// Compute the offset to the array of color indices.
//	hdr.bfOffBits = ( DWORD ) sizeof( BITMAPFILEHEADER ) + pbih->biSize + pbih->biClrUsed * sizeof( RGBQUAD );
//
//	// Copy the BITMAPFILEHEADER into the .BMP file.
//	if ( !WriteFile( hf, (LPVOID)&hdr, sizeof( BITMAPFILEHEADER ), (LPDWORD)&dwTmp, NULL ) )
//		errhandler( "WriteFile" );
//
//	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
//	if ( !WriteFile( hf, (LPVOID)pbih, sizeof( BITMAPINFOHEADER ) + pbih->biClrUsed * sizeof( RGBQUAD ), (LPDWORD)&dwTmp, ( NULL ) ) )
//		errhandler( "WriteFile" );
//
//	// Copy the array of color indices into the .BMP file.
//	dwTotal = cb = pbih->biSizeImage;
//	hp = lpBits;
//	if ( !WriteFile( hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL ) )
//		errhandler( "WriteFile" );
//
//	// Close the .BMP file.
//	if ( !CloseHandle( hf ) )
//		errhandler( "CloseHandle" );
//
//	DeleteDC( hdcMemDC );
//
//	// Free memory.
//	GlobalFree( (HGLOBAL)lpBits );
//}

//void saveImageFromClipboard( const std::string &toFolder ) {
//	if ( IsClipboardFormatAvailable( CF_BITMAP ) || IsClipboardFormatAvailable( CF_DIB ) || IsClipboardFormatAvailable( CF_DIBV5 ) ) {
//		if ( OpenClipboard( NULL ) ) {
//			//HANDLE hClipboard = GetClipboardData(CF_DIBV5);
//			//HANDLE hClipboard = GetClipboardData(CF_DIB);
//
//			HBITMAP handle = (HBITMAP)GetClipboardData( CF_BITMAP );
//			PBITMAPINFO info = CreateBitmapInfoStruct( handle );
//
//			//CreateBMPFile(L"C:/temp/Test.bmp", info, handle);
//
//			std::string filename = "data/" + toFolder + "/capture" + std::to_string( int( JMATH::unitRand()*100000.0f ) ) + ".png"; // return_current_time_and_date()
//
//			writePNGFileFromBuffer( filename, info, handle );
//
//			CloseClipboard();
//		}
//	}
//}