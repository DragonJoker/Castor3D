#include "Pass.hpp"

#include "CameraCache.hpp"
#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "ShaderCache.hpp"

#include "Render/Pipeline.hpp"
#include "Render/RenderNode.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/SceneNode.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/BlendState.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Log/Logger.hpp>

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
		static const String StrBlendModes[uint32_t( BlendMode::eCount )] =
		{
			cuT( "none" ),
			cuT( "additive" ),
			cuT( "multiplicative" ),
			cuT( "interpolative" ),
			cuT( "a_buffer" ),
			cuT( "depth_peeling" ),
		};

		Logger::LogInfo( m_tabs + cuT( "Writing Pass " ) );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "pass\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tambient " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetAmbient(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass ambient colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tdiffuse " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetDiffuse(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass diffuse colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\temissive " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetEmissive(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass emissive colour" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tspecular " ), m_tabs.c_str() ) > 0
					   && Colour::TextWriter( String() )( p_pass.GetSpecular(), p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass specular colour" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tshininess " ) + string::to_string( p_pass.GetShininess() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass shininess" );
		}

		if ( l_return && p_pass.GetAlpha() < 1 )
		{
			l_return = p_file.Print( 256, cuT( "%s\talpha %f\n" ), m_tabs.c_str(), p_pass.GetAlpha() ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass opacity" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttwo_sided " ) + String( p_pass.IsTwoSided() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass sidedness" );
		}

		if ( l_return && ( p_pass.GetColourBlendMode() != BlendMode::eInterpolative ) )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tcolour_blend_mode " ) + StrBlendModes[uint32_t( p_pass.GetColourBlendMode() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass colour blend mode" );
		}

		if ( l_return && ( p_pass.GetAlphaBlendMode() != BlendMode::eInterpolative ) )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\talpha_blend_mode " ) + StrBlendModes[uint32_t( p_pass.GetAlphaBlendMode() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( l_return, "Pass alpha blend mode" );
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
		{ TextureChannel::eColour, cuT( "Colour" ) },
		{ TextureChannel::eDiffuse, cuT( "Diffuse" ) },
		{ TextureChannel::eNormal, cuT( "Normal" ) },
		{ TextureChannel::eOpacity, cuT( "Opacity" ) },
		{ TextureChannel::eSpecular, cuT( "Specular" ) },
		{ TextureChannel::eHeight, cuT( "Height" ) },
		{ TextureChannel::eAmbient, cuT( "Ambient" ) },
		{ TextureChannel::eGloss, cuT( "Gloss" ) },
		{ TextureChannel::eEmissive, cuT( "Emissive" ) },
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
		, m_textureFlags{ 0 }
		, m_bAutomaticShader{ true }
		, m_alphaBlendMode{ BlendMode::eInterpolative }
		, m_colourBlendMode{ BlendMode::eNoBlend }
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

		for ( auto l_unit : m_arrayTextureUnits )
		{
			l_unit->Initialise();
		}
	}

	void Pass::FillRenderNode( PassRenderNode & p_node )
	{
		DoGetTextures( p_node );
	}

	void Pass::Cleanup()
	{
		for ( auto l_unit : m_arrayTextureUnits )
		{
			l_unit->Cleanup();
		}

		m_mapUnits.clear();
	}

	void Pass::Render()
	{
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
		return CheckFlag( m_textureFlags, TextureChannel::eOpacity )
			   || m_fAlpha < 1.0f;
	}

	void Pass::Bind()
	{
		for ( auto l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::FillShaderVariables( PassRenderNode & p_node )const
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
			TextureUnitSPtr l_pAmbientMap = GetTextureUnit( TextureChannel::eAmbient );
			TextureUnitSPtr l_pColourMap = GetTextureUnit( TextureChannel::eColour );
			TextureUnitSPtr l_pDiffuseMap = GetTextureUnit( TextureChannel::eDiffuse );
			TextureUnitSPtr l_pNormalMap = GetTextureUnit( TextureChannel::eNormal );
			TextureUnitSPtr l_pSpecularMap = GetTextureUnit( TextureChannel::eSpecular );
			TextureUnitSPtr l_pEmissiveMap = GetTextureUnit( TextureChannel::eEmissive );
			TextureUnitSPtr l_pOpacityMap = GetTextureUnit( TextureChannel::eOpacity );
			TextureUnitSPtr l_pGlossMap = GetTextureUnit( TextureChannel::eGloss );
			TextureUnitSPtr l_pHeightMap = GetTextureUnit( TextureChannel::eHeight );

			DoPrepareTexture( TextureChannel::eAmbient, l_pAmbientMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( TextureChannel::eColour, l_pColourMap, l_index, l_pOpaSrc, l_pImageOpa );
			DoPrepareTexture( TextureChannel::eDiffuse, l_pDiffuseMap, l_index, l_pOpaSrc, l_pImageOpa );

			DoPrepareTexture( TextureChannel::eNormal, l_pNormalMap, l_index );
			DoPrepareTexture( TextureChannel::eSpecular, l_pSpecularMap, l_index );
			DoPrepareTexture( TextureChannel::eEmissive, l_pEmissiveMap, l_index );
			DoPrepareTexture( TextureChannel::eGloss, l_pGlossMap, l_index );
			DoPrepareTexture( TextureChannel::eHeight, l_pHeightMap, l_index );

			if ( l_pOpacityMap && l_pOpacityMap->GetTexture() && l_pOpacityMap->GetTexture()->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_pReduced = l_pOpacityMap->GetTexture()->GetImage().GetBuffer();
				PF::ReduceToAlpha( l_pReduced );
				auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
				l_texture->SetSource( l_pReduced );
				l_pOpacityMap->SetTexture( l_texture );
				l_pImageOpa.reset();
			}
			else if ( l_pImageOpa )
			{
				auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
				l_texture->SetSource( l_pImageOpa );
				l_pOpacityMap = std::make_shared< TextureUnit >( *GetEngine() );
				l_pOpacityMap->SetAutoMipmaps( l_pOpaSrc->GetAutoMipmaps() );
				l_pOpacityMap->SetChannel( TextureChannel::eOpacity );
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
				AddFlag( m_textureFlags, TextureChannel::eOpacity );
			}
			else
			{
				RemFlag( m_textureFlags, TextureChannel::eOpacity );
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

	void Pass::DoGetTexture( TextureChannel p_channel, String const & p_name, PassRenderNode & p_node )
	{
		TextureUnitSPtr l_unit = GetTextureUnit( p_channel );

		if ( l_unit )
		{
			auto l_variable = p_node.m_pipeline.GetProgram().FindFrameVariable< OneIntFrameVariable >( p_name, ShaderType::ePixel );

			if ( l_variable )
			{
				p_node.m_textures.insert( { l_unit->GetIndex(), *l_variable } );
			}
		}
	}

	void Pass::DoGetTextures( PassRenderNode & p_node )
	{
		m_mapUnits.clear();
		DoGetTexture( TextureChannel::eAmbient, ShaderProgram::MapAmbient, p_node );
		DoGetTexture( TextureChannel::eColour, ShaderProgram::MapColour, p_node );
		DoGetTexture( TextureChannel::eDiffuse, ShaderProgram::MapDiffuse, p_node );
		DoGetTexture( TextureChannel::eNormal, ShaderProgram::MapNormal, p_node );
		DoGetTexture( TextureChannel::eSpecular, ShaderProgram::MapSpecular, p_node );
		DoGetTexture( TextureChannel::eEmissive, ShaderProgram::MapEmissive, p_node );
		DoGetTexture( TextureChannel::eOpacity, ShaderProgram::MapOpacity, p_node );
		DoGetTexture( TextureChannel::eGloss, ShaderProgram::MapGloss, p_node );
		DoGetTexture( TextureChannel::eHeight, ShaderProgram::MapHeight, p_node );
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

				if ( l_return )
				{
					l_texture->SetSource( l_extracted );
				}
			}

			p_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( p_unit->GetIndex() ) );
		}

		return l_return;
	}

	void Pass::DoRender()const
	{
		for ( auto l_it : m_arrayTextureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::DoEndRender()const
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
