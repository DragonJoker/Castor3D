#include "Pass.hpp"

#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Render/RenderNode.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureUnit.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
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

		void DoGetTexture( Pass const & p_pass
			, RenderPipeline const & p_pipeline
			, TextureChannel p_channel
			, String const & p_name
			, PassRenderNode & p_node )
		{
			TextureUnitSPtr l_unit = p_pass.GetTextureUnit( p_channel );

			if ( l_unit )
			{
				auto l_variable = p_pipeline.GetProgram().FindFrameVariable< OneIntFrameVariable >( p_name, ShaderType::ePixel );

				if ( l_variable )
				{
					p_node.m_textures.emplace( l_unit->GetIndex(), *l_variable );
				}
			}
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

		bool l_return = true;

		if ( l_return && p_pass.GetOpacity() < 1 )
		{
			l_return = p_file.Print( 256, cuT( "%s\talpha %f\n" ), m_tabs.c_str(), p_pass.GetOpacity() ) > 0;
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

		return l_return;
	}

	//*********************************************************************************************

	Pass::Pass( Material & p_parent )
		: OwnedBy< Material >{ p_parent }
	{
	}

	Pass::~Pass()
	{
		m_textureUnits.clear();
	}

	void Pass::Initialise()
	{
		PrepareTextures();

		for ( auto l_unit : m_textureUnits )
		{
			l_unit->Initialise();
		}

		DoInitialise();
	}

	void Pass::Cleanup()
	{
		DoCleanup();

		for ( auto l_unit : m_textureUnits )
		{
			l_unit->Cleanup();
		}
	}

	void Pass::FillRenderNode( PassRenderNode & p_node )
	{
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eAmbient, ShaderProgram::MapAmbient, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eColour, ShaderProgram::MapColour, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eDiffuse, ShaderProgram::MapDiffuse, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eNormal, ShaderProgram::MapNormal, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eSpecular, ShaderProgram::MapSpecular, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eEmissive, ShaderProgram::MapEmissive, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eOpacity, ShaderProgram::MapOpacity, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eGloss, ShaderProgram::MapGloss, p_node );
		DoGetTexture( *this, p_node.m_pipeline, TextureChannel::eHeight, ShaderProgram::MapHeight, p_node );
	}

	void Pass::BindTextures()
	{
		for ( auto l_it : m_textureUnits )
		{
			l_it->Bind();
		}
	}

	void Pass::UnbindTextures()
	{
		for ( auto l_it : m_textureUnits )
		{
			l_it->Unbind();
		}
	}

	void Pass::AddTextureUnit( TextureUnitSPtr p_unit )
	{
		m_textureUnits.push_back( p_unit );
		AddFlag( m_textureFlags, p_unit->GetChannel() );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( TextureChannel p_channel )const
	{
		TextureUnitSPtr l_return;
		auto l_it = m_textureUnits.begin();

		while ( l_it != m_textureUnits.end() && !l_return )
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

	void Pass::DestroyTextureUnit( uint32_t p_index )
	{
		REQUIRE( p_index < m_textureUnits.size() );
		Logger::LogInfo( StringStream() << cuT( "Destroying TextureUnit " ) << p_index );
		auto l_it = m_textureUnits.begin();
		m_textureUnits.erase( l_it + p_index );
		uint32_t i = 0u;

		for ( l_it = m_textureUnits.begin(); l_it != m_textureUnits.end(); ++l_it )
		{
			( *l_it )->SetIndex( ++i );
		}

		DoUpdateFlags();
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( uint32_t p_index )const
	{
		REQUIRE( p_index < m_textureUnits.size() );
		return m_textureUnits[p_index];
	}

	bool Pass::HasAlphaBlending()const
	{
		return CheckFlag( m_textureFlags, TextureChannel::eOpacity ) || m_opacity < 1.0f;
	}

	void Pass::UpdateRenderNode( PassRenderNode & p_node )const
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

		p_node.m_opacity.SetValue( GetOpacity() );
		DoUpdateRenderNode( p_node );
	}

	void Pass::PrepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto l_unit : m_textureUnits )
			{
				l_unit->SetIndex( 0u );
			}

			// Lights texture is at index 0, so start at index 1
			uint32_t l_index = 1u;
			TextureUnitSPtr l_opacitySource;
			PxBufferBaseSPtr l_opacityImage;

			DoPrepareTexture( TextureChannel::eAmbient, l_index, l_opacitySource, l_opacityImage );
			DoPrepareTexture( TextureChannel::eColour, l_index, l_opacitySource, l_opacityImage );
			DoPrepareTexture( TextureChannel::eDiffuse, l_index, l_opacitySource, l_opacityImage );

			DoPrepareTexture( TextureChannel::eNormal, l_index );
			DoPrepareTexture( TextureChannel::eSpecular, l_index );
			DoPrepareTexture( TextureChannel::eEmissive, l_index );
			DoPrepareTexture( TextureChannel::eGloss, l_index );
			DoPrepareTexture( TextureChannel::eHeight, l_index );

			DoPrepareOpacity( l_opacitySource, l_opacityImage, l_index );

			for ( auto l_unit : m_textureUnits )
			{
				if ( !l_unit->GetIndex() )
				{
					l_unit->SetIndex( l_index++ );
				}
			}

			m_texturesReduced = true;
		}
	}

	MaterialType Pass::GetType()const
	{
		return GetOwner()->GetType();
	}

	bool Pass::DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, PxBufferBaseSPtr & p_opacity )
	{
		PxBufferBaseSPtr l_opacity = DoPrepareTexture( p_channel, p_index );
		bool l_return = l_opacity != nullptr;

		if ( l_return && !p_opacity )
		{
			p_opacity = l_opacity;
			p_opacitySource = GetTextureUnit( p_channel );
		}

		return l_return;
	}

	PxBufferBaseSPtr Pass::DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index )
	{
		PxBufferBaseSPtr l_return;
		auto l_unit = GetTextureUnit( p_channel );

		if ( l_unit )
		{
			auto l_texture = l_unit->GetTexture();

			if ( l_texture && l_texture->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr l_extracted = l_texture->GetImage().GetBuffer();
				l_return = PF::ExtractAlpha( l_extracted );

				if ( l_return )
				{
					l_texture->SetSource( l_extracted );
				}
			}

			l_unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + TEXTURE_CHANNEL_NAME[p_channel] + cuT( " map at index " ) + string::to_string( l_unit->GetIndex() ) );
		}

		return l_return;
	}

	void Pass::DoPrepareOpacity( TextureUnitSPtr p_opacitySource, PxBufferBaseSPtr p_opacityImage, uint32_t & p_index )
	{
		TextureUnitSPtr l_opacityMap = GetTextureUnit( TextureChannel::eOpacity );

		if ( l_opacityMap
			&& l_opacityMap->GetTexture()
			&& l_opacityMap->GetTexture()->GetImage().GetBuffer() )
		{
			PxBufferBaseSPtr l_reduced = l_opacityMap->GetTexture()->GetImage().GetBuffer();
			PF::ReduceToAlpha( l_reduced );
			auto l_texture = GetOwner()->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			l_texture->SetSource( l_reduced );
			l_opacityMap->SetTexture( l_texture );
			p_opacityImage.reset();
		}
		else if ( p_opacityImage )
		{
			auto l_texture = GetOwner()->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			l_texture->SetSource(p_opacityImage);
			l_opacityMap = std::make_shared< TextureUnit >( *GetOwner()->GetEngine() );
			l_opacityMap->SetAutoMipmaps( p_opacitySource->GetAutoMipmaps() );
			l_opacityMap->SetChannel( TextureChannel::eOpacity );
			l_opacityMap->SetSampler( p_opacitySource->GetSampler() );
			l_opacityMap->SetTexture( l_texture );
			AddTextureUnit( l_opacityMap );
			p_opacityImage.reset();
		}
		else if ( l_opacityMap )
		{
			DestroyTextureUnit( l_opacityMap->GetIndex() );
			l_opacityMap.reset();
		}

		if ( l_opacityMap )
		{
			l_opacityMap->SetIndex( p_index++ );
			Logger::LogDebug( StringStream() << cuT( "	Opacity map at index " ) << l_opacityMap->GetIndex() );
			AddFlag( m_textureFlags, TextureChannel::eOpacity );
		}
		else
		{
			RemFlag( m_textureFlags, TextureChannel::eOpacity );
		}
	}

	void Pass::DoUpdateFlags()
	{
		m_textureFlags = 0u;

		for ( auto l_unit : m_textureUnits )
		{
			if ( l_unit->GetChannel() != TextureChannel( 0 ) )
			{
				AddFlag( m_textureFlags, l_unit->GetChannel() );
			}
		}
	}
}
