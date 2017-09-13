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

	ShaderObject::ShaderObject( ShaderProgram * p_parent, ShaderType p_type )
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
	}

	bool ShaderObject::hasSource()const
	{
		return !m_source.getSource().empty();
	}

	void ShaderObject::addUniform( PushUniformSPtr p_variable )
	{
		if ( p_variable )
		{
			m_listUniforms.push_back( p_variable );
			m_mapUniforms.insert( std::make_pair( p_variable->getBaseUniform().getName(), p_variable ) );
		}
	}

	PushUniformSPtr ShaderObject::findUniform( castor::String const & p_name )const
	{
		PushUniformSPtr result;
		auto it = m_mapUniforms.find( p_name );

		if ( it != m_mapUniforms.end() )
		{
			result = it->second.lock();
		}

		return result;
	}

	void ShaderObject::flushUniforms()
	{
		clearContainer( m_mapUniforms );
		clearContainer( m_listUniforms );
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
}
