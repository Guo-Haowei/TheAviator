#include "GfxContext.h"

#include <dxgi1_4.h>
#include <d3d12.h>
#include <wrl.h>

#include <type_traits>

#include "../d3dUtil.h"

using Microsoft::WRL::ComPtr;

// TODO: refactor
extern int s_clientWidth;
extern int s_clientHeight;
extern HWND s_hWnd;

template<typename T>
void SafeRelease( T& ptr )
{
    static_assert( std::is_pointer<T>() );
    if ( ptr )
    {
        ptr->Release();
        ptr = nullptr;
    }
}

IDXGIFactory4* s_factory;
ID3D12Device* s_device;
ID3D12Fence* s_fence;
ID3D12GraphicsCommandList* s_commandList;
ID3D12CommandAllocator* s_directCmdListAlloc;
ID3D12CommandQueue* s_commandQueue;
IDXGISwapChain* s_swapChain;
ID3D12DescriptorHeap* s_rtvHeap;
ID3D12DescriptorHeap* s_dsvHeap;

uint32_t s_rtvDescriptorSize;
uint32_t s_dsvDescriptorSize;
uint32_t s_cbvSrvUavDescriptorSize;

static void Gfx_CreateCommandObjects();
static void Gfx_CreateSwapChain();
static void Gfx_CreateRtvAndDsvDescriptorHeaps();

bool Gfx_Init()
{
    // Enable the D3D12 debug layer.
    {
        ComPtr<ID3D12Debug> debugController;
        DX_CALL( D3D12GetDebugInterface( IID_PPV_ARGS( &debugController ) ) );
        debugController->EnableDebugLayer();
    }

    DX_CALL( CreateDXGIFactory1( IID_PPV_ARGS( &s_factory ) ) );

    HRESULT hardwareResult = D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &s_device ) );

    DX_CALL( s_device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &s_fence ) ) );

    s_rtvDescriptorSize = s_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    s_dsvDescriptorSize = s_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );
    s_cbvSrvUavDescriptorSize = s_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );

    Gfx_CreateCommandObjects();
    Gfx_CreateSwapChain();
    Gfx_CreateRtvAndDsvDescriptorHeaps();

    return true;
}

void Gfx_Shutdown()
{
    SafeRelease( s_rtvHeap );
    SafeRelease( s_dsvHeap );
    SafeRelease( s_swapChain );
    SafeRelease( s_commandQueue );
    SafeRelease( s_commandList );
    SafeRelease( s_directCmdListAlloc );
    SafeRelease( s_fence );
    SafeRelease( s_device );
    SafeRelease( s_factory );
}

static void Gfx_CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    DX_CALL( s_device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &s_commandQueue ) ) );

    DX_CALL( s_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS( &s_directCmdListAlloc ) ) );

    DX_CALL( s_device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, s_directCmdListAlloc, nullptr, IID_PPV_ARGS( &s_commandList ) ) );

    // Start off in a closed state.  This is because the first time we refer
    // to the command list we will Reset it, and it needs to be closed before
    // calling Reset.
    s_commandList->Close();
}

static void Gfx_CreateSwapChain()
{
    // Release the previous swapchain we will be recreating.
    SafeRelease( s_swapChain );

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = s_clientWidth;
    sd.BufferDesc.Height = s_clientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DEFAULT_BACK_BUFFER_FORMATT;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = NUM_SWAPCHAIN_BUFFER;
    sd.OutputWindow = s_hWnd;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Note: Swap chain uses queue to perform flush.
    DX_CALL( s_factory->CreateSwapChain( s_commandQueue, &sd, &s_swapChain ) );
}

static void Gfx_CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = NUM_SWAPCHAIN_BUFFER;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    DX_CALL( s_device->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( &s_rtvHeap ) ) );

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    DX_CALL( s_device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &s_dsvHeap ) ) );
}
