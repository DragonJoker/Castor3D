#include "D3D11RenderSystem/D3D11RenderSystem.hpp"

#include <Castor3D/Engine.hpp>

#include <Ashes/Core/Renderer.hpp>

#include <CompilerHlsl/compileHlsl.hpp>
//#include <CompilerSpirV/compileSpirV.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include <D3D11Renderer/Core/D3D11CreateRenderer.hpp>

using namespace castor;

namespace D3D11Render
{
	String RenderSystem::Name = cuT( "Direct3D 11 Renderer" );
	String RenderSystem::Type = cuT( "direct3d11" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, castor::String const & appName
		, bool enableValidation )
		: castor3d::RenderSystem( engine, Name, false, true, true, true )
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
		std::string hlsl;

		if ( module.shader )
		{
			hlsl = hlsl::compileHlsl( *module.shader
				, ast::SpecialisationInfo{}
				, hlsl::HlslConfig
				{
					module.shader->getType(),
					true,
				} );

#if !defined( NDEBUG )

			// Don't do this at home !
			const_cast< castor3d::ShaderModule & >( module ).source = hlsl;

#endif
		}
		else
		{
			hlsl = module.source;
		}

		auto size = hlsl.size() + 1u;
		result.resize( size_t( std::ceil( float( size ) / sizeof( uint32_t ) ) ) );
		std::memcpy( result.data(), hlsl.data(), hlsl.size() );
		return result;
	}
}
