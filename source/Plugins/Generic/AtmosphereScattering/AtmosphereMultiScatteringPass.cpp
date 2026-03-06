#include "AtmosphereScattering/AtmosphereMultiScatteringPass.hpp"

#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
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
		enum class Bindings : uint32_t
		{
			eAtmosphere,
			eTransmittance,
			eOutput,
			eCount,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, uint32_t renderSize
			, c3d::Extent3D const & transmittanceExtent )
		{
			sdw::ComputeWriter writer{ &engine.getShaderAllocator() };

			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphere
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "transmittanceMap"
				, Bindings::eTransmittance
				, 0u );
			auto outputTexture = writer.declStorageImg< sdw::WImage2DRgba16 >("outputTexture"
				, Bindings::eOutput
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
			auto const PI{ sdw::Float{ c3d::Pi< float > } };
			auto sphereSolidAngle = writer.declConstant( "sphereSolidAngle"
				, 4.0_f * PI );
			auto isotropicPhase = writer.declConstant( "isotropicPhase"
				, 1.0_f / sphereSolidAngle );

			AtmosphereModel atmosphere{ writer, c3d_atmosphereData
				, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setUseGround( true )
					.setIlluminanceIsOne( true )
				, { transmittanceExtent.width, transmittanceExtent.height } };
			atmosphere.setTransmittanceMap( transmittanceMap );

			auto getSphericalDir = writer.implementFunction< sdw::Vec3 >( "getSphericalDir"
				, [&writer]( sdw::Float const & theta
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
				, [&multiScatteringLUTRes, &atmosphere, &planetRadiusOffset, &getSphericalDir, &lSharedMem
					, &PI, &sampleCountIni, &depthBufferValue, &multiScatAs1SharedMem, &sphereSolidAngle, &isotropicPhase
					, &writer, &outputTexture, &c3d_atmosphereData]( sdw::ComputeIn const & in )
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
						, atmosphere.getPlanetRadius() + clamp( uv.y() + planetRadiusOffset, 0.0_f, 1.0_f ) * ( atmosphere.getAtmosphereThickness() - planetRadiusOffset ) );

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
						ray.direction = getSphericalDir( theta, phi );
						auto result = writer.declLocale( "result"
							, atmosphere.integrateScatteredLuminance( pixPos
								, ray
								, sunDir
								, sampleCountIni
								, depthBufferValue ) );

						multiScatAs1SharedMem[in.globalInvocationID.z()] = result.multiScatAs1 * sphereSolidAngle / ( sqrtSample * sqrtSample );
						lSharedMem[in.globalInvocationID.z()] = result.luminance * sphereSolidAngle / ( sqrtSample * sqrtSample );
					}

					sdw::barrier( writer );

					// 64 to 32
					sdwIF( writer, in.globalInvocationID.z() < 32u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 32u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 32u];
					}
					sdwFI

					sdw::barrier( writer );

					// 32 to 16
					sdwIF( writer, in.globalInvocationID.z() < 16u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 16u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 16u];
					}
					sdwFI

					sdw::barrier( writer );

					// 16 to 8 (16 is thread group min hardware size with intel, no sync required from there)
					sdwIF( writer, in.globalInvocationID.z() < 8u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 8u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 8u];
					}
					sdwFI

					sdw::barrier( writer );

					sdwIF( writer, in.globalInvocationID.z() < 4u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 4u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 4u];
					}
					sdwFI

					sdw::barrier( writer );

					sdwIF( writer, in.globalInvocationID.z() < 2u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 2u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 2u];
					}
					sdwFI

					sdw::barrier( writer );

					sdwIF( writer, in.globalInvocationID.z() < 1u )
					{
						multiScatAs1SharedMem[in.globalInvocationID.z()] += multiScatAs1SharedMem[in.globalInvocationID.z() + 1u];
						lSharedMem[in.globalInvocationID.z()] += lSharedMem[in.globalInvocationID.z() + 1u];
					}
					sdwFI

					sdw::barrier( writer );

					sdwIF( writer, in.globalInvocationID.z() > 0u )
					{
						writer.returnStmt();
					}
					sdwFI

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
						, vec4( c3d_atmosphereData.multipleScatteringFactor() * L, 1.0_f ) );
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	AtmosphereMultiScatteringPass::AtmosphereMultiScatteringPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, c3d::Texture const & transmittanceLut
		, c3d::Texture & result
		, bool const & enabled )
		: m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "MultiScatteringPass" ), multiscatter::getProgram( c3d::getEngine( device ), result.getExtent().width, transmittanceLut.getExtent() ) }
		, m_stages{ makeShaderState( device, m_computeShader ) }
	{
		auto renderSize = result.getExtent();
		auto & pass = graph.createPass( "MultiScatteringPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = c3d::makeRawUnique< crg::ComputePass >( framePass
					, context
					, runGraph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( renderSize.width )
						.groupCountY( renderSize.height )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		atmosphereUbo.createPassBinding( pass, multiscatter::Bindings::eAtmosphere );
		pass.addInputSampledT( *transmittanceLut.getSampledLastAttach(), multiscatter::Bindings::eTransmittance
			, crg::SamplerDesc{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear } );
		result.setLastAttach( pass.addOutputStorageImageT( result.getTargetViewId(), multiscatter::Bindings::eOutput ) );
	}

	void AtmosphereMultiScatteringPass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
