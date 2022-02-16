#pragma once
#include "Prerequisites.h"
#include <cstdlib>

constexpr int INPUT_LAYOUT_NUM_MAX = 10;

struct PipelineStateObject {
    ID3D12RootSignature* rootSignature;
    ID3DBlob* vsByteCode;
    ID3DBlob* psByteCode;
    ID3D12PipelineState* pso;
    D3D12_INPUT_ELEMENT_DESC inputLayout[INPUT_LAYOUT_NUM_MAX];

    PipelineStateObject()
    {
        Reset();
    }

    ~PipelineStateObject()
    {
        SafeRelease( vsByteCode );
        SafeRelease( psByteCode );
        SafeRelease( rootSignature );
        SafeRelease( pso );
        memset( inputLayout, 0, sizeof( inputLayout ) );
    }

    void Reset()
    {
        rootSignature = nullptr;
        vsByteCode = nullptr;
        psByteCode = nullptr;
        pso = nullptr;
        memset( inputLayout, 0, sizeof( inputLayout ) );
    }
};

enum class PipelineStateObjectID {
    Default,
    Count,
};

void CreatePipelineStateObjects( ID3D12Device* device );
void DestroyPipelineStateObjects();

const PipelineStateObject& GetPipelineStateObject( PipelineStateObjectID id );
