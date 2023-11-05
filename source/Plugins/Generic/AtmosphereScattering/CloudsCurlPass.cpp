#include "AtmosphereScattering/CloudsCurlPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace curl
	{
		enum Bindings : uint32_t
		{
			eOutput,
			eCount,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device
			, uint32_t dimension )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto outputTexture = writer.declStorageImg< sdw::WImage2DRg32 >( "outputTexture"
				, uint32_t( Bindings::eOutput )
				, 0u );

			auto hash = writer.implementFunction< sdw::Vec2 >( "hash"
				, [&]( sdw::Vec2 p )
				{
					p = vec2( dot( p, vec2( 127.1_f, 311.7_f ) ),
						dot( p, vec2( 269.5_f, 183.3_f ) ) );

					writer.returnStmt( -1.0_f + 2.0_f * fract( sin( p ) * 43758.5453123_f ) );
				}
				, sdw::InVec2{ writer, "p" } );

			auto noise = writer.implementFunction< sdw::Float >( "noise"
				, [&]( sdw::Vec2 const & p )
				{
					auto i = writer.declLocale( "i"
						, floor( p ) );
					auto f = writer.declLocale( "f"
						, fract( p ) );

					auto u = writer.declLocale( "u"
						, f * f * ( 3.0_f - 2.0_f * f ) );

					writer.returnStmt( mix( mix( dot( hash( i + vec2( 0.0_f, 0.0_f ) ), f - vec2( 0.0_f, 0.0_f ) ),
												 dot( hash( i + vec2( 1.0_f, 0.0_f ) ), f - vec2( 1.0_f, 0.0_f ) ), u.x() ),
											mix( dot( hash( i + vec2( 0.0_f, 1.0_f ) ), f - vec2( 0.0_f, 1.0_f ) ),
												 dot( hash( i + vec2( 1.0_f, 1.0_f ) ), f - vec2( 1.0_f, 1.0_f ) ), u.x() ), u.y() ) );
				}
				, sdw::InVec2{ writer, "p" } );

			auto detailScale = 20.0_f;

			auto fragToUV = writer.implementFunction< sdw::Vec2 >( "fragToUV"
				, [&]( sdw::Vec2 const & coord )
				{
					auto dim = sdw::Float{ float( dimension ) };
					auto p = writer.declLocale( "p"
						, coord.xy() / dim - 0.5_f );
					p *= detailScale;
					writer.returnStmt( p );
				}
				, sdw::InVec2{ writer, "coord" } );

			auto curl = writer.implementFunction< sdw::Vec2 >( "curl"
				, [&]( sdw::Vec2 const & fragCoord )
				{
					auto pN = writer.declLocale( "pN"
						, noise( fragToUV( fragCoord + vec2( 0.0_f, 1.0_f ) ) ) );
					auto pS = writer.declLocale( "pS"
						, noise( fragToUV( fragCoord - vec2( 0.0_f, 1.0_f ) ) ) );
					auto pE = writer.declLocale( "pE"
						, noise( fragToUV( fragCoord + vec2( 1.0_f, 0.0_f ) ) ) );
					auto pW = writer.declLocale( "pW"
						, noise( fragToUV( fragCoord - vec2( 1.0_f, 0.0_f ) ) ) );

					writer.returnStmt( vec2( ( pN - pS ), -( pE - pW ) ) );
				}
				, sdw::InVec2{ writer, "fragCoord" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 4u, 4u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, ivec2( in.globalInvocationID.xy() ) );
					outputTexture.store( pixel, curl( vec2( pixel ) ) );
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsCurlPass::CloudsCurlPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & resultView
		, bool & enabled )
		: m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "Clouds/CurlPass", curl::getProgram( device, getExtent( resultView ).width ) }
		, m_stages{ makeShaderState( device, m_computeShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & computePass = graph.createPass( "Clouds/CurlPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::ComputePass >( framePass
					, context
					, graph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( renderSize.width / 4u )
						.groupCountY( renderSize.height / 4u )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		computePass.addDependencies( previousPasses );
		computePass.addOutputStorageView( resultView
			, curl::eOutput );
		m_lastPass = &computePass;
	}

	void CloudsCurlPass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
