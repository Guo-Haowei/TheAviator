//***************************************************************************************
// BoxApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//
// Shows how to draw a box in Direct3D 12.
//
// Controls:
//   Hold the left mouse button down and move the mouse to rotate.
//   Hold the right mouse button down and move the mouse to zoom in and out.
//***************************************************************************************

#include "d3dApp.h"
#include "UploadBuffer.h"
#include "FrameResource.h"

#include "core/MathHelper.h"
#include "core/Scene.h"

#include <unordered_map>

using std::vector;

using Microsoft::WRL::ComPtr;
const int gNumFrameResources = 3;

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

class BoxApp : public D3DApp {
   public:
    BoxApp( HINSTANCE hInstance );
    BoxApp( const BoxApp& rhs ) = delete;
    BoxApp& operator=( const BoxApp& rhs ) = delete;
    ~BoxApp();

    virtual bool Initialize() override;

   private:
    virtual void OnResize() override;
    virtual void Update() override;
    virtual void Draw() override;

    virtual void OnMouseDown( WPARAM btnState, int x, int y ) override;
    virtual void OnMouseUp( WPARAM btnState, int x, int y ) override;
    virtual void OnMouseMove( WPARAM btnState, int x, int y ) override;

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
};

std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> s_gpuMeshes;

int main()
{
    // Enable run-time memory check for debug builds.
#if defined( DEBUG ) | defined( _DEBUG )
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    BoxApp theApp( GetModuleHandle( NULL ) );
    if ( !theApp.Initialize() )
        return 0;

    return theApp.Run();
}

BoxApp::BoxApp( HINSTANCE hInstance )
    : D3DApp( hInstance )
{
}

BoxApp::~BoxApp()
{
}

bool BoxApp::Initialize()
{
    InitMeshes();

    if ( !D3DApp::Initialize() )
    {
        return false;
    }

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

void BoxApp::OnResize()
{
    D3DApp::OnResize();

    float fov = 0.25f * glm::pi<float>();
    mProj = glm::perspectiveLH_ZO( fov, AspectRatio(), 1.0f, 1000.0f );
}

void BoxApp::Update()
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

void BoxApp::Draw()
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

void BoxApp::OnMouseDown( WPARAM btnState, int x, int y )
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture( mhMainWnd );
}

void BoxApp::OnMouseUp( WPARAM btnState, int x, int y )
{
    ReleaseCapture();
}

void BoxApp::OnMouseMove( WPARAM btnState, int x, int y )
{
    if ( ( btnState & MK_LBUTTON ) != 0 )
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = glm::radians( 0.05f * static_cast<float>( x - mLastMousePos.x ) );
        // float dy = glm::radians( 0.25f * static_cast<float>( y - mLastMousePos.y ) );
    }
}

void BoxApp::BuildDescriptorHeaps()
{
    UINT objCount = (UINT)mRenderItems.size();

    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    UINT numDescriptors = ( objCount + 1 ) * gNumFrameResources;
    printf( "numDesc %u\n\n", numDescriptors );

    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * gNumFrameResources;

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    DX_CALL( mDevice->CreateDescriptorHeap( &cbvHeapDesc, IID_PPV_ARGS( &mCbvHeap ) ) );
}

void BoxApp::BuildRootSignature()
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

void BoxApp::BuildShadersAndInputLayout()
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

void BoxApp::BuildShapeGeometry()
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

void BoxApp::BuildPSOs()
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

void BoxApp::BuildFrameResources()
{
    for ( int i = 0; i < gNumFrameResources; ++i )
    {
        mFrameResources.push_back( std::make_unique<FrameResource>( mDevice.Get(), 1, (UINT)mRenderItems.size() ) );
    }
}

void BoxApp::BuildConstantBufferViews()
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

void BoxApp::DrawRenderItems( ID3D12GraphicsCommandList* cmdList, const std::vector<std::unique_ptr<RenderItem>>& items )
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

void BoxApp::UpdateCamera()
{
}
static float g_angle = 45.0f;
void BoxApp::UpdateObjectCBs()
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

void BoxApp::UpdateMainPassCB()
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

void BoxApp::BuildRenderItems()
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
