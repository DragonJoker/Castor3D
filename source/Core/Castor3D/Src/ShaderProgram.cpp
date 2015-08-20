﻿#include "ShaderProgram.hpp"

#include "ShaderObject.hpp"
#include "FrameVariableBuffer.hpp"
#include "RenderSystem.hpp"
#include "OneFrameVariable.hpp"

#include <Logger.hpp>
#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	ShaderProgramBase::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< ShaderProgramBase >( p_path )
	{
	}

	bool ShaderProgramBase::BinaryParser::Fill( ShaderProgramBase const & p_object, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SHADER_PROGRAM );

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_bReturn; ++i )
		{
			ShaderObjectBaseSPtr l_obj = p_object.m_pShaders[i];

			if ( l_obj )
			{
				BinaryChunk l_chunk( eCHUNK_TYPE_SHADER_OBJECT );
				l_bReturn = DoFillChunk( eSHADER_TYPE( i ), eCHUNK_TYPE_SHADER_OBJECT_TYPE, l_chunk );

				if ( l_bReturn )
				{
					l_bReturn = ShaderObjectBase::BinaryParser( m_path ).Fill( *l_obj, l_chunk );
				}

				if ( l_bReturn )
				{
					l_chunk.Finalise();
					p_chunk.AddSubChunk( l_chunk );
				}
			}
		}

		return l_bReturn;
	}

	bool ShaderProgramBase::BinaryParser::Parse( ShaderProgramBase & p_object, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		return l_bReturn;
	}

	//*************************************************************************************************

	ShaderProgramBase::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< ShaderProgramBase, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool ShaderProgramBase::TextLoader::operator()( ShaderProgramBase const & p_shaderProgram, TextFile & p_file )
	{
		bool l_bReturn = false;
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
			ShaderObjectBaseSPtr l_pObject;

			switch ( p_shaderProgram.GetLanguage() )
			{
			case eSHADER_LANGUAGE_GLSL:
				l_bReturn = p_file.WriteText( l_strTabs + cuT( "gl_shader_program\n" ) ) > 0;
				break;

			case eSHADER_LANGUAGE_HLSL:
				l_bReturn = p_file.WriteText( l_strTabs + cuT( "hl_shader_program\n" ) ) > 0;
				break;

			default:
				l_bReturn = false;
				break;
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
			}

			for ( int i = 0; i < eSHADER_TYPE_COUNT && l_bReturn; i++ )
			{
				l_pObject = p_shaderProgram.m_pShaders[i];

				if ( l_pObject )
				{
					l_bReturn = ShaderObjectBase::TextLoader()( *l_pObject, p_file );
				}
			}

			if ( l_bReturn )
			{
				l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
			}
		}
		else
		{
			l_bReturn = true;
		}

		return l_bReturn;
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

	const String ShaderProgramBase::CameraPos = cuT( "c3d_v3CameraPosition" );
	const String ShaderProgramBase::AmbientLight = cuT( "c3d_v4AmbientLight" );
	const String ShaderProgramBase::BackgroundColour = cuT( "c3d_v4BackgroundColour" );
	const String ShaderProgramBase::LightsCount = cuT( "c3d_iLightsCount" );
	const String ShaderProgramBase::Lights = cuT( "c3d_sLights" );
	const String ShaderProgramBase::MatAmbient = cuT( "c3d_v4MatAmbient" );
	const String ShaderProgramBase::MatDiffuse = cuT( "c3d_v4MatDiffuse" );
	const String ShaderProgramBase::MatSpecular = cuT( "c3d_v4MatSpecular" );
	const String ShaderProgramBase::MatEmissive = cuT( "c3d_v4MatEmissive" );
	const String ShaderProgramBase::MatShininess = cuT( "c3d_fMatShininess" );
	const String ShaderProgramBase::MatOpacity = cuT( "c3d_fMatOpacity" );
	const String ShaderProgramBase::MapColour = cuT( "c3d_mapColour" );
	const String ShaderProgramBase::MapAmbient = cuT( "c3d_mapAmbient" );
	const String ShaderProgramBase::MapDiffuse = cuT( "c3d_mapDiffuse" );
	const String ShaderProgramBase::MapSpecular = cuT( "c3d_mapSpecular" );
	const String ShaderProgramBase::MapNormal = cuT( "c3d_mapNormal" );
	const String ShaderProgramBase::MapOpacity = cuT( "c3d_mapOpacity" );
	const String ShaderProgramBase::MapGloss = cuT( "c3d_mapGloss" );
	const String ShaderProgramBase::MapHeight = cuT( "c3d_mapHeight" );
	const String ShaderProgramBase::MapText = cuT( "c3d_mapText" );
	const String ShaderProgramBase::BufferMatrix = cuT( "Matrices" );
	const String ShaderProgramBase::BufferScene = cuT( "Scene" );
	const String ShaderProgramBase::BufferPass = cuT( "Pass" );

	//*************************************************************************************************

	ShaderProgramBase::ShaderProgramBase( RenderSystem * p_pRenderSystem, eSHADER_LANGUAGE p_eLanguage )
		: m_eStatus( ePROGRAM_STATUS_NOTLINKED )
		, m_bEnabled( true )
		, m_eLanguage( p_eLanguage )
		, m_pRenderSystem( p_pRenderSystem )
	{
	}

	ShaderProgramBase::~ShaderProgramBase()
	{
	}

	void ShaderProgramBase::Cleanup()
	{
		m_activeShaders.clear();

		for ( auto l_shader: m_pShaders )
		{
			if ( l_shader )
			{
				l_shader->FlushFrameVariables();
				l_shader->DestroyProgram();
			}
		}

		clear_container( m_arrayFiles );
		
		m_frameVariableBuffersByName.clear();

		for ( auto && l_list: m_frameVariableBuffers )
		{
			l_list.clear();
		}

		for ( auto l_buffer: m_listFrameVariableBuffers )
		{
			l_buffer->Cleanup();
		}
	}

	void ShaderProgramBase::Initialise()
	{
		if ( m_eStatus == ePROGRAM_STATUS_NOTLINKED )
		{
			m_activeShaders.clear();

			if ( m_pRenderSystem->UseShaders() )
			{
				bool l_bResult = true;

				for ( auto l_shader: m_pShaders )
				{
					if ( l_shader && l_shader->HasSource() )
					{
						l_shader->DestroyProgram();
						l_shader->CreateProgram();

						if ( !l_shader->Compile() && l_shader->GetStatus() == eSHADER_STATUS_ERROR )
						{
							Logger::LogError( cuT( "ShaderProgram::Initialise - " ) + l_shader->GetCurrentFile() + cuT( " - COMPILER ERROR" ) );
							l_shader->DestroyProgram();
							m_eStatus = ePROGRAM_STATUS_ERROR;
							l_bResult = false;
						}
						else
						{
							m_activeShaders.push_back( l_shader );
						}
					}
				}

				if ( l_bResult )
				{
					if ( !Link() )
					{
						Logger::LogError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

						for ( auto l_shader: m_pShaders )
						{
							if ( l_shader )
							{
								l_shader->DestroyProgram();
							}
						}

						m_eStatus = ePROGRAM_STATUS_ERROR;
					}
					else
					{
						for ( auto && l_buffer: m_listFrameVariableBuffers )
						{
							l_buffer->Initialise( *this );
						}

						Logger::LogInfo( cuT( "ShaderProgram::Initialise - Program Linked successfully" ) );
					}
				}
			}
		}
	}

	String ShaderProgramBase::GetVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass )
	{
		String l_strReturn;
		ContextSPtr l_pContext = m_pRenderSystem->GetMainContext();

		if ( l_pContext )
		{
			if ( l_pContext->IsDeferredShadingSet() )
			{
				l_strReturn = DoGetDeferredVertexShaderSource( p_uiProgramFlags, p_bLightPass );
			}
			else
			{
				l_strReturn = DoGetVertexShaderSource( p_uiProgramFlags );
			}
		}

		return l_strReturn;
	}

	String ShaderProgramBase::GetPixelShaderSource( uint32_t p_uiFlags )
	{
		String l_strReturn;
		ContextSPtr l_pContext = m_pRenderSystem->GetMainContext();

		if ( l_pContext )
		{
			if ( l_pContext->IsDeferredShadingSet() )
			{
				l_strReturn = DoGetDeferredPixelShaderSource( p_uiFlags );
			}
			else
			{
				l_strReturn = DoGetPixelShaderSource( p_uiFlags );
			}
		}

		return l_strReturn;
	}

	ShaderObjectBaseSPtr ShaderProgramBase::CreateObject( eSHADER_TYPE p_eType )
	{
		ShaderObjectBaseSPtr l_pReturn;

		if ( p_eType > eSHADER_TYPE_NONE && p_eType < eSHADER_TYPE_COUNT )
		{
			l_pReturn = DoCreateObject( p_eType );
			m_pShaders[p_eType] = l_pReturn;
			int i = eSHADER_MODEL_1;

			for ( auto && l_file: m_arrayFiles )
			{
				if ( !l_file.empty() )
				{
					m_pShaders[p_eType]->SetFile( eSHADER_MODEL( i++ ), l_file );
				}
			}
		}
		else
		{
			CASTOR_ASSERT( false );
		}

		return l_pReturn;
	}

	bool ShaderProgramBase::Link()
	{
		bool l_bReturn = false;

		if ( m_pRenderSystem->UseShaders() && m_eStatus != ePROGRAM_STATUS_ERROR )
		{
			if ( m_eStatus != ePROGRAM_STATUS_LINKED )
			{
				for ( auto && l_shader: m_activeShaders )
				{
					std::for_each( l_shader->GetFrameVariablesBegin(), l_shader->GetFrameVariablesEnd(), [&]( std::pair< String, FrameVariableWPtr > p_pair )
					{
						p_pair.second.lock()->Initialise();
					} );
				}

				m_eStatus = ePROGRAM_STATUS_LINKED;
			}

			l_bReturn = m_eStatus == ePROGRAM_STATUS_LINKED;
		}

		return l_bReturn;
	}

	void ShaderProgramBase::Bind( uint8_t CU_PARAM_UNUSED( p_byIndex ), uint8_t CU_PARAM_UNUSED( p_byCount ) )
	{
		if ( m_pRenderSystem->UseShaders() && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			for ( auto && l_shader: m_activeShaders )
			{
				l_shader->Bind();
			}

			uint32_t l_index = 0;

			for ( auto l_variableBuffer: m_listFrameVariableBuffers )
			{
				l_variableBuffer->Bind( l_index++ );
			}
		}
	}

	void ShaderProgramBase::Unbind()
	{
		if ( m_pRenderSystem->UseShaders() && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer: m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			for ( auto && l_shader: m_activeShaders )
			{
				l_shader->Unbind();
			}
		}
	}

	void ShaderProgramBase::SetFile( eSHADER_MODEL p_eModel, Path const & p_path )
	{
		m_arrayFiles[p_eModel] = p_path;
		int i = eSHADER_TYPE_VERTEX;

		for ( auto && l_shader: m_pShaders )
		{
			if ( l_shader && m_pRenderSystem->HasShaderType( eSHADER_TYPE( i++ ) ) )
			{
				l_shader->SetFile( p_eModel, p_path );
			}
		}

		ResetToCompile();
	}

	void ShaderProgramBase::ResetToCompile()
	{
		m_eStatus = ePROGRAM_STATUS_NOTLINKED;
	}

	void ShaderProgramBase::SetInputType( eSHADER_TYPE p_eTarget, eTOPOLOGY p_ePrimitiveType )
	{
		if ( m_pShaders[p_eTarget] )
		{
			m_pShaders[p_eTarget]->SetInputType( p_ePrimitiveType );
		}
	}

	void ShaderProgramBase::SetOutputType( eSHADER_TYPE p_eTarget, eTOPOLOGY p_ePrimitiveType )
	{
		if ( m_pShaders[p_eTarget] )
		{
			m_pShaders[p_eTarget]->SetOutputType( p_ePrimitiveType );
		}
	}

	void ShaderProgramBase::SetOutputVtxCount( eSHADER_TYPE p_eTarget, uint8_t p_uiCount )
	{
		if ( m_pShaders[p_eTarget] )
		{
			m_pShaders[p_eTarget]->SetOutputVtxCount( p_uiCount );
		}
	}

	void ShaderProgramBase::SetFile( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel, Path const & p_pathFile )
	{
		if ( m_pShaders[p_eTarget] )
		{
			if ( p_eModel == eSHADER_MODEL_COUNT )
			{
				for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
				{
					if ( m_pRenderSystem->CheckSupport( eSHADER_MODEL( i ) ) )
					{
						m_pShaders[p_eTarget]->SetFile( eSHADER_MODEL( i ), p_pathFile );
					}
				}
			}
			else
			{
				m_pShaders[p_eTarget]->SetFile( p_eModel, p_pathFile );
			}
		}

		ResetToCompile();
	}

	Path ShaderProgramBase::GetFile( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel )const
	{
		Path l_pathReturn;

		if ( m_pShaders[p_eTarget] )
		{
			l_pathReturn = m_pShaders[p_eTarget]->GetFile( p_eModel );
		}

		return l_pathReturn;
	}

	void ShaderProgramBase::SetSource( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel, String const & p_strSource )
	{
		if ( m_pShaders[p_eTarget] )
		{
			if ( p_eModel == eSHADER_MODEL_COUNT )
			{
				for ( int i = 0; i < eSHADER_MODEL_COUNT; ++i )
				{
					if ( m_pRenderSystem->CheckSupport( eSHADER_MODEL( i ) ) )
					{
						m_pShaders[p_eTarget]->SetSource( eSHADER_MODEL( i ), p_strSource );
					}
				}
			}
			else
			{
				m_pShaders[p_eTarget]->SetSource( p_eModel, p_strSource );
			}
		}

		ResetToCompile();
	}

	String ShaderProgramBase::GetSource( eSHADER_TYPE p_eTarget, eSHADER_MODEL p_eModel )const
	{
		String l_strReturn;

		if ( m_pShaders[p_eTarget] )
		{
			l_strReturn = m_pShaders[p_eTarget]->GetSource( p_eModel );
		}

		return l_strReturn;
	}

	bool ShaderProgramBase::HasSource( eSHADER_TYPE p_eTarget )const
	{
		bool l_bReturn = false;

		if ( m_pShaders[p_eTarget] )
		{
			l_bReturn = m_pShaders[p_eTarget]->HasSource();
		}

		return l_bReturn;
	}

	void ShaderProgramBase::SetEntryPoint( eSHADER_TYPE p_eTarget, String const & p_strName )
	{
		if ( m_pShaders[p_eTarget] )
		{
			m_pShaders[p_eTarget]->SetEntryPoint( p_strName );
		}

		ResetToCompile();
	}

	String ShaderProgramBase::GetEntryPoint( eSHADER_TYPE p_eTarget )const
	{
		String l_strReturn;

		if ( m_pShaders[p_eTarget] )
		{
			l_strReturn = m_pShaders[p_eTarget]->GetEntryPoint();
		}

		return l_strReturn;
	}

	bool ShaderProgramBase::HasProgram( eSHADER_TYPE p_eTarget )const
	{
		return m_pShaders[p_eTarget] && m_pShaders[p_eTarget]->HasSource() && m_pShaders[p_eTarget]->GetStatus() == eSHADER_STATUS_COMPILED;
	}

	OneTextureFrameVariableSPtr ShaderProgramBase::CreateFrameVariable( String const & p_strName, eSHADER_TYPE p_eType, int p_iNbOcc )
	{
		OneTextureFrameVariableSPtr l_pReturn = FindFrameVariable( p_strName, p_eType );

		if ( !l_pReturn )
		{
			l_pReturn = DoCreateTextureVariable( p_iNbOcc );
			l_pReturn->SetName( p_strName );

			if ( m_pShaders[p_eType] )
			{
				m_pShaders[p_eType]->AddFrameVariable( l_pReturn );
			}
		}

		return l_pReturn;
	}

	OneTextureFrameVariableSPtr ShaderProgramBase::FindFrameVariable( Castor::String const & p_strName, eSHADER_TYPE p_eType )const
	{
		OneTextureFrameVariableSPtr l_pReturn;

		if ( m_pShaders[p_eType] )
		{
			l_pReturn = m_pShaders[p_eType]->FindFrameVariable( p_strName );
		}

		return l_pReturn;
	}

	void ShaderProgramBase::AddFrameVariableBuffer( FrameVariableBufferSPtr p_pVariableBuffer, uint32_t p_shaderMask )
	{
		auto l_it = m_frameVariableBuffersByName.find( p_pVariableBuffer->GetName() );

		if ( l_it == m_frameVariableBuffersByName.end() )
		{
			m_listFrameVariableBuffers.push_back( p_pVariableBuffer );
			m_frameVariableBuffersByName.insert( std::make_pair( p_pVariableBuffer->GetName(), p_pVariableBuffer ) );

			for ( int i = 0; i < eSHADER_TYPE_COUNT; ++i )
			{
				if ( p_shaderMask & ( 0x1 << i ) )
				{
					m_frameVariableBuffers[i].push_back( p_pVariableBuffer );
				}
			}
		}
	}

	FrameVariableBufferSPtr ShaderProgramBase::FindFrameVariableBuffer( Castor::String const & p_strName )const
	{
		FrameVariableBufferSPtr l_buffer;
		auto l_it = m_frameVariableBuffersByName.find( p_strName );

		if ( l_it != m_frameVariableBuffersByName.end() )
		{
			l_buffer = l_it->second.lock();
		}

		return l_buffer;
	}
}
