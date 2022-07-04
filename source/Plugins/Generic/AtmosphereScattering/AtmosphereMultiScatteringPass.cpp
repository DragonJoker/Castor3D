#include "AtmosphereScattering/AtmosphereMultiScatteringPass.hpp"

#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace multiscatter
	{
		enum Bindings : uint32_t
		{
			eAtmosphere,
			eTransmittance,
			eOutput,
			eCount,
		};

		static castor3d::ShaderPtr getProgram( uint32_t renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			sdw::ComputeWriter writer;

			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );
			auto outputTexture = writer.declStorageImg< sdw::WImage2DRgba32 >("outputTexture"
				, uint32_t( Bindings::eOutput )
				, 0u );

			auto multiScatAs1SharedMem = writer.declSharedVariable< sdw::Vec3 >( "multiScatAs1SharedMem", 64u );
			auto lSharedMem = writer.declSharedVariable< sdw::Vec3 >( "lSharedMem", 64u );

			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );
			auto multiScatteringLUTRes = writer.declConstant( "multiScatteringLUTRes"
				, sdw::Float{ float( renderSize ) } );
			auto sampleCountIni = writer.declConstant( "sampleCountIni"
				, 20.0_f );// a minimum set of step is required for accuracy unfortunately
			auto depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );
			auto const PI{ sdw::Float{ castor::Pi< float > } };
			auto sphereSolidAngle = writer.declConstant( "sphereSolidAngle"
				, 4.0_f * PI );
			auto isotropicPhase = writer.declConstant( "isotropicPhase"
				, 1.0_f / sphereSolidAngle );

			castor3d::shader::Utils utils{ writer };
			AtmosphereModel atmosphere{ writer
				, utils
				, c3d_atmosphereData
				, { true, nullptr, false, false }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			auto getSphericalDir = writer.implementFunction< sdw::Vec3 >( "getSphericalDir"
				, [&]( sdw::Float const & theta
					, sdw::Float const & phi )
				{
					auto cosPhi = writer.declLocale( "cosPhi"
						, cos( phi ) );
					auto sinPhi = writer.declLocale( "sinPhi"
						, sin( phi ) );
					auto cosTheta = writer.declLocale( "cosTheta"
						, cos( theta ) );
					auto sinTheta = writer.declLocale( "sinTheta"
						, sin( theta ) );
					//writer.returnStmt( vec3( cosTheta * sinPhi, sinTheta * sinPhi, cosPhi );
					writer.returnStmt( vec3( sinTheta * sinPhi, cosPhi, cosTheta * sinPhi ) );
				}
				, sdw::InFloat{ writer, "theta" }
				, sdw::InFloat{ writer, "phi" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 1u, 1u, 64u }
				, [&]( sdw::ComputeIn in )
				{
					auto pixPos = writer.declLocale( "pixPos"
						, vec2( in.globalInvocationID.xy() ) + vec2( 0.5_f ) );
					auto uv = writer.declLocale( "uv"
						, pixPos / vec2( multiScatteringLUTRes ) );

					uv = vec2( atmosphere.fromSubUvsToUnit( uv.x(), multiScatteringLUTRes )
						, atmosphere.fromSubUvsToUnit( uv.y(), multiScatteringLUTRes ) );

					auto cosSunZenithAngle = writer.declLocale( "cosSunZenithAngle"
						, fma( uv.x(), 2.0_f, -1.0_f ) );
					auto sinSunZenithAngle = writer.declLocale( "sinSunZenithAngle"
						, sqrt( clamp( 1.0_f - cosSunZenithAngle * cosSunZenithAngle, 0.0_f, 1.0_f ) ) );
					auto sunDir = writer.declLocale( "sunDir"
						, vec3( 0.0_f, cosSunZenithAngle, -sinSunZenithAngle ) );
					// We adjust again viewHeight according to PLANET_RADIUS_OFFSET to be in a valid range.
					auto viewHeight = writer.declLocale( "viewHeight"
						, c3d_atmosphereData.bottomRadius + clamp( uv.y() + planetRadiusOffset, 0.0_f, 1.0_f ) * ( c3d_atmosphereData.topRadius - c3d_atmosphereData.bottomRadius - planetRadiusOffset ) );

					auto ray = writer.declLocale< Ray >( "ray" );
					ray.origin = vec3( 0.0_f, viewHeight, 0.0_f );
					ray.direction = vec3( 0.0_f, 1.0_f, 0.0_f );

					// Reference. Since there are many sample, it requires MULTI_SCATTERING_POWER_SERIE to be true for accuracy and to avoid divergences (see declaration for explanations)
					auto sqrtSampleCount = 8_u;
					auto sqrtSample = writer.declLocale( "sqrtSample"
						, writer.cast< sdw::Float >( sqrtSampleCount ) );
					auto i = writer.declLocale( "i"
						, 0.5_f + writer.cast< sdw::Float >( in.globalInvocationID.z() / sqrtSampleCount ) );
					auto j = writer.declLocale( "j"
						, 0.5_f + writer.cast< sdw::Float >( in.globalInvocationID.z() - ( in.globalInvocationID.z() / sqrtSampleCount ) * sqrtSampleCount ) );
					{
						auto randA = writer.declLocale( "randA"
							, i / sqrtSample );
						auto randB = writer.declLocale( "randB"
							, j / sqrtSample );
						auto theta = writer.declLocale( "theta"
							, 2.0_f * PI * randA );
						auto phi = writer.declLocale( "phi"
							, PI * randB );
						ray.origin = getSphericalDir( theta, phi );
						auto result = writer.declLocale( "result"
							, atmosphere.integrateScatteredLuminanceNoShadow( pixPos
								, ray
								, sunDir
								, sampleCountIni
								, depthBufferValue ) );

						multiScatAs1SharedMem[in.globalInvocationID.z()] = result.multiScatAs1() * sphereSolidAngle / ( sqrtSample * sqrtSample );
						lSharedMem[in.globalInvocationID.z()] = result.luminance() * sphereSolidAngle / ( sqrtSample * sqrtSample );
					}

					sdw::barrier( writer );

					// 64 to 32
					IF( writer, in.globalInvocationID.z() < 32u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 32u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 32u];
					}
					FI;

					sdw::barrier( writer );

					// 32 to 16
					IF( writer, in.globalInvocationID.z() < 16u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 16u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 16u];
					}
					FI;

					sdw::barrier( writer );

					// 16 to 8 (16 is thread group min hardware size with intel, no sync required from there)
					IF( writer, in.globalInvocationID.z() < 8u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 8u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 8u];
					}
					FI;

					sdw::barrier( writer );

					IF( writer, in.globalInvocationID.z() < 4u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 4u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 4u];
					}
					FI;

					sdw::barrier( writer );

					IF( writer, in.globalInvocationID.z() < 2u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 2u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 2u];
					}
					FI;

					sdw::barrier( writer );

					IF( writer, in.globalInvocationID.z() < 1u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 1u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 1u];
					}
					FI;

					sdw::barrier( writer );

					IF( writer, in.globalInvocationID.z() > 0u )
					{
						writer.returnStmt();
					}
					FI;

					auto multiScatAs1 = writer.declLocale( "multiScatAs1"
						, multiScatAs1SharedMem[0] * isotropicPhase );	// Equation 7 f_ms
					auto inScatteredLuminance = writer.declLocale( "inScatteredLuminance"
						, lSharedMem[0] * isotropicPhase );				// Equation 5 L_2ndOrder

					// multiScatAs1 represents the amount of luminance scattered as if the integral of scattered luminance over the sphere would be 1.
					//  - 1st order of scattering: one can ray-march a straight path as usual over the sphere. That is inScatteredLuminance.
					//  - 2nd order of scattering: the inscattered luminance is inScatteredLuminance at each of samples of fist order integration. Assuming a uniform phase function that is represented by multiScatAs1,
					//  - 3nd order of scattering: the inscattered luminance is (inScatteredLuminance * multiScatAs1 * multiScatAs1)
					//  - etc.
