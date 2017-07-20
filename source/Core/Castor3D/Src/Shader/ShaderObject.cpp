#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"
#include "UniformBuffer.hpp"

#include <Stream/StreamPrefixManipulators.hpp>
#include <GlslShader.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::base_prefixer< CharType, PrefixType > const & prefix )
		{
			format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				sbuf = format::install_prefix_buffer< PrefixType >( stream );
				stream.register_callback( format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}
	}

	//*************************************************************************************************

	ShaderObject::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< ShaderObject >{ p_tabs }
	{
	}

	bool ShaderObject::TextWriter::operator()( ShaderObject const & p_shaderObject, TextFile & p_file )
	{
		bool result = p_file.WriteText( m_tabs + p_shaderObject.GetStrType() + cuT( "\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		Path pathFile = p_file.GetFilePath() / cuT( "Shaders" );

		if ( !File::DirectoryExists( pathFile ) )
		{
			File::DirectoryCreate( pathFile );
		}

		bool hasFile = false;

		if ( result )
		{
			Path file = p_shaderObject.GetFile();

			if ( !file.empty() )
			{
				File::CopyFile( file, pathFile );
				String fileName = Path{ cuT( "Shaders" ) } / file.GetFileName() + cuT( "." ) + file.GetExtension();
				string::replace( fileName, cuT( "\\" ), cuT( "/" ) );
				result = p_file.WriteText( m_tabs + cuT( "\tfile \"" ) + fileName + cuT( "\"\n" ) ) > 0;
				Castor::TextWriter< ShaderObject >::CheckError( result, "ShaderObject file" );
			}
		}

		if ( hasFile )
		{
			for ( auto it : p_shaderObject.GetUniforms() )
			{
				result = Uniform::TextWriter( m_tabs + cuT( "\t" ) )( it->GetBaseUniform(), p_file );
			}
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
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

	bool ShaderObject::Compile()
	{
		return true;
	}

	void ShaderObject::Bind()
	{
		for ( auto variable : m_listUniforms )
		{
			variable->Update();
		}
	}

	void ShaderObject::Unbind()
	{
	}

	void ShaderObject::SetFile( Path const & p_filename )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_file.clear();
		m_source = GLSL::Shader{};

		if ( !p_filename.empty() && File::FileExists( p_filename ) )
		{
			TextFile file( p_filename, File::OpenMode::eRead );

			if ( file.IsOk() )
			{
				if ( file.GetLength() > 0 )
				{
					m_file = p_filename;
					String source;
					file.CopyToString( source );
					m_source.SetSource( source );
				}
			}
		}
	}

	bool ShaderObject::HasFile()const
	{
		return !m_file.empty();
	}

	void ShaderObject::SetSource( String const & p_source )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_source.SetSource( p_source );
	}

	void ShaderObject::SetSource( GLSL::Shader const & p_source )
	{
		m_source = p_source;
	}

	bool ShaderObject::HasSource()const
	{
		return !m_source.GetSource().empty();
	}

	void ShaderObject::AddUniform( PushUniformSPtr p_variable )
	{
		if ( p_variable )
		{
			m_listUniforms.push_back( p_variable );
			m_mapUniforms.insert( std::make_pair( p_variable->GetBaseUniform().GetName(), p_variable ) );
		}
	}

	PushUniformSPtr ShaderObject::FindUniform( Castor::String const & p_name )const
	{
		PushUniformSPtr result;
		auto it = m_mapUniforms.find( p_name );

		if ( it != m_mapUniforms.end() )
		{
			result = it->second.lock();
		}

		return result;
	}

	void ShaderObject::FlushUniforms()
	{
		clear_container( m_mapUniforms );
		clear_container( m_listUniforms );
	}

	bool ShaderObject::DoCheckErrors()
	{
		String compilerLog = DoRetrieveCompilerLog();

		if ( !compilerLog.empty() )
		{
			if ( m_status == ShaderStatus::eError )
			{
				Logger::LogError( compilerLog );
			}
			else
			{
				Logger::LogWarning( compilerLog );
			}

			StringStream source;
			source << format::line_prefix();
			source << GetSource();
			Logger::LogInfo( source.str() );
		}
		else if ( m_status == ShaderStatus::eError )
		{
			Logger::LogWarning( cuT( "ShaderObject::Compile - Compilaton failed but shader may be usable to link." ) );
			StringStream source;
			source << format::line_prefix();
			source << GetSource();
			Logger::LogInfo( source.str() );
			m_status = ShaderStatus::eNotCompiled;
		}

		return m_status != ShaderStatus::eError;
	}
}
