#pragma once
#include "core/MathHelper.h"

#include "d3dUtil.h"
#include "UploadBuffer.h"

struct ObjectConstants
{
    mat4 Model = mat4(1.0f);
};

struct PassConstants
{
    mat4 View = mat4(1.0f);
    mat4 InvView = mat4(1.0f);
    mat4 Proj = mat4(1.0f);
    mat4 InvProj = mat4(1.0f);
    mat4 ViewProj = mat4(1.0f);
    mat4 InvViewProj = mat4(1.0f);
    vec3 EyePosW = { 0.0f, 0.0f, 0.0f };
    float cbPerObjectPad1 = 0.0f;
    vec2 RenderTargetSize = { 0.0f, 0.0f };
    vec2 InvRenderTargetSize = { 0.0f, 0.0f };
    float NearZ = 0.0f;
    float FarZ = 0.0f;
};

// Stores the resources needed for the CPU to build the command lists
// for a frame.  
struct FrameResource
{
public:
    
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    ~FrameResource();

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 Fence = 0;
};