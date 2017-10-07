#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"
#include "UniformBuffer.hpp"

#include <Stream/StreamPrefixManipulators.hpp>
#include <GlslShader.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::BasePrefixer< CharType, PrefixType > const & Prefix )
		{
			format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				sbuf = format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}

		UniformType doGetUniformType( glsl::TypeName glslType )
		{
			UniformType result;

			switch ( glslType )
			{
			case glsl::TypeName::eBool:
				result = UniformType::eBool;
				break;

			case glsl::TypeName::eInt:
				result = UniformType::eInt;
				break;

			case glsl::TypeName::eUInt:
				result = UniformType::eUInt;
				break;

			case glsl::TypeName::eFloat:
				result = UniformType::eFloat;
				break;

			case glsl::TypeName::eVec2B:
				result = UniformType::eVec2b;
				break;

			case glsl::TypeName::eVec3B:
				result = UniformType::eVec3b;
				break;

			case glsl::TypeName::eVec4B:
				result = UniformType::eVec4b;
				break;

			case glsl::TypeName::eVec2I:
				result = UniformType::eVec2i;
				break;

			case glsl::TypeName::eVec3I:
				result = UniformType::eVec3i;
				break;

			case glsl::TypeName::eVec4I:
				result = UniformType::eVec4i;
				break;

			case glsl::TypeName::eVec2F:
				result = UniformType::eVec2f;
				break;

			case glsl::TypeName::eVec3F:
				result = UniformType::eVec3f;
				break;

			case glsl::TypeName::eVec4F:
				result = UniformType::eVec4f;
				break;

			case glsl::TypeName::eMat2x2B:
				result = UniformType::eMat2x2b;
				break;

			case glsl::TypeName::eMat3x3B:
				result = UniformType::eMat3x3b;
				break;

			case glsl::TypeName::eMat4x4B:
				result = UniformType::eMat4x4b;
				break;

			case glsl::TypeName::eMat2x2I:
				result = UniformType::eMat2x2i;
				break;

			case glsl::TypeName::eMat3x3I:
				result = UniformType::eMat3x3i;
				break;

			case glsl::TypeName::eMat4x4I:
				result = UniformType::eMat4x4i;
				break;

			case glsl::TypeName::eMat2x2F:
				result = UniformType::eMat2x2f;
				break;

			case glsl::TypeName::eMat3x3F:
				result = UniformType::eMat3x3f;
				break;

			case glsl::TypeName::eMat4x4F:
				result = UniformType::eMat4x4f;
				break;

			case glsl::TypeName::eSamplerBuffer:
			case glsl::TypeName::eSampler1D:
			case glsl::TypeName::eSampler2D:
			case glsl::TypeName::eSampler3D:
			case glsl::TypeName::eSamplerCube:
			case glsl::TypeName::eSampler2DRect:
			case glsl::TypeName::eSampler1DArray:
			case glsl::TypeName::eSampler2DArray:
			case glsl::TypeName::eSamplerCubeArray:
			case glsl::TypeName::eSampler1DShadow:
			case glsl::TypeName::eSampler2DShadow:
			case glsl::TypeName::eSamplerCubeShadow:
			case glsl::TypeName::eSampler2DRectShadow:
			case glsl::TypeName::eSampler1DArrayShadow:
			case glsl::TypeName::eSampler2DArrayShadow:
			case glsl::TypeName::eSamplerCubeArrayShadow:
				result = UniformType::eSampler;
				break;

			default:
				{
					auto castorType = static_cast< shader::TypeName >( glslType );

					switch ( castorType )
					{
					case shader::TypeName::eLight:
					case shader::TypeName::eDirectionalLight:
					case shader::TypeName::ePointLight:
					case shader::TypeName::eSpotLight:
					case shader::TypeName::eMaterial:
					case shader::TypeName::eLegacyMaterial:
					case shader::TypeName::eMetallicRoughnessMaterial:
					case shader::TypeName::eSpecularGlossinessMaterial:
						result = UniformType::eCount;
						break;

					default:
						FAILURE( "Unuspported GLSL type" );
					}
				}
			}

			return result;
		}
	}

	//*************************************************************************************************

	ShaderObject::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< ShaderObject >{ p_tabs }
	{
	}

	bool ShaderObject::TextWriter::operator()( ShaderObject const & p_shaderObject, TextFile & p_file )
	{
		bool result = p_file.writeText( m_tabs + p_shaderObject.getStrType() + cuT( "\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;

		Path pathFile = p_file.getFilePath() / cuT( "Shaders" );

		if ( !File::directoryExists( pathFile ) )
		{
			File::directoryCreate( pathFile );
		}

		bool hasFile = false;

		if ( result )
		{
			Path file = p_shaderObject.getFile();

			if ( !file.empty() )
			{
				File::copyFile( file, pathFile );
				String fileName = Path{ cuT( "Shaders" ) } / file.getFileName() + cuT( "." ) + file.getExtension();
				string::replace( fileName, cuT( "\\" ), cuT( "/" ) );
				result = p_file.writeText( m_tabs + cuT( "\tfile \"" ) + fileName + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< ShaderObject >::checkError( result, "ShaderObject file" );
			}
		}

		if ( hasFile )
		{
			for ( auto it : p_shaderObject.getUniforms() )
			{
				result = Uniform::TextWriter( m_tabs + cuT( "\t" ) )( it->getBaseUniform(), p_file );
			}
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	const std::array< String, size_t( ShaderType::eCount ) > ShaderObject::string_type =
	{
		cuT( "vertex_program" ),
		cuT( "hull_program" ),
		cuT( "domain_program" ),
		cuT( "geometry_program" ),
		cuT( "pixel_program" ),
		cuT( "compute_program" ),
	};

	ShaderObject::ShaderObject( ShaderProgram & p_parent, ShaderType p_type )
		: m_type( p_type )
		, m_parent( p_parent )
	{
	}

	ShaderObject::~ShaderObject()
	{
	}

	bool ShaderObject::compile()
	{
		return true;
	}

	void ShaderObject::bind()
	{
		for ( auto variable : m_listUniforms )
		{
			variable->update();
		}
	}

	void ShaderObject::unbind()
	{
	}

	void ShaderObject::setFile( Path const & p_filename )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_file.clear();
		m_source = glsl::Shader{};

		if ( !p_filename.empty() && File::fileExists( p_filename ) )
		{
			TextFile file( p_filename, File::OpenMode::eRead );

			if ( file.isOk() )
			{
				if ( file.getLength() > 0 )
				{
					m_file = p_filename;
					String source;
					file.copytoString( source );
					m_source.setSource( source );
				}
			}
		}
	}

	bool ShaderObject::hasFile()const
	{
		return !m_file.empty();
	}

	void ShaderObject::setSource( String const & p_source )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_source.setSource( p_source );
	}

	void ShaderObject::setSource( glsl::Shader const & p_source )
	{
		m_source = p_source;
		doFillVariables();
	}

	bool ShaderObject::hasSource()const
	{
		return !m_source.getSource().empty();
	}

	PushUniformSPtr ShaderObject::createUniform( UniformType type
		, String const & name
		, int nbOccurences )
	{
		PushUniformSPtr result = findUniform( type, name );

		if ( !result )
		{
			result = doCreateUniform( type, nbOccurences );
			result->getBaseUniform().setName( name );
			doAddUniform( result );
		}

		return result;
	}

	PushUniformSPtr ShaderObject::findUniform( UniformType type
		, castor::String const & name )const
	{
		PushUniformSPtr result;
		auto it = m_mapUniforms.find( name );

		if ( it != m_mapUniforms.end() )
		{
			result = it->second.lock();

			if ( result && result->getBaseUniform().getFullType() != type )
			{
				Logger::logError( cuT( "Frame variable named " ) + name + cuT( " exists but with a different type" ) );
				FAILURE( "Variable exists with different type." );
				result.reset();
			}
		}

		return result;
	}

	void ShaderObject::flushUniforms()
	{
		clearContainer( m_mapUniforms );
		clearContainer( m_listUniforms );
	}

	void ShaderObject::doAddUniform( PushUniformSPtr variable )
	{
		if ( variable )
		{
			m_listUniforms.push_back( variable );
			m_mapUniforms.insert( std::make_pair( variable->getBaseUniform().getName(), variable ) );
		}
	}

	bool ShaderObject::doCheckErrors()
	{
		String compilerLog = doRetrieveCompilerLog();

		if ( !compilerLog.empty() )
		{
			if ( m_status == ShaderStatus::eError )
			{
				Logger::logError( compilerLog );
			}
			else
			{
				Logger::logWarning( compilerLog );
			}

			StringStream source;
			source << format::LinePrefix();
			source << getSource();
			Logger::logWarning( source.str() );
		}
		else if ( m_status == ShaderStatus::eError )
		{
			Logger::logWarning( cuT( "ShaderObject::Compile - Compilaton failed but shader may be usable to link." ) );
			StringStream source;
			source << format::LinePrefix();
			source << getSource();
			Logger::logWarning( source.str() );
			m_status = ShaderStatus::eNotCompiled;
		}

		return m_status != ShaderStatus::eError;
	}

	void ShaderObject::doFillVariables()
	{
		for ( auto & uniform : m_source.getUniforms() )
		{
			auto uniformType = doGetUniformType( uniform.second.m_type );

			if ( uniformType != UniformType::eCount )
			{
				createUniform( doGetUniformType( uniform.second.m_type ), uniform.first )->getBaseUniform().setChanged( false );
			}
		}

		for ( auto & sampler : m_source.getSamplers() )
		{
			auto samplerUniform = createUniform< UniformType::eSampler >( sampler.first, sampler.second.m_count );
			std::vector< int > values;
			values.resize( sampler.second.m_count );
			std::iota( values.begin(), values.end(), sampler.second.m_binding );
			samplerUniform->setValues( values );
		}
	}
}
