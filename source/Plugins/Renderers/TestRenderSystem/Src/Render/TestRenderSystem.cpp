#include "TestRenderSystem.hpp"

#include "FrameBuffer/TestBackBuffers.hpp"
#include "FrameBuffer/TestFrameBuffer.hpp"
#include "Mesh/TestBuffer.hpp"
#include "Mesh/TestGeometryBuffers.hpp"
#include "Miscellaneous/TestQuery.hpp"
#include "Miscellaneous/TestTransformFeedback.hpp"
#include "Miscellaneous/TestComputePipeline.hpp"
#include "Render/TestContext.hpp"
#include "Render/TestRenderPipeline.hpp"
#include "Render/TestViewport.hpp"
#include "Shader/TestAtomicCounterBuffer.hpp"
#include "Shader/TestUniformBufferBinding.hpp"
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
	String TestRenderSystem::Type = cuT( "Test Renderer" );

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

	UniformBufferBindingUPtr TestRenderSystem::CreateUniformBufferBinding( UniformBuffer & p_ubo
		, Castor3D::ShaderProgram const & p_program )
	{
		return std::make_unique< TestUniformBufferBinding >( p_ubo, p_program );
	}

	TransformFeedbackUPtr TestRenderSystem::CreateTransformFeedback( Castor3D::BufferDeclaration const & p_computed, Castor3D::Topology p_topology, Castor3D::ShaderProgram & p_program )
	{
		return std::make_unique< TestTransformFeedback >( *this, p_computed, p_topology, p_program );
	}

	std::unique_ptr< GpuBuffer< uint8_t > > TestRenderSystem::CreateUInt8Buffer( BufferType p_type )
	{
		return std::make_unique< TestBuffer< uint8_t > >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint16_t > > TestRenderSystem::CreateUInt16Buffer( BufferType p_type )
	{
		return std::make_unique< TestBuffer< uint16_t > >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< uint32_t > > TestRenderSystem::CreateUInt32Buffer( BufferType p_type )
	{
		return std::make_unique< TestBuffer< uint32_t > >( *this, p_type );
	}

	std::unique_ptr< GpuBuffer< float > > TestRenderSystem::CreateFloatBuffer( BufferType p_type )
	{
		return std::make_unique< TestBuffer< float > >( *this, p_type );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, AccessTypes const & p_cpuAccess, AccessTypes const & p_gpuAccess )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, AccessTypes const & p_cpuAccess, AccessTypes const & p_gpuAccess, PixelFormat p_format, Size const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr TestRenderSystem::CreateTexture( TextureType p_type, AccessTypes const & p_cpuAccess, AccessTypes const & p_gpuAccess, PixelFormat p_format, Point3ui const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr TestRenderSystem::CreateTextureStorage( TextureStorageType p_type, TextureLayout & p_layout, AccessTypes const & p_cpuAccess, AccessTypes const & p_gpuAccess )
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
