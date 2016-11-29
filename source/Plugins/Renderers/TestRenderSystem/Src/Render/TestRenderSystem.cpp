#include "TestRenderSystem.hpp"

#include "FrameBuffer/TestBackBuffers.hpp"
#include "FrameBuffer/TestFrameBuffer.hpp"
#include "Mesh/TestVertexBuffer.hpp"
#include "Mesh/TestIndexBuffer.hpp"
#include "Mesh/TestGeometryBuffers.hpp"
#include "Miscellaneous/TestQuery.hpp"
#include "Miscellaneous/TestTransformFeedback.hpp"
#include "Miscellaneous/TestComputePipeline.hpp"
#include "Render/TestContext.hpp"
#include "Render/TestRenderPipeline.hpp"
#include "Render/TestViewport.hpp"
#include "Shader/TestAtomicCounterBuffer.hpp"
#include "Shader/TestFrameVariableBuffer.hpp"
#include "Shader/TestShaderObject.hpp"
#include "Shader/TestShaderProgram.hpp"
#include "Shader/TestShaderStorageBuffer.hpp"
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
		RenderSystem::m_gpuInformations.AddFeature( GpuFeature::eNonPowerOfTwoTextures );
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

	GeometryBuffersSPtr TestRenderSystem::CreateGeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
	{
		return std::make_shared< TestGeometryBuffers >( p_topology, p_program );
	}

	RenderPipelineUPtr TestRenderSystem::CreateRenderPipeline(
		DepthStencilState && p_dsState,
		RasteriserState && p_rsState,
		BlendState && p_bdState,
		MultisampleState && p_msState,
		ShaderProgram & p_program,
		PipelineFlags const & p_flags )
	{
		return std::make_unique< TestRenderPipeline >( *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	ComputePipelineUPtr TestRenderSystem::CreateComputePipeline( ShaderProgram & p_program )
	{
		return std::make_unique< TestComputePipeline >( *this, p_program );
	}

	SamplerSPtr TestRenderSystem::CreateSampler( Castor::String const & p_name )
	{
		return std::make_shared< TestSampler >( this, p_name );
	}

	ShaderProgramSPtr TestRenderSystem::CreateShaderProgram()
	{
		return std::make_shared< TestShaderProgram >( *this );
	}

	TransformFeedbackUPtr TestRenderSystem::CreateTransformFeedback( Castor3D::BufferDeclaration const & p_computed, Castor3D::Topology p_topology, Castor3D::ShaderProgram & p_program )
	{
		return std::make_unique< TestTransformFeedback >( *this, p_computed, p_topology, p_program );
	}

	std::unique_ptr< GpuBuffer< uint32_t > > TestRenderSystem::CreateIndexBuffer()
	{
		return std::make_unique< TestIndexBuffer >( *this );
	}

	std::unique_ptr< GpuBuffer< uint8_t > > TestRenderSystem::CreateVertexBuffer()
	{
		return std::make_unique< TestVertexBuffer >( *this );
	}

	std::unique_ptr< GpuBuffer< uint8_t > > TestRenderSystem::CreateStorageBuffer()
	{
		return std::make_unique< TestShaderStorageBuffer >( *this );
	}

	std::unique_ptr< GpuBuffer< uint32_t > > TestRenderSystem::CreateAtomicCounterBuffer()
	{
		return std::make_unique< TestAtomicCounterBuffer >( *this );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, FlagCombination< AccessType > const & p_cpuAccess, FlagCombination< AccessType > const & p_gpuAccess )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, FlagCombination< AccessType > const & p_cpuAccess, FlagCombination< AccessType > const & p_gpuAccess, PixelFormat p_format, Size const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, FlagCombination< AccessType > const & p_cpuAccess, FlagCombination< AccessType > const & p_gpuAccess, PixelFormat p_format, Point3ui const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr TestRenderSystem::CreateTextureStorage( TextureStorageType p_type, TextureLayout & p_layout, FlagCombination< AccessType > const & p_cpuAccess, FlagCombination< AccessType > const & p_gpuAccess )
	{
		return std::make_unique< TestTextureStorage >( *this, p_type, p_layout, p_cpuAccess, p_gpuAccess );
	}

	FrameBufferSPtr TestRenderSystem::CreateFrameBuffer()
	{
		return std::make_shared< TestFrameBuffer >( *GetEngine() );
	}

	BackBuffersSPtr TestRenderSystem::CreateBackBuffers()
	{
		return std::make_shared< TestBackBuffers >( *GetEngine() );
	}

	GpuQueryUPtr TestRenderSystem::CreateQuery( QueryType p_type )
	{
		return std::make_unique< TestQuery >( *this, p_type );
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
