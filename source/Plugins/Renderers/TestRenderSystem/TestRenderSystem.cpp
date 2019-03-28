#include "TestRenderSystem/TestRenderSystem.hpp"

#include <Castor3D/Engine.hpp>

#include <Ashes/Core/Renderer.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include <TestRenderer/Core/TestCreateRenderer.hpp>

using namespace castor;

namespace TestRender
{
	String RenderSystem::Name = cuT( "Test Renderer" );
	String RenderSystem::Type = cuT( "test" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
		: castor3d::RenderSystem( engine, Name, true, true, true )
	{
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
					false,
					false,
					false,
					false,
					false,
				} );
		}
		else
		{
			glsl = module.source;
		}

		auto size = glsl.size() + 1u;
		result.resize( size_t( std::ceil( float( size ) / sizeof( uint32_t ) ) ) );
		std::memcpy( result.data(), glsl.data(), glsl.size() );
		return result;
	}
}
