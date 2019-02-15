#include "Gl4RenderSystem.hpp"

#include <Engine.hpp>
#include <Shader/GlslToSpv.hpp>

#include <Ashes/Core/Renderer.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <Log/Logger.hpp>

#include <Gl4Renderer/Core/GlCreateRenderer.hpp>

using namespace castor;

namespace Gl4Render
{
	String RenderSystem::Name = cuT( "OpenGL 4.x Renderer" );
	String RenderSystem::Type = cuT( "opengl4" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
		: castor3d::RenderSystem( engine, Name, false, false, false )
	{
		ashes::Logger::setTraceCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					Logger::logTrace( msg );
				}
				else
				{
					Logger::logTraceNoNL( msg );
				}
			} );
		ashes::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
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
		ashes::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
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
		ashes::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
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
		ashes::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
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
		m_renderer.reset( createRenderer( ashes::Renderer::Configuration
			{
				string::stringCast< char >( appName ),
				"Castor3D",
				enableValidation,
			} ) );
		Logger::logInfo( cuT( "Using " ) + Name );
		auto & gpu = m_renderer->getPhysicalDevice( 0u );
		m_memoryProperties = gpu.getMemoryProperties();
		m_properties = gpu.getProperties();
		m_features = gpu.getFeatures();
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
	{
		return std::make_unique< RenderSystem >( engine
			, appName
			, enableValidation );
	}

	castor3d::UInt32Array RenderSystem::compileShader( castor3d::ShaderModule const & module )
	{
		castor3d::UInt32Array result;
		std::string glsl;

		if ( module.shader )
		{
			glsl = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					module.shader->getType(),
					m_renderer->getPhysicalDevice( 0u ).getShaderVersion(),
					false,
					true,
					true,
					true,
					true,
					true,
				} );
		}
		else
		{
			glsl = module.source;
		}

		//result = castor3d::compileGlslToSpv( *getCurrentDevice()
		//	, module.stage
		//	, glsl );

		auto size = glsl.size() + 1u;
		result.resize( size_t( std::ceil( float( size ) / sizeof( uint32_t ) ) ) );
		std::memcpy( result.data(), glsl.data(), glsl.size() );
		return result;
	}
}
