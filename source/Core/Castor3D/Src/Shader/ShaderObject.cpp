#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"
#include "UniformBuffer.hpp"

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
		static std::array< String, size_t( ShaderModel::eCount ) > const l_arrayModels
		{
			cuT( "sm_1" ),
			cuT( "sm_2" ),
			cuT( "sm_3" ),
			cuT( "sm_4" ),
			cuT( "sm_5" ),
		};

		bool l_return = p_file.WriteText( m_tabs + p_shaderObject.GetStrType() + cuT( "\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		Path l_pathFile = p_file.GetFilePath() / cuT( "Shaders" );

		if ( !File::DirectoryExists( l_pathFile ) )
		{
			File::DirectoryCreate( l_pathFile );
		}

		bool l_hasFile = false;

		if ( l_return )
		{
			for ( size_t i = 0; i < size_t( ShaderModel::eCount ); i++ )
			{
				Path l_file = p_shaderObject.GetFile( ShaderModel( i ) );

				if ( !l_file.empty() )
				{
					File::CopyFile( l_file, l_pathFile );
					String l_fileName = Path{ cuT( "Shaders" ) } / l_file.GetFileName() + cuT( "." ) + l_file.GetExtension();
					string::replace( l_fileName, cuT( "\\" ), cuT( "/" ) );
					l_return = p_file.WriteText( m_tabs + cuT( "\tfile " ) + l_arrayModels[i] + cuT( " \"" ) + l_fileName + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< ShaderObject >::CheckError( l_return, "ShaderObject file" );
				}
			}
		}

		if ( l_hasFile )
		{
			for ( auto l_it : p_shaderObject.GetUniforms() )
			{
				l_return = Uniform::TextWriter( m_tabs + cuT( "\t" ) )( l_it->GetBaseUniform(), p_file );
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
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
		for ( auto l_variable : m_listUniforms )
		{
			l_variable->Update();
		}
	}

	void ShaderObject::Unbind()
	{
	}

	void ShaderObject::SetFile( ShaderModel p_eModel, Path const & p_filename )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_arrayFiles[size_t( p_eModel )].clear();
		m_arraySources[size_t( p_eModel )].clear();

		if ( !p_filename.empty() && File::FileExists( p_filename ) )
		{
			TextFile l_file( p_filename, File::OpenMode::eRead );

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

		for ( size_t i = 0; i < size_t( ShaderModel::eCount ) && !l_return; i++ )
		{
			l_return = !m_arrayFiles[i].empty();
		}

		return l_return;
	}

	void ShaderObject::SetSource( ShaderModel p_eModel, String const & p_strSource )
	{
		m_status = ShaderStatus::eNotCompiled;
		m_arraySources[size_t( p_eModel )] = p_strSource;
	}

	bool ShaderObject::HasSource()const
	{
		bool l_return = false;

		for ( size_t i = 0; i < size_t( ShaderModel::eCount ) && !l_return; i++ )
		{
			l_return = !m_arraySources[i].empty();
		}

		return l_return;
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
		PushUniformSPtr l_return;
		auto l_it = m_mapUniforms.find( p_name );

		if ( l_it != m_mapUniforms.end() )
		{
			l_return = l_it->second.lock();
		}

		return l_return;
	}

	void ShaderObject::FlushUniforms()
	{
		clear_container( m_mapUniforms );
		clear_container( m_listUniforms );
	}

	bool ShaderObject::DoCheckErrors()
	{
		String l_compilerLog = DoRetrieveCompilerLog();

		if ( !l_compilerLog.empty() )
		{
			if ( m_status == ShaderStatus::eError )
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
		else if ( m_status == ShaderStatus::eError )
		{
			Logger::LogWarning( cuT( "ShaderObject::Compile - Compilaton failed but shader may be usable to link." ) );
			StringStream l_source;
			l_source << format::line_prefix();
			l_source << m_loadedSource;
			Logger::LogInfo( l_source.str() );
			m_status = ShaderStatus::eNotCompiled;
		}

		return m_status != ShaderStatus::eError;
	}
}
