#include "AtmosphereScattering/CloudsResolvePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>
#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace cloudsres
	{
		enum Bindings : uint32_t
		{
			eCamera,
			eAtmosphere,
			eClouds,
			eEmissions,
			eCount,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( VkExtent3D renderSize )
		{
			sdw::FragmentWriter writer;

			C3D_Camera( writer
				, uint32_t( Bindings::eCamera )
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("cloudsMap"
				, uint32_t( Bindings::eClouds )
				, 0u );
			auto emissionsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("emissionsMap"
				, uint32_t( Bindings::eEmissions )
				, 0u );

			auto fragColour = writer.declOutput< sdw::Vec4 >( "fragColour", 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
			auto offsetX = 1.0_f / targetSize.x();
			auto offsetY = 1.0_f / targetSize.y();
			auto kernel = std::vector< sdw::Float >{ 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f
					, 2.0_f / 16.0_f, 4.0_f / 16.0_f, 2.0_f / 16.0_f
					, 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f };
			auto offsets = std::vector< sdw::Vec2 >{ vec2( -offsetX, offsetY ) // top-left
					, vec2( 0.0_f, offsetY ) // top-center
					, vec2( offsetX, offsetY ) // top-right
					, vec2( -offsetX, 0.0_f )   // center-left
					, vec2( 0.0_f, 0.0_f )   // center-center
					, vec2( offsetX, 0.0_f )   // center-right
					, vec2( -offsetX, -offsetY ) // bottom-left
					, vec2( 0.0_f, -offsetY ) // bottom-center
					, vec2( offsetX, -offsetY ) };  // bottom-right

			auto gaussianBlur = writer.implementFunction< sdw::Vec4 >( "gaussianBlur"
				, [&]( sdw::CombinedImage2DRgba32 const & tex
					, sdw::Vec2 const & uv )
				{
					auto col = writer.declLocale( "col"
						, vec4( 0.0_f ) );

					for ( uint32_t i = 0u; i < 9u; ++i )
					{
						col += kernel[i] * tex.sample( uv + offsets[i] );
					}

					writer.returnStmt( col );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "tex" }
				, sdw::InVec2{ writer, "uv" } );

			auto getSunUVPos = writer.implementFunction< sdw::Vec2 >( "getSunUVPos"
				, [&]()
				{
					auto worldPos = writer.declLocale( "worldPos"
						, vec4( c3d_cameraData.position() + vec3( 0.0_f, c3d_atmosphereData.bottomRadius, 0.0_f ) + c3d_atmosphereData.sunDirection * 1e6_f, 1.0_f ) );
					auto ndcPos = writer.declLocale( "ndcPos"
						, c3d_cameraData.camWorldToProj( worldPos ) );
					ndcPos /= ndcPos.w();
					writer.returnStmt( fma( ndcPos.xy(), vec2( 0.5_f ), vec2( 0.5_f ) ) );
				} );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto texCoords = writer.declLocale( "texCoords"
						, vec2( in.fragCoord.xy() ) / targetSize );
					texCoords.y() = 1.0_f - texCoords.y();
					fragColour = gaussianBlur( cloudsMap, texCoords );
					fragColour += emissionsMap.sample( texCoords );
/*
					// RADIAL BLUR - CREPUSCOLAR RAYS
					IF( writer, c3d_cameraData.lightDotCameraFront() > 0.0_f )
					{
						auto lightUV = writer.declLocale( "lightPos"
							, getSunUVPos() );

						// Screen coordinates.
						auto uv = writer.declLocale( "uv"
							, vec2( in.fragCoord.xy() ) / targetSize );

						auto colResult = writer.declLocale( "colResult"
							, emissionsMap.sample( uv ) );

						IF( writer, colResult.a() > 0.0_f )
						{
							// Radial blur factors.
							//
							auto decay = 0.98_f;
							auto density = 0.9_f;
							auto weight = 0.07_f;
							auto exposure = 0.15_f;
							const int SAMPLES = 64;

							// Light offset. 
							auto l = writer.declLocale( "l"
								, vec3( lightUV, 0.5_f ) );

							auto illuminationDecay = writer.declLocale( "illuminationDecay"
								, 1.0_f );

							auto dTuv = writer.declLocale( "dTuv"
								, uv - lightUV );
							dTuv *= density / float( SAMPLES );

							auto colRays = writer.declLocale( "colRays"
								, colResult.rgb() * 0.4_f );

							FOR( writer, sdw::Int, i, 0, i < SAMPLES, i++ )
							{
								uv -= dTuv;
								colRays += emissionsMap.sample( uv ).rgb() * illuminationDecay * weight;
								illuminationDecay *= decay;
							}
							ROF;

							auto colorWithRays = writer.declLocale( "colorWithRays"
								, fragColour.rgb() + ( smoothStep( vec3( 0.0_f ), vec3( 1.0_f ), colRays ) * exposure ) );
							fragColour.rgb() = mix( fragColour.rgb()
								, colorWithRays * 0.9_f
								, vec3( c3d_cameraData.lightDotCameraFront() * c3d_cameraData.lightDotCameraFront() ) );
						}
						FI;
					}
					FI;
*/
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	CloudsResolvePass::CloudsResolvePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & clouds
		, crg::ImageViewId const & emission
		, crg::ImageViewId const & resultView
		, uint32_t index )
		: castor::Named{ "Clouds/ResolvePass" + castor::string::toString( index ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), cloudsres::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), cloudsres::getPixelProgram( getExtent( resultView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( getName()
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		cameraUbo.createPassBinding( pass
			, cloudsres::eCamera );
		atmosphereUbo.createPassBinding( pass
			, cloudsres::eAtmosphere );
		pass.addSampledView( clouds
			, cloudsres::eClouds );
		pass.addSampledView( emission
			, cloudsres::eEmissions );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void CloudsResolvePass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
