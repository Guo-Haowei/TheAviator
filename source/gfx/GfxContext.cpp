#include "GfxContext.h"

#include "base/Log.h"

#include "Prerequisites.h"
#include "PipelineStateObjects.h"

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

extern ID3D12RootSignature* s_rootSignature;

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

static void Gfx_BuildRootSignature();
static void Gfx_BuildShapeGeometry();
static void Gfx_BuildRenderItems();
static void Gfx_BuildFrameResources();
static void Gfx_BuildDescriptorHeaps();
static void Gfx_BuildConstantBufferViews();

void Gfx_CreateAssets()
{
    Gfx_BuildRootSignature();

    CreatePipelineStateObjects( s_device );

    // Gfx_BuildShapeGeometry();
    // Gfx_BuildRenderItems();
    // Gfx_BuildFrameResources();
    // Gfx_BuildDescriptorHeaps();
    // Gfx_BuildConstantBufferViews();
}

static void Gfx_BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE cbvTable0;
    cbvTable0.Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0 );

    CD3DX12_DESCRIPTOR_RANGE cbvTable1;
    cbvTable1.Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1 );

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[2];

    // Create root CBVs.
    slotRootParameter[0].InitAsDescriptorTable( 1, &cbvTable0 );
    slotRootParameter[1].InitAsDescriptorTable( 1, &cbvTable1 );

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc( 2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );
    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf() );
    if ( errorBlob != nullptr )
    {
        LOGE( "Failed to serialize root signature: %s", (char*)errorBlob->GetBufferPointer() );
    }

    DX_CALL( s_device->CreateRootSignature( 0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS( &s_rootSignature ) ) );
}

static void Gfx_BuildShapeGeometry();
static void Gfx_BuildRenderItems();
static void Gfx_BuildFrameResources();
static void Gfx_BuildDescriptorHeaps();
static void Gfx_BuildConstantBufferViews();

void Gfx_Shutdown()
{
    SafeRelease( s_rootSignature );

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