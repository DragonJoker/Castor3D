﻿#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"
#include "FrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"

#if defined( _WIN32 )
#	include <direct.h>
#else
#	include <sys/stat.h>
#endif

#include <StreamPrefixManipulators.hpp>

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

	ShaderObject::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< ShaderObject >( p_path )
	{
	}

	bool ShaderObject::BinaryParser::Fill( ShaderObject const & p_object, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SHADER_PROGRAM );

		if ( l_return )
		{
			l_return = DoFillChunk( p_object.GetLoadedSource(), eCHUNK_TYPE_SHADER_OBJECT_SOURCE, l_chunk );
		}

		if ( p_object.GetType() == eSHADER_TYPE_GEOMETRY )
		{
			if ( l_return )
			{
				l_return = DoFillChunk( p_object.GetInputType(), eCHUNK_TYPE_SHADER_OBJECT_INPUT, l_chunk );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( p_object.GetOutputType(), eCHUNK_TYPE_SHADER_OBJECT_OUTPUT, l_chunk );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( p_object.GetOutputVtxCount(), eCHUNK_TYPE_SHADER_OBJECT_OUTCOUNT, l_chunk );
			}
		}

		for ( auto && l_it : p_object.GetFrameVariables() )
		{
			FrameVariableSPtr l_variable = l_it;
			BinaryChunk l_chunkVariable( eCHUNK_TYPE_PROGRAM_VARIABLE );

			if ( l_return )
			{
				l_return = DoFillChunk( l_variable->GetOccCount(), eCHUNK_TYPE_VARIABLE_COUNT, l_chunkVariable );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( l_variable->GetFullType(), eCHUNK_TYPE_VARIABLE_TYPE, l_chunkVariable );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( l_variable->GetName(), eCHUNK_TYPE_NAME, l_chunkVariable );
			}

			if ( l_return )
			{
				l_return = DoFillChunk( l_variable->GetStrValue(), eCHUNK_TYPE_VARIABLE_VALUE, l_chunkVariable );
			}

			if ( l_return )
			{
				l_chunkVariable.Finalise();
				l_return = l_chunk.AddSubChunk( l_chunkVariable );
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			l_return = p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool ShaderObject::BinaryParser::Parse( ShaderObject & p_object, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_strText;
		eTOPOLOGY l_ePrimType;
		eFRAME_VARIABLE_TYPE l_type;
		FrameVariableSPtr l_variable;
		uint8_t l_count;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_SHADER_OBJECT_SOURCE:
				l_return = DoParseChunk( l_strText, l_chunk );

				if ( l_return )
				{
					p_object.SetSource( eSHADER_MODEL_5, l_strText );
					p_object.SetSource( eSHADER_MODEL_4, l_strText );
					p_object.SetSource( eSHADER_MODEL_3, l_strText );
					p_object.SetSource( eSHADER_MODEL_2, l_strText );
					p_object.SetSource( eSHADER_MODEL_1, l_strText );
				}

				break;

			case eCHUNK_TYPE_PROGRAM_VARIABLE:
				l_count = 0;
				l_type = eFRAME_VARIABLE_TYPE( -1 );
				l_strText.clear();
				l_variable.reset();

				while ( l_chunk.CheckAvailable( 1 ) )
				{
					BinaryChunk l_chunkVariable;
					l_return = l_chunk.GetSubChunk( l_chunkVariable );

					switch ( l_chunkVariable.GetChunkType() )
					{
					case eCHUNK_TYPE_VARIABLE_COUNT:
						l_return = DoParseChunk( l_count, l_chunkVariable );
						break;

					case eCHUNK_TYPE_NAME:
						l_return = DoParseChunk( l_strText, l_chunkVariable );
						break;

					case eCHUNK_TYPE_VARIABLE_TYPE:
						l_return = DoParseChunk( l_type, l_chunkVariable );
						break;

					case eCHUNK_TYPE_VARIABLE_VALUE:
						l_return = DoParseChunk( l_strText, l_chunkVariable );

						if ( l_return )
						{
							l_variable->SetValueStr( l_strText );
						}

						break;
					}

					//if ( l_return && !l_variable )
					//{
					//	if ( l_count && l_type != eFRAME_VARIABLE_TYPE( -1 ) && !l_strText.empty() )
					//	{
					//		FrameVariableBufferSPtr l_buffer = p_object.GetParent()->GetUserBuffer();
					//		l_variable = l_buffer->CreateVariable( *p_object.GetParent(), l_type, l_strText, l_count );
					//	}
					//}

					if ( !l_return )
					{
						p_chunk.EndParse();
					}
				}

				break;
			}

			if ( p_object.GetType() == eSHADER_TYPE_GEOMETRY )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_SHADER_OBJECT_INPUT:
					l_return = DoParseChunk( l_ePrimType, l_chunk );

					if ( l_return )
					{
						p_object.SetInputType( l_ePrimType );
					}

					break;

				case eCHUNK_TYPE_SHADER_OBJECT_OUTPUT:
					l_return = DoParseChunk( l_ePrimType, l_chunk );

					if ( l_return )
					{
						p_object.SetOutputType( l_ePrimType );
					}

					break;

				case eCHUNK_TYPE_SHADER_OBJECT_OUTCOUNT:
					l_return = DoParseChunk( l_count, l_chunk );

					if ( l_return )
					{
						p_object.SetOutputVtxCount( l_count );
					}

					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	ShaderObject::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< ShaderObject, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool ShaderObject::TextLoader::operator()( ShaderObject const & p_shaderObject, TextFile & p_file )
	{
		String l_strTabs = cuT( "\t\t\t" );
		bool l_return = p_file.WriteText( l_strTabs + p_shaderObject.GetStrType() ) > 0;
		static std::array< String, eSHADER_MODEL_COUNT > l_arrayModels;
		l_arrayModels[eSHADER_MODEL_1] = cuT( "sm_1" );
		l_arrayModels[eSHADER_MODEL_2] = cuT( "sm_2" );
		l_arrayModels[eSHADER_MODEL_3] = cuT( "sm_3" );
		l_arrayModels[eSHADER_MODEL_4] = cuT( "sm_4" );
		l_arrayModels[eSHADER_MODEL_5] = cuT( "sm_5" );

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\n{\n" ) ) > 0;
		}

		Path l_pathFile = p_file.GetFilePath() / cuT( "Shaders" );
		File::DirectoryCreate( l_pathFile );
		bool l_hasFile = false;

		if ( l_return )
		{
			for ( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
			{
				Path l_file = p_shaderObject.GetFile( eSHADER_MODEL( i ) );

				if ( !l_file.empty() )
				{
					File::CopyFile( l_file, l_pathFile );
					l_file = Path( cuT( "Shaders" ) ) / l_file.GetFileName() + cuT( "." ) + l_file.GetExtension();
					l_return = p_file.WriteText( l_strTabs + cuT( "\tfile \"" ) + l_arrayModels[i] + cuT( " " ) + l_file + cuT( "\"\n" ) ) > 0;
				}
			}
		}

		if ( l_hasFile )
		{
			for ( auto && l_it : p_shaderObject.GetFrameVariables() )
			{
				l_return = FrameVariable::TextLoader()( *l_it, p_file );
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	const std::array< String, eSHADER_TYPE_COUNT > ShaderObject::string_type =
	{
		cuT( "vertex_program" ),
		cuT( "hull_program" ),
		cuT( "domain_program" ),
		cuT( "geometry_program" ),
		cuT( "pixel_program" ),
	};

	ShaderObject::ShaderObject( ShaderProgram * p_parent, eSHADER_TYPE p_type )
		: m_status( eSHADER_STATUS_NOTCOMPILED )
		, m_type( p_type )
		, m_parent( p_parent )
		, m_eInputType( eTOPOLOGY_TRIANGLES )
		, m_eOutputType( eTOPOLOGY_TRIANGLES )
		, m_eShaderModel( eSHADER_MODEL_1 )
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
		for ( auto && l_variable : m_listFrameVariables )
		{
			l_variable->Bind();
		}
	}

	void ShaderObject::Unbind()
	{
		for ( auto && l_variable : m_listFrameVariables )
		{
			l_variable->Unbind();
		}
	}

	void ShaderObject::SetFile( eSHADER_MODEL p_eModel, Path const & p_filename )
	{
		m_status = eSHADER_STATUS_NOTCOMPILED;
		m_arrayFiles[p_eModel].clear();
		m_arraySources[p_eModel].clear();

		if ( !p_filename.empty() && File::FileExists( p_filename ) )
		{
			TextFile l_file( p_filename.c_str(), File::eOPEN_MODE_READ );

			if ( l_file.IsOk() )
			{
				if ( l_file.GetLength() > 0 )
				{
					m_arrayFiles[p_eModel] = p_filename;
					l_file.CopyToString( m_arraySources[p_eModel] );
				}
			}
		}
	}

	bool ShaderObject::HasFile()const
	{
		bool l_return = false;

		for ( int i = 0; i < eSHADER_MODEL_COUNT && !l_return; i++ )
		{
			l_return = !m_arrayFiles[i].empty();
		}

		return l_return;
	}

	void ShaderObject::SetSource( eSHADER_MODEL p_eModel, String const & p_strSource )
	{
		m_status = eSHADER_STATUS_NOTCOMPILED;
		m_arraySources[p_eModel] = p_strSource;
	}

	bool ShaderObject::HasSource()const
	{
		bool l_return = false;

		for ( int i = 0; i < eSHADER_MODEL_COUNT && !l_return; i++ )
		{
			l_return = !m_arraySources[i].empty();
		}

		return l_return;
	}

	void ShaderObject::AddFrameVariable( OneIntFrameVariableSPtr p_variable )
	{
		if ( p_variable )
		{
			m_listFrameVariables.push_back( p_variable );
			m_mapFrameVariables.insert( std::make_pair( p_variable->GetName(), p_variable ) );
		}
	}

	OneIntFrameVariableSPtr ShaderObject::FindFrameVariable( Castor::String const & p_name )const
	{
		OneIntFrameVariableSPtr l_return;
		FrameVariablePtrStrMapConstIt l_it = m_mapFrameVariables.find( p_name );

		if ( l_it != m_mapFrameVariables.end() )
		{
			FrameVariableSPtr l_pFound = l_it->second.lock();

			if ( l_pFound && l_pFound->GetFullType() == OneIntFrameVariable::GetFrameVariableType() )
			{
				l_return = std::static_pointer_cast< OneIntFrameVariable >( l_pFound );
			}
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
			if ( m_status == eSHADER_STATUS_ERROR )
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
			Logger::LogDebug( l_source.str() );
			m_loadedSource.clear();
		}
		else if ( m_status == eSHADER_STATUS_ERROR )
		{
			Logger::LogWarning( cuT( "ShaderObject::Compile - Compilaton failed but shader may be usable to link." ) );
			StringStream l_source;
			l_source << format::line_prefix();
			l_source << m_loadedSource;
			Logger::LogDebug( l_source.str() );
			m_status = eSHADER_STATUS_NOTCOMPILED;
		}

		return m_status != eSHADER_STATUS_ERROR;
	}
}
