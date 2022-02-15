#ifdef UNICODE
#undef UNICODE
#endif

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

const int gNumFrameResources = 3;

HINSTANCE mhAppInst = nullptr;  // application instance handle
HWND mhMainWnd = nullptr;       // main window handle
bool mAppPaused = false;        // is the application paused?
bool mMinimized = false;        // is the application minimized?
bool mMaximized = false;        // is the application maximized?
bool mResizing = false;         // are the resize bars being dragged?
bool mFullscreenState = false;  // fullscreen enabled

Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
UINT64 mCurrentFence = 0;

Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

static const int SwapChainBufferCount = 2;
int mCurrBackBuffer = 0;
Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

D3D12_VIEWPORT mScreenViewport;
D3D12_RECT mScissorRect;

UINT mRtvDescriptorSize = 0;
UINT mDsvDescriptorSize = 0;
UINT mCbvSrvUavDescriptorSize = 0;

// Derived class should set these in derived constructor to customize starting values.
D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
int mClientWidth = 800;
int mClientHeight = 600;

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
    Application();
    Application( const Application& rhs ) = delete;
    Application& operator=( const Application& rhs ) = delete;
    ~Application();
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
        return static_cast<float>( mClientWidth ) / mClientHeight;
    }

    int Run();

   private:
    bool InitMainWindow();
    bool InitDirect3D();
    void CreateCommandObjects();
    void CreateSwapChain();

    void FlushCommandQueue();
    void CreateRtvAndDsvDescriptorHeaps();

   private:
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

   private:
};

static LRESULT CALLBACK MainWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
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

    return Application::GetApp()->Run();
}

Application::Application()
{
}

Application::~Application()
{
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

    if ( !InitMainWindow() )
    {
        return false;
    }

    if ( !InitDirect3D() )
    {
        return false;
    }

    OnResize();

    // Reset the command list to prep for initialization commands.
    DX_CALL( mCommandList->Reset( mDirectCmdListAlloc.Get(), nullptr ) );

    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildShapeGeometry();
    BuildRenderItems();
    BuildFrameResources();
    BuildDescriptorHeaps();
    BuildConstantBufferViews();
    BuildPSOs();

    // Execute the initialization commands.
    DX_CALL( mCommandList->Close() );
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Wait until initialization is complete.
    FlushCommandQueue();

    return true;
}

void Application::OnResize()
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
    if ( mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence )
    {
        HANDLE eventHandle = CreateEventEx( nullptr, false, false, EVENT_ALL_ACCESS );
        DX_CALL( mFence->SetEventOnCompletion( mCurrFrameResource->Fence, eventHandle ) );
        WaitForSingleObject( eventHandle, INFINITE );
        CloseHandle( eventHandle );
    }

    UpdateObjectCBs();
    UpdateMainPassCB();
}

void Application::Draw()
{
#if 0
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    DX_CALL( mDirectCmdListAlloc->Reset() );

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    DX_CALL( mCommandList->Reset( mDirectCmdListAlloc.Get(), mPSO.Get() ) );

    mCommandList->RSSetViewports( 1, &mScreenViewport );
    mCommandList->RSSetScissorRects( 1, &mScissorRect );

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

    // Clear the back buffer and depth buffer.
    float clearColor[] = { .3f, .4f, .3f, 1.f };
    mCommandList->ClearRenderTargetView( CurrentBackBufferView(), clearColor, 0, nullptr );
    mCommandList->ClearDepthStencilView( DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );

    // Specify the buffers we are going to render to.
    mCommandList->OMSetRenderTargets( 1, &CurrentBackBufferView(), true, &DepthStencilView() );

    ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
    mCommandList->SetDescriptorHeaps( _countof( descriptorHeaps ), descriptorHeaps );

    mCommandList->SetGraphicsRootSignature( mRootSignature.Get() );

    const auto& gpuMesh = s_gpuMeshes[MESH_KEY_AIRPLANE_BODY];
    mCommandList->IASetVertexBuffers( 0, 1, &gpuMesh->VertexBufferView() );
    mCommandList->IASetIndexBuffer( &gpuMesh->IndexBufferView() );
    mCommandList->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    mCommandList->SetGraphicsRootDescriptorTable( 0, mCbvHeap->GetGPUDescriptorHandleForHeapStart() );

    mCommandList->DrawIndexedInstanced( gpuMesh->IndexCount, 1, 0, 0, 0 );

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );

    // Done recording commands.
    DX_CALL( mCommandList->Close() );

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // swap the back and front buffers
    DX_CALL( mSwapChain->Present( 0, 0 ) );
    mCurrBackBuffer = ( mCurrBackBuffer + 1 ) % SwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
