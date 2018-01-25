#include "RenderSystem.hpp"

#include <Core/Renderer.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderSystem::RenderSystem( Engine & engine, String const & p_name )
		: OwnedBy< Engine >{ engine }
		, m_name{ p_name }
		, m_initialised{ false }
		, m_gpuInformations{}
		, m_gpuBufferPool{ *this }
	{
	}

	RenderSystem::~RenderSystem()
	{
		//m_mainDevice.reset();
	}

	void RenderSystem::initialise( GpuInformations && p_informations )
	{
		m_gpuInformations = std::move( p_informations );
		doInitialise();
		m_gpuInformations.updateMaxShaderModel();
		REQUIRE( m_gpuInformations.getMaxShaderModel() >= ShaderModel::eModel2 );
		Logger::logInfo( cuT( "Vendor: " ) + m_gpuInformations.getVendor() );
		Logger::logInfo( cuT( "Renderer: " ) + m_gpuInformations.getRenderer() );
		Logger::logInfo( cuT( "Version: " ) + m_gpuInformations.getVersion() );
		//m_gpuInformations.removeFeature( GpuFeature::eShaderStorageBuffers );
	}

	void RenderSystem::cleanup()
	{
		//if ( m_mainDevice )
		//{
		//	delete m_mainDevice;
		//}

		doCleanup();

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
			, m_gpuInformations.hasConstantsBuffers()
			, m_gpuInformations.hasTextureBuffers()
			, m_gpuInformations.hasShaderStorageBuffers() } };
	}

	void RenderSystem::setCurrentDevice( renderer::Device const * device )
	{
		m_currentDevices[std::this_thread::get_id()] = device;
	}

	renderer::Device const * RenderSystem::getCurrentDevice()const
	{
		renderer::Device const * result{ nullptr };
		auto it = m_currentDevices.find( std::this_thread::get_id() );

		if ( it != m_currentDevices.end() )
		{
			result = it->second;
		}

		return result;
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
		return m_renderer->createDevice( m_renderer->createConnection( gpu, std::move( handle ) ) );
	}
}
