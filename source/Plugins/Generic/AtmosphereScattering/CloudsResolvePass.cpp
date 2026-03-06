#include "AtmosphereScattering/CloudsResolvePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentRegister.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace cloudsres
	{
		enum class Bindings : uint32_t
		{
			eCamera,
			eAtmosphere,
			eClouds,
			eTransmittance,
			eMultiScatter,
			eSkyView,
			eVolume,
			eMapScattering,
			eMapTransmittance,
			eMapDepth,
			eCount,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D const & renderSize
			, bool hasDepth )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			c3d::shader::Utils utils{ writer };

			ATM_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphere
				, 0u );
			C3D_Clouds( writer
				, Bindings::eClouds
				, 0u );
			auto scatteringMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "atm_scatteringMap"
				, Bindings::eMapScattering
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("atm_transmittanceMap"
				, Bindings::eMapTransmittance
				, 0u );
			auto depthMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "depthMap"
				, Bindings::eMapDepth
				, 0u
				, hasDepth ) };

			AtmosphereModel atmosphere{ writer, c3d_atmosphereData
				, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setCameraData( &atm_cameraData ) };
			auto binding = uint32_t( Bindings::eTransmittance );
			ScatteringModel scattering{ writer
				, atmosphere
				, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setBloomSunDisk( true )
				, binding
				, 0u };

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			writer.implementEntryPointT< c3ds::Position2FT, sdw::VoidT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			if constexpr ( useUnified )
			{
				writer.implementEntryPointT< sdw::VoidT, c3ds::Colour4FT >( [&writer, &atmosphere, &scattering, hasDepth, &depthMap
						, &scatteringMap, &transmittanceMap, &targetSize]( sdw::FragmentIn const & in
					, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
					{
						auto texCoords = writer.declLocale( "texCoords"
							, in.fragCoord.xy() / targetSize );
						auto depthObj = writer.declLocale( "depthObj"
							, hasDepth ? depthMap.lod( texCoords, 0.0_f ) : vec4( -1.0_f, -1.0_f, 0.0_f, 0.0_f ) );
						texCoords.y() = 1.0_f - texCoords.y();

						auto inscatter = writer.declLocale( "inscatter"
							, scatteringMap.sample( texCoords ) );
						auto transmittance = writer.declLocale( "transmittance"
							, transmittanceMap.sample( texCoords ) );
						auto ray = writer.declLocale( "ray"
							, atmosphere.castRay( texCoords ) );
						auto sun = writer.declLocale( "sun"
							, vec4( writer.ternary( depthObj.b() == 0.0_f, scattering.getSunLuminance( ray ), vec3( 0.0_f, 0.0_f, 0.0_f ) ), 1.0_f ) );

						auto hdr = writer.declLocale( "hdr"
							, inscatter.xyz() + transmittance.xyz() * sun.xyz() );
						auto whitePoint = vec3( 1.08241_f, 0.96756_f, 0.95003_f );
						auto exposure = 10.0_f;
						hdr = vec3( 1.0_f ) - exp( -hdr / whitePoint * exposure );

						out.colour() = vec4( hdr, 1.0_f );
					} );
			}
			else
			{
				auto offsetX = writer.declConstant( "offsetX"
					, 1.0_f / targetSize.x() );
				auto offsetY = writer.declConstant( "offsetY"
					, 1.0_f / targetSize.y() );
				auto kernel = c3d::Vector< sdw::Float >{ 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f
						, 2.0_f / 16.0_f, 4.0_f / 16.0_f, 2.0_f / 16.0_f
						, 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f };
				auto offsets = c3d::Vector< sdw::Vec2 >{ vec2( -offsetX, offsetY ) // top-left
						, vec2( 0.0_f, offsetY ) // top-center
						, vec2( offsetX, offsetY ) // top-right
						, vec2( -offsetX, 0.0_f )   // center-left
						, vec2( 0.0_f, 0.0_f )   // center-center
						, vec2( offsetX, 0.0_f )   // center-right
						, vec2( -offsetX, -offsetY ) // bottom-left
						, vec2( 0.0_f, -offsetY ) // bottom-center
						, vec2( offsetX, -offsetY ) };  // bottom-right

				auto gaussianBlur = writer.implementFunction< sdw::Vec4 >( "gaussianBlur"
					, [&writer, &kernel, &offsets]( sdw::CombinedImage2DRgba32 const & tex
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

				auto computeLighting = writer.implementFunction< sdw::Vec3 >( "computeLighting"
					, [&writer, &c3d_cloudsData]( sdw::Vec3 const & skyColor
						, sdw::Vec3 const & cloudsColor
						, sdw::Float const & skyBlendFactor
						, sdw::Float const & cloudsDensity )
					{
						// Blend background and clouds.
						auto blendSkyColor = writer.declLocale( "blendSkyColor"
							, mix( skyColor
								, c3d_cloudsData.bottomColor()
								, vec3( c3d_cloudsData.coverage() ) ) );
						writer.returnStmt( mix( skyColor
							, cloudsColor + ( skyBlendFactor * blendSkyColor )
							, vec3( cloudsDensity ) ) );
					}
					, sdw::InVec3{ writer, "skyColor" }
					, sdw::InVec3{ writer, "cloudsColor" }
					, sdw::InFloat{ writer, "skyBlendFactor" }
					, sdw::InFloat{ writer, "cloudsDensity" } );

				writer.implementEntryPointT< sdw::VoidT, c3ds::Colour4FT >( [&writer, &scattering, &scatteringMap, &atmosphere, &gaussianBlur, &targetSize
						, hasDepth, &depthMap, &transmittanceMap, &computeLighting, &c3d_cloudsData]( sdw::FragmentIn const & in
					, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
					{
						auto texCoords = writer.declLocale( "texCoords"
							, in.fragCoord.xy() / targetSize );
						auto depthObj = writer.declLocale( "depthObj"
							, hasDepth ? depthMap.lod( texCoords, 0.0_f ) : vec4( -1.0_f, -1.0_f, 0.0_f, 0.0_f ) );
						texCoords.y() = 1.0_f - texCoords.y();

						auto sky = writer.declLocale( "sky"
							, scatteringMap.sample( texCoords ) );
						auto ray = writer.declLocale( "ray"
							, atmosphere.castRay( in.fragCoord.xy(), targetSize ) );
						auto sun = writer.declLocale( "sun"
							, vec4( writer.ternary( depthObj.b() == 0.0_f, scattering.getSunLuminance( ray ), vec3( 0.0_f, 0.0_f, 0.0_f ) ), 1.0_f ) );

						sdwIF( writer, c3d_cloudsData.coverage() > 0.0_f )
						{
							auto ray = writer.declLocale( "ray"
								, atmosphere.castRay( texCoords ) );
							auto clouds = writer.declLocale( "clouds"
								, gaussianBlur( transmittanceMap, texCoords ) );
							out.colour() = vec4( computeLighting( sky.rgb()
									, clouds.rgb()
									, sky.a()
									, clouds.a() )
								, clouds.a() );
						}
						sdwELSE
						{
							out.colour() = sky + sun;
						}
						sdwFI
					} );
				}

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsResolvePass::CloudsResolvePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CloudsUbo const & cloudsUbo
		, c3d::Texture const & transmittance
		, c3d::Texture const & multiscatter
		, c3d::Texture const & skyview
		, c3d::Texture const & volume
		, c3d::Texture const & inscatterResult
		, c3d::Texture const & transmittanceResult
		, c3d::Texture const * depthObj
		, c3d::Texture & result
		, uint32_t index )
		: c3d::Named{ cuT( "Clouds/ResolvePass" ) + c3d::string::toString( index ) }
		, m_shader{ getName(), cloudsres::getProgram( c3d::getEngine( device ), result.getExtent(), depthObj != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = result.getExtent();
		auto & pass = graph.createPass( c3d::toUtf8( getName() )
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto runPass = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( framePass, context, graph );
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, runPass->getTimer() );
				return runPass;
			} );
		crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear };
		cameraUbo.createPassBinding( pass, cloudsres::Bindings::eCamera );
		atmosphereUbo.createPassBinding( pass, cloudsres::Bindings::eAtmosphere );
		cloudsUbo.createPassBinding( pass, cloudsres::Bindings::eClouds );
		pass.addInputSampledT( *transmittance.getSampledLastAttach(), cloudsres::Bindings::eTransmittance, linearClampSampler );
		pass.addInputSampledT( *multiscatter.getSampledLastAttach(), cloudsres::Bindings::eMultiScatter, linearClampSampler );
		pass.addInputSampledT( *skyview.getSampledLastAttach(), cloudsres::Bindings::eSkyView, linearClampSampler );
		pass.addInputSampledT( *volume.getSampledLastAttach(), cloudsres::Bindings::eVolume, linearClampSampler );
		pass.addInputSampledT( *inscatterResult.getSampledLastAttach(), cloudsres::Bindings::eMapScattering );
		pass.addInputSampledT( *transmittanceResult.getSampledLastAttach(), cloudsres::Bindings::eMapTransmittance );
		if ( depthObj )
			pass.addInputSampledT( *depthObj->getSampledLastAttach(), cloudsres::Bindings::eMapDepth );
		result.setLastAttach( pass.addOutputColourTarget( result.getTargetViewId() ) );
	}

	void CloudsResolvePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
