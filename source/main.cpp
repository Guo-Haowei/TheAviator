#ifdef UNICODE
#undef UNICODE
#endif

#include "core/MainWindow.h"
#include "gfx/GfxContext.h"

#include "UploadBuffer.h"
#include "FrameResource.h"

#include "core/MathHelper.h"
#include "core/Scene.h"

#include <unordered_map>
#include <WindowsX.h>

using Microsoft::WRL::ComPtr;
using std::vector;

struct PerFrameConstant {
    mat4 PV{ 1.0f };
};

struct PerDrawConstant {
    mat4 W{ 1.0f };
};

struct RenderItem {
    RenderItem() = default;

    std::shared_ptr<MeshGeometry> Geo;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    mat4 World = mat4( 1.0f );

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify obect data we should set
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    int NumFramesDirty = gNumFrameResources;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

// window
extern int s_clientWidth;
extern int s_clientHeight;
extern HWND s_hWnd;

// graphics
extern IDXGIFactory4* s_factory;
extern ID3D12Device* s_device;
extern ID3D12Fence* s_fence;
extern ID3D12GraphicsCommandList* s_commandList;
extern ID3D12CommandAllocator* s_directCmdListAlloc;
extern ID3D12CommandQueue* s_commandQueue;
extern IDXGISwapChain* s_swapChain;

extern ID3D12DescriptorHeap* s_rtvHeap;
extern ID3D12DescriptorHeap* s_dsvHeap;

extern uint32_t s_rtvDescriptorSize;
extern uint32_t s_dsvDescriptorSize;
extern uint32_t s_cbvSrvUavDescriptorSize;

const int gNumFrameResources = 3;

HINSTANCE mhAppInst = nullptr;  // application instance handle
bool mAppPaused = false;        // is the application paused?
bool mMinimized = false;        // is the application minimized?
bool mMaximized = false;        // is the application maximized?
bool mResizing = false;         // are the resize bars being dragged?
bool mFullscreenState = false;  // fullscreen enabled

UINT64 mCurrentFence = 0;

int mCurrBackBuffer = 0;
Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[NUM_SWAPCHAIN_BUFFER];
Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

D3D12_VIEWPORT mScreenViewport;
D3D12_RECT mScissorRect;

// Derived class should set these in derived constructor to customize starting values.

std::vector<std::unique_ptr<FrameResource>> mFrameResources;
FrameResource* mCurrFrameResource = nullptr;
int mCurrFrameResourceIndex = 0;

ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

// std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
// std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

// List of all the render items.
std::vector<std::unique_ptr<RenderItem>> mRenderItems;

PassConstants mMainPassCB;

uint32_t mPassCbvOffset = 0;

ComPtr<ID3DBlob> mvsByteCode = nullptr;
ComPtr<ID3DBlob> mpsByteCode = nullptr;

ComPtr<ID3D12PipelineState> mPSO = nullptr;

mat4 mView{ 1.0f };
mat4 mProj{ 1.0f };

POINT mLastMousePos;

class Application {
   public:
    Application() = default;
    ~Application() = default;
    LRESULT MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

    bool Initialize();
    static Application* GetApp()
    {
        static Application app;
        return &app;
    }

    ID3D12Resource* CurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

    float AspectRatio() const
    {
        return static_cast<float>( s_clientWidth ) / s_clientHeight;
    }

    int Run();

    void CreateSwapChain();

    void FlushCommandQueue();
    void CreateRtvAndDsvDescriptorHeaps();

    void OnResize();
    void Update();
    void Draw();

    void OnMouseDown( WPARAM btnState, int x, int y );
    void OnMouseUp( WPARAM btnState, int x, int y );
    void OnMouseMove( WPARAM btnState, int x, int y );

    void UpdateCamera();
    void UpdateObjectCBs();
    void UpdateMainPassCB();

    void BuildDescriptorHeaps();
    void BuildConstantBufferViews();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildShapeGeometry();
    void BuildPSOs();
    void BuildFrameResources();
    void BuildRenderItems();
    void DrawRenderItems( ID3D12GraphicsCommandList* cmdList, const std::vector<std::unique_ptr<RenderItem>>& items );
};

LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // Forward hwnd on because we can get messages (e.g., WM_CREATE)
    // before CreateWindow returns, and thus before mhMainWnd is valid.
    return Application::GetApp()->MsgProc( hwnd, msg, wParam, lParam );
}

std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> s_gpuMeshes;