#if	MULTI_SCATTERING_POWER_SERIE==0
					auto multiScatAs1SQR = writer.declLocale( "multiScatAs1SQR"
						, multiScatAs1 * multiScatAs1 );
					auto L = writer.declLocale( "L"
						, inScatteredLuminance * ( 1.0_f + multiScatAs1 + multiScatAs1SQR + multiScatAs1 * multiScatAs1SQR + multiScatAs1SQR * multiScatAs1SQR ) );
#else
					// For a serie, sum_{n=0}^{n=+inf} = 1 + r + r^2 + r^3 + ... + r^n = 1 / (1.0 - r), see https://en.wikipedia.org/wiki/Geometric_series 
					auto r = writer.declLocale( "r"
						, multiScatAs1 );
					auto sumOfAllMultiScatteringEventsContribution = writer.declLocale( "sumOfAllMultiScatteringEventsContribution"
						, vec3( 1.0_f ) / ( vec3( 1.0_f ) - r ) );
					auto L = writer.declLocale( "L"
						, inScatteredLuminance * sumOfAllMultiScatteringEventsContribution );// Equation 10 Psi_ms
#endif

					outputTexture.store( ivec2( in.globalInvocationID.xy() )
						, vec4( c3d_atmosphereData.multipleScatteringFactor * L, 1.0_f ) );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereMultiScatteringPass::AtmosphereMultiScatteringPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & transmittanceLut
		, crg::ImageViewId const & resultView
		, bool const & enabled )
		: m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "MultiScatteringPass", multiscatter::getProgram( getExtent( resultView ).width, getExtent( transmittanceLut ) ) }
		, m_stages{ makeShaderState( device, m_computeShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( "MultiScatteringPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::ComputePass >( framePass
					, context
					, graph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( renderSize.width )
						.groupCountY( renderSize.height )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		atmosphereUbo.createPassBinding( pass
			, multiscatter::eAtmosphere );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( transmittanceLut
			, multiscatter::eTransmittance
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addOutputStorageView( resultView
			, multiscatter::eOutput );
		m_lastPass = &pass;
	}

	void AtmosphereMultiScatteringPass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
