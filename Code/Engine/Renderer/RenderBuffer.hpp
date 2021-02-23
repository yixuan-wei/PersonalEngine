#pragma once

struct ID3D11Buffer;
class RenderContext;

typedef unsigned int uint;
#define BIT_FLAG(b)  (1<<(b))

enum eRenderBufferUsageBit : uint
{
	VERTEX_BUFFER_BIT   = BIT_FLAG(0), //A02
	INDEX_BUFFER_BIT    = BIT_FLAG(1), //A05
	UNIFORM_BUFFER_BIT  = BIT_FLAG(2), //A03

	//others exist
};
typedef uint RenderBufferUsage; //Indication for bit manipulation, and better readability

enum eRenderMemoryHint:uint
{
	//MEMORY_HINT_STATIC,
	MEMORY_HINT_GPU,     //GPU can read/write, CPU can't touch
	MEMORY_HINT_DYNAMIC, //GPU memory (r/w), changes OFTEN from CPU, "Map" memory
	MEMORY_HINT_STAGING, //Cpu r/w, copy from CPU/Gpu
};

//////////////////////////////////////////////////////////////////////////
//GPU allocator
class RenderBuffer
{
public:
	RenderBuffer( RenderContext* owner, RenderBufferUsage usage, eRenderMemoryHint memHint );
	~RenderBuffer();

	bool Update( void const* data, size_t dataByteSize, size_t elementSize );

	ID3D11Buffer* GetHandle() const { return m_handle; }

private:
	bool IsCompatible( size_t dataByteSize, size_t elementByteSize );
	bool Create(size_t dataByteSize, size_t elementByteSize);
	void CleanUp();

public:
	RenderContext* m_owner;
	ID3D11Buffer* m_handle;

	RenderBufferUsage m_usage;
	eRenderMemoryHint m_memHint;

	size_t m_bufferByteSize;
	size_t m_elementByteSize;
};
