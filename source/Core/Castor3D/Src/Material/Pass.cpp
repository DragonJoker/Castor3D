#include "Pass.hpp"

#include "BlendStateCache.hpp"
#include "CameraCache.hpp"
#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "ShaderCache.hpp"

#include "Render/RenderNode.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Texture/TextureLayout.hpp"
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

	//*********************************************************************************************

	Pass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Pass >{ p_tabs }
	{
	}

	bool Pass::TextWriter::operator()( Pass const & p_pass, TextFile & p_file )
	{
		static const String StrBlendFactors[uint32_t( BlendOperand::Count )] =
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

		Logger::LogInfo( m_tabs + cuT( "Writing Pass " ) );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
			&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tambient " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetAmbient(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\temissive " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetEmissive(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
				&& Colour::TextWriter( String() )( p_pass.GetSpecular(), p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tshininess " ) + string::to_string( p_pass.GetShininess() ) + cuT( "\n" ) ) > 0;
		}

		if ( l_return && p_pass.GetAlpha() < 1 )
		{
			l_return = p_file.Print( 256, cuT( "%s\talpha %f\n" ), m_tabs.c_str(), p_pass.GetAlpha() ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttwo_sided " ) + String( p_pass.IsTwoSided() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
		}

		if ( l_return && ( p_pass.GetBlendState()->GetAlphaSrcBlend() != BlendOperand::One || p_pass.GetBlendState()->GetAlphaDstBlend() != BlendOperand::Zero ) )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tblend_func " ) + StrBlendFactors[uint32_t( p_pass.GetBlendState()->GetAlphaSrcBlend() )] + cuT( " " ) + StrBlendFactors[uint32_t( p_pass.GetBlendState()->GetAlphaDstBlend() )] + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			for ( auto l_unit : p_pass )
			{
				l_return = TextureUnit::TextWriter( m_tabs + cuT( "\t" ) )( *l_unit, p_file );
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*********************************************************************************************

	std::map< TextureChannel, String > TEXTURE_CHANNEL_NAME =
	{
		{ TextureChannel::Colour, cuT( "Colour" ) },
		{ TextureChannel::Diffuse, cuT( "Diffuse" ) },
		{ TextureChannel::Normal, cuT( "Normal" ) },
		{ TextureChannel::Opacity, cuT( "Opacity" ) },
		{ TextureChannel::Specular, cuT( "Specular" ) },
		{ TextureChannel::Height, cuT( "Height" ) },
		{ TextureChannel::Ambient, cuT( "Ambient" ) },
		{ TextureChannel::Gloss, cuT( "Gloss" ) },
		{ TextureChannel::Emissive, cuT( "Emissive" ) },
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
		, m_alphaBlendMode{ BlendMode::Interpolative }
		, m_colourBlendMode{ BlendMode::Interpolative }
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
		bool l_bHasAlpha = m_fAlpha < 1 || GetTextureUnit( TextureChannel::Opacity );

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
				m_pBlendState->SetAlphaSrcBlend( BlendOperand::SrcAlpha );
				m_pBlendState->SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
				m_pBlendState->SetRgbSrcBlend( BlendOperand::SrcAlpha );
				m_pBlendState->SetRgbDstBlend( BlendOperand::InvSrcAlpha );
			}
			else
			{
				switch ( m_alphaBlendMode )
				{
				case BlendMode::NoBlend:
					m_pBlendState->SetAlphaSrcBlend( BlendOperand::One );
					m_pBlendState->SetAlphaDstBlend( BlendOperand::Zero );
					m_pBlendState->SetRgbSrcBlend( BlendOperand::One );
					m_pBlendState->SetRgbDstBlend( BlendOperand::Zero );
					break;

				case BlendMode::Additive:
					m_pBlendState->SetAlphaSrcBlend( BlendOperand::One );
					m_pBlendState->SetAlphaDstBlend( BlendOperand::One );
					m_pBlendState->SetRgbSrcBlend( BlendOperand::One );
					m_pBlendState->SetRgbDstBlend( BlendOperand::One );
					break;

				case BlendMode::Multiplicative:
					m_pBlendState->SetAlphaSrcBlend( BlendOperand::Zero );
					m_pBlendState->SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
					m_pBlendState->SetRgbSrcBlend( BlendOperand::Zero );
					m_pBlendState->SetRgbDstBlend( BlendOperand::InvSrcAlpha );
					break;

				case BlendMode::Interpolative:
					m_pBlendState->SetAlphaSrcBlend( BlendOperand::SrcAlpha );
					m_pBlendState->SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
					m_pBlendState->SetRgbSrcBlend( BlendOperand::SrcAlpha );
					m_pBlendState->SetRgbDstBlend( BlendOperand::InvSrcAlpha );
					break;

				default:
					m_pBlendState->SetAlphaSrcBlend( BlendOperand::SrcAlpha );
					m_pBlendState->SetAlphaDstBlend( BlendOperand::InvSrcAlpha );
					m_pBlendState->SetRgbSrcBlend( BlendOperand::SrcAlpha );
					m_pBlendState->SetRgbDstBlend( BlendOperand::InvSrcAlpha );
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

		for ( auto l_unit : m_arrayTextureUnits )
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
		AddFlag( m_textureFlags, p_unit->GetChannel() );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( TextureChannel p_channel )
	{
		TextureUnitSPtr l_return;
		auto l_it = m_arrayTextureUnits.begin();

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

	bool Pass::HasAlphaBlending()const
	{
		return m_pBlendState->IsBlendEnabled()
			   || CheckFlag( m_textureFlags, TextureChannel::Opacity )
			   || m_fAlpha < 1.0f;
	}

	void Pass::Bind()
	{
		for ( auto l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::FillShaderVariables( RenderNode & p_node )
	{
		for ( auto l_pair : p_node.m_textures )
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
			TextureUnitSPtr l_pAmbientMap = GetTextureUnit( TextureChannel::Ambient );
			TextureUnitSPtr l_pColourMap = GetTextureUnit( TextureChannel::Colour );
			TextureUnitSPtr l_pDiffuseMap = GetTextureUnit( TextureChannel::Diffuse );
			TextureUnitSPtr l_pNormalMap = GetTextureUnit( TextureChannel::Normal );
			TextureUnitSPtr l_pSpecularMap = GetTextureUnit( TextureChannel::Specular );
			TextureUnitSPtr l_pEmissiveMap = GetTextureUnit( TextureChannel::Emissive );
			TextureUnitSPtr l_pOpacityMap = GetTextureUnit( TextureChannel::Opacity );
			TextureUnitSPtr l_pGlossMap = GetTextureUnit( TextureChannel::Gloss );
			TextureUnitSPtr l_pHeightMap = GetTextureUnit( TextureChannel::Height );

			DoPrepareTexture( TextureChannel::Ambient, l_pAmbientMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( TextureChannel::Colour, l_pColourMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( TextureChannel::Diffuse, l_pDiffuseMap, l_index, l_pOpaSrc, l_pImageOpa );

			DoPrepareTexture( TextureChannel::Normal, l_pNormalMap, l_index );
			DoPrepareTexture( TextureChannel::Specular, l_pSpecularMap, l_index );
			DoPrepareTexture( TextureChannel::Emissive, l_pEmissiveMap, l_index );
			DoPrepareTexture( TextureChannel::Gloss, l_pGlossMap, l_index );
			DoPrepareTexture( TextureChannel::Height, l_pHeightMap, l_index );

			if ( l_pOpacityMap && l_pOpacityMap->GetTexture() && l_pOpacityMap->GetTexture()->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_pReduced = l_pOpacityMap->GetTexture()->GetImage().GetBuffer();
				PF::ReduceToAlpha( l_pReduced );
				l_pOpacityMap->GetTexture()->GetImage().SetBuffer( l_pReduced );
				l_pImageOpa.reset();
			}
			else if ( l_pImageOpa )
			{
				auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, 0, eACCESS_TYPE_READ );
				l_texture->GetImage().SetSource( l_pImageOpa );
				l_pOpacityMap = std::make_shared< TextureUnit >( *GetEngine() );
				l_pOpacityMap->SetAutoMipmaps( l_pOpaSrc->GetAutoMipmaps() );
				l_pOpacityMap->SetChannel( TextureChannel::Opacity );
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
				AddFlag( m_textureFlags, TextureChannel::Opacity );
			}
			else
			{
				RemFlag( m_textureFlags, TextureChannel::Opacity );
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

	void Pass::DoGetTexture( TextureChannel p_channel, String const & p_name, RenderNode & p_node )
	{
		TextureUnitSPtr l_unit = GetTextureUnit( p_channel );

		if ( l_unit )
		{
			auto l_variable = p_node.m_program.FindFrameVariable< OneIntFrameVariable >( p_name, eSHADER_TYPE_PIXEL );

			if ( l_variable )
			{
				p_node.m_textures.insert( { l_unit->GetIndex(), *l_variable } );
			}
		}
	}

	void Pass::DoGetTextures( RenderNode & p_node )
	{
		m_mapUnits.clear();
		DoGetTexture( TextureChannel::Ambient, ShaderProgram::MapAmbient, p_node );
		DoGetTexture( TextureChannel::Colour, ShaderProgram::MapColour, p_node );
		DoGetTexture( TextureChannel::Diffuse, ShaderProgram::MapDiffuse, p_node );
		DoGetTexture( TextureChannel::Normal, ShaderProgram::MapNormal, p_node );
		DoGetTexture( TextureChannel::Specular, ShaderProgram::MapSpecular, p_node );
		DoGetTexture( TextureChannel::Emissive, ShaderProgram::MapEmissive, p_node );
		DoGetTexture( TextureChannel::Opacity, ShaderProgram::MapOpacity, p_node );
		DoGetTexture( TextureChannel::Gloss, ShaderProgram::MapGloss, p_node );
		DoGetTexture( TextureChannel::Height, ShaderProgram::MapHeight, p_node );
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

	bool Pass::DoPrepareTexture( TextureChannel p_channel, TextureUnitSPtr p_unit, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, PxBufferBaseSPtr & p_opacity )
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

	PxBufferBaseSPtr Pass::DoPrepareTexture( TextureChannel p_channel, TextureUnitSPtr p_unit, uint32_t & p_index )
	{
		PxBufferBaseSPtr l_return;

		if ( p_unit )
		{
			auto l_texture = p_unit->GetTexture();

			if ( l_texture && l_texture->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_extracted = l_texture->GetImage().GetBuffer();
				l_return = PF::ExtractAlpha( l_extracted );
				l_texture->GetImage().SetBuffer( l_extracted );
			}

			p_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( p_unit->GetIndex() ) );
		}

		return l_return;
	}

	void Pass::DoRender()
	{
		for ( auto l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::DoEndRender()
	{
		for ( auto l_rit = m_arrayTextureUnits.rbegin(); l_rit != m_arrayTextureUnits.rend(); ++l_rit )
		{
			( *l_rit )->Unbind();
		}
	}

	void Pass::DoUpdateFlags()
	{
		m_textureFlags = 0;

		for ( auto l_unit : m_arrayTextureUnits )
		{
			if ( l_unit->GetChannel() != TextureChannel( 0 ) )
			{
				AddFlag( m_textureFlags, l_unit->GetChannel() );
			}
		}
	}
}
