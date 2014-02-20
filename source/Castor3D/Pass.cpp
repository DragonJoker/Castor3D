#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Pass.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/ShaderManager.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/BlendState.hpp"

using namespace Castor3D;
using namespace Castor;

//*********************************************************************************************

namespace
{
	template< class Type >
	std::shared_ptr< Type > RetrieveVariable( std::weak_ptr< Type > & p_pMemberVariable, String const & p_strName, ShaderProgramBaseWPtr p_pMemberProgram, ShaderProgramBaseSPtr p_pParameterProgram, FrameVariableBufferSPtr p_pFrameVariablesBuffer )
	{
		std::shared_ptr< Type > l_pReturn;

		if( p_pMemberVariable.expired() || p_pParameterProgram != p_pMemberProgram.lock() )
		{
			p_pFrameVariablesBuffer->GetVariable( p_strName, l_pReturn );
			p_pMemberVariable = l_pReturn;
		}
		else
		{
			l_pReturn = p_pMemberVariable.lock();
		}

		return l_pReturn;
	}
}

//*********************************************************************************************

bool PassBinaryParser :: DoFill( Pass const & p_pass, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	Colour l_colour;
	ShaderProgramBaseSPtr l_pProgram;
	TextureUnitSPtr l_pUnit;

	stCHUNK l_chunk;
	l_chunk.m_eChunkType = eCHUNK_TYPE_MATERIAL_PASS;

	if( l_bReturn )
	{
		l_bReturn = DoFillColourChunk( p_pass.GetAmbient(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillColourChunk( p_pass.GetDiffuse(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillColourChunk( p_pass.GetSpecular(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillColourChunk( p_pass.GetEmissive(), eCHUNK_TYPE_PASS_AMBIENT, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_pass.GetShininess(), eCHUNK_TYPE_PASS_EXPONENT, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_pass.GetAlpha(), eCHUNK_TYPE_PASS_ALPHA, l_chunk );
	}

	if( l_bReturn )
	{
		l_bReturn = DoFillValueChunk( p_pass.IsTwoSided(), eCHUNK_TYPE_PASS_TWOSIDED, l_chunk );
	}

	if( l_bReturn )
	{
		uint32_t l_uiValues[] = { p_pass.GetBlendState()->GetAlphaSrcBlend(), p_pass.GetBlendState()->GetAlphaDstBlend() };
		l_bReturn = DoFillValuesChunk( l_uiValues, eCHUNK_TYPE_PASS_BLEND_FUNC, l_chunk );
	}

	if( l_bReturn )
	{
		l_pProgram = p_pass.GetShader< ShaderProgramBase >();

		if( l_pProgram )
		{
			l_bReturn = ShaderProgramBinaryParser()( *l_pProgram, l_chunk );
		}
	}

	if( l_bReturn )
	{
		for( uint32_t i = 0; i < p_pass.GetTextureUnitsCount(); i++ )
		{
			l_pUnit = p_pass.GetTextureUnit( i );

			if( l_pUnit )
			{
				l_bReturn = UnitBinaryParser()( *l_pUnit, l_chunk );
			}
		}
	}

	if( l_bReturn )
	{
		l_bReturn = DoAddSubChunk( l_chunk, p_chunk );
	}

	return l_bReturn;
}

bool PassBinaryParser :: DoParse( Pass & p_pass, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	p_pass.Cleanup();
	float l_fFloat;
	bool l_bBool;
	Colour l_colour;
	eBLEND l_eSrc = eBLEND_COUNT;
	eBLEND l_eDst = eBLEND_COUNT;
	ShaderProgramBaseSPtr l_pProgram;
	TextureUnitSPtr l_pUnit;

	while( p_chunk.m_uiIndex < p_chunk.m_pData.size() )
	{
		stCHUNK l_chunk;
		l_bReturn = DoGetSubChunk( p_chunk, l_chunk );

		if( l_bReturn )
		{
			switch( l_chunk.m_eChunkType )
			{
			case eCHUNK_TYPE_PASS_ALPHA:
				l_bReturn = DoParseValue( l_fFloat, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetAlpha( l_fFloat );
				}
				break;
			case eCHUNK_TYPE_PASS_EXPONENT:
				l_bReturn = DoParseValue( l_fFloat, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetShininess( l_fFloat );
				}
				break;

			case eCHUNK_TYPE_PASS_AMBIENT:
				l_bReturn = DoParseColour( p_pass.GetAmbient(), l_chunk );
				break;

			case eCHUNK_TYPE_PASS_DIFFUSE:
				l_bReturn = DoParseColour( p_pass.GetAmbient(), l_chunk );
				break;

			case eCHUNK_TYPE_PASS_SPECULAR:
				l_bReturn = DoParseColour( p_pass.GetAmbient(), l_chunk );
				break;

			case eCHUNK_TYPE_PASS_EMISSIVE:
				l_bReturn = DoParseColour( p_pass.GetAmbient(), l_chunk );
				break;

			case eCHUNK_TYPE_PASS_TWOSIDED:
				l_bReturn = DoParseValue( l_bBool, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetDoubleFace( l_bBool );
				}
				break;

			case eCHUNK_TYPE_PASS_CGSHADER:
				l_pProgram = p_pass.GetRenderer()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_CG );
				l_bReturn = ShaderProgramBinaryParser()( *l_pProgram, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetShader( l_pProgram );
				}
				break;

			case eCHUNK_TYPE_PASS_GLSHADER:
				l_pProgram = p_pass.GetRenderer()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
				l_bReturn = ShaderProgramBinaryParser()( *l_pProgram, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetShader( l_pProgram );
				}
				break;

			case eCHUNK_TYPE_PASS_HLSHADER:
				l_pProgram = p_pass.GetRenderer()->GetRenderSystem()->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_HLSL );
				l_bReturn = ShaderProgramBinaryParser()( *l_pProgram, l_chunk );
				if( l_bReturn )
				{
					p_pass.SetShader( l_pProgram );
				}
				break;

			case eCHUNK_TYPE_PASS_TEXTURE:
				l_pUnit = p_pass.AddTextureUnit();
				l_bReturn = UnitBinaryParser()( *l_pUnit, l_chunk );
				if( !l_bReturn )
				{
					p_pass.DestroyTextureUnit( p_pass.GetTextureUnitsCount() - 1 );
				}
				break;

			case eCHUNK_TYPE_PASS_BLEND_FUNC:
				l_bReturn = DoParseValue( l_eSrc, l_chunk ) && DoParseValue( l_eDst, l_chunk );
				if( l_bReturn )
				{
					p_pass.GetBlendState()->SetRgbSrcBlend( l_eSrc );
					p_pass.GetBlendState()->SetRgbDstBlend( l_eDst );
					p_pass.GetBlendState()->SetAlphaSrcBlend( l_eSrc );
					p_pass.GetBlendState()->SetAlphaDstBlend( l_eDst );
				}
				break;
			}
		}

		if( !l_bReturn )
		{
			p_chunk.m_uiIndex = uint32_t( p_chunk.m_pData.size() );
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

//*********************************************************************************************

Pass::BinaryLoader :: BinaryLoader()
	:	Loader< Pass, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
{
}

//*********************************************************************************************

Pass::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< Pass, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool Pass::TextLoader :: operator ()( Pass const & p_pass, TextFile & p_file)
{
	static const String StrBlendFactors[eBLEND_COUNT] =
	{	cuT( "zero"				)
	,	cuT( "one"				)
	,	cuT( "src_colour"		)
	,	cuT( "inv_src_colour"	)
	,	cuT( "dst_colour"		)
	,	cuT( "inv_dst_colour"	)
	,	cuT( "src_alpha"		)
	,	cuT( "inv_src_alpha"	)
	,	cuT( "dst_alpha"		)
	,	cuT( "inv_dst_alpha"	)
	,	cuT( "constant"			)
	,	cuT( "inv_constant"		)
	,	cuT( "src_alpha_sat"	)
	,	cuT( "src1_colour"		)
	,	cuT( "inv_src1_colour"	)
	,	cuT( "src1_alpha"		)
	,	cuT( "inv_src1_alpha"	)
	};

	bool l_bReturn = p_file.WriteText( cuT( "\tpass\n\t{\n")) > 0;

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\t\tambient ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_pass.GetAmbient(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tdiffuse ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_pass.GetDiffuse(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\temissive ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_pass.GetEmissive(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tspecular ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = Colour::TextLoader()( p_pass.GetSpecular(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\n\t\tshininess %f\n"), p_pass.GetShininess()) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Print( 256, cuT( "\t\ttex_base ")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\n\t\tdouble_face ") + String( p_pass.IsTwoSided() ? cuT( "true") : cuT( "false")) + cuT( "\n")) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\t\tblend_func ") + StrBlendFactors[p_pass.GetBlendState()->GetAlphaSrcBlend()] + cuT( " ") + StrBlendFactors[p_pass.GetBlendState()->GetAlphaDstBlend()] + cuT( "\n")) > 0;
	}

	if (l_bReturn)
	{
		uint32_t l_uiNbTextureUnits = p_pass.GetTextureUnitsCount();
		bool l_bFirst = true;

		for (uint32_t i = 0; i < l_uiNbTextureUnits && l_bReturn; i++)
		{
			if (l_bFirst)
			{
				l_bFirst = false;
			}
			else
			{
				p_file.WriteText( cuT( "\n" ) );
			}

			l_bReturn = TextureUnit::TextLoader()( * p_pass.GetTextureUnit( i), p_file);
		}
	}

	if (l_bReturn && p_pass.HasShader())
	{
		l_bReturn = ShaderProgramBase::TextLoader()( * p_pass.GetShader<ShaderProgramBase>(), p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*********************************************************************************************

Pass :: Pass( Engine * p_pEngine, Material * p_parent )
	:	Renderable< Pass, PassRenderer >( p_pEngine )
	,	m_fShininess		( 50.0												)
	,	m_bDoubleFace		( false												)
	,	m_pParent			( p_parent											)
	,	m_clrDiffuse		( Colour::from_rgba( 0xFFFFFFFF )					)
	,	m_clrAmbient		( Colour::from_rgba( 0x000000FF )					)
	,	m_clrSpecular		( Colour::from_rgba( 0xFFFFFFFF )					)
	,	m_clrEmissive		( Colour::from_rgba( 0x000000FF )					)
	,	m_fAlpha			( 1.0f												)
	,	m_pBlendState		( p_pEngine->GetRenderSystem()->CreateBlendState()	)
	,	m_uiTextureFlags	( 0													)
	,	m_bAutomaticShader	( true												)
{
}

Pass :: ~Pass()
{
	m_arrayTextureUnits.clear();
}

void Pass :: Initialise()
{
	TextureUnitSPtr		l_pColourMap;
	TextureUnitSPtr		l_pAmbientMap;
	TextureUnitSPtr		l_pDiffuseMap;
	TextureUnitSPtr		l_pNormalMap;
	TextureUnitSPtr		l_pOpacityMap;
	TextureUnitSPtr		l_pSpecularMap;
	TextureUnitSPtr		l_pGlossMap;
	TextureUnitSPtr		l_pHeightMap;
	TextureUnitSPtr		l_pTexture;
	TextureUnitSPtr		l_pOpaSrc;
	PxBufferBaseSPtr	l_pImageOpa;
	bool				l_bHasAlpha = m_fAlpha < 1;
	String				l_strOpaName;
	String				l_strIndex;

	m_uiTextureFlags = 0;

	for( uint32_t i = 0; i < GetParent()->GetPassCount() && l_strIndex.empty(); i++ )
	{
		if( GetParent()->GetPass( i ).get() == this )
		{
			l_strIndex = str_utils::to_string( i );
		}
	}

	for( TextureUnitPtrArrayIt l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		(*l_it)->SetIndex( 0 );
	}

	uint32_t l_uiIndex = 1;
	l_pAmbientMap	= GetTextureUnit( eTEXTURE_CHANNEL_AMBIENT	);
	l_pColourMap	= GetTextureUnit( eTEXTURE_CHANNEL_COLOUR	);
	l_pDiffuseMap	= GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE	);
	l_pNormalMap	= GetTextureUnit( eTEXTURE_CHANNEL_NORMAL	);
	l_pSpecularMap	= GetTextureUnit( eTEXTURE_CHANNEL_SPECULAR	);
	l_pOpacityMap	= GetTextureUnit( eTEXTURE_CHANNEL_OPACITY	);
	l_pGlossMap		= GetTextureUnit( eTEXTURE_CHANNEL_GLOSS	);
	l_pHeightMap	= GetTextureUnit( eTEXTURE_CHANNEL_HEIGHT	);

	if( l_pAmbientMap && l_pAmbientMap->GetImagePixels() )
	{
		if( PF::HasAlpha( l_pAmbientMap->GetPixelFormat() ) )
		{
			PxBufferBaseSPtr l_pExtracted = l_pAmbientMap->GetImagePixels();
			PxBufferBaseSPtr l_pTmp = PF::ExtractAlpha( l_pExtracted );
			l_pAmbientMap->GetTexture()->SetImage( l_pExtracted );

			if( !l_pImageOpa )
			{
				l_pImageOpa = l_pTmp;
				l_pOpaSrc = l_pAmbientMap;
			}

			l_bHasAlpha = true;
		}

		l_pAmbientMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Ambient map at index %d" ), l_pAmbientMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_AMBIENT;
		l_pAmbientMap->Initialise();
	}

	if( l_pColourMap && l_pColourMap->GetImagePixels() )
	{
		if( PF::HasAlpha( l_pColourMap->GetPixelFormat() ) )
		{
			PxBufferBaseSPtr l_pExtracted = l_pColourMap->GetImagePixels();
			PxBufferBaseSPtr l_pTmp = PF::ExtractAlpha( l_pExtracted );
			l_pColourMap->GetTexture()->SetImage( l_pExtracted );

			if( !l_pImageOpa )
			{
				l_pImageOpa = l_pTmp;
				l_pOpaSrc = l_pColourMap;
			}

			l_bHasAlpha = true;
		}

		l_pColourMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Colour map at index %d" ), l_pColourMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_COLOUR;
		l_pColourMap->Initialise();
	}

	if( l_pDiffuseMap && l_pDiffuseMap->GetImagePixels() )
	{
		if( PF::HasAlpha( l_pDiffuseMap->GetPixelFormat() ) )
		{
			PxBufferBaseSPtr l_pExtracted = l_pDiffuseMap->GetImagePixels();
			PxBufferBaseSPtr l_pTmp = PF::ExtractAlpha( l_pExtracted );
			l_pDiffuseMap->GetTexture()->SetImage( l_pExtracted );

			if( !l_pImageOpa )
			{
				l_pImageOpa = l_pTmp;
				l_pOpaSrc = l_pDiffuseMap;
			}

			l_bHasAlpha = true;
		}

		l_pDiffuseMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Diffuse map at index %d" ), l_pDiffuseMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_DIFFUSE;
		l_pDiffuseMap->Initialise();
	}

	if( l_pNormalMap && l_pNormalMap->GetImagePixels() )
	{
		PxBufferBaseSPtr l_pExtracted = l_pNormalMap->GetImagePixels();
		PF::ExtractAlpha( l_pExtracted );
		l_pNormalMap->GetTexture()->SetImage( l_pExtracted );
		l_pNormalMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Normal map at index %d" ), l_pNormalMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_NORMAL;
		l_pNormalMap->Initialise();
	}

	if( l_pSpecularMap && l_pSpecularMap->GetImagePixels() )
	{
		PxBufferBaseSPtr l_pExtracted = l_pSpecularMap->GetImagePixels();
		PF::ExtractAlpha( l_pExtracted );
		l_pSpecularMap->GetTexture()->SetImage( l_pExtracted );
		l_pSpecularMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Specular map at index %d" ), l_pSpecularMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_SPECULAR;
		l_pSpecularMap->Initialise();
	}

	if( l_pOpacityMap && l_pOpacityMap->GetImagePixels() )
	{
		PxBufferBaseSPtr l_pReducted = l_pOpacityMap->GetImagePixels();
		PF::ReduceToAlpha( l_pReducted );
		l_pOpacityMap->GetTexture()->SetImage( l_pReducted );
		l_bHasAlpha = true;
		l_pImageOpa.reset();
	}
	else if( l_pImageOpa )
	{
		l_strOpaName = GetParent()->GetName() + cuT( "_P" ) + l_strIndex + cuT( "_OpacityMap" );
		l_pOpacityMap = AddTextureUnit();
		l_pOpacityMap->SetAutoMipmaps( l_pOpaSrc->GetAutoMipmaps() );
		l_pOpacityMap->SetChannel( eTEXTURE_CHANNEL_OPACITY );
		StaticTextureSPtr l_pTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
		l_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		l_pTexture->SetImage( l_pImageOpa );
		l_pTexture->SetSampler( l_pOpaSrc->GetTexture()->GetSampler() );
		l_pOpacityMap->SetTexture( l_pTexture );
		l_pImageOpa.reset();
	}

	if( l_pOpacityMap )
	{
		l_pOpacityMap->SetIndex( l_uiIndex++ );
/*
		l_pOpacityMap->SetAlpFunction( eALPHA_BLEND_FUNC_MODULATE );
		l_pOpacityMap->SetAlpArgument( eBLEND_SRC_INDEX_0, eBLEND_SOURCE_PREVIOUS );
		l_pOpacityMap->SetAlpArgument( eBLEND_SRC_INDEX_1, eBLEND_SOURCE_TEXTURE );
/**/
		Logger::LogDebug( cuT( "	Opacity map at index %d" ), l_pOpacityMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_OPACITY;
		l_pOpacityMap->Initialise();
		l_bHasAlpha = true;
	}

	if( l_pGlossMap && l_pGlossMap->GetImagePixels() )
	{
		PxBufferBaseSPtr l_pExtracted = l_pGlossMap->GetImagePixels();
		PF::ExtractAlpha( l_pExtracted );
		l_pGlossMap->GetTexture()->SetImage( l_pExtracted );
		l_pGlossMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Gloss map at index %d" ), l_pGlossMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_GLOSS;
		l_pGlossMap->Initialise();
	}

	if( l_pHeightMap && l_pHeightMap->GetImagePixels() )
	{
		PxBufferBaseSPtr l_pExtracted = l_pHeightMap->GetImagePixels();
		PF::ExtractAlpha( l_pExtracted );
		l_pHeightMap->GetTexture()->SetImage( l_pExtracted );
		l_pHeightMap->SetIndex( l_uiIndex++ );
		Logger::LogDebug( cuT( "	Height map at index %d" ), l_pHeightMap->GetIndex() );
		m_uiTextureFlags |= eTEXTURE_CHANNEL_HEIGHT;
		l_pHeightMap->Initialise();
	}

	if( l_bHasAlpha && !m_pBlendState->IsBlendEnabled() )
	{
		m_pBlendState->EnableBlend( true );

		m_pBlendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
		m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );

		m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );

		if( m_pEngine->GetRenderSystem()->GetCurrentContext()->IsMultiSampling() )
		{
			m_pBlendState->EnableAlphaToCoverage( true );
		}
	}

	for( TextureUnitPtrArrayIt l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		l_pTexture = (*l_it);

		if( l_pTexture->GetIndex() == 0 )
		{
			l_pTexture->SetIndex( l_uiIndex++ );
			l_pTexture->Initialise();
		}
	}

	m_pBlendState->Initialise();

	if( HasShader() && !HasAutomaticShader() )
	{
		m_pShaderProgram.lock()->Initialise();
		DoBindTextures();
	}

	if( !m_pRenderer.expired() )
	{
		m_pRenderer.lock()->Initialise();
	}
}

void Pass :: BindToProgram( ShaderProgramBaseSPtr p_pProgram )
{
	ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
	m_bAutomaticShader = true;

	if( l_pProgram != p_pProgram && p_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
	{
		m_pShaderProgram = p_pProgram;
		DoBindTextures();
	}
}

void Pass :: Cleanup()
{
	m_pBlendState->Cleanup();

	for( TextureUnitPtrArrayIt l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		(*l_it)->Cleanup();
	}

	m_mapUnits.clear();
}

void Pass :: Render( uint8_t p_uiIndex, uint8_t p_uiCount )
{
	ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
	PassRendererSPtr l_pRenderer = GetRenderer();

	if( l_pRenderer )
	{
		l_pRenderer->Render();
		m_pBlendState->Apply();
	}

	if ( l_pProgram )
	{
		for( UnitVariableChannelMapIt l_it = m_mapUnits.begin(); l_it != m_mapUnits.end(); ++l_it )
		{
			l_it->second.second.lock()->SetValue( l_it->second.first.lock()->GetTexture().get() );
		}

		l_pProgram->Begin( p_uiIndex, p_uiCount );
	}

	for( TextureUnitPtrArrayIt l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		(*l_it)->Render();
	}
}

void Pass :: Render2D( uint8_t p_uiIndex, uint8_t p_uiCount )
{
	ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
	PassRendererSPtr l_pRenderer = GetRenderer();

	if( l_pRenderer )
	{
		l_pRenderer->Render2D();
	}

	if ( l_pProgram )
	{
		for( UnitVariableChannelMapIt l_it = m_mapUnits.begin(); l_it != m_mapUnits.end(); ++l_it )
		{
			l_it->second.second.lock()->SetValue( l_it->second.first.lock()->GetTexture().get() );
		}

		l_pProgram->Begin( p_uiIndex, p_uiCount );
	}

	for( TextureUnitPtrArrayIt l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		(*l_it)->Render();
	}
}

void Pass :: EndRender()
{
	ShaderProgramBaseSPtr l_pShader = m_pShaderProgram.lock();
	PassRendererSPtr l_pRenderer = GetRenderer();

	for( TextureUnitPtrArrayRIt l_rit = m_arrayTextureUnits.rbegin(); l_rit != m_arrayTextureUnits.rend(); ++l_rit )
	{
		(*l_rit)->EndRender();
	}

	if ( l_pShader )
	{
		l_pShader->End();
	}

	if( l_pRenderer )
	{
		l_pRenderer->EndRender();
	}
}

TextureUnitSPtr Pass :: AddTextureUnit()
{
	TextureUnitSPtr l_pReturn = std::make_shared< TextureUnit >( m_pEngine );
	uint32_t l_uiID = uint32_t( m_arrayTextureUnits.size() + 1 );
	l_pReturn->SetIndex( l_uiID );
	m_arrayTextureUnits.push_back( l_pReturn);
	return l_pReturn;
}

TextureUnitSPtr Pass :: GetTextureUnit( eTEXTURE_CHANNEL p_eChannel )
{
	TextureUnitSPtr l_pReturn;
	TextureUnitPtrArray::iterator l_it = m_arrayTextureUnits.begin();

	while( l_it != m_arrayTextureUnits.end() && !l_pReturn )
	{
		if( (*l_it)->GetChannel() == p_eChannel )
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

bool Pass :: DestroyTextureUnit( uint32_t p_uiIndex )
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	bool l_bReturn = false;
	Logger::LogMessage( cuT( "Destroying TextureUnit %d"), p_uiIndex );

	TextureUnitPtrArray::iterator l_it = m_arrayTextureUnits.begin();
	m_arrayTextureUnits.erase( l_it + p_uiIndex );
	uint32_t i = 0;

	for( l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
	{
		(*l_it)->SetIndex( ++i );
	}

	l_bReturn = true;

	return l_bReturn;
}

TextureUnitSPtr Pass :: GetTextureUnit( uint32_t p_uiIndex)const
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	return m_arrayTextureUnits[p_uiIndex];
}

String Pass :: GetTexturePath( uint32_t p_uiIndex)
{
	CASTOR_ASSERT( p_uiIndex < m_arrayTextureUnits.size());
	return m_arrayTextureUnits[p_uiIndex]->GetTexturePath();
}

void Pass :: SetShader( ShaderProgramBaseSPtr p_pProgram)
{
	m_mapUnits.clear();
	m_pShaderProgram = p_pProgram;
	m_bAutomaticShader = false;

	if( !m_pRenderer.expired() )
	{
		m_pRenderer.lock()->Initialise();
	}
}

bool Pass :: HasShader()const
{
	return ! m_pShaderProgram.expired();
}

bool Pass :: HasAlphaBlending()const 
{
	return m_pBlendState->IsBlendEnabled();
}

void Pass :: DoBindTextures()
{
	ShaderProgramBaseSPtr l_pProgram = m_pShaderProgram.lock();
	m_mapUnits.clear();

	TextureUnitSPtr l_pAmbientMap	= GetTextureUnit( eTEXTURE_CHANNEL_AMBIENT	);
	TextureUnitSPtr l_pColourMap	= GetTextureUnit( eTEXTURE_CHANNEL_COLOUR	);
	TextureUnitSPtr l_pDiffuseMap	= GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE	);
	TextureUnitSPtr l_pNormalMap	= GetTextureUnit( eTEXTURE_CHANNEL_NORMAL	);
	TextureUnitSPtr l_pSpecularMap	= GetTextureUnit( eTEXTURE_CHANNEL_SPECULAR	);
	TextureUnitSPtr l_pOpacityMap	= GetTextureUnit( eTEXTURE_CHANNEL_OPACITY	);
	TextureUnitSPtr l_pGlossMap		= GetTextureUnit( eTEXTURE_CHANNEL_GLOSS	);
	TextureUnitSPtr l_pHeightMap	= GetTextureUnit( eTEXTURE_CHANNEL_HEIGHT	);

	if( l_pAmbientMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapAmbient, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pAmbientMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_AMBIENT, std::make_pair( l_pAmbientMap, l_pVariable ) ) );
	}

	if( l_pColourMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapColour, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pColourMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_COLOUR, std::make_pair( l_pColourMap, l_pVariable ) ) );
	}

	if( l_pDiffuseMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pDiffuseMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_DIFFUSE, std::make_pair( l_pDiffuseMap, l_pVariable ) ) );
	}

	if( l_pNormalMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapNormal, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pNormalMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_NORMAL, std::make_pair( l_pNormalMap, l_pVariable ) ) );
	}

	if( l_pSpecularMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapSpecular, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pSpecularMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_SPECULAR, std::make_pair( l_pSpecularMap, l_pVariable ) ) );
	}

	if( l_pOpacityMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapOpacity, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pOpacityMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_OPACITY, std::make_pair( l_pOpacityMap, l_pVariable ) ) );
	}

	if( l_pGlossMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapGloss, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pGlossMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_GLOSS, std::make_pair( l_pGlossMap, l_pVariable ) ) );
	}

	if( l_pHeightMap )
	{
		OneTextureFrameVariableSPtr l_pVariable = l_pProgram->FindFrameVariable( ShaderProgramBase::MapHeight, eSHADER_TYPE_PIXEL );
		l_pVariable->SetValue( l_pHeightMap->GetTexture().get() );
		m_mapUnits.insert( std::make_pair( eTEXTURE_CHANNEL_HEIGHT, std::make_pair( l_pHeightMap, l_pVariable ) ) );
	}
}

