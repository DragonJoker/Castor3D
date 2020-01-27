#include "GlRenderSystem/GlRenderSystem.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/GlslToSpv.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include <CompilerGlsl/compileGlsl.hpp>

namespace castor3d::gl
{
	castor::String RenderSystem::Name = cuT( "OpenGL Renderer" );
	castor::String RenderSystem::Type = cuT( "gl" );

	RenderSystem::RenderSystem( castor3d::Engine & engine
		, AshPluginDescription desc )
		: castor3d::RenderSystem{ engine, std::move( desc ), false, false, false }
	{
		ashes::Logger::setTraceCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					castor::Logger::logTrace( msg );
				}
				else
				{
					castor::Logger::logTraceNoNL( msg );
				}
			} );
		ashes::Logger::setDebugCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					castor::Logger::logDebug( msg );
				}
				else
				{
					castor::Logger::logDebugNoNL( msg );
				}
			} );
		ashes::Logger::setInfoCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					castor::Logger::logInfo( msg );
				}
				else
				{
					castor::Logger::logInfoNoNL( msg );
				}
			} );
		ashes::Logger::setWarningCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					castor::Logger::logWarning( msg );
				}
				else
				{
					castor::Logger::logWarningNoNL( msg );
				}
			} );
		ashes::Logger::setErrorCallback( []( std::string const & msg, bool newLine )
			{
				if ( newLine )
				{
					castor::Logger::logError( msg );
				}
				else
				{
					castor::Logger::logErrorNoNL( msg );
				}
			} );
		getEngine()->getRenderersList().selectPlugin( m_desc.name );
		castor::Logger::logInfo( cuT( "Using " ) + Name );
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
					m_desc.features.maxShaderLanguageVersion,
					false, // vulkanGlsl
					true, // flipVertY
					true, // fixupClipDepth
					m_desc.features.maxShaderLanguageVersion > 430, // hasStd430Layout
					m_desc.features.hasStorageBuffers != VK_FALSE, // hasShaderStorageBuffers
					m_desc.features.maxShaderLanguageVersion >= 430, // hasDescriptorSets
					m_desc.features.hasBaseInstance != VK_FALSE, // hasBaseInstance
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