int main()
{
    if ( !Application::GetApp()->Initialize() )
    {
        return 0;
    }

    Application::GetApp()->Run();

    Gfx_Shutdown();
    Com_Window_Shutdown();

    return 0;
}

int Application::Run()
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

bool Application::Initialize()
{
    InitMeshes();

    if ( !Com_Window_Create( "TheAviator", 800, 600 ) )
    {
        return false;
    }

    if ( !Gfx_Init() )
    {
        return false;
    }

    OnResize();

    // Reset the command list to prep for initialization commands.
    DX_CALL( s_commandList->Reset( s_directCmdListAlloc, nullptr ) );

    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildShapeGeometry();
    BuildRenderItems();
    BuildFrameResources();
    BuildDescriptorHeaps();
    BuildConstantBufferViews();
    BuildPSOs();

    // Execute the initialization commands.
    DX_CALL( s_commandList->Close() );
    ID3D12CommandList* cmdsLists[] = { s_commandList };
    s_commandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

void Application::OnResize()
{
    assert( s_device );
    assert( s_swapChain );
    assert( s_directCmdListAlloc );

    // Flush before changing any resources.
    FlushCommandQueue();

    DX_CALL( s_commandList->Reset( s_directCmdListAlloc, nullptr ) );

    // Release the previous resources we will be recreating.
    for ( int i = 0; i < NUM_SWAPCHAIN_BUFFER; ++i )
        mSwapChainBuffer[i].Reset();
    mDepthStencilBuffer.Reset();

    // Resize the swap chain.
    DX_CALL( s_swapChain->ResizeBuffers(
        NUM_SWAPCHAIN_BUFFER,
        s_clientWidth, s_clientHeight,
        DEFAULT_BACK_BUFFER_FORMATT,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ) );

    mCurrBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle( s_rtvHeap->GetCPUDescriptorHandleForHeapStart() );
    for ( UINT i = 0; i < NUM_SWAPCHAIN_BUFFER; i++ )
    {
        DX_CALL( s_swapChain->GetBuffer( i, IID_PPV_ARGS( &mSwapChainBuffer[i] ) ) );
        s_device->CreateRenderTargetView( mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle );
        rtvHeapHandle.Offset( 1, s_rtvDescriptorSize );
    }

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = s_clientWidth;
    depthStencilDesc.Height = s_clientHeight;
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
    optClear.Format = DEFAULT_DEPTH_STENCIL_FORMAT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    DX_CALL( s_device->CreateCommittedResource(
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
    dsvDesc.Format = DEFAULT_DEPTH_STENCIL_FORMAT;
    dsvDesc.Texture2D.MipSlice = 0;
    s_device->CreateDepthStencilView( mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView() );

    // Transition the resource from its initial state to be used as a depth buffer.
    s_commandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( mDepthStencilBuffer.Get(),
                                                                              D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE ) );

    // Execute the resize commands.
    DX_CALL( s_commandList->Close() );
    ID3D12CommandList* cmdsLists[] = { s_commandList };
    s_commandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    mScreenViewport.TopLeftX = 0;
    mScreenViewport.TopLeftY = 0;
    mScreenViewport.Width = static_cast<float>( s_clientWidth );
    mScreenViewport.Height = static_cast<float>( s_clientHeight );
    mScreenViewport.MinDepth = 0.0f;
    mScreenViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, s_clientWidth, s_clientHeight };

    float fov = 0.25f * glm::pi<float>();
    mProj = glm::perspectiveLH_ZO( fov, AspectRatio(), 1.0f, 1000.0f );
}

void Application::Update()
{
    UpdateCamera();

    // Cycle through the circular frame resource array.
    mCurrFrameResourceIndex = ( mCurrFrameResourceIndex + 1 ) % gNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if ( mCurrFrameResource->Fence != 0 && s_fence->GetCompletedValue() < mCurrFrameResource->Fence )
    {
        HANDLE eventHandle = CreateEventEx( nullptr, false, false, EVENT_ALL_ACCESS );
        DX_CALL( s_fence->SetEventOnCompletion( mCurrFrameResource->Fence, eventHandle ) );
        WaitForSingleObject( eventHandle, INFINITE );
        CloseHandle( eventHandle );
    }

    UpdateObjectCBs();
    UpdateMainPassCB();
}

void Application::Draw()
{
    auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    DX_CALL( cmdListAlloc->Reset() );

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    DX_CALL( s_commandList->Reset( s_directCmdListAlloc, mPSO.Get() ) );

    s_commandList->RSSetViewports( 1, &mScreenViewport );
    s_commandList->RSSetScissorRects( 1, &mScissorRect );

    // Indicate a state transition on the resource usage.
    s_commandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

    // Clear the back buffer and depth buffer.
    float clearColor[] = { .3f, .4f, .3f, 1.f };
    s_commandList->ClearRenderTargetView( CurrentBackBufferView(), clearColor, 0, nullptr );
    s_commandList->ClearDepthStencilView( DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );

    // Specify the buffers we are going to render to.
    s_commandList->OMSetRenderTargets( 1, &CurrentBackBufferView(), true, &DepthStencilView() );

    ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
    s_commandList->SetDescriptorHeaps( _countof( descriptorHeaps ), descriptorHeaps );

    s_commandList->SetGraphicsRootSignature( mRootSignature.Get() );

    int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
    auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE( mCbvHeap->GetGPUDescriptorHandleForHeapStart() );
    passCbvHandle.Offset( passCbvIndex, s_cbvSrvUavDescriptorSize );
    s_commandList->SetGraphicsRootDescriptorTable( 1, passCbvHandle );

    DrawRenderItems( s_commandList, mRenderItems );

    // Indicate a state transition on the resource usage.
    s_commandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );

    // Done recording commands.
    DX_CALL( s_commandList->Close() );

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { s_commandList };
    s_commandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Swap the back and front buffers
    DX_CALL( s_swapChain->Present( 0, 0 ) );
    mCurrBackBuffer = ( mCurrBackBuffer + 1 ) % NUM_SWAPCHAIN_BUFFER;

    // Advance the fence value to mark commands up to this fence point.
    mCurrFrameResource->Fence = ++mCurrentFence;

    // Add an instruction to the command queue to set a new fence point.
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to this Signal().
    s_commandQueue->Signal( s_fence, mCurrentFence );
}

