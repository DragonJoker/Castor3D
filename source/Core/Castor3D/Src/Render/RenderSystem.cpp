#include "RenderSystem.hpp"

#include <Core/Renderer.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderSystem::RenderSystem( Engine & engine
		, String const & name
		, bool topDown )
		: OwnedBy< Engine >{ engine }
		, m_name{ name }
		, m_initialised{ false }
		, m_topDown{ topDown }
		, m_gpuInformations{}
		, m_gpuBufferPool{ *this }
	{
	}

	RenderSystem::~RenderSystem()
	{
	}

	void RenderSystem::initialise( GpuInformations && p_informations )
	{
		m_gpuInformations = std::move( p_informations );

		if ( !m_initialised )
		{
			auto & device = *getMainDevice();
			StringStream stream( makeStringStream() );
			stream << ( device.getProperties().apiVersion >> 22 ) << cuT( "." ) << ( ( device.getProperties().apiVersion >> 12 ) & 0x0FFF );

			m_gpuInformations.setRenderer( device.getProperties().deviceName );
			m_gpuInformations.setVersion( stream.str() );
			m_gpuInformations.setShaderLanguageVersion( device.getShaderVersion() );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, device.getRenderer().getFeatures().hasStorageBuffers );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eStereoRendering, false );

			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eCompute, device.getRenderer().getFeatures().hasComputeShaders );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationControl, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationEvaluation, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eGeometry, device.getFeatures().geometryShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eFragment, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eVertex, true );

			m_initialised = true;
		}

		Logger::logInfo( cuT( "Vendor: " ) + m_gpuInformations.getVendor() );
		Logger::logInfo( cuT( "Renderer: " ) + m_gpuInformations.getRenderer() );
		Logger::logInfo( cuT( "Version: " ) + m_gpuInformations.getVersion() );
		//m_gpuInformations.removeFeature( GpuFeature::eShaderStorageBuffers );
	}

	void RenderSystem::cleanup()
	{
		m_mainDevice.reset();

#if C3D_TRACE_OBJECTS

		m_tracker.reportTracked();

#endif
	}

	void RenderSystem::pushScene( Scene * p_scene )
	{
		m_stackScenes.push( p_scene );
	}

	void RenderSystem::popScene()
	{
		m_stackScenes.pop();
	}

	Scene * RenderSystem::getTopScene()const
	{
		Scene * result = nullptr;

		if ( !m_stackScenes.empty() )
		{
			result = m_stackScenes.top();
		}

		return result;
	}

	glsl::GlslWriter RenderSystem::createGlslWriter()
	{
		return glsl::GlslWriter{ glsl::GlslWriterConfig{ m_gpuInformations.getShaderLanguageVersion()
			, true
			, true
			, m_gpuInformations.hasShaderStorageBuffers()
			, false } };
	}

	GpuBufferOffset RenderSystem::getBuffer( renderer::BufferTarget type
		, uint32_t size
		, renderer::MemoryPropertyFlags flags )
	{
		return m_gpuBufferPool.getGpuBuffer( type
			, size
			, flags );
	}

	void RenderSystem::putBuffer( renderer::BufferTarget type
		, GpuBufferOffset const & bufferOffset )
	{
		m_gpuBufferPool.putGpuBuffer( type
			, bufferOffset );
	}

	void RenderSystem::cleanupPool()
	{
		m_gpuBufferPool.cleanup();
	}
	
	renderer::DevicePtr RenderSystem::createDevice( renderer::WindowHandle && handle
		, uint32_t gpu )
	{
		renderer::DevicePtr result = m_renderer->createDevice( m_renderer->createConnection( gpu, std::move( handle ) ) );
		return result;
	}

	castor::Matrix4x4r RenderSystem::getFrustum( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer->frustum( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getPerspective( float radiansFovY
		, float aspect
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer->perspective( radiansFovY, aspect, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getOrtho( float left
		, float right
		, float bottom
		, float top
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer->ortho( left, right, bottom, top, zNear, zFar ) );
	}

	castor::Matrix4x4r RenderSystem::getInfinitePerspective( float radiansFovY
		, float aspect
		, float zNear )const
	{
		return convert( m_renderer->infinitePerspective( radiansFovY, aspect, zNear ) );
	}
}
