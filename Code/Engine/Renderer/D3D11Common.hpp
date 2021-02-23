#if !defined(WIN32_LEAN_AND_MEAN) 
#define WIN32_LEAN_AND_MEAN
#endif

#define RENDER_DEBUG
#define DX_SAFE_RELEASE(ptr) if(nullptr!=ptr){ptr->Release();ptr=nullptr;}

#define INITGUID
#include <d3d11.h>  // d3d11 specific objects
#include <dxgi.h>   // shared library used across multiple dx graphical interfaces
#include <dxgidebug.h>  // debug utility (mostly used for reporting and analytics)

//////////////////////////////////////////////////////////////////////////
typedef unsigned int RenderBufferUsage;
enum eRenderMemoryHint : unsigned int;
enum eBufferFormatType : int;
enum eCompareFunc : int;
enum eCullMode : int;
enum eFillMode : int;

UINT                    ToDXUsage(RenderBufferUsage usage);
D3D11_USAGE             ToDXMemoryUsage( eRenderMemoryHint hint );
D3D11_CULL_MODE         ToDXCullMode( eCullMode mode );
D3D11_FILL_MODE         ToDXFillMode( eFillMode mode );
DXGI_FORMAT             ToDXGIFormat(eBufferFormatType type);
D3D11_COMPARISON_FUNC   ToDXCompareFunc(eCompareFunc func);