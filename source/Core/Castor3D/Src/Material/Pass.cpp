#include "Pass.hpp"

#include "BlendStateManager.hpp"
#include "CameraManager.hpp"
#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "ShaderManager.hpp"

#include "Render/RenderNode.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Texture/StaticTexture.hpp"
#include "Texture/TextureUnit.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		template< class Type >
		void RetrieveVariable( Type * p_member, String const & p_name, FrameVariableBuffer & p_frameVariablesBuffer )
		{
			std::shared_ptr< Type > l_variable;
			p_frameVariablesBuffer.GetVariable( p_name, l_variable );
			p_member = l_variable.get();
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
		ShaderProgramSPtr l_program;
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

				case eCHUNK_TYPE_PASS_TEXTURE:
					l_pUnit = std::make_shared< TextureUnit >( *p_pass.GetEngine() );
					l_return = TextureUnit::BinaryParser( m_path ).Parse( *l_pUnit, l_chunk );

					if ( l_return )
					{
						p_pass.AddTextureUnit( l_pUnit );
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

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	std::map< eTEXTURE_CHANNEL, String > TEXTURE_CHANNEL_NAME =
	{
		{ eTEXTURE_CHANNEL_COLOUR, cuT( "Colour" ) },
		{ eTEXTURE_CHANNEL_DIFFUSE, cuT( "Diffuse" ) },
		{ eTEXTURE_CHANNEL_NORMAL, cuT( "Normal" ) },
		{ eTEXTURE_CHANNEL_OPACITY, cuT( "Opacity" ) },
		{ eTEXTURE_CHANNEL_SPECULAR, cuT( "Specular" ) },
		{ eTEXTURE_CHANNEL_HEIGHT, cuT( "Height" ) },
		{ eTEXTURE_CHANNEL_AMBIENT, cuT( "Ambient" ) },
		{ eTEXTURE_CHANNEL_GLOSS, cuT( "Gloss" ) },
		{ eTEXTURE_CHANNEL_EMISSIVE, cuT( "Emissive" ) },
	};

	//*********************************************************************************************

	Pass::Pass( Engine & p_engine, MaterialSPtr p_parent )
		: OwnedBy< Engine >{ p_engine }
		, m_fShininess{ 50.0 }
		, m_bDoubleFace{ false }
		, m_parent{ p_parent }
		, m_clrDiffuse{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_clrAmbient{ Colour::from_rgba( 0x000000FF ) }
		, m_clrSpecular{ Colour::from_rgba( 0xFFFFFFFF ) }
		, m_clrEmissive{ Colour::from_rgba( 0x000000FF ) }
		, m_fAlpha{ 1.0f }
		, m_pBlendState{ p_engine.GetRenderSystem()->CreateBlendState() }
		, m_textureFlags{ 0 }
		, m_bAutomaticShader{ true }
		, m_alphaBlendMode{ eBLEND_MODE_INTERPOLATIVE }
		, m_colourBlendMode{ eBLEND_MODE_INTERPOLATIVE }
		, m_texturesReduced{ false }
	{
	}

	Pass::~Pass()
	{
		m_arrayTextureUnits.clear();
	}

	void Pass::Initialise()
	{
		PrepareTextures();
		bool l_bHasAlpha = m_fAlpha < 1 || GetTextureUnit( eTEXTURE_CHANNEL_OPACITY );

		for ( auto l_unit : m_arrayTextureUnits )
		{
			l_unit->Initialise();
		}

		if ( l_bHasAlpha && !m_pBlendState->IsBlendEnabled() )
		{
			m_pBlendState->EnableBlend( true );

			if ( GetEngine()->GetRenderSystem()->GetCurrentContext()->IsMultiSampling() )
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
				case eBLEND_MODE_NONE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_ONE );
					m_pBlendState->SetAlphaDstBlend( eBLEND_ZERO );
					m_pBlendState->SetRgbSrcBlend( eBLEND_ONE );
					m_pBlendState->SetRgbDstBlend( eBLEND_ZERO );
					break;

				case eBLEND_MODE_ADDITIVE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_ONE );
					m_pBlendState->SetAlphaDstBlend( eBLEND_ONE );
					m_pBlendState->SetRgbSrcBlend( eBLEND_ONE );
					m_pBlendState->SetRgbDstBlend( eBLEND_ONE );
					break;

				case eBLEND_MODE_MULTIPLICATIVE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_ZERO );
					m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
					m_pBlendState->SetRgbSrcBlend( eBLEND_ZERO );
					m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
					break;

				case eBLEND_MODE_INTERPOLATIVE:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
					m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
					m_pBlendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
					m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
					break;

				default:
					m_pBlendState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
					m_pBlendState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
					m_pBlendState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
					m_pBlendState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
					break;
				}
			}
		}

		m_pBlendState->Initialise();
	}

	void Pass::BindToNode( RenderNode & p_node )
	{
		DoGetTextures( p_node );
		DoGetBuffers( p_node );
	}

	void Pass::BindToNode( SceneRenderNode & p_node )
	{
		BindToNode( p_node.m_node );
		DoGetBuffers( p_node );
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

	void Pass::Render()
	{
		m_pBlendState->Apply();
		DoRender();
	}

	void Pass::Render2D()
	{
		DoRender();
	}

	void Pass::EndRender()
	{
		DoEndRender();
	}

	void Pass::AddTextureUnit( TextureUnitSPtr p_unit )
	{
		m_arrayTextureUnits.push_back( p_unit );
		m_textureFlags |= p_unit->GetChannel();
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( eTEXTURE_CHANNEL p_channel )
	{
		TextureUnitSPtr l_return;
		auto && l_it = m_arrayTextureUnits.begin();

		while ( l_it != m_arrayTextureUnits.end() && !l_return )
		{
			if ( ( *l_it )->GetChannel() == p_channel )
			{
				l_return = *l_it;
			}
			else
			{
				++l_it;
			}
		}

		return l_return;
	}

	bool Pass::DestroyTextureUnit( uint32_t p_index )
	{
		REQUIRE( p_index < m_arrayTextureUnits.size() );
		Logger::LogInfo( StringStream() << cuT( "Destroying TextureUnit " ) << p_index );
		TextureUnitPtrArray::iterator l_it = m_arrayTextureUnits.begin();
		m_arrayTextureUnits.erase( l_it + p_index );
		uint32_t i = 0;

		for ( l_it = m_arrayTextureUnits.begin(); l_it != m_arrayTextureUnits.end(); ++l_it )
		{
			( *l_it )->SetIndex( ++i );
		}

		DoUpdateFlags();
		m_texturesReduced = false;
		return true;
	}

	TextureUnitSPtr Pass::GetTextureUnit( uint32_t p_index )const
	{
		REQUIRE( p_index < m_arrayTextureUnits.size() );
		return m_arrayTextureUnits[p_index];
	}

	String Pass::GetTexturePath( uint32_t p_index )
	{
		REQUIRE( p_index < m_arrayTextureUnits.size() );
		return m_arrayTextureUnits[p_index]->GetTexturePath();
	}

	bool Pass::HasAlphaBlending()const
	{
		return m_pBlendState->IsBlendEnabled()
			   || ( ( m_textureFlags & eTEXTURE_CHANNEL_OPACITY ) == eTEXTURE_CHANNEL_OPACITY )
			   || m_fAlpha < 1.0f;
	}

	void Pass::Bind()
	{
		for ( auto && l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::FillShaderVariables( RenderNode & p_node )
	{
		for ( auto && l_pair : p_node.m_textures )
		{
			auto l_texture = l_pair.first;
			auto & l_variable = l_pair.second;

			if ( l_texture )
			{
				l_variable.get().SetValue( l_texture );
			}
		}

		p_node.m_ambient.SetValue( rgba_float( GetAmbient() ) );
		p_node.m_diffuse.SetValue( rgba_float( GetDiffuse() ) );
		p_node.m_specular.SetValue( rgba_float( GetSpecular() ) );
		p_node.m_emissive.SetValue( rgba_float( GetEmissive() ) );
		p_node.m_shininess.SetValue( GetShininess() );
		p_node.m_opacity.SetValue( GetAlpha() );
	}

	void Pass::PrepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto l_unit : m_arrayTextureUnits )
			{
				l_unit->SetIndex( 0 );
			}

			// Lights texture is at index 0, so start at index 1
			uint32_t l_index = 1;
			TextureUnitSPtr l_pOpaSrc;
			PxBufferBaseSPtr l_pImageOpa;
			TextureUnitSPtr l_pAmbientMap = GetTextureUnit( eTEXTURE_CHANNEL_AMBIENT );
			TextureUnitSPtr l_pColourMap = GetTextureUnit( eTEXTURE_CHANNEL_COLOUR );
			TextureUnitSPtr l_pDiffuseMap = GetTextureUnit( eTEXTURE_CHANNEL_DIFFUSE );
			TextureUnitSPtr l_pNormalMap = GetTextureUnit( eTEXTURE_CHANNEL_NORMAL );
			TextureUnitSPtr l_pSpecularMap = GetTextureUnit( eTEXTURE_CHANNEL_SPECULAR );
			TextureUnitSPtr l_pEmissiveMap = GetTextureUnit( eTEXTURE_CHANNEL_EMISSIVE );
			TextureUnitSPtr l_pOpacityMap = GetTextureUnit( eTEXTURE_CHANNEL_OPACITY );
			TextureUnitSPtr l_pGlossMap = GetTextureUnit( eTEXTURE_CHANNEL_GLOSS );
			TextureUnitSPtr l_pHeightMap = GetTextureUnit( eTEXTURE_CHANNEL_HEIGHT );

			DoPrepareTexture( eTEXTURE_CHANNEL_AMBIENT, l_pAmbientMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( eTEXTURE_CHANNEL_COLOUR, l_pColourMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( eTEXTURE_CHANNEL_DIFFUSE, l_pDiffuseMap, l_index, l_pOpaSrc, l_pImageOpa );

			DoPrepareTexture( eTEXTURE_CHANNEL_NORMAL, l_pNormalMap, l_index );
			DoPrepareTexture( eTEXTURE_CHANNEL_SPECULAR, l_pSpecularMap, l_index );
			DoPrepareTexture( eTEXTURE_CHANNEL_EMISSIVE, l_pEmissiveMap, l_index );
			DoPrepareTexture( eTEXTURE_CHANNEL_GLOSS, l_pGlossMap, l_index );
			DoPrepareTexture( eTEXTURE_CHANNEL_HEIGHT, l_pHeightMap, l_index );

			if ( l_pOpacityMap && l_pOpacityMap->GetTexture()->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_pReduced = l_pOpacityMap->GetTexture()->GetImage().GetBuffer();
				PF::ReduceToAlpha( l_pReduced );
				l_pOpacityMap->GetTexture()->GetImage().SetSource( l_pReduced );
				l_pImageOpa.reset();
			}
			else if ( l_pImageOpa )
			{
				StaticTextureSPtr l_texture = GetEngine()->GetRenderSystem()->CreateStaticTexture( eTEXTURE_TYPE_2D, eACCESS_TYPE_READ, eACCESS_TYPE_READ );
				l_texture->SetImage( std::make_unique< TextureImage >( *GetEngine() ) );
				l_texture->GetImage().SetSource( l_pImageOpa );
				l_pOpacityMap = std::make_shared< TextureUnit >( *GetEngine() );
				l_pOpacityMap->SetAutoMipmaps( l_pOpaSrc->GetAutoMipmaps() );
				l_pOpacityMap->SetChannel( eTEXTURE_CHANNEL_OPACITY );
				l_pOpacityMap->SetSampler( l_pOpaSrc->GetSampler() );
				l_pOpacityMap->SetTexture( l_texture );
				AddTextureUnit( l_pOpacityMap );
				l_pImageOpa.reset();
			}
			else if ( l_pOpacityMap )
			{
				DestroyTextureUnit( l_pOpacityMap->GetIndex() );
				l_pOpacityMap.reset();
			}

			if ( l_pOpacityMap )
			{
				l_pOpacityMap->SetIndex( l_index++ );
				Logger::LogDebug( StringStream() << cuT( "	Opacity map at index " ) << l_pOpacityMap->GetIndex() );
				m_textureFlags |= eTEXTURE_CHANNEL_OPACITY;
			}
			else
			{
				m_textureFlags &= ~eTEXTURE_CHANNEL_OPACITY;
			}

			for ( auto l_unit : m_arrayTextureUnits )
			{
				if ( l_unit->GetIndex() == 0 )
				{
					l_unit->SetIndex( l_index++ );
				}
			}

			m_texturesReduced = true;
		}
	}

	void Pass::DoGetTexture( eTEXTURE_CHANNEL p_channel, String const & p_name, RenderNode & p_node )
	{
		TextureUnitSPtr l_unit = GetTextureUnit( p_channel );

		if ( l_unit )
		{
			auto l_variable = p_node.m_program.FindFrameVariable( p_name, eSHADER_TYPE_PIXEL );

			if ( l_variable )
			{
				p_node.m_textures.insert( { l_unit->GetIndex(), *l_variable } );
			}
		}
	}

	void Pass::DoGetTextures( RenderNode & p_node )
	{
		m_mapUnits.clear();
		DoGetTexture( eTEXTURE_CHANNEL_AMBIENT, ShaderProgram::MapAmbient, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_COLOUR, ShaderProgram::MapColour, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_DIFFUSE, ShaderProgram::MapDiffuse, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_NORMAL, ShaderProgram::MapNormal, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_SPECULAR, ShaderProgram::MapSpecular, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_EMISSIVE, ShaderProgram::MapEmissive, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_OPACITY, ShaderProgram::MapOpacity, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_GLOSS, ShaderProgram::MapGloss, p_node );
		DoGetTexture( eTEXTURE_CHANNEL_HEIGHT, ShaderProgram::MapHeight, p_node );
	}

	void Pass::DoGetBuffers( RenderNode & p_node )
	{
		RetrieveVariable( &p_node.m_ambient, ShaderProgram::MatAmbient, p_node.m_passUbo );
		RetrieveVariable( &p_node.m_diffuse, ShaderProgram::MatDiffuse, p_node.m_passUbo );
		RetrieveVariable( &p_node.m_specular, ShaderProgram::MatSpecular, p_node.m_passUbo );
		RetrieveVariable( &p_node.m_emissive, ShaderProgram::MatEmissive, p_node.m_passUbo );
		RetrieveVariable( &p_node.m_shininess, ShaderProgram::MatShininess, p_node.m_passUbo );
		RetrieveVariable( &p_node.m_opacity, ShaderProgram::MatOpacity, p_node.m_passUbo );
	}

	void Pass::DoGetBuffers( SceneRenderNode & p_node )
	{
		RetrieveVariable( &p_node.m_cameraPos, ShaderProgram::CameraPos, p_node.m_sceneUbo );
	}

	bool Pass::DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, PxBufferBaseSPtr & p_opacity )
	{
		PxBufferBaseSPtr l_opacity = DoPrepareTexture( p_channel, p_unit, p_index );
		bool l_return = l_opacity != nullptr;

		if ( l_return && !p_opacity )
		{
			p_opacity = l_opacity;
			p_opacitySource = p_unit;
		}

		return l_return;
	}

	PxBufferBaseSPtr Pass::DoPrepareTexture( eTEXTURE_CHANNEL p_channel, TextureUnitSPtr p_unit, uint32_t & p_index )
	{
		PxBufferBaseSPtr l_return;

		if ( p_unit && ( p_unit->GetTexture()->GetImage().GetBuffer() || p_unit->GetRenderTarget() ) )
		{
			if ( p_unit->GetTexture()->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_extracted = p_unit->GetTexture()->GetImage().GetBuffer();
				l_return = PF::ExtractAlpha( l_extracted );
				p_unit->GetTexture()->GetImage().SetSource( l_extracted );
			}

			p_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( p_unit->GetIndex() ) );
		}

		return l_return;
	}

	void Pass::DoRender()
	{
		for ( auto && l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::DoEndRender()
	{
		for ( auto && l_rit = m_arrayTextureUnits.rbegin(); l_rit != m_arrayTextureUnits.rend(); ++l_rit )
		{
			( *l_rit )->Unbind();
		}
	}

	void Pass::DoUpdateFlags()
	{
		m_textureFlags = 0;

		for ( auto l_unit : m_arrayTextureUnits )
		{
			if ( l_unit->GetChannel() != eTEXTURE_CHANNEL( 0 ) )
			{
				m_textureFlags |= l_unit->GetChannel();
			}
		}
	}
}
