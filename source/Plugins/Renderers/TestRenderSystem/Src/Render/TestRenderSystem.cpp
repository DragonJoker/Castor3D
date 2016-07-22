#include "TestRenderSystem.hpp"

#include "FrameBuffer/TestBackBuffers.hpp"
#include "FrameBuffer/TestFrameBuffer.hpp"
#include "Mesh/TestVertexBuffer.hpp"
#include "Mesh/TestIndexBuffer.hpp"
#include "Mesh/TestGeometryBuffers.hpp"
#include "Miscellaneous/TestQuery.hpp"
#include "Render/TestContext.hpp"
#include "Render/TestPipeline.hpp"
#include "Render/TestViewport.hpp"
#include "Shader/TestFrameVariableBuffer.hpp"
#include "Shader/TestShaderObject.hpp"
#include "Shader/TestShaderProgram.hpp"
#include "State/TestBlendState.hpp"
#include "State/TestDepthStencilState.hpp"
#include "State/TestRasteriserState.hpp"
#include "Texture/TestSampler.hpp"
#include "Texture/TestTextureStorage.hpp"
#include "Texture/TestTexture.hpp"

#include <Texture/TextureStorage.hpp>

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	String TestRenderSystem::Name = cuT( "test" );

	TestRenderSystem::TestRenderSystem( Engine & p_engine )
		: RenderSystem{ p_engine, Name }
	{
		RenderSystem::m_gpuInformations.AddFeature( GpuFeature::NonPowerOfTwoTextures );
	}

	TestRenderSystem::~TestRenderSystem()
	{
	}

	RenderSystemUPtr TestRenderSystem::Create( Engine & p_engine )
	{
		return std::make_unique< TestRenderSystem >( p_engine );
	}

	ContextSPtr TestRenderSystem::CreateContext()
	{
		return std::make_shared< TestContext >( *this );
	}

	GeometryBuffersSPtr TestRenderSystem::CreateGeometryBuffers( eTOPOLOGY p_topology, ShaderProgram const & p_program )
	{
		return std::make_shared< TestGeometryBuffers >( p_topology, p_program );
	}

	DepthStencilStateSPtr TestRenderSystem::CreateDepthStencilState()
	{
		return std::make_shared< TestDepthStencilState >( this );
	}

	RasteriserStateSPtr TestRenderSystem::CreateRasteriserState()
	{
		return std::make_shared< TestRasteriserState >( this );
	}

	BlendStateSPtr TestRenderSystem::CreateBlendState()
	{
		return std::make_shared< TestBlendState >( this );
	}

	SamplerSPtr TestRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< TestSampler >( this, p_name );
	}

	ShaderProgramSPtr TestRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< TestShaderProgram >( *this );
	}

	std::shared_ptr< GpuBuffer< uint32_t > > TestRenderSystem::CreateIndexBuffer( CpuBuffer<uint32_t> * p_buffer )
	{
		return std::make_shared< TestIndexBuffer >( *this );
	}

	std::shared_ptr< GpuBuffer< uint8_t > > TestRenderSystem::CreateVertexBuffer( CpuBuffer< uint8_t > * p_buffer )
	{
		return std::make_shared< TestVertexBuffer >( *this );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureStorageUPtr TestRenderSystem::CreateTextureStorage( TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return std::make_unique< TestTextureStorage >( *this, p_type, p_image, p_cpuAccess, p_gpuAccess );
	}

	FrameVariableBufferSPtr TestRenderSystem::CreateFrameVariableBuffer( Castor::String const & p_name )
	{
		return std::make_shared< TestFrameVariableBuffer >( p_name, *this );
	}

	FrameBufferSPtr TestRenderSystem::CreateFrameBuffer()
	{
		return std::make_shared< TestFrameBuffer >( *GetEngine() );
	}

	BackBuffersSPtr TestRenderSystem::CreateBackBuffers()
	{
		return std::make_shared< TestBackBuffers >( *GetEngine() );
	}

	GpuQuerySPtr TestRenderSystem::CreateQuery( eQUERY_TYPE p_type )
	{
		return std::make_shared< TestQuery >( *this, p_type );
	}

	IViewportImplUPtr TestRenderSystem::CreateViewport( Viewport & p_viewport )
	{
		return std::make_unique< TestViewport >( *this, p_viewport );
	}

	void TestRenderSystem::DoInitialise()
	{
	}

	void TestRenderSystem::DoCleanup()
	{
	}
}
