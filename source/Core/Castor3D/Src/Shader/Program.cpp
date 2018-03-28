#include "Program.hpp"

#include "Render/RenderSystem.hpp"

#include <Stream/StreamPrefixManipulators.hpp>

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
		uint8_t i = 0;

		for ( auto module : shaderProgram.m_modules )
		{
			hasFile |= !module.second.file.empty();
		}

		if ( hasFile )
		{
			auto tabs = m_tabs + cuT( "\t" );
			result = file.writeText( cuT( "\n" ) + m_tabs + m_name + cuT( "\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			checkError( result, "Shader program" );

			for ( auto module : shaderProgram.m_modules )
			{
				//result = ShaderObject::TextWriter( tabs )( *shaderProgram.m_shaders[i], file );
			}

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
	const String ShaderProgram::Position2 = cuT( "position2" );
	const String ShaderProgram::Normal2 = cuT( "normal2" );
	const String ShaderProgram::Tangent2 = cuT( "tangent2" );
	const String ShaderProgram::Bitangent2 = cuT( "bitangent2" );
	const String ShaderProgram::Texture2 = cuT( "texture2" );
	const String ShaderProgram::Colour2 = cuT( "colour2" );
	const String ShaderProgram::Text = cuT( "text" );
	const String ShaderProgram::BoneIds0 = cuT( "bone_ids0" );
	const String ShaderProgram::BoneIds1 = cuT( "bone_ids1" );
	const String ShaderProgram::Weights0 = cuT( "weights0" );
	const String ShaderProgram::Weights1 = cuT( "weights1" );
	const String ShaderProgram::Transform = cuT( "transform" );
	const String ShaderProgram::Material = cuT( "material" );

	const String ShaderProgram::Lights = cuT( "c3d_sLights" );
	const String ShaderProgram::MapDiffuse = cuT( "c3d_mapDiffuse" );
	const String ShaderProgram::MapAlbedo = cuT( "c3d_mapAlbedo" );
	const String ShaderProgram::MapSpecular = cuT ("c3d_mapSpecular");
	const String ShaderProgram::MapRoughness = cuT( "c3d_mapRoughness" );
	const String ShaderProgram::MapEmissive = cuT( "c3d_mapEmissive" );
	const String ShaderProgram::MapNormal = cuT( "c3d_mapNormal" );
	const String ShaderProgram::MapOpacity = cuT( "c3d_mapOpacity" );
	const String ShaderProgram::MapGloss = cuT( "c3d_mapGloss" );
	const String ShaderProgram::MapMetallic = cuT ("c3d_mapMetallic");
	const String ShaderProgram::MapHeight = cuT( "c3d_mapHeight" );
	const String ShaderProgram::MapEnvironment = cuT( "c3d_mapEnvironment" );
	const String ShaderProgram::MapAmbientOcclusion = cuT( "c3d_mapAmbientOcclusion" );
	const String ShaderProgram::MapTransmittance = cuT( "c3d_mapTransmittance" );
	const String ShaderProgram::MapIrradiance = cuT( "c3d_mapIrradiance" );
	const String ShaderProgram::MapPrefiltered = cuT( "c3d_mapPrefiltered" );
	const String ShaderProgram::MapBrdf = cuT( "c3d_mapBrdf" );
	const String ShaderProgram::MapText = cuT( "c3d_mapText" );

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	bool ShaderProgram::initialise()
	{
		auto & device = *getRenderSystem()->getCurrentDevice();

		auto loadShader = [this, &device]( renderer::ShaderStageFlag stage )
		{
			if ( hasSource( stage ) )
			{
				m_states.push_back( { device.createShaderModule( stage ) } );
				m_states.back().module->loadShader( getSource( stage ) );
			}
			else if ( hasFile( stage ) )
			{
				m_states.push_back( { device.createShaderModule( stage ) } );
				TextFile file{ getFile( stage ), File::OpenMode::eRead };
				String source;
				file.copytoString( source );
				m_states.back().module->loadShader( source );
			}
		};

		if ( hasSource( renderer::ShaderStageFlag::eCompute )
			|| hasFile( renderer::ShaderStageFlag::eCompute ) )
		{
			loadShader( renderer::ShaderStageFlag::eCompute );
		}
		else
		{
			loadShader( renderer::ShaderStageFlag::eVertex );
			loadShader( renderer::ShaderStageFlag::eGeometry );
			loadShader( renderer::ShaderStageFlag::eTessellationControl );
			loadShader( renderer::ShaderStageFlag::eTessellationEvaluation );
			loadShader( renderer::ShaderStageFlag::eFragment );
		}

		return true;
	}

	void ShaderProgram::cleanup()
	{
		m_states.clear();
	}

	void ShaderProgram::setFile( renderer::ShaderStageFlag target, Path const & pathFile )
	{
		m_modules[target].file = pathFile;
		m_modules[target].source = glsl::Shader{};
	}

	Path ShaderProgram::getFile( renderer::ShaderStageFlag target )const
	{
		REQUIRE( m_modules.find( target ) != m_modules.end() );
		return m_modules.at( target ).file;
	}

	bool ShaderProgram::hasFile( renderer::ShaderStageFlag target )const
	{
		return m_modules.find( target ) != m_modules.end()
			&& !getFile( target ).empty();
	}

	void ShaderProgram::setSource( renderer::ShaderStageFlag target, String const & source )
	{
		m_modules[target].file.clear();
		m_modules[target].source.setSource( source );
	}

	void ShaderProgram::setSource( renderer::ShaderStageFlag target, glsl::Shader const & source )
	{
		m_modules[target].file.clear();
		m_modules[target].source = source;
	}

	String ShaderProgram::getSource( renderer::ShaderStageFlag target )const
	{
		REQUIRE( m_modules.find( target ) != m_modules.end() );
		return m_modules.at( target ).source.getSource();
	}

	bool ShaderProgram::hasSource( renderer::ShaderStageFlag target )const
	{
		return m_modules.find( target ) != m_modules.end()
			&& !getSource( target ).empty();
	}
}