void Application::OnMouseDown( WPARAM btnState, int x, int y )
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture( s_hWnd );
}

void Application::OnMouseUp( WPARAM btnState, int x, int y )
{
    ReleaseCapture();
}

void Application::OnMouseMove( WPARAM btnState, int x, int y )
{
    if ( ( btnState & MK_LBUTTON ) != 0 )
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = glm::radians( 0.05f * static_cast<float>( x - mLastMousePos.x ) );
        // float dy = glm::radians( 0.25f * static_cast<float>( y - mLastMousePos.y ) );
    }
}

void Application::BuildDescriptorHeaps()
{
    UINT objCount = (UINT)mRenderItems.size();

    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    UINT numDescriptors = ( objCount + 1 ) * gNumFrameResources;

    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * gNumFrameResources;

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    DX_CALL( s_device->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &mCbvHeap ) ) );
}

void Application::BuildRootSignature()
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
    HRESULT hr = D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf() );
    if ( errorBlob != nullptr )
    {
        ::OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
    }
    DX_CALL( hr );

    DX_CALL( s_device->CreateRootSignature( 0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS( mRootSignature.GetAddressOf() ) ) );
}

void Application::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    char folder[] = __FILE__;
    char* p = strrchr( folder, '\\' );
    assert( p );
    p[1] = '\0';
    std::string shaderPath( folder );
    shaderPath.append( "shaders/main.hlsl" );
    std::wstring wShaderPath( shaderPath.begin(), shaderPath.end() );

    mvsByteCode = d3dUtil::CompileShader( wShaderPath.c_str(), nullptr, "VS", "vs_5_0" );
    mpsByteCode = d3dUtil::CompileShader( wShaderPath.c_str(), nullptr, "PS", "ps_5_0" );

    mInputLayout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}

static std::shared_ptr<MeshGeometry> BuildGeometry( ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const char* key )
{
    const MeshGroup* meshGroup = FindMesh( key );
    vector<Vertex> vertices;
    vector<uvec3> faces;
    meshGroup->BuildBuffers( vertices, faces );

    const UINT vbByteSize = (UINT)vertices.size() * sizeof( Vertex );
    const UINT ibByteSize = (UINT)faces.size() * sizeof( faces.front() );

    std::shared_ptr<MeshGeometry> mesh = std::make_shared<MeshGeometry>();
    mesh->Name = key;

    DX_CALL( D3DCreateBlob( vbByteSize, &mesh->VertexBufferCPU ) );
    CopyMemory( mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize );

    DX_CALL( D3DCreateBlob( ibByteSize, &mesh->IndexBufferCPU ) );
    CopyMemory( mesh->IndexBufferCPU->GetBufferPointer(), faces.data(), ibByteSize );
    mesh->VertexBufferGPU = d3dUtil::CreateDefaultBuffer( device, commandList, vertices.data(), vbByteSize, mesh->VertexBufferUploader );
    mesh->IndexBufferGPU = d3dUtil::CreateDefaultBuffer( device, commandList, faces.data(), ibByteSize, mesh->IndexBufferUploader );

    mesh->VertexByteStride = sizeof( Vertex );
    mesh->VertexBufferByteSize = vbByteSize;
    mesh->IndexFormat = DXGI_FORMAT_R32_UINT;
    mesh->IndexBufferByteSize = ibByteSize;
    mesh->IndexCount = static_cast<uint32_t>( faces.size() ) * 3;
    return mesh;
}

