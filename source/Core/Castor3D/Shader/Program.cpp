#include "Castor3D/Shader/Program.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Stream/StreamPrefixManipulators.hpp>

CU_ImplementSmartPtr( castor3d, ShaderProgram )

namespace castor3d
{
	namespace shdprog
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream
			, castor::format::BasePrefixer< CharType, PrefixType > const & prefix )
		{
			auto * sbuf = dynamic_cast< castor::format::BasicPrefixBuffer< castor::format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				castor::format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( castor::format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}

		static bool doAddModule( VkShaderStageFlagBits stage
			, std::string const & name
			, std::map< VkShaderStageFlagBits, ShaderModule > & modules )
		{
			bool added = false;

			if ( modules.find( stage ) == modules.end() )
			{
				modules.emplace( stage, ShaderModule{ stage, name } );
				added = true;
			}

			return added;
		}

		static ashes::PipelineShaderStageCreateInfo loadShader( RenderDevice const & device
			, ShaderProgram::CompiledShader const & compiled
			, VkShaderStageFlagBits stage )
		{
			return makeShaderState( *device
				, stage
				, *compiled.shader
				, compiled.name );
		}
	}

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( castor::String const & name
		, RenderSystem & renderSystem )
		: castor::Named( name )
		, OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	void ShaderProgram::setFile( VkShaderStageFlagBits target, castor::Path const & pathFile )
	{
		m_files[target] = pathFile;
		castor::TextFile file{ pathFile, castor::File::OpenMode::eRead };
		castor::String source;
		file.copyToString( source );
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	castor::Path ShaderProgram::getFile( VkShaderStageFlagBits target )const
	{
		auto it = m_files.find( target );
		CU_Require( it != m_files.end() );
		return it->second;
	}

	bool ShaderProgram::hasFile( VkShaderStageFlagBits target )const
	{
		auto it = m_files.find( target );
		return it != m_files.end()
			&& !it->second.empty();
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, castor::String const & source )
	{
		m_files[target].clear();
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, ShaderPtr shader )
	{
		m_files[target].clear();
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.shader = std::move( shader );
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	ShaderModule const & ShaderProgram::getSource( VkShaderStageFlagBits target )const
	{
		auto it = m_modules.find( target );
		CU_Require( it != m_modules.end() );
		return it->second;
	}

	bool ShaderProgram::hasSource( VkShaderStageFlagBits target )const
	{
		auto it = m_compiled.find( target );
		return it != m_compiled.end()
			&& !it->second.shader->spirv.empty();
	}

	SpirVShader const & compileShader( RenderSystem & renderSystem
		, ShaderModule & module )
	{
		return renderSystem.compileShader( module );
	}

	SpirVShader const & compileShader( RenderDevice const & device
		, ShaderModule & module )
	{
		return compileShader( device.renderSystem, module );
	}
}
