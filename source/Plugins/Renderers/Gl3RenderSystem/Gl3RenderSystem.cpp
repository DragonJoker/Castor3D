#include "Gl3RenderSystem/Gl3RenderSystem.hpp"

#include <Castor3D/Engine.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

#include <CastorUtils/Log/Logger.hpp>

using namespace castor;

namespace Gl3Render
{
	String RenderSystem::Name = cuT( "OpenGL 3.x Renderer" );
	String RenderSystem::Type = cuT( "opengl3" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, AshPluginDescription desc )
		: castor3d::RenderSystem{ engine, std::move( desc ), false, false, false }
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
		getEngine()->getRenderersList().selectPlugin( m_desc.name );
		Logger::logInfo( cuT( "Using " ) + Name );
	}

	RenderSystem::~RenderSystem()
	{
	}

	castor3d::RenderSystemUPtr RenderSystem::create( castor3d::Engine & engine
		, AshPluginDescription desc )
	{
		return std::make_unique< RenderSystem >( engine
			, desc );
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
					330,
					false,
					true,
					true,
					false,
					false,
					false,
					false,
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
