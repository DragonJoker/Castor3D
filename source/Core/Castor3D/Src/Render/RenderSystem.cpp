#include "RenderSystem.hpp"

#include <Core/Renderer.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	RenderSystem::RenderSystem( Engine & engine
		, String const & name
		, bool topDown
		, bool zeroToOneDepth
		, bool invertedNormals )
		: OwnedBy< Engine >{ engine }
		, m_name{ name }
		, m_initialised{ false }
		, m_topDown{ topDown }
		, m_zeroToOneDepth{ zeroToOneDepth }
		, m_invertedNormals{ invertedNormals }
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
			static std::map< uint32_t, String > vendors
			{
				{ 0x1002, cuT( "AMD" ) },
				{ 0x10DE, cuT( "NVIDIA" ) },
				{ 0x8086, cuT( "INTEL" ) },
				{ 0x13B5, cuT( "ARM" ) },
			};
			auto & device = *getMainDevice();
			StringStream stream( makeStringStream() );
			stream << ( device.getProperties().apiVersion >> 22 ) << cuT( "." ) << ( ( device.getProperties().apiVersion >> 12 ) & 0x0FFF );
			m_gpuInformations.setVendor( vendors[device.getProperties().vendorID] );
			m_gpuInformations.setRenderer( device.getProperties().deviceName );
			m_gpuInformations.setVersion( stream.str() );
			m_gpuInformations.setShaderLanguageVersion( device.getShaderVersion() );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, device.getRenderer().getFeatures().hasStorageBuffers );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eStereoRendering, false );

			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eCompute, device.getRenderer().getFeatures().hasComputeShaders );
			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eTessellationControl, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eTessellationEvaluation, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eGeometry, device.getFeatures().geometryShader );
			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eFragment, true );
			m_gpuInformations.useShaderType( ashes::ShaderStageFlag::eVertex, true );

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

	GpuBufferOffset RenderSystem::getBuffer( ashes::BufferTarget type
		, uint32_t size
		, ashes::MemoryPropertyFlags flags )
	{
		return m_gpuBufferPool.getGpuBuffer( type
			, size
			, flags );
	}

	void RenderSystem::putBuffer( ashes::BufferTarget type
		, GpuBufferOffset const & bufferOffset )
	{
		m_gpuBufferPool.putGpuBuffer( type
			, bufferOffset );
	}

	void RenderSystem::cleanupPool()
	{
		m_gpuBufferPool.cleanup();
	}
	
	ashes::DevicePtr RenderSystem::createDevice( ashes::WindowHandle && handle
		, uint32_t gpu )
	{
		ashes::DevicePtr result = m_renderer->createDevice( m_renderer->createConnection( gpu, std::move( handle ) ) );
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

	castor::Matrix4x4r RenderSystem::getPerspective( castor::Angle const & fovy
		, float aspect
		, float zNear
		, float zFar )const
	{
		return convert( m_renderer->perspective( fovy.radians(), aspect, zNear, zFar ) );
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
