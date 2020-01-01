#include "GlRenderSystem/GlRenderSystem.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

using namespace castor;

namespace castor3d::gl
{
	String RenderSystem::Name = cuT( "OpenGL Renderer" );
	String RenderSystem::Type = cuT( "gl" );

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
			, std::move( desc ) );
	}

	castor3d::SpirVShader RenderSystem::doCompileShader( castor3d::ShaderModule const & module )const
	{
		castor3d::SpirVShader result;

		if ( module.shader )
		{
			result.text = glsl::compileGlsl( *module.shader
				, ast::SpecialisationInfo{}
				, glsl::GlslConfig
				{
					module.shader->getType(),
					430,
					false,
					true,
					true,
					true,
					true,
					true,
					true,
				} );
		}
		else
		{
			result.text = module.source;
		}

		auto size = result.text.size() + 1u;
		result.spirv.resize( size_t( std::ceil( float( size ) / sizeof( uint32_t ) ) ) );
		std::memcpy( result.spirv.data(), result.text.data(), result.text.size() );
		return result;
	}
}
