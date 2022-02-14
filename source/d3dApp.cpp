//***************************************************************************************
// d3dApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#include "d3dApp.h"
#include <WindowsX.h>

using Microsoft::WRL::ComPtr;
using namespace std;

LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // Forward hwnd on because we can get messages (e.g., WM_CREATE)
    // before CreateWindow returns, and thus before mhMainWnd is valid.
    return D3DApp::GetApp()->MsgProc( hwnd, msg, wParam, lParam );
}

D3DApp* D3DApp::mApp = nullptr;
D3DApp* D3DApp::GetApp()
{
    return mApp;
}

D3DApp::D3DApp( HINSTANCE hInstance )
    : mhAppInst( hInstance )
{
    // Only one D3DApp can be constructed.
    assert( mApp == nullptr );
    mApp = this;
}

D3DApp::~D3DApp()
{
    if ( mDevice != nullptr )
        FlushCommandQueue();
}

float D3DApp::AspectRatio() const
{
    return static_cast<float>( mClientWidth ) / mClientHeight;
}

int D3DApp::Run()
{
    MSG msg = { 0 };

    while ( msg.message != WM_QUIT )
    {
        // If there are Window messages then process them.
        if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        // Otherwise, do animation/game stuff.
        else
        {
            if ( !mAppPaused )
            {
                Update();
                Draw();
            }
            else
            {
                Sleep( 100 );
            }
        }
    }

    return (int)msg.wParam;
}

bool D3DApp::Initialize()
{
    if ( !InitMainWindow() )
        return false;

    if ( !InitDirect3D() )
        return false;

    // Do the initial resize code.
    OnResize();

    return true;
}

void D3DApp::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    DX_CALL( mDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS( mRtvHeap.GetAddressOf() ) ) );

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    DX_CALL( mDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS( mDsvHeap.GetAddressOf() ) ) );
}

void D3DApp::OnResize()
{
    assert( mDevice );
    assert( mSwapChain );
    assert( mDirectCmdListAlloc );

    // Flush before changing any resources.
    FlushCommandQueue();

    DX_CALL( mCommandList->Reset( mDirectCmdListAlloc.Get(), nullptr ) );

    // Release the previous resources we will be recreating.
    for ( int i = 0; i < SwapChainBufferCount; ++i )
        mSwapChainBuffer[i].Reset();
    mDepthStencilBuffer.Reset();

    // Resize the swap chain.
    DX_CALL( mSwapChain->ResizeBuffers(
        SwapChainBufferCount,
        mClientWidth, mClientHeight,
        mBackBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ) );

    mCurrBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle( mRtvHeap->GetCPUDescriptorHandleForHeapStart() );
    for ( UINT i = 0; i < SwapChainBufferCount; i++ )
    {
        DX_CALL( mSwapChain->GetBuffer( i, IID_PPV_ARGS( &mSwapChainBuffer[i] ) ) );
        mDevice->CreateRenderTargetView( mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle );
        rtvHeapHandle.Offset( 1, mRtvDescriptorSize );
    }

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;

    // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from
    // the depth buffer.  Therefore, because we need to create two views to the same resource:
    //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
    // we need to create the depth buffer resource with a typeless format.
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    DX_CALL( mDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT ),
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS( mDepthStencilBuffer.GetAddressOf() ) ) );

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    mDevice->CreateDepthStencilView( mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView() );

    // Transition the resource from its initial state to be used as a depth buffer.
    mCommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( mDepthStencilBuffer.Get(),
                                                                             D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE ) );

    // Execute the resize commands.
    DX_CALL( mCommandList->Close() );
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    mScreenViewport.TopLeftX = 0;
    mScreenViewport.TopLeftY = 0;
    mScreenViewport.Width = static_cast<float>( mClientWidth );
    mScreenViewport.Height = static_cast<float>( mClientHeight );
    mScreenViewport.MinDepth = 0.0f;
    mScreenViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}

