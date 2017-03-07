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
	namespace
	{
		void DoInitialiseShadowMap( Engine & p_engine, Size const & p_size, TextureUnit & p_unit )
		{
			auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Spot" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensionsArray,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F,
				Point3ui{ p_size.width(), p_size.height(), GLSL::SpotShadowMapCount } );
			p_unit.SetTexture( l_texture );
			p_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			p_unit.Initialise();
		}
	}

	ShadowMapSpot::ShadowMapSpot( Engine & p_engine )
		: ShadowMap{ p_engine }
	{
	}

	ShadowMapSpot::~ShadowMapSpot()
	{
	}
	
	void ShadowMapSpot::Update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_shadowMaps.empty() )
		{
			m_sorted.clear();

			for ( auto & l_it : m_shadowMaps )
			{
				m_sorted.emplace( point::distance_squared( p_camera.GetParent()->GetDerivedPosition()
						, l_it.first->GetParent()->GetDerivedPosition() )
					, l_it.second );
			}

			auto l_it = m_sorted.begin();
			const int32_t l_max = DoGetMaxPasses();

			for ( int32_t i = 0; i < l_max && l_it != m_sorted.end(); ++i, ++l_it )
			{
				l_it->second->Update( p_queues, i );
			}
		}
	}

	void ShadowMapSpot::Render()
	{
		if ( !m_sorted.empty() )
		{
			m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );
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

	int32_t ShadowMapSpot::DoGetMaxPasses()const
	{
		return GLSL::SpotShadowMapCount;
	}

	void ShadowMapSpot::DoInitialise( Size const & p_size )
	{
		DoInitialiseShadowMap( *GetEngine(), p_size, m_shadowMap );
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
	}

	ShadowMapPassSPtr ShadowMapSpot::DoCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *GetEngine(), p_light, *this );
	}

	void ShadowMapSpot::DoUpdateFlags( TextureChannels & p_textureFlags
		, ProgramFlags & p_programFlags )const
	{
		AddFlag( p_programFlags, ProgramFlag::eShadowMapSpot );
	}

	String ShadowMapSpot::DoGetPixelShaderSource( TextureChannels const & p_textureFlags
		, ProgramFlags const & p_programFlags
		, uint8_t p_sceneFlags )const
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