#endif

    auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    DX_CALL( cmdListAlloc->Reset() );

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    DX_CALL( mCommandList->Reset( mDirectCmdListAlloc.Get(), mPSO.Get() ) );

    mCommandList->RSSetViewports( 1, &mScreenViewport );
    mCommandList->RSSetScissorRects( 1, &mScissorRect );

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

    // Clear the back buffer and depth buffer.
    float clearColor[] = { .3f, .4f, .3f, 1.f };
    mCommandList->ClearRenderTargetView( CurrentBackBufferView(), clearColor, 0, nullptr );
    mCommandList->ClearDepthStencilView( DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );

    // Specify the buffers we are going to render to.
    mCommandList->OMSetRenderTargets( 1, &CurrentBackBufferView(), true, &DepthStencilView() );

    ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
    mCommandList->SetDescriptorHeaps( _countof( descriptorHeaps ), descriptorHeaps );

    mCommandList->SetGraphicsRootSignature( mRootSignature.Get() );

    int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
    auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE( mCbvHeap->GetGPUDescriptorHandleForHeapStart() );
    passCbvHandle.Offset( passCbvIndex, mCbvSrvUavDescriptorSize );
    mCommandList->SetGraphicsRootDescriptorTable( 1, passCbvHandle );

    DrawRenderItems( mCommandList.Get(), mRenderItems );

    // Indicate a state transition on the resource usage.
    mCommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );

    // Done recording commands.
    DX_CALL( mCommandList->Close() );

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists( _countof( cmdsLists ), cmdsLists );

    // Swap the back and front buffers
    DX_CALL( mSwapChain->Present( 0, 0 ) );
    mCurrBackBuffer = ( mCurrBackBuffer + 1 ) % SwapChainBufferCount;

    // Advance the fence value to mark commands up to this fence point.
    mCurrFrameResource->Fence = ++mCurrentFence;

    // Add an instruction to the command queue to set a new fence point.
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to this Signal().
    mCommandQueue->Signal( mFence.Get(), mCurrentFence );
}

void Application::OnMouseDown( WPARAM btnState, int x, int y )
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture( mhMainWnd );
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
    DX_CALL( mDevice->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &mCbvHeap ) ) );
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

    DX_CALL( mDevice->CreateRootSignature( 0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS( mRootSignature.GetAddressOf() ) ) );
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
        s_gpuMeshes[key] = BuildGeometry( mDevice.Get(), mCommandList.Get(), key.c_str() );
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
    // psoDesc.RTVFormats[0] = mBackBufferFormat;
    // psoDesc.SampleDesc.Count = 1;
    // psoDesc.SampleDesc.Quality = 0;
    // psoDesc.DSVFormat = mDepthStencilFormat;
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
    desc.RTVFormats[0] = mBackBufferFormat;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.DSVFormat = mDepthStencilFormat;
    DX_CALL( mDevice->CreateGraphicsPipelineState( &desc, IID_PPV_ARGS( &mPSO ) ) );
}

void Application::BuildFrameResources()
{
    for ( int i = 0; i < gNumFrameResources; ++i )
    {
        mFrameResources.push_back( std::make_unique<FrameResource>( mDevice.Get(), 1, (UINT)mRenderItems.size() ) );
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
            handle.Offset( heapIndex, mCbvSrvUavDescriptorSize );

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = objCBByteSize;

            mDevice->CreateConstantBufferView( &cbvDesc, handle );
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
        handle.Offset( heapIndex, mCbvSrvUavDescriptorSize );

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = cbAddress;
        cbvDesc.SizeInBytes = passCBByteSize;

        mDevice->CreateConstantBufferView( &cbvDesc, handle );
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
        cbvHandle.Offset( cbvIndex, mCbvSrvUavDescriptorSize );

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

bool Application::InitMainWindow()
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle( nullptr );
    wc.hIcon = LoadIcon( 0, IDI_APPLICATION );
    wc.hCursor = LoadCursor( 0, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = "MainWnd";

    if ( !RegisterClass( &wc ) )
    {
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT R = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect( &R, WS_OVERLAPPEDWINDOW, false );
    int width = R.right - R.left;
    int height = R.bottom - R.top;

    mhMainWnd = CreateWindow( "MainWnd", "TheAviator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0 );
    if ( !mhMainWnd )
    {
        MessageBox( 0, "CreateWindow Failed.", 0, 0 );
        return false;
    }

    ShowWindow( mhMainWnd, SW_SHOW );
    UpdateWindow( mhMainWnd );

    return true;
}

bool Application::InitDirect3D()
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

void Application::CreateRtvAndDsvDescriptorHeaps()
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

void Application::CreateSwapChain()
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

void Application::FlushCommandQueue()
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

ID3D12Resource* Application::CurrentBackBuffer() const
{
    return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Application::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE( mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize );
}

D3D12_CPU_DESCRIPTOR_HANDLE Application::DepthStencilView() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void Application::CreateCommandObjects()
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