LRESULT D3DApp::MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch ( msg )
    {
        // WM_ACTIVATE is sent when the window is activated or deactivated.
        // We pause the game when the window is deactivated and unpause it
        // when it becomes active.
        case WM_ACTIVATE:
            if ( LOWORD( wParam ) == WA_INACTIVE )
            {
                mAppPaused = true;
            }
            else
            {
                mAppPaused = false;
            }
            return 0;

        // WM_SIZE is sent when the user resizes the window.
        case WM_SIZE:
            // Save the new client area dimensions.
            mClientWidth = LOWORD( lParam );
            mClientHeight = HIWORD( lParam );
            if ( mDevice )
            {
                if ( wParam == SIZE_MINIMIZED )
                {
                    mAppPaused = true;
                    mMinimized = true;
                    mMaximized = false;
                }
                else if ( wParam == SIZE_MAXIMIZED )
                {
                    mAppPaused = false;
                    mMinimized = false;
                    mMaximized = true;
                    OnResize();
                }
                else if ( wParam == SIZE_RESTORED )
                {
                    // Restoring from minimized state?
                    if ( mMinimized )
                    {
                        mAppPaused = false;
                        mMinimized = false;
                        OnResize();
                    }

                    // Restoring from maximized state?
                    else if ( mMaximized )
                    {
                        mAppPaused = false;
                        mMaximized = false;
                        OnResize();
                    }
                    else if ( mResizing )
                    {
                        // If user is dragging the resize bars, we do not resize
                        // the buffers here because as the user continuously
                        // drags the resize bars, a stream of WM_SIZE messages are
                        // sent to the window, and it would be pointless (and slow)
                        // to resize for each WM_SIZE message received from dragging
                        // the resize bars.  So instead, we reset after the user is
                        // done resizing the window and releases the resize bars, which
                        // sends a WM_EXITSIZEMOVE message.
                    }
                    else  // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                    {
                        OnResize();
                    }
                }
            }
            return 0;

        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
        case WM_ENTERSIZEMOVE:
            mAppPaused = true;
            mResizing = true;
            return 0;

        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
        case WM_EXITSIZEMOVE:
            mAppPaused = false;
            mResizing = false;
            OnResize();
            return 0;

        // WM_DESTROY is sent when the window is being destroyed.
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;

        // The WM_MENUCHAR message is sent when a menu is active and the user presses
        // a key that does not correspond to any mnemonic or accelerator key.
        case WM_MENUCHAR:
            // Don't beep when we alt-enter.
            return MAKELRESULT( 0, MNC_CLOSE );

        // Catch this message so to prevent the window from becoming too small.
        case WM_GETMINMAXINFO:
            ( (MINMAXINFO*)lParam )->ptMinTrackSize.x = 200;
            ( (MINMAXINFO*)lParam )->ptMinTrackSize.y = 200;
            return 0;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            OnMouseDown( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
            return 0;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            OnMouseUp( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
            return 0;
        case WM_MOUSEMOVE:
            OnMouseMove( wParam, GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
            return 0;
        case WM_KEYUP:
            if ( wParam == VK_ESCAPE )
            {
                PostQuitMessage( 0 );
            }

            return 0;
    }

    return DefWindowProc( hwnd, msg, wParam, lParam );
}

bool D3DApp::InitMainWindow()
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = mhAppInst;
    wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
    wc.hCursor = LoadCursor( 0, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"MainWnd";

    if ( !RegisterClass( &wc ) )
    {
        MessageBox( 0, L"RegisterClass Failed.", 0, 0 );
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect( &R, WS_OVERLAPPEDWINDOW, false );
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    mhMainWnd = CreateWindow( L"MainWnd", mMainWndCaption.c_str(),
                              WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0 );
    if ( !mhMainWnd )
    {
        MessageBox( 0, L"CreateWindow Failed.", 0, 0 );
        return false;
    }

    ShowWindow( mhMainWnd, SW_SHOW );
    UpdateWindow( mhMainWnd );

    return true;
}

bool D3DApp::InitDirect3D()
{
    // Enable the D3D12 debug layer.
    {
        ComPtr<ID3D12Debug> debugController;
        DX_CALL( D3D12GetDebugInterface( IID_PPV_ARGS( &debugController ) ) );
        debugController->EnableDebugLayer();
    }

    DX_CALL( CreateDXGIFactory1( IID_PPV_ARGS( &mdxgiFactory ) ) );

    // Try to create hardware device.
    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr,  // default adapter
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS( &mDevice ) );

    // Fallback to WARP device.
    if ( FAILED( hardwareResult ) )
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        DX_CALL( mdxgiFactory->EnumWarpAdapter( IID_PPV_ARGS( &pWarpAdapter ) ) );

        DX_CALL( D3D12CreateDevice(
            pWarpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS( &mDevice ) ) );
    }

    DX_CALL( mDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE,
                                   IID_PPV_ARGS( &mFence ) ) );

    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
    mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();

    return true;
}

void D3DApp::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    DX_CALL( mDevice->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &mCommandQueue ) ) );

    DX_CALL( mDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS( mDirectCmdListAlloc.GetAddressOf() ) ) );

    DX_CALL( mDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        mDirectCmdListAlloc.Get(),  // Associated command allocator
        nullptr,                    // Initial PipelineStateObject
        IID_PPV_ARGS( mCommandList.GetAddressOf() ) ) );

    // Start off in a closed state.  This is because the first time we refer
    // to the command list we will Reset it, and it needs to be closed before
    // calling Reset.
    mCommandList->Close();
}

void D3DApp::CreateSwapChain()
{
    // Release the previous swapchain we will be recreating.
    mSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = mClientWidth;
    sd.BufferDesc.Height = mClientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = mBackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = mhMainWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Note: Swap chain uses queue to perform flush.
    DX_CALL( mdxgiFactory->CreateSwapChain(
        mCommandQueue.Get(),
        &sd,
        mSwapChain.GetAddressOf() ) );
}

void D3DApp::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    DX_CALL( mCommandQueue->Signal( mFence.Get(), mCurrentFence ) );

    // Wait until the GPU has completed commands up to this fence point.
    if ( mFence->GetCompletedValue() < mCurrentFence )
    {
        HANDLE eventHandle = CreateEventEx( nullptr, false, false, EVENT_ALL_ACCESS );

        // Fire event when GPU hits current fence.
        DX_CALL( mFence->SetEventOnCompletion( mCurrentFence, eventHandle ) );

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject( eventHandle, INFINITE );
        CloseHandle( eventHandle );
    }
}

ID3D12Resource* D3DApp::CurrentBackBuffer() const
{
    return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
        mCurrBackBuffer,
        mRtvDescriptorSize );
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}
