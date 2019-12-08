#include "TestRenderSystem/TestRenderSystem.hpp"

#include <Castor3D/Engine.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <Core/CastorUtils/Log/Logger.hpp>

using namespace castor;

namespace TestRender
{
	String RenderSystem::Name = cuT( "Test Renderer" );
	String RenderSystem::Type = cuT( "test" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, AshPluginDescription desc )
		: castor3d::RenderSystem( engine, std::move( desc ), true, true, true )
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
		getEngine()->getRenderersList().selectPlugin( m_desc.name );
		Logger::logInfo( cuT( "Using " ) + Name );
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, AshPluginDescription desc )
	{
		return std::make_unique< RenderSystem >( engine, std::move( desc ) );
	}

	castor3d::UInt32Array RenderSystem::compileShader( castor3d::ShaderModule const & module )const
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
					460,
					true,
					false,
					false,
					true,
					true,
					true,
					true,
				} );

#if !defined( NDEBUG )

			// Don't do this at home !
			const_cast< castor3d::ShaderModule & >( module ).source = glsl;

#endif
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