//*********************************************************************************************

PassRenderer :: PassRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer<Pass, PassRenderer>( p_pRenderSystem )
{
}

PassRenderer :: ~PassRenderer()
{
	Cleanup();
}

void PassRenderer :: Cleanup()
{
}

void PassRenderer :: Render()
{
	DoFillShader();
}

void PassRenderer :: Render2D()
{
	DoFillShader();
}

void PassRenderer :: DoFillShader()
{
	ShaderProgramBaseSPtr l_pProgram = m_target->GetShader< ShaderProgramBase >();

	if( l_pProgram )
	{
		FrameVariableBufferSPtr		l_pPassBuffer	= l_pProgram->GetPassBuffer();
		FrameVariableBufferSPtr		l_pSceneBuffer	= l_pProgram->GetSceneBuffer();
		Point4fFrameVariableSPtr	l_pAmbient		= RetrieveVariable( m_pAmbient,		ShaderProgramBase::MatAmbient,		m_pProgram, l_pProgram, l_pPassBuffer	);
		Point4fFrameVariableSPtr	l_pDiffuse		= RetrieveVariable( m_pDiffuse,		ShaderProgramBase::MatDiffuse,		m_pProgram, l_pProgram, l_pPassBuffer	);
		Point4fFrameVariableSPtr	l_pSpecular		= RetrieveVariable( m_pSpecular,	ShaderProgramBase::MatSpecular,		m_pProgram, l_pProgram, l_pPassBuffer	);
		Point4fFrameVariableSPtr	l_pEmissive		= RetrieveVariable( m_pEmissive,	ShaderProgramBase::MatEmissive,		m_pProgram, l_pProgram, l_pPassBuffer	);
		OneFloatFrameVariableSPtr	l_pShininess	= RetrieveVariable( m_pShininess,	ShaderProgramBase::MatShininess,	m_pProgram, l_pProgram, l_pPassBuffer	);
		OneFloatFrameVariableSPtr	l_pOpacity		= RetrieveVariable( m_pOpacity,		ShaderProgramBase::MatOpacity,		m_pProgram, l_pProgram, l_pPassBuffer	);
		Point3rFrameVariableSPtr	l_pCameraPos	= RetrieveVariable( m_pCameraPos,	ShaderProgramBase::CameraPos,		m_pProgram, l_pProgram, l_pSceneBuffer	);

		if( l_pProgram != m_pProgram.lock() )
		{
			m_pProgram = l_pProgram;
		}

		m_target->GetAmbient().to_rgba( m_ptAmbient );
		m_target->GetDiffuse().to_rgba( m_ptDiffuse );
		m_target->GetSpecular().to_rgba( m_ptSpecular );
		m_target->GetEmissive().to_rgba( m_ptEmissive );

		if( l_pAmbient )
		{
			l_pAmbient->SetValue( m_ptAmbient );
		}

		if( l_pDiffuse )
		{
			l_pDiffuse->SetValue( m_ptDiffuse );
		}

		if( l_pSpecular )
		{
			l_pSpecular->SetValue( m_ptSpecular );
		}

		if( l_pEmissive )
		{
			l_pEmissive->SetValue( m_ptEmissive );
		}

		if( l_pShininess )
		{
			l_pShininess->SetValue( m_target->GetShininess() );
		}

		if( l_pOpacity )
		{
			l_pOpacity->SetValue( m_target->GetAlpha() );
		}

		if( l_pCameraPos && m_pRenderSystem && m_pRenderSystem->GetCurrentCamera() )
		{
			Point3r l_position = m_pRenderSystem->GetCurrentCamera()->GetParent()->GetDerivedPosition();
			l_pCameraPos->SetValue( l_position );
		}
	}
}

//*********************************************************************************************
