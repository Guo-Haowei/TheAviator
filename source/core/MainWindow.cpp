#include "MainWindow.h"

#include "base/Log.h"

#include <cassert>

int s_clientWidth;
int s_clientHeight;
HWND s_hWnd;

extern LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

bool Com_Window_Create( const char* title, int width, int height )
{
    assert( title );
    assert( width > 0 && height > 0 );

    s_clientWidth = width;
    s_clientHeight = height;

    WNDCLASSA wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle( nullptr );
    wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
    wc.hCursor = LoadCursor( 0, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "MainWnd";

    if ( !RegisterClassA( &wc ) )
    {
        LOGF( "Failed to create WNDCLASS '%s'", wc.lpszClassName );
        return false;
    }

    // adjust window size
    RECT R = { 0, 0, s_clientWidth, s_clientHeight };
    AdjustWindowRect( &R, WS_OVERLAPPEDWINDOW, false );
    width = R.right - R.left;
    height = R.bottom - R.top;

    s_hWnd = CreateWindowA( "MainWnd", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, wc.hInstance, 0 );
    if ( !s_hWnd )
    {
        LOGF( "Failed to create window '%s'", title );
        return false;
    }

    ShowWindow( s_hWnd, SW_SHOW );
    UpdateWindow( s_hWnd );

    LOG( "window \"%s\" (w:%d h:%d) created", title, s_clientWidth, s_clientHeight );
    return true;
}

void Com_Window_Shutdown()
{
    if ( s_hWnd )
    {
        DestroyWindow( s_hWnd );
    }
}
