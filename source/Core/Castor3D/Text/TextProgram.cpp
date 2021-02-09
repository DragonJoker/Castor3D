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

		auto doAddModule( VkShaderStageFlagBits stage
			, std::string const & name
			, std::map< VkShaderStageFlagBits, ShaderModule > & modules )
		{
			auto it = modules.find( stage );

			if ( it == modules.end() )
			{
				it = modules.emplace( stage, ShaderModule{ stage, name } ).first;
			}

			return it;
		}
	}

	//*************************************************************************************************

	ShaderProgram::TextWriter::TextWriter( String const & tabs
		, String const & name )
		: castor::TextWriter< ShaderProgram >{ tabs }
		, m_name{ name }
	{
	}

	bool ShaderProgram::TextWriter::operator()( ShaderProgram const & shaderProgram
		, TextFile & file )
	{
		bool result = false;
		bool hasFile = false;

		for ( auto file : shaderProgram.m_files )
		{
			hasFile |= !file.second.empty();
		}

		if ( hasFile )
		{
			// TODO : Implement castor3d::ShaderModule::TextWriter (to support at least SPIR-V external shaders).
			//auto tabs = m_tabs + cuT( "\t" );
			result = file.writeText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			checkError( result, "Shader program" );

			//for ( auto & module : shaderProgram.m_modules )
			//{
			//	result = ShaderModule::TextWriter( tabs, module.first )( module.second, file );
			//}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}
		else
		{
			result = true;
		}

		return result;
	}

	//*************************************************************************************************

	struct VariableApply
	{
		template <class T> void operator()( T & p ) const
		{
			p->apply();
		}
	};

	//*************************************************************************************************

	const String ShaderProgram::Position = cuT( "position" );
	const String ShaderProgram::Normal = cuT( "normal" );
	const String ShaderProgram::Tangent = cuT( "tangent" );
	const String ShaderProgram::Bitangent = cuT( "bitangent" );
	const String ShaderProgram::Texture = cuT( "texture" );
	const String ShaderProgram::Colour = cuT( "colour" );
	const String ShaderProgram::Position2 = cuT( "inPosition2" );
	const String ShaderProgram::Normal2 = cuT( "inNormal2" );
	const String ShaderProgram::Tangent2 = cuT( "inTangent2" );
	const String ShaderProgram::Bitangent2 = cuT( "inBitangent2" );
	const String ShaderProgram::Texture2 = cuT( "inTexture2" );
	const String ShaderProgram::Colour2 = cuT( "colour2" );
	const String ShaderProgram::Text = cuT( "text" );
	const String ShaderProgram::BoneIds0 = cuT( "inBoneIds0" );
	const String ShaderProgram::BoneIds1 = cuT( "inBoneIds1" );
	const String ShaderProgram::Weights0 = cuT( "inWeights0" );
	const String ShaderProgram::Weights1 = cuT( "inWeights1" );
	const String ShaderProgram::Transform = cuT( "transform" );
	const String ShaderProgram::Material = cuT( "material" );

	const String ShaderProgram::Lights = cuT( "c3d_sLights" );

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
				static std::map< VkShaderStageFlagBits, std::string > type
				{
					{ VK_SHADER_STAGE_VERTEX_BIT, "Vtx" },
					{ VK_SHADER_STAGE_GEOMETRY_BIT, "Geo" },
					{ VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "Tsc" },
					{ VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "Tse" },
					{ VK_SHADER_STAGE_FRAGMENT_BIT, "Pxl" },
				};
				auto itModule = m_modules.find( stage );

				if ( itModule != m_modules.end() )
				{
					auto & module = itModule->second;
					auto itFile = m_files.find( stage );

					if ( !itFile->second.empty() )
					{
						TextFile file{ getFile( stage ), File::OpenMode::eRead };
						file.copyToString( module.source );
					}

					if ( module.shader || !module.source.empty() )
					{
						auto compiled = compileShader( device, module );
						m_states.push_back( makeShaderState( device
							, module.stage
							, compiled
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
		doAddModule( target, getName(), m_modules );
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
		auto it = doAddModule( target, getName(), m_modules );
		it->second.source = source;
		it->second.shader = nullptr;
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, ShaderPtr shader )
	{
		m_files[target].clear();
		auto it = doAddModule( target, getName(), m_modules );
		it->second.source.clear();
		it->second.shader = std::move( shader );
	}

	ShaderModule const & ShaderProgram::getSource( VkShaderStageFlagBits target )const
	{
		auto it = m_modules.find( target );
		CU_Require( it != m_modules.end() );
		return it->second;
	}

	bool ShaderProgram::hasSource( VkShaderStageFlagBits target )const
	{
		auto it = m_modules.find( target );
		return it != m_modules.end()
			&& ( !it->second.source.empty() || it->second.shader != nullptr );
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
