#include "GlRenderSystem.hpp"

#include <Log/Logger.hpp>

#include <Engine.hpp>

#include <Core/GlDevice.hpp>
#include <Core/GlPhysicalDevice.hpp>
#include <Core/GlRenderer.hpp>

using namespace castor;

namespace GlRender
{
	String RenderSystem::Name = cuT( "OpenGL Renderer" );
	String RenderSystem::Type = cuT( "opengl" );

	RenderSystem::RenderSystem( castor3d::Engine & engine )
		: castor3d::RenderSystem( engine, Name )
	{
		renderer::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logDebug( msg );
			}
			else
			{
				Logger::logDebugNoNL( msg );
			}
		} );
		renderer::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logInfo( msg );
			}
			else
			{
				Logger::logInfoNoNL( msg );
			}
		} );
		renderer::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logWarning( msg );
			}
			else
			{
				Logger::logWarningNoNL( msg );
			}
		} );
		renderer::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
		{
			if ( newLine )
			{
				Logger::logError( msg );
			}
			else
			{
				Logger::logErrorNoNL( msg );
			}
		} );
		m_renderer = std::make_unique< gl_renderer::Renderer >( renderer::Renderer::Configuration
		{
			"Castor3D",
			"Castor3D",
#ifdef NDEBUG
			false,
#else
			true,
#endif
		} );
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine )
	{
		return std::make_unique< RenderSystem >( engine );
	}

	void RenderSystem::doInitialise()
	{
		if ( !m_initialised )
		{
			auto & device = static_cast< gl_renderer::PhysicalDevice const & >( getMainDevice()->getPhysicalDevice() );
			StringStream stream;
			int major = device.getProperties().apiVersion >> 22;
			int minor = ( device.getProperties().apiVersion >> 12 ) & 0x0FFF;
			stream << major << cuT( "." ) << minor;

			m_gpuInformations.setRenderer( device.getProperties().deviceName );
			m_gpuInformations.setVersion( stream.str() );
			m_gpuInformations.setShaderLanguageVersion( device.getGlslVersion() );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eConstantsBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eTextureBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eInstancing, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eAccumulationBuffer, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eNonPowerOfTwoTextures, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eAtomicCounterBuffers, true );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eImmutableTextureStorage, device.findAll( { "GL_ARB_texture_view", "GL_ARB_texture_storage" } ) );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eShaderStorageBuffers, device.find( "GL_ARB_shader_storage_buffer_object" ) );
			m_gpuInformations.updateFeature( castor3d::GpuFeature::eTransformFeedback, true );

			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eCompute, device.find( "GL_ARB_gpu_shader5" ) );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationControl, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eTessellationEvaluation, device.getFeatures().tessellationShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eGeometry, device.getFeatures().geometryShader );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eFragment, true );
			m_gpuInformations.useShaderType( renderer::ShaderStageFlag::eVertex, true );
			m_initialised = true;
		}
	}

	void RenderSystem::doCleanup()
	{
		m_initialised = false;
	}
}
