#pragma once
#include <type_traits>
#include <d3d12.h>

constexpr DXGI_FORMAT DEFAULT_BACK_BUFFER_FORMATT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DEFAULT_DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
constexpr int NUM_SWAPCHAIN_BUFFER = 2;

template<typename T>
static void SafeRelease( T& ptr )
{
    static_assert( std::is_pointer<T>() );
    if ( ptr )
    {
        ptr->Release();
        ptr = nullptr;
    }
}

namespace internal {
void CheckWin32Result( HRESULT hr, const char* expr, const char* file, int line );
}  // namespace internal

#define DX_CALL( expr ) ::internal::CheckWin32Result( ( expr ), #expr, __FILE__, __LINE__ )
