#include "ForwardRenderTechniquePass.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderNode/RenderNode_Render.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Shader/Uniform.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		TextureUnit DoInitialiseDirectional( Engine & p_engine, Size const & p_size )
		{
			auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Directional" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
			l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

			auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
		}

		std::vector< TextureUnit > DoInitialisePoint( Engine & p_engine, Size const & p_size )
		{
			std::vector< TextureUnit > l_result;

			for ( auto i = 0u; i < GLSL::PointShadowMapCount; ++i )
			{
				auto l_sampler = p_engine.GetSamplerCache().Add( cuT( "ShadowMap_Point_" ) + string::to_string( i ) );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				l_sampler->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				l_sampler->SetComparisonMode( ComparisonMode::eRefToTexture );
				l_sampler->SetComparisonFunc( ComparisonFunc::eLEqual );

				auto l_texture = p_engine.GetRenderSystem()->CreateTexture(
					TextureType::eCube,
					AccessType::eNone,
					AccessType::eRead | AccessType::eWrite,
					PixelFormat::eD32F,
					p_size );
				l_result.emplace_back( p_engine );
				TextureUnit & l_unit = l_result.back();
				l_unit.SetTexture( l_texture );
				l_unit.SetSampler( l_sampler );

				for ( auto & l_image : *l_texture )
				{
					l_image->InitialiseSource();
				}
			}

			return l_result;
		}

		TextureUnit DoInitialiseSpot( Engine & p_engine, Size const & p_size )
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
			TextureUnit l_unit{ p_engine };
			l_unit.SetTexture( l_texture );
			l_unit.SetSampler( l_sampler );

			for ( auto & l_image : *l_texture )
			{
				l_image->InitialiseSource();
			}

			return l_unit;
		}
	}

	//*********************************************************************************************

	ForwardRenderTechniquePass::ForwardRenderTechniquePass( String const & p_name
		, RenderTarget & p_renderTarget
		, RenderTechnique & p_technique
		, bool p_opaque
		, bool p_multisampling )
		: RenderTechniquePass{ p_name
			, p_renderTarget
			, p_technique
			, DoInitialiseDirectional( *p_renderTarget.GetEngine(), Size{ 4096, 4096 } )
			, DoInitialisePoint( *p_renderTarget.GetEngine(), Size{ 1024, 1024 } )
			, DoInitialiseSpot( *p_renderTarget.GetEngine(), Size{ 1024, 1024 } )
			, p_opaque
			, p_multisampling }
	{
	}

	ForwardRenderTechniquePass::~ForwardRenderTechniquePass()
	{
	}

	void ForwardRenderTechniquePass::RenderShadowMaps()
	{
		m_directionalShadowMap.Render();
		m_pointShadowMap.Render();
		m_spotShadowMap.Render();
	}

	void ForwardRenderTechniquePass::DoGetDepthMaps( DepthMapArray & p_depthMaps )
	{
		p_depthMaps.push_back( std::ref( m_directionalShadowMap.GetTexture() ) );
		p_depthMaps.push_back( std::ref( m_spotShadowMap.GetTexture() ) );

		for ( auto & l_map : m_pointShadowMap.GetTextures() )
		{
			p_depthMaps.push_back( std::ref( l_map ) );
		}
	}

	//*********************************************************************************************
}
