#include "AtmosphereScattering/AtmosphereVolumetricCloudsPass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsModel.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace volclouds
	{
		static constexpr bool useCompute = false;

		template< bool ComputeT >
		struct ShaderWriter;

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::FragmentWriter;

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

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMain( [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.fragCoord.xy() ) );
						func( fragCoord );
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Type = sdw::ComputeWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMain( [&]( sdw::ComputeIn in )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.globalInvocationID.xy() ) );
						func( fragCoord );
					} );
			}
		};

		enum Bindings : uint32_t
		{
			eAtmosphere,
			eClouds,
			eCamera,
			eTransmittance,
			eMultiScatter,
			eSkyView,
			eVolume,
			ePerlinWorley,
			eWorley,
			eCurl,
			eWeatherMap,
			eOutColour,
			eOutEmission,
			eCount,
		};

		static castor3d::ShaderPtr getProgram( VkExtent3D renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			ShaderWriter< useCompute >::Type writer;

			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			C3D_Clouds( writer
				, uint32_t( Bindings::eClouds )
				, 0u );
			C3D_Camera( writer
				, uint32_t( Bindings::eCamera )
				, 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			auto depthBufferValue = 1.0_f;

			castor3d::shader::Utils utils{ writer };
			AtmosphereModel atmosphere{ writer
				, utils
				, c3d_atmosphereData
				, { false, &c3d_cameraData, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height } };
			ScatteringModel scattering{ writer
				, atmosphere
				, false /*colorTransmittance*/
				, true /*fastSky*/
				, true /*fastAerialPerspective*/
				, true /*renderSunDisk*/
				, false /*bloomSunDisk*/
				, uint32_t( Bindings::eTransmittance ) };
			CloudsModel clouds{ writer
				, utils
				, atmosphere
				, scattering
				, c3d_cloudsData
				, uint32_t( Bindings::ePerlinWorley ) };

			if constexpr ( useCompute )
			{
				auto outColour = writer.declStorageImg< sdw::WImage2DRgba32 >( "outColour"
					, uint32_t( Bindings::eOutColour )
					, 0u );
				auto outEmission = writer.declStorageImg< sdw::WImage2DRgba32 >("outEmission"
					, uint32_t( Bindings::eOutEmission )
					, 0u );

				ShaderWriter< useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & fragCoord )
					{
						auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
						auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
						scattering.getPixelTransLum( vec2( fragCoord.xy() )
							, targetSize
							, depthBufferValue
							, transmittance
							, luminance );
						luminance = scattering.rescaleLuminance( luminance );
						auto emission = writer.declLocale( "emission", vec4( 0.0_f ) );
						clouds.applyClouds( fragCoord
							, targetSize
							, luminance
							, emission );
						outColour.store( fragCoord, luminance );
						outEmission.store( fragCoord, emission );
					} );
			}
			else
			{
				auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", 0u );
				auto outEmission = writer.declOutput< sdw::Vec4 >( "outEmission", 1u );

				ShaderWriter< useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & fragCoord )
					{
						auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
						auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
						scattering.getPixelTransLum( vec2( fragCoord.xy() )
							, targetSize
							, depthBufferValue
							, transmittance
							, luminance );
						luminance = scattering.rescaleLuminance( luminance );
						auto distance = writer.declLocale( "distance", vec4( 0.0_f ) );
						auto emission = writer.declLocale( "emission", vec4( 0.0_f ) );
						clouds.applyClouds( fragCoord
							, targetSize
							, luminance
							, emission );
						outColour = luminance;
						outEmission = emission;
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereVolumetricCloudsPass::AtmosphereVolumetricCloudsPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CameraUbo const & cameraUbo
		, CloudsUbo const & cloudsUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & multiscatter
		, crg::ImageViewId const & skyview
		, crg::ImageViewId const & volume
		, crg::ImageViewId const & perlinWorley
		, crg::ImageViewId const & worley
		, crg::ImageViewId const & curl
		, crg::ImageViewId const & weather
		, crg::ImageViewId const & colourResult
		, crg::ImageViewId const & emissionResult
		, uint32_t index )
		: castor::Named{ "VolumetricCloudsPass" + castor::string::toString( index ) }
		, m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName()
			, ( volclouds::useCompute
				? volclouds::getProgram( getExtent( colourResult ), getExtent( transmittance ) )
				: nullptr) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, getName()
			, ( volclouds::useCompute
				? nullptr
				: volclouds::ShaderWriter< false >::getVertexProgram() ) }
		, m_fragmentShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, getName()
			, ( volclouds::useCompute
				? nullptr
				: volclouds::getProgram( getExtent( colourResult ), getExtent( transmittance ) ) ) }
		, m_stages{ ( volclouds::useCompute
			? ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeShader ) }
			: ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_vertexShader )
				, makeShaderState( device, m_fragmentShader ) } ) }
	{
		auto renderSize = getExtent( colourResult );
		auto & pass = graph.createPass( getName()
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( volclouds::useCompute )
				{
					result = std::make_unique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( renderSize.width / 16u )
							.groupCountY( renderSize.height / 16u )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				}
				else
				{
					result = crg::RenderQuadBuilder{}
						.renderSize( { renderSize.width, renderSize.height } )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.build( framePass, context, graph );
				}

				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		atmosphereUbo.createPassBinding( pass
			, volclouds::eAtmosphere );
		cloudsUbo.createPassBinding( pass
			, volclouds::eClouds );
		cameraUbo.createPassBinding( pass
			, volclouds::eCamera );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		crg::SamplerDesc mipLinearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		pass.addSampledView( transmittance
			, volclouds::eTransmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( multiscatter
			, volclouds::eMultiScatter
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( skyview
			, volclouds::eSkyView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( volume
			, volclouds::eVolume
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( perlinWorley
			, volclouds::ePerlinWorley
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, mipLinearSampler );
		pass.addSampledView( worley
			, volclouds::eWorley
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, mipLinearSampler );
		pass.addSampledView( curl
			, volclouds::eCurl
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( weather
			, volclouds::eWeatherMap
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT } );

		if constexpr ( volclouds::useCompute )
		{
			pass.addOutputStorageView( colourResult
				, volclouds::eOutColour );
			pass.addOutputStorageView( emissionResult
				, volclouds::eOutEmission );
		}
		else
		{
			pass.addOutputColourView( colourResult );
			pass.addOutputColourView( emissionResult );
		}

		m_lastPass = &pass;
	}

	void AtmosphereVolumetricCloudsPass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
