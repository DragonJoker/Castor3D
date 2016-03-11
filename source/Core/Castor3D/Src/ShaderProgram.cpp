#include "ShaderProgram.hpp"

#include "ShaderObject.hpp"
#include "FrameVariableBuffer.hpp"
#include "RenderSystem.hpp"
#include "OneFrameVariable.hpp"

#include <Logger.hpp>
#include <TransformationMatrix.hpp>
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

	ShaderProgram::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< ShaderProgram >( p_path )
	{
	}

	bool ShaderProgram::BinaryParser::Fill( ShaderProgram const & p_object, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SHADER_PROGRAM );

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_return; ++i )
		{
			ShaderObjectSPtr l_obj = p_object.m_pShaders[i];

			if ( l_obj )
			{
				BinaryChunk l_chunk( eCHUNK_TYPE_SHADER_OBJECT );
				l_return = DoFillChunk( eSHADER_TYPE( i ), eCHUNK_TYPE_SHADER_OBJECT_TYPE, l_chunk );

				if ( l_return )
				{
					l_return = ShaderObject::BinaryParser( m_path ).Fill( *l_obj, l_chunk );
				}

				if ( l_return )
				{
					l_chunk.Finalise();
					p_chunk.AddSubChunk( l_chunk );
				}
			}
		}

		return l_return;
	}

	bool ShaderProgram::BinaryParser::Parse( ShaderProgram & p_object, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		return l_return;
	}

	//*************************************************************************************************

	ShaderProgram::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< ShaderProgram, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool ShaderProgram::TextLoader::operator()( ShaderProgram const & p_shaderProgram, TextFile & p_file )
	{
		bool l_return = false;
		bool l_hasFile = false;
		int i = 0;
		int j = 0;

		while ( i < eSHADER_TYPE_COUNT && !l_hasFile )
		{
			while ( j < eSHADER_MODEL_COUNT && !l_hasFile )
			{
				l_hasFile = !p_shaderProgram.GetFile( eSHADER_TYPE( i ), eSHADER_MODEL( j ) ).empty();
				++j;
			}

			++i;
		}

		if ( l_hasFile )
		{
			String l_strTabs = cuT( "\t\t" );
			ShaderObjectSPtr l_object;

			switch ( p_shaderProgram.GetLanguage() )
			{
			case eSHADER_LANGUAGE_GLSL:
				l_return = p_file.WriteText( l_strTabs + cuT( "gl_shader_program\n" ) ) > 0;
				break;

			default:
				l_return = false;
				break;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
			}

			for ( int i = 0; i < eSHADER_TYPE_COUNT && l_return; i++ )
			{
				l_object = p_shaderProgram.m_pShaders[i];

				if ( l_object )
				{
					l_return = ShaderObject::TextLoader()( *l_object, p_file );
				}
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
			}
		}
		else
		{
			l_return = true;
		}

		return l_return;
	}

	//*************************************************************************************************

	struct VariableApply
	{
		template <class T> void operator()( T & p ) const
		{
			p->Apply();
		}
	};

	//*************************************************************************************************

	const String ShaderProgram::Position = cuT( "position" );
	const String ShaderProgram::Normal = cuT( "normal" );
	const String ShaderProgram::Tangent = cuT( "tangent" );
	const String ShaderProgram::Bitangent = cuT( "bitangent" );
	const String ShaderProgram::Texture = cuT( "texture" );
	const String ShaderProgram::Colour = cuT( "colour" );
	const String ShaderProgram::BoneIds0 = cuT( "bone_ids0" );
	const String ShaderProgram::BoneIds1 = cuT( "bone_ids1" );
	const String ShaderProgram::Weights0 = cuT( "weights0" );
	const String ShaderProgram::Weights1 = cuT( "weights1" );
	const String ShaderProgram::Transform = cuT( "transform" );

	const String ShaderProgram::CameraPos = cuT( "c3d_v3CameraPosition" );
	const String ShaderProgram::AmbientLight = cuT( "c3d_v4AmbientLight" );
	const String ShaderProgram::BackgroundColour = cuT( "c3d_v4BackgroundColour" );
	const String ShaderProgram::LightsCount = cuT( "c3d_iLightsCount" );
	const String ShaderProgram::Lights = cuT( "c3d_sLights" );
	const String ShaderProgram::MatAmbient = cuT( "c3d_v4MatAmbient" );
	const String ShaderProgram::MatDiffuse = cuT( "c3d_v4MatDiffuse" );
	const String ShaderProgram::MatSpecular = cuT( "c3d_v4MatSpecular" );
	const String ShaderProgram::MatEmissive = cuT( "c3d_v4MatEmissive" );
	const String ShaderProgram::MatShininess = cuT( "c3d_fMatShininess" );
	const String ShaderProgram::MatOpacity = cuT( "c3d_fMatOpacity" );
	const String ShaderProgram::MapColour = cuT( "c3d_mapColour" );
	const String ShaderProgram::MapAmbient = cuT( "c3d_mapAmbient" );
	const String ShaderProgram::MapDiffuse = cuT( "c3d_mapDiffuse" );
	const String ShaderProgram::MapSpecular = cuT( "c3d_mapSpecular" );
	const String ShaderProgram::MapEmissive = cuT( "c3d_mapEmissive" );
	const String ShaderProgram::MapNormal = cuT( "c3d_mapNormal" );
	const String ShaderProgram::MapOpacity = cuT( "c3d_mapOpacity" );
	const String ShaderProgram::MapGloss = cuT( "c3d_mapGloss" );
	const String ShaderProgram::MapHeight = cuT( "c3d_mapHeight" );
	const String ShaderProgram::MapText = cuT( "c3d_mapText" );
	const String ShaderProgram::BufferMatrix = cuT( "Matrices" );
	const String ShaderProgram::BufferScene = cuT( "Scene" );
	const String ShaderProgram::BufferPass = cuT( "Pass" );

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( RenderSystem & p_renderSystem, eSHADER_LANGUAGE p_eLanguage )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_status( ePROGRAM_STATUS_NOTLINKED )
		, m_eLanguage( p_eLanguage )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	ShaderObjectSPtr ShaderProgram::CreateObject( eSHADER_TYPE p_type )
	{
		ShaderObjectSPtr l_return;
		REQUIRE( p_type > eSHADER_TYPE_NONE && p_type < eSHADER_TYPE_COUNT );

		if ( p_type > eSHADER_TYPE_NONE && p_type < eSHADER_TYPE_COUNT )
		{
			l_return = DoCreateObject( p_type );
			m_pShaders[p_type] = l_return;
			int i = eSHADER_MODEL_1;

			for ( auto && l_file : m_arrayFiles )
			{
				if ( !l_file.empty() )
				{
					m_pShaders[p_type]->SetFile( eSHADER_MODEL( i++ ), l_file );
				}
			}
		}

		return l_return;
	}

	void ShaderProgram::DoCleanup()
	{
		for ( auto l_shader : m_activeShaders )
		{
			if ( l_shader )
			{
				l_shader->Detach();
				l_shader->FlushFrameVariables();
				l_shader->Destroy();
			}
		}

		m_activeShaders.clear();
		clear_container( m_arrayFiles );

		m_frameVariableBuffersByName.clear();

		for ( auto && l_list : m_frameVariableBuffers )
		{
			l_list.clear();
		}

		for ( auto l_buffer : m_listFrameVariableBuffers )
		{
			l_buffer->Cleanup();
		}
	}

	void ShaderProgram::SetFile( eSHADER_MODEL p_eModel, Path const & p_path )
	{
		m_arrayFiles[p_eModel] = p_path;
		int i = eSHADER_TYPE_VERTEX;

		for ( auto && l_shader : m_pShaders )
		{
			if ( l_shader && GetRenderSystem()->HasShaderType( eSHADER_TYPE( i++ ) ) )
			{
				l_shader->SetFile( p_eModel, p_path );
			}
		}

		ResetToCompile();
	}

	void ShaderProgram::ResetToCompile()
	{
		m_status = ePROGRAM_STATUS_NOTLINKED;
	}

	void ShaderProgram::SetInputType( eSHADER_TYPE p_target, eTOPOLOGY p_topology )
	{
		if ( m_pShaders[p_target] )
		{
			m_pShaders[p_target]->SetInputType( p_topology );
		}
	}

	void ShaderProgram::SetOutputType( eSHADER_TYPE p_target, eTOPOLOGY p_topology )
	{
		if ( m_pShaders[p_target] )
		{
			m_pShaders[p_target]->SetOutputType( p_topology );
		}
	}

	void ShaderProgram::SetOutputVtxCount( eSHADER_TYPE p_target, uint8_t p_count )
	{
		if ( m_pShaders[p_target] )
		{
			m_pShaders[p_target]->SetOutputVtxCount( p_count );
		}
	}

	void ShaderProgram::SetFile( eSHADER_TYPE p_target, eSHADER_MODEL p_eModel, Path const & p_pathFile )
	{
		if ( m_pShaders[p_target] )
		{
			if ( p_eModel == eSHADER_MODEL_COUNT )
			{
				for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
				{
					if ( GetRenderSystem()->CheckSupport( eSHADER_MODEL( i ) ) )
					{
						m_pShaders[p_target]->SetFile( eSHADER_MODEL( i ), p_pathFile );
					}
				}
			}
			else
			{
				m_pShaders[p_target]->SetFile( p_eModel, p_pathFile );
			}
		}

		ResetToCompile();
	}

	Path ShaderProgram::GetFile( eSHADER_TYPE p_target, eSHADER_MODEL p_eModel )const
	{
		Path l_pathReturn;

		if ( m_pShaders[p_target] )
		{
			l_pathReturn = m_pShaders[p_target]->GetFile( p_eModel );
		}

		return l_pathReturn;
	}

	bool ShaderProgram::HasFile( eSHADER_TYPE p_target )const
	{
		bool l_return = false;

		if ( m_pShaders[p_target] )
		{
			l_return = m_pShaders[p_target]->HasFile();
		}

		return l_return;
	}

	void ShaderProgram::SetSource( eSHADER_TYPE p_target, eSHADER_MODEL p_eModel, String const & p_strSource )
	{
		if ( m_pShaders[p_target] )
		{
			if ( p_eModel == eSHADER_MODEL_COUNT )
			{
				for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
				{
					if ( GetRenderSystem()->CheckSupport( eSHADER_MODEL( i ) ) )
					{
						m_pShaders[p_target]->SetSource( eSHADER_MODEL( i ), p_strSource );
					}
				}
			}
			else
			{
				m_pShaders[p_target]->SetSource( p_eModel, p_strSource );
			}
		}

		ResetToCompile();
	}

	String ShaderProgram::GetSource( eSHADER_TYPE p_target, eSHADER_MODEL p_eModel )const
	{
		String l_strReturn;

		if ( m_pShaders[p_target] )
		{
			l_strReturn = m_pShaders[p_target]->GetSource( p_eModel );
		}

		return l_strReturn;
	}

	bool ShaderProgram::HasSource( eSHADER_TYPE p_target )const
	{
		bool l_return = false;

		if ( m_pShaders[p_target] )
		{
			l_return = m_pShaders[p_target]->HasSource();
		}

		return l_return;
	}

	bool ShaderProgram::HasObject( eSHADER_TYPE p_target )const
	{
		return m_pShaders[p_target] && m_pShaders[p_target]->HasSource() && m_pShaders[p_target]->GetStatus() == eSHADER_STATUS_COMPILED;
	}

	eSHADER_STATUS ShaderProgram::GetObjectStatus( eSHADER_TYPE p_target )const
	{
		eSHADER_STATUS l_return = eSHADER_STATUS_DONTEXIST;

		if ( m_pShaders[p_target] )
		{
			l_return = m_pShaders[p_target]->GetStatus();
		}

		return l_return;
	}

	OneIntFrameVariableSPtr ShaderProgram::CreateFrameVariable( String const & p_name, eSHADER_TYPE p_type, int p_iNbOcc )
	{
		OneIntFrameVariableSPtr l_return = FindFrameVariable( p_name, p_type );

		if ( !l_return )
		{
			l_return = DoCreateTextureVariable( p_iNbOcc );
			l_return->SetName( p_name );

			if ( m_pShaders[p_type] )
			{
				m_pShaders[p_type]->AddFrameVariable( l_return );
			}
		}

		return l_return;
	}

	OneIntFrameVariableSPtr ShaderProgram::FindFrameVariable( Castor::String const & p_name, eSHADER_TYPE p_type )const
	{
		OneIntFrameVariableSPtr l_return;

		if ( m_pShaders[p_type] )
		{
			l_return = m_pShaders[p_type]->FindFrameVariable( p_name );
		}

		return l_return;
	}

	void ShaderProgram::AddFrameVariableBuffer( FrameVariableBufferSPtr p_pVariableBuffer, uint64_t p_shaderMask )
	{
		auto l_it = m_frameVariableBuffersByName.find( p_pVariableBuffer->GetName() );

		if ( l_it == m_frameVariableBuffersByName.end() )
		{
			m_listFrameVariableBuffers.push_back( p_pVariableBuffer );
			m_frameVariableBuffersByName.insert( std::make_pair( p_pVariableBuffer->GetName(), p_pVariableBuffer ) );

			for ( int i = 0; i < eSHADER_TYPE_COUNT; ++i )
			{
				if ( p_shaderMask & ( uint64_t( 0x1 ) << i ) )
				{
					m_frameVariableBuffers[i].push_back( p_pVariableBuffer );
				}
			}
		}
	}

	FrameVariableBufferSPtr ShaderProgram::FindFrameVariableBuffer( Castor::String const & p_name )const
	{
		FrameVariableBufferSPtr l_buffer;
		auto l_it = m_frameVariableBuffersByName.find( p_name );

		if ( l_it != m_frameVariableBuffersByName.end() )
		{
			l_buffer = l_it->second.lock();
		}

		return l_buffer;
	}

	FrameVariablePtrList & ShaderProgram::GetFrameVariables( eSHADER_TYPE p_type )
	{
		return m_pShaders[p_type]->GetFrameVariables();
	}

	FrameVariablePtrList const & ShaderProgram::GetFrameVariables( eSHADER_TYPE p_type )const
	{
		return m_pShaders[p_type]->GetFrameVariables();
	}

	bool ShaderProgram::DoInitialise()
	{
		if ( m_status == ePROGRAM_STATUS_NOTLINKED )
		{
			m_activeShaders.clear();

			for ( auto l_shader : m_pShaders )
			{
				if ( l_shader && l_shader->HasSource() )
				{
					l_shader->Destroy();
					l_shader->Create();

					if ( !l_shader->Compile() && l_shader->GetStatus() == eSHADER_STATUS_ERROR )
					{
						Logger::LogError( cuT( "ShaderProgram::Initialise - COMPILER ERROR" ) );
						l_shader->Destroy();
						m_status = ePROGRAM_STATUS_ERROR;
					}
					else
					{
						l_shader->AttachTo( *this );
						m_activeShaders.push_back( l_shader );
					}
				}
			}

			if ( !Link() )
			{
				Logger::LogError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

				for ( auto l_shader : m_activeShaders )
				{
					StringStream l_source;
					l_source << format::line_prefix();
					l_source << l_shader->GetLoadedSource();
					Logger::LogDebug( l_source.str() );
					l_shader->Destroy();
				}

				m_status = ePROGRAM_STATUS_ERROR;
			}
			else
			{
				for ( auto && l_buffer : m_listFrameVariableBuffers )
				{
					l_buffer->Initialise( *this );
				}

				Logger::LogInfo( cuT( "ShaderProgram::Initialise - Program Linked successfully" ) );
			}
		}

		return m_status == ePROGRAM_STATUS_LINKED;
	}

	void ShaderProgram::DoBind( bool p_bindUbo )
	{
		if ( m_status == ePROGRAM_STATUS_LINKED )
		{
			for ( auto && l_shader : m_activeShaders )
			{
				l_shader->Bind();
			}

			if ( p_bindUbo )
			{
				uint32_t l_index = 0;

				for ( auto l_variableBuffer : m_listFrameVariableBuffers )
				{
					l_variableBuffer->Bind( l_index++ );
				}
			}
		}
	}

	void ShaderProgram::DoUnbind()
	{
		if ( m_status == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer : m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			for ( auto && l_shader : m_activeShaders )
			{
				l_shader->Unbind();
			}
		}
	}

	bool ShaderProgram::DoLink()
	{
		if ( m_status != ePROGRAM_STATUS_ERROR )
		{
			if ( m_status != ePROGRAM_STATUS_LINKED )
			{
				for ( auto && l_shader : m_activeShaders )
				{
					for ( auto && l_it : l_shader->GetFrameVariables() )
					{
						l_it->Initialise();
					}
				}

				m_status = ePROGRAM_STATUS_LINKED;
			}
		}

		return m_status == ePROGRAM_STATUS_LINKED;
	}
}
