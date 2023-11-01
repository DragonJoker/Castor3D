#include "SmaaPostEffect/NeighbourhoodBlending.hpp"

#include "SmaaPostEffect/SmaaUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <numeric>

namespace smaa
{
	namespace neighblend
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			ColorTexIdx = SmaaUboIdx + 1,
			BlendTexIdx,
			VelocityTexIdx,
		};

		template< sdw::var::Flag FlagT >
		using VertexStructT = sdw::IOStructInstanceHelperT< FlagT
			, "SMAANB_Vertex"
			, sdw::IOVec2Field< "texcoord", 0u >
			, sdw::IOVec4Field< "offset", 1u > >;

		template< sdw::var::Flag FlagT >
		struct VertexT
			: public VertexStructT< FlagT >
		{
			VertexT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: VertexStructT< FlagT >{ writer, std::move( expr ), enabled }
			{
			}

			auto texcoord()const { return this->template getMember< "texcoord" >(); }
			auto offset()const { return this->template getMember< "offset" >(); }
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device
			, bool reprojection )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Smaa( writer, SmaaUboIdx, 0u );
			auto c3d_colourTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_colourTex", ColorTexIdx, 0u );
			auto c3d_blendTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_blendTex", BlendTexIdx, 0u );
			auto c3d_velocityTex = writer.declCombinedImg< FImg2DRgba32 >( "c3d_velocityTex", VelocityTexIdx, 0u, reprojection );

			auto SMAANeighborhoodBlendingVS = writer.implementFunction< sdw::Void >( "SMAANeighborhoodBlendingVS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4 offset )
				{
					offset = fma( c3d_smaaData.rtMetrics.xyxy()
						, vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f )
						, vec4( texcoord.xy(), texcoord.xy() ) );
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::OutVec4{ writer, "offset" } );

			/**
			 * Conditional move:
			 */
			auto SMAAMovc2 = writer.implementFunction< sdw::Void >( "SMAAMovc2"
				, [&]( sdw::BVec2 const & cond
					, sdw::Vec2 variable
					, sdw::Vec2 const & value )
				{
					IF( writer, cond.x() )
					{
						variable.x() = value.x();
					}
					FI;

					IF( writer, cond.y() )
					{
						variable.y() = value.y();
					}
					FI;
				}
				, sdw::InBVec2{ writer, "cond" }
				, sdw::InOutVec2{ writer, "variable" }
				, sdw::InVec2{ writer, "value" } );

			auto SMAAMovc4 = writer.implementFunction< sdw::Void >( "SMAAMovc4"
				, [&]( sdw::BVec4 const & cond
					, sdw::Vec4 variable
					, sdw::Vec4 const & value )
				{
					SMAAMovc2( cond.xy(), variable.xy(), value.xy() );
					SMAAMovc2( cond.zw(), variable.zw(), value.zw() );
				}
				, sdw::InBVec4{ writer, "cond" }
				, sdw::InOutVec4{ writer, "variable" }
				, sdw::InVec4{ writer, "value" } );

			auto SMAANeighborhoodBlendingPS = writer.implementFunction< sdw::Vec4 >( "SMAANeighborhoodBlendingPS"
				, [&]( sdw::Vec2 const & texcoord
					, sdw::Vec4 const & offset
					, sdw::CombinedImage2DRgba32 const & colorTex
					, sdw::CombinedImage2DRgba32 const & blendTex )
				{
					// Fetch the blending weights for current pixel:
					auto a = writer.declLocale< sdw::Vec4 >( "a" );
					a.x() = blendTex.sample( offset.xy() ).a(); // Right
					a.y() = blendTex.sample( offset.zw() ).g(); // Top
					a.wz() = blendTex.sample( texcoord ).xz(); // Bottom / Left

					// Is there any blending weight with a value greater than 0.0?
					IF ( writer, dot( a, vec4( 1.0_f, 1.0_f, 1.0_f, 1.0_f ) ) < 1e-5_f )
					{
						auto color = writer.declLocale( "color"
							, colorTex.lod( texcoord, 0.0_f ) );

						if ( reprojection )
						{
							auto velocity = writer.declLocale( "velocity"
								, c3d_velocityTex.lod( texcoord, 0.0_f ).rg() );

							// Pack velocity into the alpha channel:
							color.a() = sqrt( 5.0_f * length( velocity ) );
						}

						writer.returnStmt( color );
					}
					ELSE
					{
						auto h = writer.declLocale( "h"
							, max( a.x(), a.z() ) > max( a.y(), a.w() ) ); // max(horizontal) > max(vertical)

						// Calculate the blending offsets:
						auto blendingOffset = writer.declLocale( "blendingOffset"
							, vec4( 0.0_f, a.y(), 0.0_f, a.w() ) );
						auto blendingWeight = writer.declLocale( "blendingWeight"
							, a.yw() );
						SMAAMovc4( bvec4( h, h, h, h ), blendingOffset, vec4( a.x(), 0.0_f, a.z(), 0.0_f ) );
						SMAAMovc2( bvec2( h, h ), blendingWeight, a.xz() );
						blendingWeight /= dot( blendingWeight, vec2( 1.0_f, 1.0_f ) );

						// Calculate the texture coordinates:
						auto blendingCoord = writer.declLocale( "blendingCoord"
							, fma( blendingOffset
								, vec4( c3d_smaaData.rtMetrics.xy(), -c3d_smaaData.rtMetrics.xy() )
								, vec4( texcoord.xy(), texcoord.xy() ) ) );

						// We exploit bilinear filtering to mix current pixel with the chosen
						// neighbor:
						auto color = writer.declLocale( "color"
							, blendingWeight.x() * colorTex.lod( blendingCoord.xy(), 0.0_f ) );
						color += blendingWeight.y() * colorTex.lod( blendingCoord.zw(), 0.0_f );

						if ( reprojection )
						{
							// Antialias velocity for proper reprojection in a later stage:
							auto velocity = writer.declLocale( "velocity"
								, blendingWeight.x() * c3d_velocityTex.lod( blendingCoord.xy(), 0.0_f ).rg() );
							velocity += blendingWeight.y() * c3d_velocityTex.lod( blendingCoord.zw(), 0.0_f ).rg();

							// Pack velocity into the alpha channel:
							color.a() = sqrt( 5.0_f * length( velocity ) );
						}

						writer.returnStmt( color );
					}
					FI;
				}
				, sdw::InVec2{ writer, "texcoord" }
				, sdw::InVec4{ writer, "offset" }
				, sdw::InCombinedImage2DRgba32{ writer, "colourTex" }
				, sdw::InCombinedImage2DRgba32{ writer, "blendTex" } );

			writer.implementEntryPointT< c3d::PosUv2FT, VertexT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< VertexT > out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					out.texcoord() = in.uv();
					out.offset() = vec4( 0.0_f );
					SMAANeighborhoodBlendingVS( out.texcoord(), out.offset() );
				} );

			writer.implementEntryPointT< VertexT, c3d::Colour4FT >( [&]( sdw::FragmentInT< VertexT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					out.colour() = SMAANeighborhoodBlendingPS( in.texcoord(), in.offset(), c3d_colourTex, c3d_blendTex );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	NeighbourhoodBlending::NeighbourhoodBlending( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, SmaaUbo const & ubo
		, crg::ImageViewIdArray const & sourceView
		, crg::ImageViewId const & blendView
		, crg::ImageViewId const * velocityView
		, SmaaConfig const & config
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_device{ device }
		, m_graph{ graph }
		, m_blendView{ blendView }
		, m_velocityView{ velocityView }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_shader{ "SmaaNeighbourhood", neighblend::getProgram( device, velocityView != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_pass{ m_graph.createPass( "NeighbourhoodBlending"
			, [this, &device, &config, enabled, passIndex]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & frameGraph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.passIndex( passIndex )
					.enabled( enabled )
					.build( pass, context, frameGraph, { config.maxSubsampleIndices * 2u } );
				device.renderSystem.getEngine()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} ) }
	{
		auto & source = sourceView.front();
		auto & target = sourceView.back();
		crg::ImageViewIdArray inputs;
		crg::ImageViewIdArray addInputs;

		for ( auto index = 0u; index < config.maxSubsampleIndices; ++index )
		{
			inputs.push_back( source );
			addInputs.push_back( target );
		}

		inputs.insert( inputs.end(), addInputs.begin(), addInputs.end() );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		m_pass.addDependency( previousPass );
		ubo.createPassBinding( m_pass
			, SmaaUboIdx );
		m_pass.addSampledView( inputs
			, neighblend::ColorTexIdx
			, linearSampler );
		m_pass.addSampledView( m_blendView
			, neighblend::BlendTexIdx
			, linearSampler );

		if ( m_velocityView )
		{
			m_pass.addSampledView( *m_velocityView
				, neighblend::VelocityTexIdx
				, linearSampler );
		}

		for ( uint32_t i = 0; i < config.maxSubsampleIndices; ++i )
		{
			m_images.emplace_back( m_device
				, renderTarget.getResources()
				, "SMNBRes" + std::to_string( i )
				, 0u
				, m_extent
				, 1u
				, 1u
				, VK_FORMAT_R8G8B8A8_SRGB
				, ( VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) );
			auto & image = m_images.back();
			image.create();
			m_imageViews.push_back( image.wholeViewId );
		}

		m_pass.addOutputColourView( m_imageViews
			, castor3d::transparentBlackClearColor );
	}

	NeighbourhoodBlending::~NeighbourhoodBlending()
	{
		for ( auto & image : m_images )
		{
			image.destroy();
		}
	}

	void NeighbourhoodBlending::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );

		for ( uint32_t i = 0; i < m_images.size(); ++i )
		{
			visitor.visit( "SMAA NeighbourhoodBlending " + std::to_string( i )
				, m_images[i]
				, m_graph.getFinalLayoutState( m_images[i].wholeViewId ).layout
				, castor3d::TextureFactors{}.invert( true ) );
		}
	}

	//*********************************************************************************************
}
