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
#include "Shader/TestUniformBufferBinding.hpp"
#include "Shader/TestShaderObject.hpp"
#include "Shader/TestShaderProgram.hpp"
#include "Texture/TestSampler.hpp"
#include "Texture/TestTextureStorage.hpp"
#include "Texture/TestTexture.hpp"

#include <Texture/TextureStorage.hpp>

using namespace castor;
using namespace castor3d;

namespace TestRender
{
	String TestRenderSystem::Type = cuT( "test" );
	String TestRenderSystem::Name = cuT( "Test Renderer" );

	TestRenderSystem::TestRenderSystem( Engine & engine )
		: RenderSystem{ engine, Name }
	{
		RenderSystem::m_gpuInformations.addFeature( GpuFeature::eNonPowerOfTwoTextures );
		RenderSystem::m_gpuInformations.addFeature( GpuFeature::eTextureBuffers );
		RenderSystem::m_gpuInformations.addFeature( GpuFeature::eConstantsBuffers );
		RenderSystem::m_gpuInformations.setShaderLanguageVersion( 450 );
	}

	TestRenderSystem::~TestRenderSystem()
	{
	}

	RenderSystemUPtr TestRenderSystem::create( Engine & engine )
	{
		return std::make_unique< TestRenderSystem >( engine );
	}

	ContextSPtr TestRenderSystem::createContext()
	{
		return std::make_shared< TestContext >( *this );
	}

	GeometryBuffersSPtr TestRenderSystem::createGeometryBuffers( Topology p_topology, ShaderProgram const & p_program )
	{
		return std::make_shared< TestGeometryBuffers >( p_topology, p_program );
	}

	RenderPipelineUPtr TestRenderSystem::createRenderPipeline(
		DepthStencilState && p_dsState,
		RasteriserState && p_rsState,
		BlendState && p_bdState,
		MultisampleState && p_msState,
		ShaderProgram & p_program,
		PipelineFlags const & p_flags )
	{
		return std::make_unique< TestRenderPipeline >( *this, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags );
	}

	ComputePipelineUPtr TestRenderSystem::createComputePipeline( ShaderProgram & p_program )
	{
		return std::make_unique< TestComputePipeline >( *this, p_program );
	}

	SamplerSPtr TestRenderSystem::createSampler( castor::String const & p_name )
	{
		return std::make_shared< TestSampler >( this, p_name );
	}

	ShaderProgramSPtr TestRenderSystem::createShaderProgram()
	{
		return std::make_shared< TestShaderProgram >( *this );
	}

	UniformBufferBindingUPtr TestRenderSystem::createUniformBufferBinding( UniformBuffer & p_ubo
		, castor3d::ShaderProgram const & p_program )
	{
		return std::make_unique< TestUniformBufferBinding >( p_ubo, p_program );
	}

	TransformFeedbackUPtr TestRenderSystem::createTransformFeedback( castor3d::BufferDeclaration const & p_computed, castor3d::Topology p_topology, castor3d::ShaderProgram & p_program )
	{
		return std::make_unique< TestTransformFeedback >( *this, p_computed, p_topology, p_program );
	}

	TextureLayoutSPtr TestRenderSystem::createTexture( TextureType p_type, AccessTypes const & cpuAccess, AccessTypes const & gpuAccess )
	{
		return std::make_shared< TestTexture >( *this, p_type, cpuAccess, gpuAccess );
	}

	TextureLayoutSPtr TestRenderSystem::createTexture( TextureType p_type, AccessTypes const & cpuAccess, AccessTypes const & gpuAccess, uint32_t mipmapCount )
	{
		return std::make_shared< TestTexture >( *this, p_type, cpuAccess, gpuAccess, mipmapCount );
	}

	TextureLayoutSPtr TestRenderSystem::createTexture( TextureType p_type, AccessTypes const & cpuAccess, AccessTypes const & gpuAccess, PixelFormat p_format, Size const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, cpuAccess, gpuAccess, p_format, p_size );
	}

	TextureLayoutSPtr TestRenderSystem::createTexture( TextureType p_type, AccessTypes const & cpuAccess, AccessTypes const & gpuAccess, PixelFormat p_format, Point3ui const & p_size )
	{
		return std::make_shared< TestTexture >( *this, p_type, cpuAccess, gpuAccess, p_format, p_size );
	}

	TextureStorageUPtr TestRenderSystem::createTextureStorage( TextureStorageType p_type, TextureLayout & p_layout, AccessTypes const & cpuAccess, AccessTypes const & gpuAccess )
	{
		return std::make_unique< TestTextureStorage >( *this, p_type, p_layout, cpuAccess, gpuAccess );
	}

	FrameBufferSPtr TestRenderSystem::createFrameBuffer()
	{
		return std::make_shared< TestFrameBuffer >( *getEngine() );
	}

	BackBuffersSPtr TestRenderSystem::createBackBuffers()
	{
		return std::make_shared< TestBackBuffers >( *getEngine() );
	}

	GpuQueryUPtr TestRenderSystem::createQuery( QueryType p_type )
	{
		return std::make_unique< TestQuery >( *this, p_type );
	}

	IViewportImplUPtr TestRenderSystem::createViewport( Viewport & p_viewport )
	{
		return std::make_unique< TestViewport >( *this, p_viewport );
	}

	void TestRenderSystem::doInitialise()
	{
	}

	void TestRenderSystem::doCleanup()
	{
	}

	GpuBufferSPtr TestRenderSystem::doCreateBuffer( BufferType p_type )
	{
		return std::make_shared< TestBuffer >( *this, p_type );
	}
}
