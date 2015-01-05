#include "ShaderProgram.hpp"
#include "ShaderObject.hpp"
#include "FrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"
#include "Camera.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "Engine.hpp"
#include "DynamicTexture.hpp"
#include "TextureUnit.hpp"
#include "Scene.hpp"
#include "Light.hpp"
#include "Context.hpp"
#include "Sampler.hpp"
#include "Pipeline.hpp"

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

	const String ShaderProgramBase::CameraPos			=	cuT( "c3d_v3CameraPosition" );
	const String ShaderProgramBase::AmbientLight		=	cuT( "c3d_v4AmbientLight" );
	const String ShaderProgramBase::BackgroundColour	=	cuT( "c3d_v4BackgroundColour" );
	const String ShaderProgramBase::LightsCount			=	cuT( "c3d_iLightsCount" );
	const String ShaderProgramBase::Lights				=	cuT( "c3d_sLights" );
	const String ShaderProgramBase::MatAmbient			=	cuT( "c3d_v4MatAmbient" );
	const String ShaderProgramBase::MatDiffuse			=	cuT( "c3d_v4MatDiffuse" );
	const String ShaderProgramBase::MatSpecular			=	cuT( "c3d_v4MatSpecular" );
	const String ShaderProgramBase::MatEmissive			=	cuT( "c3d_v4MatEmissive" );
	const String ShaderProgramBase::MatShininess		=	cuT( "c3d_fMatShininess" );
	const String ShaderProgramBase::MatOpacity			=	cuT( "c3d_fMatOpacity" );
	const String ShaderProgramBase::MapColour			=	cuT( "c3d_mapColour" );
	const String ShaderProgramBase::MapAmbient			=	cuT( "c3d_mapAmbient" );
	const String ShaderProgramBase::MapDiffuse			=	cuT( "c3d_mapDiffuse" );
	const String ShaderProgramBase::MapSpecular			=	cuT( "c3d_mapSpecular" );
	const String ShaderProgramBase::MapNormal			=	cuT( "c3d_mapNormal" );
	const String ShaderProgramBase::MapOpacity			=	cuT( "c3d_mapOpacity" );
	const String ShaderProgramBase::MapGloss			=	cuT( "c3d_mapGloss" );
	const String ShaderProgramBase::MapHeight			=	cuT( "c3d_mapHeight" );

	//*************************************************************************************************

	ShaderProgramBase::ShaderProgramBase( RenderSystem * p_pRenderSystem, eSHADER_LANGUAGE p_eLanguage )
		:	m_eStatus( ePROGRAM_STATUS_NOTLINKED )
		,	m_bEnabled( true )
		,	m_eLanguage( p_eLanguage )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pLightsTexture( std::make_shared< TextureUnit >( p_pRenderSystem->GetEngine() ) )
		,	m_bLightsChanged( true )
	{
		m_pPassBuffer = p_pRenderSystem->CreateFrameVariableBuffer( cuT( "Pass" ) );
		m_pMatrixBuffer = p_pRenderSystem->CreateFrameVariableBuffer( cuT( "Matrices" ) );
		m_pSceneBuffer = p_pRenderSystem->CreateFrameVariableBuffer( cuT( "Scene" ) );
		m_pUserBuffer = p_pRenderSystem->CreateFrameVariableBuffer( cuT( "User" ) );
		m_pLightsData = PxBufferBase::create( Size( 1000, 1 ), ePIXEL_FORMAT_ARGB32F );
		DynamicTextureSPtr l_pTexture = p_pRenderSystem->CreateDynamicTexture();
		l_pTexture->SetDimension( eTEXTURE_DIMENSION_1D );
		l_pTexture->SetImage( m_pLightsData );
		SamplerSPtr l_pSampler = p_pRenderSystem->GetEngine()->CreateSampler( cuT( "LightsSampler" ) );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
		m_pLightsTexture->SetAutoMipmaps( true );
		m_pLightsTexture->SetSampler( l_pSampler );
		m_pLightsTexture->SetTexture( l_pTexture );

		for ( int i = 0; i < 100; i++ )
		{
			m_setFreeLights.insert( i );
		}
	}

	ShaderProgramBase::~ShaderProgramBase()
	{
	}

	void ShaderProgramBase::Cleanup()
	{
		m_pCameraPos.reset();
		m_pLights.reset();
		m_pLightsCount.reset();

		for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pShaders[i] )
			{
				m_pShaders[i]->FlushFrameVariables();
				m_pShaders[i]->DestroyProgram();
			}
		}

		clear_container( m_arrayFiles );
		m_pSceneBuffer->Cleanup();
		m_pMatrixBuffer->Cleanup();
		m_pPassBuffer->Cleanup();
		m_pUserBuffer->Cleanup();
	}

	void ShaderProgramBase::Initialise()
	{
		if ( m_eStatus == ePROGRAM_STATUS_NOTLINKED )
		{
			if ( m_pRenderSystem->UseShaders() )
			{
				bool l_bResult = true;

				for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT && l_bResult; i++ )
				{
					if ( m_pShaders[i] && m_pShaders[i]->HasSource() )
					{
						m_pShaders[i]->DestroyProgram();
						m_pShaders[i]->CreateProgram();

						if ( !m_pShaders[i]->Compile() && m_pShaders[i]->GetStatus() == eSHADER_STATUS_ERROR )
						{
							Logger::LogError( cuT( "ShaderProgram::Initialise - " ) + m_pShaders[i]->GetCurrentFile() + cuT( " - COMPILER ERROR" ) );
							m_pShaders[i]->DestroyProgram();
							m_eStatus = ePROGRAM_STATUS_ERROR;
							l_bResult = false;
						}
					}
				}

				if ( l_bResult )
				{
					if ( !Link() )
					{
						Logger::LogError( cuT( "ShaderProgram::Initialise - LINKER ERROR" ) );

						for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
						{
							if ( m_pShaders[i] )
							{
								m_pShaders[i]->DestroyProgram();
							}
						}

						m_eStatus = ePROGRAM_STATUS_ERROR;
					}
					else
					{
						m_pLightsTexture->Initialise();
						Logger::LogMessage( cuT( "ShaderProgram::Initialise - Program Linked successfully" ) );
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

			for ( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
			{
				if ( !m_arrayFiles[i].empty() )
				{
					m_pShaders[p_eType]->SetFile( eSHADER_MODEL( i ), m_arrayFiles[i] );
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
				m_pLights = CreateFrameVariable( ShaderProgramBase::Lights, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapAmbient, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapNormal, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapSpecular, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapOpacity, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapGloss, eSHADER_TYPE_PIXEL );
				CreateFrameVariable( ShaderProgramBase::MapHeight, eSHADER_TYPE_PIXEL );
				m_pLights->SetValue( m_pLightsTexture->GetTexture().get() );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgramBase::MatAmbient, 1 );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgramBase::MatDiffuse, 1 );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgramBase::MatEmissive, 1 );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgramBase::MatSpecular, 1 );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_FLOAT, ShaderProgramBase::MatShininess, 1 );
				m_pPassBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_FLOAT, ShaderProgramBase::MatOpacity, 1 );
				m_pSceneBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_INT, ShaderProgramBase::LightsCount, 1 );
				m_pSceneBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC4F, ShaderProgramBase::AmbientLight, 1 );
				m_pSceneBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_VEC3R, ShaderProgramBase::CameraPos, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxProjection, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxModel, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxView, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxModelView, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxProjectionView, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxProjectionModelView, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxNormal, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxTexture0, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxTexture1, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxTexture2, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxTexture3, 1 );
				m_pMatrixBuffer->CreateVariable( *this, eFRAME_VARIABLE_TYPE_MAT4X4R, Pipeline::MtxBones, 100 );
				m_pSceneBuffer->GetVariable( ShaderProgramBase::LightsCount, m_pLightsCount );
				m_pSceneBuffer->GetVariable( ShaderProgramBase::CameraPos, m_pCameraPos );
				m_pSceneBuffer->Initialise( *this );
				m_pMatrixBuffer->Initialise( *this );
				m_pPassBuffer->Initialise( *this );
				m_pUserBuffer->Initialise( *this );

				for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
				{
					if ( m_pShaders[i] )
					{
						std::for_each( m_pShaders[i]->GetFrameVariablesBegin(), m_pShaders[i]->GetFrameVariablesEnd(), [&]( std::pair< String, FrameVariableWPtr > p_pair )
						{
							p_pair.second.lock()->Initialise();
						} );
					}
				}

				m_eStatus = ePROGRAM_STATUS_LINKED;
			}

			l_bReturn = m_eStatus == ePROGRAM_STATUS_LINKED;
		}

		return l_bReturn;
	}

	void ShaderProgramBase::Begin( uint8_t CU_PARAM_UNUSED( p_byIndex ), uint8_t CU_PARAM_UNUSED( p_byCount ) )
	{
		if ( m_pRenderSystem->UseShaders() && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			Scene * l_pScene = m_pRenderSystem->GetTopScene();

			if ( l_pScene )
			{
				m_pRenderSystem->RenderAmbientLight( l_pScene->GetAmbientLight(), this );

				for ( LightPtrIntMap::iterator l_it = l_pScene->LightsBegin(); l_it != l_pScene->LightsEnd(); ++l_it )
				{
					l_it->second->Render( this );
					m_pLightsCount->GetValue( 0 )++;
				}
			}

			Camera * l_pCamera = m_pRenderSystem->GetCurrentCamera();

			if ( l_pCamera )
			{
				Point3r l_position = l_pCamera->GetParent()->GetDerivedPosition();

				if ( m_pRenderSystem->GetMainContext()->IsDeferredShadingSet() )
				{
					m_pCameraPos->SetValue( Castor::MtxUtils::mult( m_pRenderSystem->GetPipeline()->GetMatrix( eMTXMODE_VIEW ), l_position ) );
//					m_pCameraPos->SetValue( l_position );
				}
				else
				{
					m_pCameraPos->SetValue( l_position );
				}
			}

			for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
			{
				if ( m_pShaders[i] )
				{
					m_pShaders[i]->Bind();
				}
			}

			if ( l_pScene )
			{
				m_pLightsTexture->Render();

				if ( m_bLightsChanged )
				{
					m_pLightsTexture->UploadImage( false );
					m_bLightsChanged = false;
				}
			}
		}
	}

	void ShaderProgramBase::End()
	{
		if ( m_pRenderSystem->UseShaders() && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			Scene * l_pScene = m_pRenderSystem->GetTopScene();

			if ( l_pScene )
			{
				m_pLightsTexture->EndRender();

				for ( LightPtrIntMap::iterator l_it = l_pScene->LightsBegin(); l_it != l_pScene->LightsEnd(); ++l_it )
				{
					l_it->second->EndRender( this );
					m_pLightsCount->GetValue( 0 )--;
				}
			}

			for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
			{
				if ( m_pShaders[i] )
				{
					m_pShaders[i]->Unbind();
				}
			}
		}
	}

	void ShaderProgramBase::SetFile( eSHADER_MODEL p_eModel, Path const & p_path )
	{
		m_arrayFiles[p_eModel] = p_path;

		for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pShaders[i] && m_pRenderSystem->HasShaderType( eSHADER_TYPE( i ) ) )
			{
				m_pShaders[i]->SetFile( p_eModel, p_path );
			}
		}

		ResetToCompile();
	}

	int ShaderProgramBase::AssignLight()
	{
		int l_iReturn = -1;

		if ( m_setFreeLights.size() > 0 )
		{
			l_iReturn = ( * m_setFreeLights.begin() );
			m_setFreeLights.erase( m_setFreeLights.begin() );
		}

		return l_iReturn;
	}

	void ShaderProgramBase::FreeLight( int p_iIndex )
	{
		if ( p_iIndex != -1 )
		{
			if ( m_setFreeLights.find( p_iIndex ) == m_setFreeLights.end() )
			{
				m_setFreeLights.insert( p_iIndex );
			}
		}
		else
		{
			m_pLightsCount->SetValue( 0 );
			Logger::LogError( "coin" );
		}
	}

	void ShaderProgramBase::SetLightAmbient( int p_iIndex, Colour const & p_crColour )
	{
		m_bLightsChanged = true;
		Point4ub l_ptComponents;
		p_crColour.to_bgra( l_ptComponents );
		uint8_t const * l_pSrc = l_ptComponents.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 0, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_A8R8G8B8, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 0, p_crColour );
	}

	void ShaderProgramBase::SetLightDiffuse( int p_iIndex, Colour const & p_crColour )
	{
		m_bLightsChanged = true;
		Point4ub l_ptComponents;
		p_crColour.to_bgra( l_ptComponents );
		uint8_t const * l_pSrc = l_ptComponents.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 1, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_A8R8G8B8, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 1, p_crColour );
	}

	void ShaderProgramBase::SetLightSpecular( int p_iIndex, Colour const & p_crColour )
	{
		m_bLightsChanged = true;
		Point4ub l_ptComponents;
		p_crColour.to_bgra( l_ptComponents );
		uint8_t const * l_pSrc = l_ptComponents.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 2, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_A8R8G8B8, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 2, p_crColour );
	}

	void ShaderProgramBase::SetLightPosition( int p_iIndex, Point4f const & p_ptPosition )
	{
		m_bLightsChanged = true;
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( p_ptPosition.const_ptr() ) );
		uint8_t const * l_pSrc = l_px.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 3, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 3, l_px );
	}

	void ShaderProgramBase::SetLightOrientation( int p_iIndex, Matrix4x4r const & p_mtxOrientation )
	{
		m_bLightsChanged = true;
#if CASTOR_USE_DOUBLE
		Matrix4x4f l_mtxOrientation( p_mtxOrientation.const_ptr() );
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( l_mtxOrientation[0] ) );
		uint8_t const * l_pSrc = l_px.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 4, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 4, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( l_mtxOrientation[1] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 5, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 5, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( l_mtxOrientation[2] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 6, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 6, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( l_mtxOrientation[3] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 7, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 7, l_px );
#else
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( p_mtxOrientation[0] ) );
		uint8_t const * l_pSrc = l_px.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 8, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 4, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( p_mtxOrientation[1] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 5, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 5, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( p_mtxOrientation[2] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 6, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 6, l_px );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( p_mtxOrientation[3] ) );
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 7, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 7, l_px );
#endif
	}

	void ShaderProgramBase::SetLightAttenuation( int p_iIndex, Point3f const & p_ptAtt )
	{
		m_bLightsChanged = true;
		Point4f l_ptAtt( p_ptAtt[0], p_ptAtt[1], p_ptAtt[2] );
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		l_px.set< ePIXEL_FORMAT_ARGB32F >( reinterpret_cast< uint8_t const * >( l_ptAtt.const_ptr() ) );
		uint8_t const * l_pSrc = l_px.const_ptr();
		uint8_t * l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 8, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 8, l_px );
	}

	void ShaderProgramBase::SetLightExponent( int p_iIndex, float p_fExp )
	{
		m_bLightsChanged = true;
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		uint8_t const * l_pSrc = m_pLightsData->get_at( p_iIndex * 10 + 9, 0 );
		uint8_t * l_pDst = l_px.ptr();
		PF::ConvertPixel( m_pLightsData->format(), l_pSrc, ePIXEL_FORMAT_ARGB32F, l_pDst );
		//	m_pLightsData->GetPixel( p_iIndex, 9, l_px );
		reinterpret_cast< float * >( l_px.ptr() )[0] = p_fExp;
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 9, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 9, l_pxComponents );
	}

	void ShaderProgramBase::SetLightCutOff( int p_iIndex, float p_fCut )
	{
		m_bLightsChanged = true;
		Pixel< ePIXEL_FORMAT_ARGB32F > l_px( true );
		uint8_t const * l_pSrc = m_pLightsData->get_at( p_iIndex * 10 + 9, 0 );
		uint8_t * l_pDst = l_px.ptr();
		PF::ConvertPixel( m_pLightsData->format(), l_pSrc, ePIXEL_FORMAT_ARGB32F, l_pDst );
		//	m_pLightsData->GetPixel( p_iIndex, 9, l_px );
		reinterpret_cast< float * >( l_px.ptr() )[1] = p_fCut;
		l_pSrc = l_px.const_ptr();
		l_pDst = m_pLightsData->get_at( p_iIndex * 10 + 9, 0 );
		PF::ConvertPixel( ePIXEL_FORMAT_ARGB32F, l_pSrc, m_pLightsData->format(), l_pDst );
		//	m_pLightsData->SetPixel( p_iIndex, 9, l_px );
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
}
