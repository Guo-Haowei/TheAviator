//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// General helper code.
//***************************************************************************************

#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>

#include "gfx/d3dx12.h"
#include "gfx/Prerequisites.h"

extern const int gNumFrameResources;

inline void d3dSetDebugName( IDXGIObject* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}
inline void d3dSetDebugName( ID3D12Device* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}
inline void d3dSetDebugName( ID3D12DeviceChild* obj, const char* name )
{
    if ( obj )
    {
        obj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA( name ), name );
    }
}

inline std::wstring AnsiToWString( const std::string& str )
{
    WCHAR buffer[512];
    MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, buffer, 512 );
    return std::wstring( buffer );
}

class d3dUtil {
   public:
    static bool IsKeyDown( int vkeyCode );

    static std::string ToString( HRESULT hr );

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary( const std::wstring& filename );

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer );

    static ID3DBlob* CompileShader( const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target );
};

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index
// buffers so that we can implement the technique described by Figure 6.3.
struct SubmeshGeometry {
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;
};

struct MeshGeometry {
    // Give it a name so we can look it up by name.
    std::string Name;

    // System memory copies.  Use Blobs because the vertex/index format can be generic.
    // It is up to the client to cast appropriately.
    Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;

    // Data about the buffers.
    uint32_t VertexByteStride = 0;
    uint32_t VertexBufferByteSize = 0;
    uint32_t IndexCount = 0;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    // We can free this memory after we finish upload to the GPU.
    void DisposeUploaders()
    {
        VertexBufferUploader = nullptr;
    }
};

#ifndef ReleaseCom
#define ReleaseCom( x )   \
    {                     \
        if ( x )          \
        {                 \
            x->Release(); \
            x = 0;        \
        }                 \
    }
#endif