void Application::BuildShapeGeometry()
{
    std::array<std::string, 2> meshKeys = {
        MESH_KEY_AIRPLANE_BODY,
        MESH_KEY_AIRPLANE_PROPELLER,
    };

    for ( const auto& key : meshKeys )
    {
        s_gpuMeshes[key] = BuildGeometry( s_device, s_commandList, key.c_str() );
    }
}

void Application::BuildPSOs()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
    ZeroMemory( &desc, sizeof( D3D12_GRAPHICS_PIPELINE_STATE_DESC ) );

    // psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    // psoDesc.pRootSignature = mRootSignature.Get();
    // psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
    // psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
    // psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
    // psoDesc.SampleMask = UINT_MAX;
    // psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    // psoDesc.NumRenderTargets = 1;
    // psoDesc.RTVFormats[0] = DEFAULT_BACKBUFFER_FMT;
    // psoDesc.SampleDesc.Count = 1;
    // psoDesc.SampleDesc.Quality = 0;
    // psoDesc.DSVFormat = DEFAULT_DEPTH_STENCIL_FMT;
    // DX_CALL( mDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &mPSO ) ) );

    desc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    desc.pRootSignature = mRootSignature.Get();
    desc.VS = {
        reinterpret_cast<BYTE*>( mvsByteCode->GetBufferPointer() ),
        mvsByteCode->GetBufferSize()
    };
    desc.PS = {
        reinterpret_cast<BYTE*>( mpsByteCode->GetBufferPointer() ),
        mpsByteCode->GetBufferSize()
    };
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
    desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    desc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DEFAULT_BACK_BUFFER_FORMATT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.DSVFormat = DEFAULT_DEPTH_STENCIL_FORMAT;
    DX_CALL( s_device->CreateGraphicsPipelineState( &desc, IID_PPV_ARGS( &mPSO ) ) );
}

void Application::BuildFrameResources()
{
    for ( int i = 0; i < gNumFrameResources; ++i )
    {
        mFrameResources.push_back( std::make_unique<FrameResource>( s_device, 1, (UINT)mRenderItems.size() ) );
    }
}

void Application::BuildConstantBufferViews()
{
    uint32_t objCBByteSize = d3dUtil::CalcConstantBufferByteSize( sizeof( ObjectConstants ) );

    uint32_t objCount = (uint32_t)mRenderItems.size();

    // Need a CBV descriptor for each object for each frame resource.
    for ( int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex )
    {
        auto objectCB = mFrameResources[frameIndex]->ObjectCB->Resource();
        for ( UINT i = 0; i < objCount; ++i )
        {
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i * objCBByteSize;

            // Offset to the object cbv in the descriptor heap.
            int heapIndex = frameIndex * objCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE( mCbvHeap->GetCPUDescriptorHandleForHeapStart() );
            handle.Offset( heapIndex, s_cbvSrvUavDescriptorSize );

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = objCBByteSize;

            s_device->CreateConstantBufferView( &cbvDesc, handle );
        }
    }

    UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize( sizeof( PassConstants ) );

    // Last three descriptors are the pass CBVs for each frame resource.
    for ( int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex )
    {
        auto passCB = mFrameResources[frameIndex]->PassCB->Resource();
        D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

        // Offset to the pass cbv in the descriptor heap.
        int heapIndex = mPassCbvOffset + frameIndex;
        auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE( mCbvHeap->GetCPUDescriptorHandleForHeapStart() );
        handle.Offset( heapIndex, s_cbvSrvUavDescriptorSize );

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = cbAddress;
        cbvDesc.SizeInBytes = passCBByteSize;

        s_device->CreateConstantBufferView( &cbvDesc, handle );
    }
}

