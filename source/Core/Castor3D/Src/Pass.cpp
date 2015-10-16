#include "Pass.hpp"

#include "BlendState.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "Material.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "RenderSystem.hpp"
#include "SceneNode.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "StaticTexture.hpp"
#include "TextureUnit.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< class Type >
		void RetrieveVariable( std::weak_ptr< Type > & p_member, String const & p_name, FrameVariableBufferSPtr p_frameVariablesBuffer )
		{
			std::shared_ptr< Type > l_variable;

			if ( p_frameVariablesBuffer )
			{
				p_frameVariablesBuffer->GetVariable( p_name, l_variable );
				p_member = l_variable;
			}
		}
	}

	Pass::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< Pass >( p_path )
	{
	}

	bool Pass::BinaryParser::Fill( Pass const & p_pass, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MATERIAL_PASS );

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetAmbient(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetDiffuse(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetSpecular(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetEmissive(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetShininess(), eCHUNK_TYPE_PASS_EXPONENT, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.GetAlpha(), eCHUNK_TYPE_PASS_ALPHA, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_pass.IsTwoSided(), eCHUNK_TYPE_PASS_TWOSIDED, l_chunk );
		}

		if ( l_return )
		{
			uint32_t l_uiValues[] = { p_pass.GetBlendState()->GetAlphaSrcBlend(), p_pass.GetBlendState()->GetAlphaDstBlend() };
			l_return = DoFillChunk( l_uiValues, eCHUNK_TYPE_PASS_BLEND_FUNC, l_chunk );
		}

		if ( l_return )
		{
			ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();

			if ( l_pProgram )
			{
				l_return = ShaderProgramBase::BinaryParser( m_path ).Fill( *l_pProgram, l_chunk );
			}
		}

		if ( l_return )
		{
			for ( uint32_t i = 0; i < p_pass.GetTextureUnitsCount(); i++ )
			{
				TextureUnitSPtr l_pUnit = p_pass.GetTextureUnit( i );

				if ( l_pUnit )
				{
					l_return = TextureUnit::BinaryParser( m_path ).Fill( *l_pUnit, l_chunk );
				}
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			l_return = p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool Pass::BinaryParser::Parse( Pass & p_pass, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		p_pass.Cleanup();
		float l_fFloat;
		bool l_bBool;
		Colour l_colour;
		eBLEND l_eSrc = eBLEND_COUNT;
		eBLEND l_eDst = eBLEND_COUNT;
		ShaderProgramBaseSPtr l_pProgram;
		TextureUnitSPtr l_pUnit;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_PASS_ALPHA:
					l_return = DoParseChunk( l_fFloat, l_chunk );

					if ( l_return )
					{
						p_pass.SetAlpha( l_fFloat );
					}

					break;

				case eCHUNK_TYPE_PASS_EXPONENT:
					l_return = DoParseChunk( l_fFloat, l_chunk );

					if ( l_return )
					{
						p_pass.SetShininess( l_fFloat );
					}

					break;

				case eCHUNK_TYPE_PASS_AMBIENT:
					l_return = DoParseChunk( p_pass.GetAmbient(), l_chunk );
					break;

				case eCHUNK_TYPE_PASS_DIFFUSE:
					l_return = DoParseChunk( p_pass.GetAmbient(), l_chunk );
					break;

				case eCHUNK_TYPE_PASS_SPECULAR:
					l_return = DoParseChunk( p_pass.GetAmbient(), l_chunk );
					break;

				case eCHUNK_TYPE_PASS_EMISSIVE:
					l_return = DoParseChunk( p_pass.GetAmbient(), l_chunk );
					break;

				case eCHUNK_TYPE_PASS_TWOSIDED:
					l_return = DoParseChunk( l_bBool, l_chunk );

					if ( l_return )
					{
						p_pass.SetTwoSided( l_bBool );
					}

					break;

				case eCHUNK_TYPE_PASS_GLSHADER:
					l_pProgram = p_pass.GetOwner()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
					l_return = ShaderProgramBase::BinaryParser( m_path ).Parse( *l_pProgram, l_chunk );

					if ( l_return )
					{
						p_pass.SetShader( l_pProgram );
					}

					break;

				case eCHUNK_TYPE_PASS_HLSHADER:
					l_pProgram = p_pass.GetOwner()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_HLSL );
					l_return = ShaderProgramBase::BinaryParser( m_path ).Parse( *l_pProgram, l_chunk );

					if ( l_return )
					{
						p_pass.SetShader( l_pProgram );
					}

					break;

				case eCHUNK_TYPE_PASS_TEXTURE:
					l_pUnit = p_pass.AddTextureUnit();
					l_return = TextureUnit::BinaryParser( m_path ).Parse( *l_pUnit, l_chunk );

					if ( !l_return )
					{
						p_pass.DestroyTextureUnit( p_pass.GetTextureUnitsCount() - 1 );
					}

					break;

				case eCHUNK_TYPE_PASS_BLEND_FUNC:
					l_return = DoParseChunk( l_eSrc, l_chunk ) && DoParseChunk( l_eDst, l_chunk );

					if ( l_return )
					{
						p_pass.GetBlendState()->SetRgbSrcBlend( l_eSrc );
						p_pass.GetBlendState()->SetRgbDstBlend( l_eDst );
						p_pass.GetBlendState()->SetAlphaSrcBlend( l_eSrc );
						p_pass.GetBlendState()->SetAlphaDstBlend( l_eDst );
					}

					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
				l_return = false;
			}
		}

		return l_return;
	}

	//*********************************************************************************************

	Pass::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< Pass, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool Pass::TextLoader::operator()( Pass const & p_pass, TextFile & p_file )
	{
		static const String StrBlendFactors[eBLEND_COUNT] =
		{
			cuT( "zero" ),
			cuT( "one" ),
			cuT( "src_colour" ),
			cuT( "inv_src_colour" ),
			cuT( "dst_colour" ),
			cuT( "inv_dst_colour" ),
			cuT( "src_alpha" ),
			cuT( "inv_src_alpha" ),
			cuT( "dst_alpha" ),
			cuT( "inv_dst_alpha" ),
			cuT( "constant" ),
			cuT( "inv_constant" ),
			cuT( "src_alpha_sat" ),
			cuT( "src1_colour" ),
			cuT( "inv_src1_colour" ),
			cuT( "src1_alpha" ),
			cuT( "inv_src1_alpha" ),
		};
		bool l_return = p_file.WriteText( cuT( "\tpass\n\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\t\tambient " ) ) > 0;
		}

		if ( l_return )
		{
			l_return = Colour::TextLoader()( p_pass.GetAmbient(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\n\t\tdiffuse " ) ) > 0;
		}

		if ( l_return )
		{
			l_return = Colour::TextLoader()( p_pass.GetDiffuse(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\n\t\temissive " ) ) > 0;
		}

		if ( l_return )
		{
			l_return = Colour::TextLoader()( p_pass.GetEmissive(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\n\t\tspecular " ) ) > 0;
		}

		if ( l_return )
		{
			l_return = Colour::TextLoader()( p_pass.GetSpecular(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\n\t\tshininess %f\n" ), p_pass.GetShininess() ) > 0;
		}

		if ( l_return && p_pass.GetAlpha() < 1 )
		{
			l_return = p_file.Print( 256, cuT( "\n\t\talpha %f\n" ), p_pass.GetAlpha() ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\ttwo_sided " ) + String( p_pass.IsTwoSided() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
		}

		if ( l_return && ( p_pass.GetBlendState()->GetAlphaSrcBlend() != eBLEND_ONE || p_pass.GetBlendState()->GetAlphaDstBlend() != eBLEND_ZERO ) )
		{
			l_return = p_file.WriteText( cuT( "\t\tblend_func " ) + StrBlendFactors[p_pass.GetBlendState()->GetAlphaSrcBlend()] + cuT( " " ) + StrBlendFactors[p_pass.GetBlendState()->GetAlphaDstBlend()] + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			uint32_t l_uiNbTextureUnits = p_pass.GetTextureUnitsCount();
			bool l_first = true;

			for ( uint32_t i = 0; i < l_uiNbTextureUnits && l_return; i++ )
			{
				p_file.WriteText( cuT( "\n" ) );
				l_return = TextureUnit::TextLoader()( * p_pass.GetTextureUnit( i ), p_file );
			}
		}

		if ( l_return && p_pass.HasShader() )
		{
			l_return = ShaderProgramBase::TextLoader()( * p_pass.GetShader<ShaderProgramBase>(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	std::map< eTEXTURE_CHANNEL, String > TEXTURE_CHANNEL_NAME;

	//*********************************************************************************************

	Pass::Pass( Engine & p_engine, MaterialSPtr p_parent )
		: OwnedBy< Engine >( p_engine )
		, m_fShininess( 50.0 )
		, m_bDoubleFace( false )
		, m_pParent( p_parent )
		, m_clrDiffuse( Colour::from_rgba( 0xFFFFFFFF ) )
		, m_clrAmbient( Colour::from_rgba( 0x000000FF ) )
		, m_clrSpecular( Colour::from_rgba( 0xFFFFFFFF ) )
		, m_clrEmissive( Colour::from_rgba( 0x000000FF ) )
		, m_fAlpha( 1.0f )
		, m_pBlendState( p_engine.GetRenderSystem()->CreateBlendState() )
		, m_uiTextureFlags( 0 )
		, m_bAutomaticShader( true )
		, m_alphaBlendMode( eBLEND_MODE_ADDITIVE )
		, m_colourBlendMode( eBLEND_MODE_ADDITIVE )
	{
		if ( TEXTURE_CHANNEL_NAME.empty() )
		{
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_COLOUR] = cuT( "Colour" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_DIFFUSE] = cuT( "Diffuse" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_NORMAL] = cuT( "Normal" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_OPACITY] = cuT( "Opacity" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_SPECULAR] = cuT( "Specular" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_HEIGHT] = cuT( "Height" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_AMBIENT] = cuT( "Ambient" );
			TEXTURE_CHANNEL_NAME[eTEXTURE_CHANNEL_GLOSS] = cuT( "Gloss" );
		}
	}

	Pass::~Pass()
	{
		m_arrayTextureUnits.clear();
	}

	void Pass::Initialise()
	{
		TextureUnitSPtr l_pOpaSrc;
		PxBufferBaseSPtr l_pImageOpa;
		bool l_bHasAlpha = m_fAlpha < 1;
		m_uiTextureFlags = 0;

		for ( auto l_unit : m_arrayTextureUnits )
		{
			l_unit->SetIndex( 0 );
		}

		// Lights texture is at index 0, so start at index 1
		uint32_t l_uiIndex = 1;
		TextureUnitSPtr l_pAmbientMap = GetTextureUnit( eTEXTURE_CHANNEL_AMBIENT );
		TextureUnitSPtr l_pColourMap = GetTextureUnit( eTEXTURE_CHANNEL_COLOUR );
		TextureUnitSPtr l_pDiffuseMap = GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE );
		TextureUnitSPtr l_pNormalMap = GetTextureUnit( eTEXTURE_CHANNEL_NORMAL );
		TextureUnitSPtr l_pSpecularMap = GetTextureUnit( eTEXTURE_CHANNEL_SPECULAR );
		TextureUnitSPtr l_pOpacityMap = GetTextureUnit( eTEXTURE_CHANNEL_OPACITY );
		TextureUnitSPtr l_pGlossMap = GetTextureUnit( eTEXTURE_CHANNEL_GLOSS );
		TextureUnitSPtr l_pHeightMap = GetTextureUnit( eTEXTURE_CHANNEL_HEIGHT );

		l_bHasAlpha |= DoPrepareTexture( eTEXTURE_CHANNEL_AMBIENT, l_pAmbientMap, l_uiIndex, l_pOpaSrc, l_pImageOpa );
		l_bHasAlpha |= DoPrepareTexture( eTEXTURE_CHANNEL_COLOUR, l_pColourMap, l_uiIndex, l_pOpaSrc, l_pImageOpa );
		l_bHasAlpha |= DoPrepareTexture( eTEXTURE_CHANNEL_DIFFUSE, l_pDiffuseMap, l_uiIndex, l_pOpaSrc, l_pImageOpa );

		DoPrepareTexture( eTEXTURE_CHANNEL_NORMAL, l_pNormalMap, l_uiIndex );
		DoPrepareTexture( eTEXTURE_CHANNEL_SPECULAR, l_pSpecularMap, l_uiIndex );
		DoPrepareTexture( eTEXTURE_CHANNEL_GLOSS, l_pGlossMap, l_uiIndex );
		DoPrepareTexture( eTEXTURE_CHANNEL_HEIGHT, l_pHeightMap, l_uiIndex );

		if ( l_pOpacityMap && l_pOpacityMap->GetImagePixels() )
		{
			PxBufferBaseSPtr l_pReducted = l_pOpacityMap->GetImagePixels();
			PF::ReduceToAlpha( l_pReducted );
			l_pOpacityMap->GetTexture()->SetImage( l_pReducted );
			l_bHasAlpha = true;
			l_pImageOpa.reset();
		}
		else if ( l_pImageOpa )
		{
			l_pOpacityMap = AddTextureUnit();
			l_pOpacityMap->SetAutoMipmaps( l_pOpaSrc->GetAutoMipmaps() );
			l_pOpacityMap->SetChannel( eTEXTURE_CHANNEL_OPACITY );
			StaticTextureSPtr l_pTexture = GetOwner()->GetRenderSystem()->CreateStaticTexture();
			l_pTexture->SetType( eTEXTURE_TYPE_2D );
			l_pTexture->SetImage( l_pImageOpa );
			l_pTexture->SetSampler( l_pOpaSrc->GetTexture()->GetSampler() );
			l_pOpacityMap->SetTexture( l_pTexture );
			l_pImageOpa.reset();
		}

		if ( l_pOpacityMap )
		{
			l_pOpacityMap->SetIndex( l_uiIndex++ );
			Logger::LogDebug( StringStream() << cuT( "	Opacity map at index " ) << l_pOpacityMap->GetIndex() );
			m_uiTextureFlags |= eTEXTURE_CHANNEL_OPACITY;
			l_pOpacityMap->Initialise();
			l_bHasAlpha = true;
		}

		if ( l_bHasAlpha && !m_pBlendState->IsBlendEnabled() )
		{
			m_pBlendState->EnableBlend( true );

			if ( GetOwner()->GetRenderSystem()->GetCurrentContext()->IsMultiSampling() )
			{
				m_pBlendState->EnableAlphaToCoverage( true );
				m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
				m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
				m_pBlendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
				m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
			}
			else
			{
				switch ( m_alphaBlendMode )
				{
				case eBLEND_MODE_ADDITIVE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_ONE );
					m_pBlendState->SetAlphaDstBlend( eBLEND_ONE );
					break;

				case eBLEND_MODE_MULTIPLICATIVE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_ZERO );
					m_pBlendState->SetAlphaDstBlend( eBLEND_SRC_COLOUR );
					break;

				default:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
					m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
					break;
				}

				m_pBlendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
				m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
			}
		}

		for ( auto l_unit : m_arrayTextureUnits )
		{
			if ( l_unit->GetIndex() == 0 )
			{
				l_unit->SetIndex( l_uiIndex++ );
				l_unit->Initialise();
			}
		}

		m_pBlendState->Initialise();

		if ( HasShader() && !HasAutomaticShader() )
		{
			m_pShaderProgram.lock()->Initialise();
			DoGetTextures();
		}
	}

	void Pass::BindToAutomaticProgram( ShaderProgramBaseSPtr p_pProgram )
	{
		ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
		m_bAutomaticShader = true;

		if ( l_pProgram != p_pProgram && p_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_pShaderProgram = p_pProgram;
			DoGetTextures();
			DoGetBuffers();
		}
	}

	void Pass::Cleanup()
	{
		m_pBlendState->Cleanup();

		for ( auto && l_unit : m_arrayTextureUnits )
		{
			l_unit->Cleanup();
		}

		m_mapUnits.clear();
	}

	void Pass::Render( uint8_t p_index, uint8_t p_uiCount )
	{
		m_pBlendState->Apply();
		DoRender( p_index, p_uiCount );
	}

	void Pass::Render2D( uint8_t p_index, uint8_t p_uiCount )
	{
		DoRender( p_index, p_uiCount );
	}

	void Pass::EndRender()
	{
		ShaderProgramBaseSPtr l_pShader = m_pShaderProgram.lock();

		for ( auto && l_rit = m_arrayTextureUnits.rbegin(); l_rit != m_arrayTextureUnits.rend(); ++l_rit )
		{
			( *l_rit )->Unbind();
		}

		if ( l_pShader )
		{
			l_pShader->Unbind();
		}
	}

	TextureUnitSPtr Pass::AddTextureUnit()
	{
		TextureUnitSPtr l_pReturn = std::make_shared< TextureUnit >( *GetOwner() );
		uint32_t l_uiID = uint32_t( m_arrayTextureUnits.size() + 1 );
		l_pReturn->SetIndex( l_uiID );
		m_arrayTextureUnits.push_back( l_pReturn );
		return l_pReturn;
	}

	TextureUnitSPtr Pass::GetTextureUnit( eTEXTURE_CHANNEL p_eChannel )
	{
		TextureUnitSPtr l_pReturn;
		auto && l_it = m_arrayTextureUnits.begin();

		while ( l_it != m_arrayTextureUnits.end() && !l_pReturn )
		{
			if ( ( *l_it )->GetChannel() == p_eChannel )
			{
				l_pReturn = *l_it;
			}
			else
			{
				++l_it;
			}
		}

		return l_pReturn;
	}

	bool Pass::DestroyTextureUnit( uint32_t p_index )
	{
		CASTOR_ASSERT( p_index < m_arrayTextureUnits.size(), "Pass subscript out of range" );
		bool l_return = false;
		Logger::LogInfo( StringStream() << cuT( "Destroying TextureUnit " ) << p_index );
		TextureUnitPtrArray::iterator l_it = m_arrayTextureUnits.begin();
		m_arrayTextureUnits.erase( l_it + p_index );
		uint32_t i = 0;

		for ( l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
		{
			( *l_it )->SetIndex( ++i );
		}

		l_return = true;
		return l_return;
	}

	TextureUnitSPtr Pass::GetTextureUnit( uint32_t p_index )const
	{
		CASTOR_ASSERT( p_index < m_arrayTextureUnits.size(), "Pass subscript out of range" );
		return m_arrayTextureUnits[p_index];
	}

	String Pass::GetTexturePath( uint32_t p_index )
	{
		CASTOR_ASSERT( p_index < m_arrayTextureUnits.size(), "Pass subscript out of range" );
		return m_arrayTextureUnits[p_index]->GetTexturePath();
	}

	void Pass::SetShader( ShaderProgramBaseSPtr p_pProgram )
	{
		m_mapUnits.clear();
		m_pShaderProgram = p_pProgram;
		m_bAutomaticShader = false;
		DoGetBuffers();
	}

	bool Pass::HasShader()const
	{
		return ! m_pShaderProgram.expired();
	}

	bool Pass::HasAlphaBlending()const
	{
		return m_pBlendState->IsBlendEnabled();
	}

	void Pass::DoGetTexture( eTEXTURE_CHANNEL p_channel, String const & p_name, ShaderProgramBase & p_program, OneTextureFrameVariableWPtr & p_variable )
	{
		TextureUnitSPtr l_unit = GetTextureUnit( p_channel );

		if ( l_unit )
		{
			OneTextureFrameVariableSPtr l_variable = p_program.FindFrameVariable( p_name, eSHADER_TYPE_PIXEL );

			if ( l_variable )
			{
				m_mapUnits.insert( std::make_pair( p_channel, std::make_pair( l_unit, l_variable ) ) );
				p_variable = l_variable;
			}
		}
	}

	void Pass::DoFillShaderVariables()
	{
		for ( auto && l_pair : m_mapUnits )
		{
			TextureUnitSPtr l_unit = l_pair.second.first.lock();
			OneTextureFrameVariableSPtr l_variable = l_pair.second.second.lock();

			if ( l_unit && l_variable )
			{
				l_variable->SetValue( l_unit->GetTexture().get() );
			}
		}

		FrameVariableBufferSPtr l_passBuffer = m_passBuffer.lock();

		if ( l_passBuffer )
		{
			Point4f l_colour;
			GetAmbient().to_rgba( l_colour );
			m_pAmbient.lock()->SetValue( l_colour );
			GetDiffuse().to_rgba( l_colour );
			m_pDiffuse.lock()->SetValue( l_colour );
			GetSpecular().to_rgba( l_colour );
			m_pSpecular.lock()->SetValue( l_colour );
			GetEmissive().to_rgba( l_colour );
			m_pEmissive.lock()->SetValue( l_colour );
			m_pShininess.lock()->SetValue( GetShininess() );
			m_pOpacity.lock()->SetValue( GetAlpha() );
		}

		FrameVariableBufferSPtr l_sceneBuffer = m_sceneBuffer.lock();

		if ( l_sceneBuffer )
		{
			RenderSystem * l_renderSystem = GetOwner()->GetRenderSystem();

			if ( l_renderSystem->GetCurrentCamera() )
			{
				m_pCameraPos.lock()->SetValue( l_renderSystem->GetCurrentCamera()->GetParent()->GetDerivedPosition() );
			}
		}
	}

	void Pass::DoGetTextures()
	{
		ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
		m_mapUnits.clear();
		DoGetTexture( eTEXTURE_CHANNEL_AMBIENT, ShaderProgramBase::MapAmbient, *l_pProgram, m_pAmbientMap );
		DoGetTexture( eTEXTURE_CHANNEL_COLOUR, ShaderProgramBase::MapColour, *l_pProgram, m_pColourMap );
		DoGetTexture( eTEXTURE_CHANNEL_DIFFUSE, ShaderProgramBase::MapDiffuse, *l_pProgram, m_pDiffuseMap );
		DoGetTexture( eTEXTURE_CHANNEL_NORMAL, ShaderProgramBase::MapNormal, *l_pProgram, m_pNormalMap );
		DoGetTexture( eTEXTURE_CHANNEL_SPECULAR, ShaderProgramBase::MapSpecular, *l_pProgram, m_pSpecularMap );
		DoGetTexture( eTEXTURE_CHANNEL_OPACITY, ShaderProgramBase::MapOpacity, *l_pProgram, m_pOpacityMap );
		DoGetTexture( eTEXTURE_CHANNEL_GLOSS, ShaderProgramBase::MapGloss, *l_pProgram, m_pGlossMap );
		DoGetTexture( eTEXTURE_CHANNEL_HEIGHT, ShaderProgramBase::MapHeight, *l_pProgram, m_pHeightMap );
	}

	void Pass::DoGetBuffers()
	{
		ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();

		if ( l_pProgram )
		{
			m_matrixBuffer = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );
			FrameVariableBufferSPtr l_passBuffer = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferPass );

			if ( l_passBuffer )
			{
				RetrieveVariable( m_pAmbient, ShaderProgramBase::MatAmbient, l_passBuffer );
				RetrieveVariable( m_pDiffuse, ShaderProgramBase::MatDiffuse, l_passBuffer );
				RetrieveVariable( m_pSpecular, ShaderProgramBase::MatSpecular, l_passBuffer );
				RetrieveVariable( m_pEmissive, ShaderProgramBase::MatEmissive, l_passBuffer );
				RetrieveVariable( m_pShininess, ShaderProgramBase::MatShininess, l_passBuffer );
				RetrieveVariable( m_pOpacity, ShaderProgramBase::MatOpacity, l_passBuffer );
				m_passBuffer = l_passBuffer;
			}

			FrameVariableBufferSPtr l_sceneBuffer = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferScene );

			if ( l_sceneBuffer )
			{
				RetrieveVariable( m_pCameraPos, ShaderProgramBase::CameraPos, l_sceneBuffer );
				m_sceneBuffer = l_sceneBuffer;
			}
		}
	}

	bool Pass::DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, PxBufferBaseSPtr & p_opacity )
	{
		bool l_return = false;

		if ( p_unit && ( p_unit->GetImagePixels() || p_unit->GetRenderTarget() ) )
		{
			if ( p_unit->GetImagePixels() )
			{
				if ( PF::HasAlpha( p_unit->GetPixelFormat() ) )
				{
					PxBufferBaseSPtr l_pExtracted = p_unit->GetImagePixels();
					PxBufferBaseSPtr l_pTmp = PF::ExtractAlpha( l_pExtracted );
					p_unit->GetTexture()->SetImage( l_pExtracted );

					if ( !p_opacity )
					{
						p_opacity = l_pTmp;
						p_opacitySource = p_unit;
					}

					l_return = true;
				}
			}

			p_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( p_unit->GetIndex() ) );
			m_uiTextureFlags |= p_channel;
			p_unit->Initialise();
		}

		return l_return;
	}

	void Pass::DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index )
	{
		if ( p_unit && ( p_unit->GetImagePixels() || p_unit->GetRenderTarget() ) )
		{
			if ( p_unit->GetImagePixels() )
			{
				PxBufferBaseSPtr l_pExtracted = p_unit->GetImagePixels();
				PF::ExtractAlpha( l_pExtracted );
				p_unit->GetTexture()->SetImage( l_pExtracted );
			}

			p_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( p_unit->GetIndex() ) );
			m_uiTextureFlags |= p_channel;
			p_unit->Initialise();
		}
	}

	void Pass::DoRender( uint8_t p_index, uint8_t p_count )
	{
		ShaderProgramBaseSPtr l_program = m_pShaderProgram.lock();

		if ( l_program )
		{
			DoFillShaderVariables();
			l_program->Bind( p_index, p_count );
		}

		for ( auto && l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}
}
