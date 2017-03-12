#include "ShadowMapSpot.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "ShadowMap/ShadowMapPassSpot.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	ShadowMapSpot::ShadowMapSpot( Engine & p_engine
		, TextureUnit && p_texture )
		: ShadowMap{ p_engine }
		, m_shadowMap{ std::move( p_texture ) }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			const int32_t l_max = DoGetMaxPasses();

			if ( l_max > 1 )
			{
				m_sorted.clear();

				for ( auto & l_it : m_passes )
				{
					m_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
						, l_it.first->GetParent()->GetDerivedPosition() )
						, l_it.second );
				}

				auto l_it = m_sorted.begin();

				for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
				{
					l_it->second->Update( p_queues, i );
				}
			}
			else
			{
				for ( auto & l_pass : m_passes )
				{
					l_pass.second->Update( p_queues, 0 );
				}
			}
		}
	}

	void ShadowMapSpot::Render()
	{
		if ( !m_sorted.empty() )
		{
			m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
			auto l_it = m_sorted.begin();
			const int32_t l_max = DoGetMaxPasses();

			for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
			{
				m_depthAttach[i]->Attach( AttachmentPoint::eDepth );
				m_frameBuffer->Clear( BufferComponent::eDepth );
				l_it->second->Render();
				m_depthAttach[i]->Detach();
			}

			m_frameBuffer->Unbind();
		}
	}

	void ShadowMapSpot::Render( SpotLight const & p_light )
	{
		auto l_it = m_passes.find( &p_light.GetLight() );
		REQUIRE( l_it != m_passes.end() && "Light not found, call AddLight..." );
		m_frameBuffer->Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
		m_depthAttach[0]->Attach( AttachmentPoint::eDepth );
		m_frameBuffer->Clear( BufferComponent::eDepth );
		l_it->second->Render();
		m_depthAttach[0]->Detach();
		m_frameBuffer->Unbind();
	}

	int32_t ShadowMapSpot::DoGetMaxPasses()const
	{
		return int32_t( m_shadowMap.GetTexture()->GetLayersCount() );
	}

	Size ShadowMapSpot::DoGetSize()const
	{
		return m_shadowMap.GetTexture()->GetDimensions();
	}

	void ShadowMapSpot::DoInitialise()
	{
		m_shadowMap.Initialise();
		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );

		auto l_texture = m_shadowMap.GetTexture();
		m_depthAttach.resize( DoGetMaxPasses() );
		int i = 0;

		for ( auto & l_attach : m_depthAttach )
		{
			l_attach = m_frameBuffer->CreateAttachment( l_texture );
			l_attach->SetTarget( l_texture->GetType() );
			l_attach->SetLayer( i++ );
		}
	}

	void ShadowMapSpot::DoCleanup()
	{
		for ( auto & l_attach : m_depthAttach )
		{
			l_attach.reset();
		}

		m_shadowMap.Cleanup();
	}

	ShadowMapPassSPtr ShadowMapSpot::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *GetEngine(), p_light, *this );
	}

	void ShadowMapSpot::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags
		, SceneFlags & p_sceneFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapSpot );
	}

	String ShadowMapSpot::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, SceneFlags const & p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Fragment Intputs
		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( l_writer.GetFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return l_writer.Finalise();
	}
}
