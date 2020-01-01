#include "D3D11RenderSystem/D3D11RenderSystem.hpp"

#include <Castor3D/Engine.hpp>

#include <CompilerHlsl/compileHlsl.hpp>
//#include <CompilerSpirV/compileSpirV.hpp>

#include <CastorUtils/Log/Logger.hpp>

using namespace castor;

namespace D3D11Render
{
	String RenderSystem::Name = cuT( "Direct3D 11 Renderer" );
	String RenderSystem::Type = cuT( "d3d11" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, AshPluginDescription desc )
		: castor3d::RenderSystem( engine, std::move( desc ), true, true, true )
	{
		const_cast< AshPluginDescription & >( m_desc ).features.hasStorageBuffers = VK_FALSE;
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
			result.text = hlsl::compileHlsl( *module.shader
				, ast::SpecialisationInfo{}
				, hlsl::HlslConfig
				{
					module.shader->getType(),
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
