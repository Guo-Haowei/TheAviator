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

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

using glm::mat4;
using glm::vec3;
using glm::vec4;

using Microsoft::WRL::ComPtr;

struct Vertex {
    vec3 Pos;
    vec4 Color;
};

struct ObjectConstants {
    mat4 WorldViewProj{ 1.0f };
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

    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildPSO();

   private:
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    mat4 mWorld{ 1.0f };
    mat4 mView{ 1.0f };
    mat4 mProj{ 1.0f };

    POINT mLastMousePos;
};

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
    if ( !D3DApp::Initialize() )
        return false;

    // Reset the command list to prep for initialization commands.
    DX_CALL( mCommandList->Reset( mDirectCmdListAlloc.Get(), nullptr ) );

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();

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
    mProj = mat4( { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0.5, 0 }, { 0, 0, 0, 1 } ) *
            mat4( { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 1, 1 } ) *
            glm::perspectiveRH_NO( fov, AspectRatio(), 1.0f, 1000.0f );
}

void BoxApp::Update()
{
    float dist = 5.0f;
    mView = glm::lookAtRH( vec3( 0.0f, dist, dist ), vec3( 0.0f ), vec3( 0.0f, 1.0f, 0.0f ) );

    mat4 worldViewProj = mProj * mView * mWorld;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    objConstants.WorldViewProj = worldViewProj;
    mObjectCB->CopyData( 0, objConstants );
}

void BoxApp::Draw()
{
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

    mCommandList->IASetVertexBuffers( 0, 1, &mBoxGeo->VertexBufferView() );
    mCommandList->IASetIndexBuffer( &mBoxGeo->IndexBufferView() );
    mCommandList->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    mCommandList->SetGraphicsRootDescriptorTable( 0, mCbvHeap->GetGPUDescriptorHandleForHeapStart() );

    mCommandList->DrawIndexedInstanced(
        mBoxGeo->DrawArgs["box"].IndexCount,
        1, 0, 0, 0 );

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
#if 0
    if ( ( btnState & MK_LBUTTON ) != 0 )
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = XMConvertToRadians( 0.25f * static_cast<float>( x - mLastMousePos.x ) );
        float dy = XMConvertToRadians( 0.25f * static_cast<float>( y - mLastMousePos.y ) );

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.
        mPhi = MathHelper::Clamp( mPhi, 0.1f, MathHelper::Pi - 0.1f );
    }
    else if ( ( btnState & MK_RBUTTON ) != 0 )
    {
        // Make each pixel correspond to 0.005 unit in the scene.
        float dx = 0.005f * static_cast<float>( x - mLastMousePos.x );
        float dy = 0.005f * static_cast<float>( y - mLastMousePos.y );

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = MathHelper::Clamp( mRadius, 3.0f, 15.0f );
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
#endif
}

void BoxApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    DX_CALL( md3dDevice->CreateDescriptorHeap( &cbvHeapDesc,
                                               IID_PPV_ARGS( &mCbvHeap ) ) );
}

void BoxApp::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>( md3dDevice.Get(), 1, true );

    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize( sizeof( ObjectConstants ) );

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize( sizeof( ObjectConstants ) );

    md3dDevice->CreateConstantBufferView(
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart() );
}

void BoxApp::BuildRootSignature()
{
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Create a single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0 );
    slotRootParameter[0].InitAsDescriptorTable( 1, &cbvTable );

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc( 1, slotRootParameter, 0, nullptr,
                                             D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature( &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                              serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf() );

    if ( errorBlob != nullptr )
    {
        ::OutputDebugStringA( (char*)errorBlob->GetBufferPointer() );
    }
    DX_CALL( hr );

    DX_CALL( md3dDevice->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS( &mRootSignature ) ) );
}

void BoxApp::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    char folder[] = __FILE__;
    char* p = strrchr( folder, '\\' );
    assert( p );
    p[1] = '\0';
    std::string shaderPath( folder );
    shaderPath.append( "shaders/color.hlsl" );
    std::wstring wShaderPath( shaderPath.begin(), shaderPath.end() );

    mvsByteCode = d3dUtil::CompileShader( wShaderPath.c_str(), nullptr, "VS", "vs_5_0" );
    mpsByteCode = d3dUtil::CompileShader( wShaderPath.c_str(), nullptr, "PS", "ps_5_0" );

    mInputLayout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}

void BoxApp::BuildBoxGeometry()
{
    std::array<Vertex, 8> vertices = {
        Vertex{ vec3{ -1.0f, -1.0f, -1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ -1.0f, +1.0f, -1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ +1.0f, +1.0f, -1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ +1.0f, -1.0f, -1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ -1.0f, -1.0f, +1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ -1.0f, +1.0f, +1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ +1.0f, +1.0f, +1.0f }, vec4( 1.0f ) },
        Vertex{ vec3{ +1.0f, -1.0f, +1.0f }, vec4( 1.0f ) }
    };

    std::array<std::uint16_t, 36> indices = {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    const UINT vbByteSize = (UINT)vertices.size() * sizeof( Vertex );
    const UINT ibByteSize = (UINT)indices.size() * sizeof( std::uint16_t );

    mBoxGeo = std::make_unique<MeshGeometry>();
    mBoxGeo->Name = "boxGeo";

    DX_CALL( D3DCreateBlob( vbByteSize, &mBoxGeo->VertexBufferCPU ) );
    CopyMemory( mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize );

    DX_CALL( D3DCreateBlob( ibByteSize, &mBoxGeo->IndexBufferCPU ) );
    CopyMemory( mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize );

    mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer( md3dDevice.Get(),
                                                             mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader );

    mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer( md3dDevice.Get(),
                                                            mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader );

    mBoxGeo->VertexByteStride = sizeof( Vertex );
    mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mBoxGeo->DrawArgs["box"] = submesh;
}

void BoxApp::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory( &psoDesc, sizeof( D3D12_GRAPHICS_PIPELINE_STATE_DESC ) );
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = {
        reinterpret_cast<BYTE*>( mvsByteCode->GetBufferPointer() ),
        mvsByteCode->GetBufferSize()
    };
    psoDesc.PS = {
        reinterpret_cast<BYTE*>( mpsByteCode->GetBufferPointer() ),
        mpsByteCode->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
    psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT );
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? ( m4xMsaaQuality - 1 ) : 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    DX_CALL( md3dDevice->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( &mPSO ) ) );
}