#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"

#if defined( _WIN32 )
#	include <direct.h>
#else
#	include <sys/stat.h>
#endif

#include <Stream/StreamPrefixManipulators.hpp>

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
		static std::array< String, size_t( ShaderModel::Count ) > const l_arrayModels
		{
			cuT( "sm_1" ),
			cuT( "sm_2" ),
			cuT( "sm_3" ),
			cuT( "sm_4" ),
			cuT( "sm_5" ),
		};

		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + p_shaderObject.GetStrType() + cuT( "\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		}

		Path l_pathFile = p_file.GetFilePath() / cuT( "Shaders" );
		File::DirectoryCreate( l_pathFile );
		bool l_hasFile = false;

		if ( l_return )
		{
			for ( size_t i = 0; i < size_t( ShaderModel::Count ); i++ )
			{
				Path l_file = p_shaderObject.GetFile( ShaderModel( i ) );

				if ( !l_file.empty() )
				{
					File::CopyFile( l_file, l_pathFile );
					l_file = Path{ Path{ cuT( "Shaders" ) } / l_file.GetFileName() + cuT( "." ) + l_file.GetExtension() };
					l_return = p_file.WriteText( m_tabs + cuT( "\tfile " ) + l_arrayModels[i] + cuT( " \"" ) + l_file + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< ShaderObject >::CheckError( l_return, "ShaderObject file" );
				}
			}
		}

		if ( l_hasFile )
		{
			for ( auto l_it : p_shaderObject.GetFrameVariables() )
			{
				l_return = FrameVariable::TextWriter( m_tabs + cuT( "\t" ) )( *l_it, p_file );
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	const std::array< String, size_t( ShaderType::Count ) > ShaderObject::string_type =
	{
		cuT( "vertex_program" ),
		cuT( "hull_program" ),
		cuT( "domain_program" ),
		cuT( "geometry_program" ),
		cuT( "pixel_program" ),
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
		for ( auto l_variable : m_listFrameVariables )
		{
			l_variable->Bind();
		}
	}

	void ShaderObject::Unbind()
	{
		for ( auto l_variable : m_listFrameVariables )
		{
			l_variable->Unbind();
		}
	}

	void ShaderObject::SetFile( ShaderModel p_eModel, Path const & p_filename )
	{
		m_status = ShaderStatus::NotCompiled;
		m_arrayFiles[size_t( p_eModel )].clear();
		m_arraySources[size_t( p_eModel )].clear();

		if ( !p_filename.empty() && File::FileExists( p_filename ) )
		{
			TextFile l_file( p_filename, File::OpenMode::Read );

			if ( l_file.IsOk() )
			{
				if ( l_file.GetLength() > 0 )
				{
					m_arrayFiles[size_t( p_eModel )] = p_filename;
					l_file.CopyToString( m_arraySources[size_t( p_eModel )] );
				}
			}
		}
	}

	bool ShaderObject::HasFile()const
	{
		bool l_return = false;

		for ( size_t i = 0; i < size_t( ShaderModel::Count ) && !l_return; i++ )
		{
			l_return = !m_arrayFiles[i].empty();
		}

		return l_return;
	}

	void ShaderObject::SetSource( ShaderModel p_eModel, String const & p_strSource )
	{
		m_status = ShaderStatus::NotCompiled;
		m_arraySources[size_t( p_eModel )] = p_strSource;
	}

	bool ShaderObject::HasSource()const
	{
		bool l_return = false;

		for ( size_t i = 0; i < size_t( ShaderModel::Count ) && !l_return; i++ )
		{
			l_return = !m_arraySources[i].empty();
		}

		return l_return;
	}

	void ShaderObject::AddFrameVariable( FrameVariableSPtr p_variable )
	{
		if ( p_variable )
		{
			m_listFrameVariables.push_back( p_variable );
			m_mapFrameVariables.insert( std::make_pair( p_variable->GetName(), p_variable ) );
		}
	}

	FrameVariableSPtr ShaderObject::FindFrameVariable( Castor::String const & p_name )const
	{
		FrameVariableSPtr l_return;
		auto l_it = m_mapFrameVariables.find( p_name );

		if ( l_it != m_mapFrameVariables.end() )
		{
			l_return = l_it->second.lock();
		}

		return l_return;
	}

	void ShaderObject::FlushFrameVariables()
	{
		clear_container( m_mapFrameVariables );
		clear_container( m_listFrameVariables );
	}

	bool ShaderObject::DoCheckErrors()
	{
		String l_compilerLog = DoRetrieveCompilerLog();

		if ( !l_compilerLog.empty() )
		{
			if ( m_status == ShaderStatus::Error )
			{
				Logger::LogError( l_compilerLog );
			}
			else
			{
				Logger::LogWarning( l_compilerLog );
			}

			StringStream l_source;
			l_source << format::line_prefix();
			l_source << m_loadedSource;
			Logger::LogInfo( l_source.str() );
			m_loadedSource.clear();
		}
		else if ( m_status == ShaderStatus::Error )
		{
			Logger::LogWarning( cuT( "ShaderObject::Compile - Compilaton failed but shader may be usable to link." ) );
			StringStream l_source;
			l_source << format::line_prefix();
			l_source << m_loadedSource;
			Logger::LogInfo( l_source.str() );
			m_status = ShaderStatus::NotCompiled;
		}

		return m_status != ShaderStatus::Error;
	}
}