void Application::DrawRenderItems( ID3D12GraphicsCommandList* cmdList, const std::vector<std::unique_ptr<RenderItem>>& items )
{
    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize( sizeof( ObjectConstants ) );

    auto objectCB = mCurrFrameResource->ObjectCB->Resource();

    // For each render item...
    for ( size_t i = 0; i < items.size(); ++i )
    {
        const auto& item = items[i];

        cmdList->IASetVertexBuffers( 0, 1, &item->Geo->VertexBufferView() );
        cmdList->IASetIndexBuffer( &item->Geo->IndexBufferView() );
        cmdList->IASetPrimitiveTopology( item->PrimitiveType );

        // Offset to the CBV in the descriptor heap for this object and for this frame resource.
        UINT cbvIndex = mCurrFrameResourceIndex * (UINT)items.size() + item->ObjCBIndex;
        auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE( mCbvHeap->GetGPUDescriptorHandleForHeapStart() );
        cbvHandle.Offset( cbvIndex, s_cbvSrvUavDescriptorSize );

        cmdList->SetGraphicsRootDescriptorTable( 0, cbvHandle );

        cmdList->DrawIndexedInstanced( item->IndexCount, 1, item->StartIndexLocation, item->BaseVertexLocation, 0 );
    }
}

void Application::UpdateCamera()
{
}
static float g_angle = 45.0f;
void Application::UpdateObjectCBs()
{
    g_angle -= 1.0f;

    auto currObjectCB = mCurrFrameResource->ObjectCB.get();

    int i = 0;
    for ( auto& e : mRenderItems )
    {
        // Only update the cbuffer data if the constants have changed.
        // This needs to be tracked per frame resource.
        // if ( e->NumFramesDirty > 0 )
        {
            ObjectConstants objConstants;
            // HACK:

            if ( i == 0 )
            {
                objConstants.Model = mat4( 1.0 );
            }
            else
            {
                objConstants.Model = glm::rotate( mat4( 1 ), glm::radians( g_angle ), vec3( 1, 0, 0 ) );
            }

            currObjectCB->CopyData( e->ObjCBIndex, objConstants );

            // Next FrameResource need to be updated too.
            // e->NumFramesDirty--;
        }
        ++i;
    }
}

void Application::UpdateMainPassCB()
{
    mView = glm::lookAtLH( vec3( 0.0f, 0.0f, -30.0f ), vec3( 0.0f ), vec3( 0.0f, 1.0f, 0.0f ) );

    // Update the constant buffer with the latest worldViewProj matrix.
    PerFrameConstant perFrameCB;
    perFrameCB.PV = mProj * mView;

    mMainPassCB.View = mView;
    mMainPassCB.Proj = mProj;

    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData( 0, mMainPassCB );
}

void Application::BuildRenderItems()
{
    {
        auto body = std::make_unique<RenderItem>();
        body->ObjCBIndex = 0;
        body->Geo = s_gpuMeshes[MESH_KEY_AIRPLANE_BODY];
        body->IndexCount = body->Geo->IndexCount;
        body->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        mRenderItems.push_back( std::move( body ) );
    }
    {
        auto propeller = std::make_unique<RenderItem>();
        propeller->ObjCBIndex = 1;
        propeller->Geo = s_gpuMeshes[MESH_KEY_AIRPLANE_PROPELLER];
        propeller->IndexCount = propeller->Geo->IndexCount;
        propeller->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        mRenderItems.push_back( std::move( propeller ) );
    }
}

LRESULT Application::MsgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
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
            s_clientWidth = LOWORD( lParam );
            s_clientHeight = HIWORD( lParam );
            if ( s_device )
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

void Application::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    DX_CALL( s_commandQueue->Signal( s_fence, mCurrentFence ) );

    // Wait until the GPU has completed commands up to this fence point.
    if ( s_fence->GetCompletedValue() < mCurrentFence )
    {
        HANDLE eventHandle = CreateEventEx( nullptr, false, false, EVENT_ALL_ACCESS );

        // Fire event when GPU hits current fence.
        DX_CALL( s_fence->SetEventOnCompletion( mCurrentFence, eventHandle ) );

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject( eventHandle, INFINITE );
        CloseHandle( eventHandle );
    }
}

ID3D12Resource* Application::CurrentBackBuffer() const
{
    return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Application::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( s_rtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, s_rtvDescriptorSize );
}

D3D12_CPU_DESCRIPTOR_HANDLE Application::DepthStencilView() const
{
    return s_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}
