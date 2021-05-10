#include "Castor3D/Shader/Program.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Stream/StreamPrefixManipulators.hpp>

CU_ImplementCUSmartPtr( castor3d, ShaderProgram )

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream
			, format::BasePrefixer< CharType, PrefixType > const & prefix )
		{
			auto * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}

		bool doAddModule( VkShaderStageFlagBits stage
			, std::string const & name
			, std::map< VkShaderStageFlagBits, ShaderModule > & modules )
		{
			bool added = false;
			auto it = modules.find( stage );

			if ( it == modules.end() )
			{
				it = modules.emplace( stage, ShaderModule{ stage, name } ).first;
				added = true;
			}

			return added;
		}
	}

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( castor::String const & name
		, RenderSystem & renderSystem )
		: Named( name )
		, OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	bool ShaderProgram::initialise( RenderDevice const & device )
	{
		if ( m_states.empty() )
		{
			auto loadShader = [this, &device]( VkShaderStageFlagBits stage )
			{
				auto it = m_compiled.find( stage );

				if ( it != m_compiled.end() )
				{
					auto & module = it->second;

					if ( !module.shader.spirv.empty() )
					{
						m_states.push_back( makeShaderState( *device
							, stage
							, module.shader
							, module.name ) );
					}
				}
			};

			if ( hasSource( VK_SHADER_STAGE_COMPUTE_BIT )
				|| hasFile( VK_SHADER_STAGE_COMPUTE_BIT ) )
			{
				loadShader( VK_SHADER_STAGE_COMPUTE_BIT );
			}
			else
			{
				loadShader( VK_SHADER_STAGE_VERTEX_BIT );
				loadShader( VK_SHADER_STAGE_GEOMETRY_BIT );
				loadShader( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT );
				loadShader( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT );
				loadShader( VK_SHADER_STAGE_FRAGMENT_BIT );
			}
		}

		return m_states.size() == m_modules.size();
	}

	void ShaderProgram::cleanup( RenderDevice const & device )
	{
		m_states.clear();
	}

	void ShaderProgram::setFile( VkShaderStageFlagBits target, Path const & pathFile )
	{
		m_files[target] = pathFile;
		TextFile file{ pathFile, File::OpenMode::eRead };
		castor::String source;
		file.copyToString( source );
		auto added = doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
				, getRenderSystem()->compileShader( it->second ) } );
		}
	}

	Path ShaderProgram::getFile( VkShaderStageFlagBits target )const
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

	void ShaderProgram::setSource( VkShaderStageFlagBits target, String const & source )
	{
		m_files[target].clear();
		auto added = doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, getRenderSystem()->compileShader( it->second ) } );
		}
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, ShaderPtr shader )
	{
		m_files[target].clear();
		auto added = doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto it = m_modules.find( target );
			it->second.shader = std::move( shader );
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, getRenderSystem()->compileShader( target, getName(), *it->second.shader ) } );
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
			&& !it->second.shader.spirv.empty();
	}

	SpirVShader compileShader( RenderSystem const & renderSystem
		, ShaderModule const & module )
	{
		return renderSystem.compileShader( module );
	}

	SpirVShader compileShader( RenderDevice const & device
		, ShaderModule const & module )
	{
		return compileShader( device.renderSystem, module );
	}
}
