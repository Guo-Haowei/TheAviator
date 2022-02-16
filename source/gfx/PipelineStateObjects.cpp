#include "PipelineStateObjects.h"

#include "base/Log.h"
#include "d3dx12.h"

#include <d3dcompiler.h>
#include <wrl.h>

#include <cassert>
#include <cstdlib>
#include <string>
#include <vector>
using Microsoft::WRL::ComPtr;
using std::string;
using std::vector;
using std::wstring;

static struct
{
    vector<PipelineStateObject> PSOs;
} s_psoGlob;

// TODO: refactor
ID3D12RootSignature* s_rootSignature;

static wstring BuildShaderPath( const char* shaderFileName )
{
    string fullpath( PROJECT_SOURCE_DIR );
    fullpath.append( "source/shaders/" );
    fullpath.append( shaderFileName );
    return wstring( fullpath.begin(), fullpath.end() );
}

static ID3DBlob* CompileShader( const wstring& filename,
                                const D3D_SHADER_MACRO* defines,
                                const char* entry,
                                const char* target )
{
    UINT compileFlags = 0;
    // TODO: configure debug
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

    ID3DBlob* byteCode = nullptr;
    ComPtr<ID3DBlob> errors;
    const HRESULT hr = D3DCompileFromFile( filename.c_str(),
                                           defines,
                                           D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                           entry,
                                           target,
                                           compileFlags,
                                           0,
                                           &byteCode,
                                           &errors );

    if ( hr != S_OK )
    {
        const char* error = errors ? (const char*)errors->GetBufferPointer() : "";
        LOGF( "failed to compile shader '%S'\ndetails: %s", filename.c_str(), error );
    }
    else
    {
        LOG( "compiling shader '%S'", filename.c_str() );
    }

    return byteCode;
}

void CreatePipelineStateObjects( ID3D12Device* device )
{
    assert( device );

    s_psoGlob.PSOs.resize( static_cast<int>( PipelineStateObjectID::Count ) );
    const wstring shader = BuildShaderPath( "main.hlsl" );

    int idx = 0;
    PipelineStateObject& pso = s_psoGlob.PSOs[idx];

    pso.vsByteCode = CompileShader( shader.c_str(), nullptr, "VS", "vs_5_0" );
    pso.psByteCode = CompileShader( shader.c_str(), nullptr, "PS", "ps_5_0" );

    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.InputLayout = { inputLayout, ARRAYSIZE( inputLayout ) };
    desc.pRootSignature = s_rootSignature;
    desc.VS = {
        reinterpret_cast<BYTE*>( pso.vsByteCode->GetBufferPointer() ),
        pso.vsByteCode->GetBufferSize()
    };
    desc.PS = {
        reinterpret_cast<BYTE*>( pso.psByteCode->GetBufferPointer() ),
        pso.psByteCode->GetBufferSize()
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

    DX_CALL( device->CreateGraphicsPipelineState( &desc, IID_PPV_ARGS( &pso.pso ) ) );
}

void DestroyPipelineStateObjects()
{
    s_psoGlob.PSOs.clear();
}

const PipelineStateObject& GetPipelineStateObject( PipelineStateObjectID id )
{
    assert( s_psoGlob.PSOs.size() > (size_t)id );
    return s_psoGlob.PSOs[(size_t)id];
}
