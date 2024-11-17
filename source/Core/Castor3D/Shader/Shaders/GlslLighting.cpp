#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementDeleter( castor3d::shader, LightingModel )
CU_ImplementDeleter( castor3d::shader, ScatteringModel )

#define C3D_DebugCascades 0

namespace castor3d::shader
{
	//*********************************************************************************************

	ScatteringModel::ScatteringModel( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	void ScatteringModel::initialiseBackground( BackgroundModel & pbackground
		, Shadow & shadowModel )
	{
	}

	sdw::Vec3 ScatteringModel::computeRadiance( Light const & light
			, sdw::Vec3 const & lightDirection )const
	{
		return light.colour();
	}

	void ScatteringModel::computeScattering( LightingModel & lighting
		, ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Vec2 const & lightIntensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		lighting.applyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, false /*multiply*/ );
	}

	ScatteringModelPtr ScatteringModel::create( sdw::ShaderWriter & writer )
	{
		return castor::makeUnique< ScatteringModel >( writer );
	}

	//*********************************************************************************************

	LightingModel::LightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, LightingModelSpec spec
		, Shadow & shadowModel
		, Lights & lights
		, bool hasIblSupport
		, bool hasBackgroundReflectionsSupport
		, bool hasBackgroundRefractionSupport
		, bool enableVolumetric
		, castor::String prefix )
		: m_lightingModelId{ lightingModelId }
		, m_writer{ writer }
		, m_materials{ materials }
		, m_utils{ utils }
		, m_brdfHelpers{ brdfHelpers }
		, m_shadowModel{ shadowModel }
		, m_lights{ lights }
		, m_diffuse{ std::move( spec.diffuse ) }
		, m_specular{ std::move( spec.specular ) }
		, m_sheen{ std::move( spec.sheen ) }
		, m_clearcoat{ std::move( spec.clearcoat ) }
		, m_scattering{ std::move( spec.scattering ) }
		, m_hasBackgroundReflectionsSupport{ hasBackgroundReflectionsSupport }
		, m_hasBackgroundRefractionSupport{ hasBackgroundRefractionSupport }
		, m_hasIblSupport{ hasIblSupport }
		, m_enableVolumetric{ enableVolumetric }
		, m_prefix{ castor::move( prefix ) }
	{
	}

	sdw::Vec3 LightingModel::combine( DebugOutput & debugOutput
		, ReflectionModel & reflections
		, sdw::CombinedImage2DRgba32 const & brdf
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & incident
		, sdw::Float const & ambientOcclusion
		, DirectLighting const & directLighting
		, IndirectLighting const & indirectLighting
		, ReflectionRefraction const & reflRefr )
	{
		auto debugOutputBlock = debugOutput.pushBlock( cuT( "Combine" ) );
		auto backgroundResult = m_writer.declLocale( "backgroundResult"
			, mix( reflRefr.dielectric, reflRefr.metal, vec3( components.metalness ) ) );
		debugOutputBlock.registerOutput( "Background Result", backgroundResult );

		auto lightingResult = m_writer.declLocale( "lightingResult"
			, mix( directLighting.dielectric, directLighting.metal, vec3( components.metalness ) ) );
		debugOutputBlock.registerOutput( "Lighting Result", lightingResult );

		if ( components.sheenColour )
		{
			auto bgSheen = m_writer.declLocale( "bgSheen"
				, reflRefr.sheen.rgb() );
			debugOutputBlock.registerOutput( "Background Sheen", bgSheen );
			auto bgAlbedoSheenScaling = m_writer.declLocale( "bgAlbedoSheenScaling"
				, 1.0_f - reflRefr.sheen.w() * Utils::max3( components.sheenColour ) );
			debugOutputBlock.registerOutput( "Background Albedo Sheen Scaling", bgAlbedoSheenScaling );
			backgroundResult = bgSheen + backgroundResult * bgAlbedoSheenScaling;
			debugOutputBlock.registerOutput( "Background With Sheen", backgroundResult );

			auto ltSheen = m_writer.declLocale( "ltSheen"
				, directLighting.sheen.rgb() );
			debugOutputBlock.registerOutput( "Lighting Sheen", ltSheen );
			auto ltAlbedoSheenScaling = m_writer.declLocale( "ltAlbedoSheenScaling"
				, 1.0_f - directLighting.sheen.w() * Utils::max3( components.sheenColour ) );
			debugOutputBlock.registerOutput( "Lighting Albedo Sheen Scaling", ltAlbedoSheenScaling );
			lightingResult = bgSheen + lightingResult * ltAlbedoSheenScaling;
			debugOutputBlock.registerOutput( "Lighting With Sheen", lightingResult );
		}

		auto clearcoatFresnel = components.getMember( "clearcoatFresnel", vec3( 0.0_f ) );

		if ( components.clearcoatFactor )
		{
			debugOutputBlock.registerOutput( "Clearcoat Fresnel", clearcoatFresnel );

			backgroundResult = mix( backgroundResult, reflRefr.coating, components.clearcoatFactor * clearcoatFresnel );
			debugOutputBlock.registerOutput( "Background With Clearcoat", backgroundResult );

			lightingResult = mix( lightingResult, directLighting.coating, components.clearcoatFactor * clearcoatFresnel );
			debugOutputBlock.registerOutput( "Lighting With Clearcoat", lightingResult );
		}

		auto emissiveResult = m_writer.declLocale( "emissiveResult"
			, ( components.emissiveColour
				* components.emissiveFactor
				* ( 1.0_f - components.clearcoatFactor * clearcoatFresnel ) ) );
		debugOutputBlock.registerOutput( "Emissive Result", emissiveResult );

		auto combineResult = m_writer.declLocale( "combineResult"
			, backgroundResult * ambientOcclusion
			+ lightingResult
			+ emissiveResult );
		return combineResult;
	}

	void LightingModel::compute( DebugOutputCategory const & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeDirectional )
		{
			m_scattering->initialiseBackground( background, m_shadowModel );
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computeDirectionalLight"
				, [this, &debugOutput]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					auto output = m_writer.declLocale( "output"
						, DirectLighting{ m_writer } );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( derivVec3( -light.direction() ) );
					lightSurface.registerDebug( debugOutput );
					doComputeLight( light.base(), components, lightSurface
						, 1.0_f, radiance
						, output );
					auto shadows = m_writer.declLocale( "shadows"
						, m_shadowModel.getDirectionalShadows() );

					if ( m_shadowModel.isEnabled() )
					{
						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								parentOutput.diffuse += output.diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadows( shadows
							, light.shadowMapIndex()
							, light.base().intensity()
							, lightSurface
							, radiance
							, receivesShadows
							, output );
					}

					m_scattering->computeScattering( *this
						, shadows.base()
						, light.shadowMapIndex()
						, radiance
						, light.base().intensity()
						, components
						, lightSurface
						, output.scattering );
					parentOutput += output;
				}
				, PDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computeDirectional( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::compute( DebugOutputCategory const & debugOutput
		, PointLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computePoint )
		{
			m_computePoint = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computePointLight"
				, [this, &debugOutput]( PointLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					auto output = m_writer.declLocale( "output"
						, DirectLighting{ m_writer } );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					doComputeLight( light.base(), components, lightSurface
						, light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance
						, output );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getPointShadows( light.shadowMapIndex() ) );

						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								output.diffuse += output.diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadows( shadows
							, light.shadowMapIndex()
							, computeRange( light )
							, lightSurface
							, radiance
							, receivesShadows
							, output );
					}

					parentOutput += output;
				}
				, PPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computePoint( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::compute( DebugOutputCategory const & debugOutput
		, SpotLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			m_computeSpot = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computeSpotLight"
				, [this, &debugOutput]( SpotLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto output = m_writer.declLocale( "output"
							, DirectLighting{ m_writer } );
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						doComputeLight( light.base(), components, lightSurface
							, spotFactor * light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance
							, output );

						if ( m_shadowModel.isEnabled() )
						{
							auto shadows = m_writer.declLocale( "shadows"
								, m_shadowModel.getSpotShadows( light.shadowMapIndex() ) );

							if ( m_lights.hasSssTransmittance()
								&& components.hasMember( "sssProfileIndex" ) )
							{
								auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

								IF( m_writer
									, ( light.shadowMapIndex() >= 0_i )
									&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
									&& ( receivesShadows != 0_u )
									&& ( sssProfileIndex != 0_u ) )
								{
									output.diffuse += output.diffuse
										* m_lights.computeSssTransmittance( debugOutput
											, components
											, light
											, shadows
											, lightSurface );
								}
								FI
							}

							doApplyShadows( shadows
								, light.shadowMapIndex()
								, computeRange( light )
								, lightSurface
								, radiance
								, receivesShadows
								, output );
						}

						parentOutput += output;
					}
					FI
				}
				, PSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computeSpot( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	sdw::Vec3 LightingModel::computeDiffuse( DebugOutputCategory const & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_scattering->initialiseBackground( background, m_shadowModel );
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( castor::toUtf8( m_prefix ) + "computeDirectionalLightDiffuse"
				, [this, &debugOutput]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( derivVec3( -light.direction() ) );
					lightSurface.registerDebug( debugOutput );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					auto diffuse = doComputeLightDiffuse( light.base(), components, lightSurface
						, 1.0_f, radiance );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getDirectionalShadows() );

						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								diffuse += diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadowsDiffuse( shadows
							, light.shadowMapIndex()
							, lightSurface
							, radiance
							, receivesShadows
							, diffuse );
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InOutDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows );
	}

	sdw::Vec3 LightingModel::computeDiffuse( DebugOutputCategory const & debugOutput
		, PointLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( castor::toUtf8( m_prefix ) + "computePointLightDiffuse"
				, [this, &debugOutput]( PointLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					auto diffuse = doComputeLightDiffuse( light.base(), components, lightSurface
						, light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getPointShadows( light.shadowMapIndex() ) );

						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								diffuse += diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadowsDiffuse( shadows
							, light.shadowMapIndex()
							, computeRange( light )
							, lightSurface
							, radiance
							, receivesShadows
							, diffuse );
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InOutPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows );
	}
	
	sdw::Vec3 LightingModel::computeDiffuse( DebugOutputCategory const & debugOutput
		, SpotLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( castor::toUtf8( m_prefix ) + "computeSpotLightDiffuse"
				, [this, &debugOutput]( SpotLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						diffuse = doComputeLightDiffuse( light.base(), components, lightSurface
							, spotFactor * light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance );

						if ( m_shadowModel.isEnabled() )
						{
							auto shadows = m_writer.declLocale( "shadows"
								, m_shadowModel.getSpotShadows( light.shadowMapIndex() ) );

							if ( m_lights.hasSssTransmittance()
								&& components.hasMember( "sssProfileIndex" ) )
							{
								auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

								IF( m_writer
									, ( light.shadowMapIndex() >= 0_i )
									&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
									&& ( receivesShadows != 0_u )
									&& ( sssProfileIndex != 0_u ) )
								{
									diffuse += diffuse
										* m_lights.computeSssTransmittance( debugOutput
											, components
											, light
											, shadows
											, lightSurface );
								}
								FI
							}

							doApplyShadowsDiffuse( shadows
								, light.shadowMapIndex()
								, computeRange( light )
								, lightSurface
								, radiance
								, receivesShadows
								, diffuse );
						}

						diffuse = max( vec3( 0.0_f ), diffuse );
					}
					FI;

					m_writer.returnStmt( diffuse );
				}
				, InOutSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows );
	}

	void LightingModel::computeAllButDiffuse( DebugOutputCategory const & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeDirectionalAllButDiffuse )
		{
			m_scattering->initialiseBackground( background, m_shadowModel );
			m_computeDirectionalAllButDiffuse = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computeDirectionalLightAllButDiffuse"
				, [this, &debugOutput]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					auto output = m_writer.declLocale< DirectLighting >( "output"
						, DirectLighting{ m_writer } );
					output.diffuse = parentOutput.diffuse;
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( derivVec3( -light.direction() ) );
					lightSurface.registerDebug( debugOutput );
					doComputeLightAllButDiffuse( light.base(), components, lightSurface
						, 1.0_f, radiance
						, output );
					auto shadows = m_writer.declLocale( "shadows"
						, m_shadowModel.getDirectionalShadows() );

					if ( m_shadowModel.isEnabled() )
					{
						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								output.diffuse += output.diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadows( shadows
							, light.shadowMapIndex()
							, light.base().intensity()
							, lightSurface
							, radiance
							, receivesShadows
							, output
							, false );
					}

					m_scattering->computeScattering( *this
						, shadows.base()
						, light.shadowMapIndex()
						, radiance
						, light.base().intensity()
						, components
						, lightSurface
						, output.scattering );
					parentOutput += output;
				}
				, PDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computeDirectionalAllButDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::computeAllButDiffuse( DebugOutputCategory const & debugOutput
		, PointLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computePointAllButDiffuse )
		{
			m_computePointAllButDiffuse = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computePointLightLightAllButDiffuse"
				, [this, &debugOutput]( PointLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					auto output = m_writer.declLocale< DirectLighting >( "output"
						, DirectLighting{ m_writer } );
					output.diffuse = parentOutput.diffuse;
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					doComputeLightAllButDiffuse( light.base(), components, lightSurface
						, light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance
						, output );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getPointShadows( light.shadowMapIndex() ) );

						if ( m_lights.hasSssTransmittance()
							&& components.hasMember( "sssProfileIndex" ) )
						{
							auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

							IF( m_writer
								, ( light.shadowMapIndex() >= 0_i )
								&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
								&& ( receivesShadows != 0_u )
								&& ( sssProfileIndex != 0_u ) )
							{
								output.diffuse += output.diffuse
									* m_lights.computeSssTransmittance( debugOutput
										, components
										, light
										, shadows
										, lightSurface );
							}
							FI
						}

						doApplyShadows( shadows
							, light.shadowMapIndex()
							, computeRange( light )
							, lightSurface
							, radiance
							, receivesShadows
							, output
							, false );
					}

					parentOutput += output;
				}
				, PPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computePointAllButDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::computeAllButDiffuse( DebugOutputCategory const & debugOutput
		, SpotLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeSpotAllButDiffuse )
		{
			m_computeSpotAllButDiffuse = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computeSpotLightLightAllButDiffuse"
				, [this, &debugOutput]( SpotLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					lightSurface.updateL( derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() ) );
					lightSurface.registerDebug( debugOutput );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto output = m_writer.declLocale< DirectLighting >( "output"
							, DirectLighting{ m_writer } );
						output.diffuse = parentOutput.diffuse;
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						doComputeLightAllButDiffuse( light.base(), components, lightSurface
							, spotFactor * light.getAttenuationFactor( lightSurface.lengthL().value() ), radiance
							, output );

						if ( m_shadowModel.isEnabled() )
						{
							auto shadows = m_writer.declLocale( "shadows"
								, m_shadowModel.getSpotShadows( light.shadowMapIndex() ) );

							if ( m_lights.hasSssTransmittance()
								&& components.hasMember( "sssProfileIndex" ) )
							{
								auto sssProfileIndex = components.getMember< sdw::UInt >( "sssProfileIndex" );

								IF( m_writer
									, ( light.shadowMapIndex() >= 0_i )
									&& ( shadows.base().shadowType() != sdw::UInt( uint32_t( ShadowType::eNone ) ) )
									&& ( receivesShadows != 0_u )
									&& ( sssProfileIndex != 0_u ) )
								{
									output.diffuse += output.diffuse
										* m_lights.computeSssTransmittance( debugOutput
											, components
											, light
											, shadows
											, lightSurface );
								}
								FI
							}

							doApplyShadows( shadows
								, light.shadowMapIndex()
								, computeRange( light )
								, lightSurface
								, radiance
								, receivesShadows
								, output
								, false );
						}

						parentOutput += output;
					}
					FI
				}
				, PSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, InOutDirectLighting{ m_writer, "parentOutput" } );
		}

		m_computeSpotAllButDiffuse( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::applyVolumetric( ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec2 const & lightIntensity
		, LightSurface const & lightSurface
		, sdw::Vec3 output
		, bool multiply )
	{
		if ( m_enableVolumetric
			&& m_directionalTransform
			&& m_directionalCascadeIndex
			&& m_directionalCascadeCount )
		{
			IF( m_writer, shadows.volumetricSteps() != 0_u
				&& shadowMapIndex >= 0_i )
			{
				auto volumetric = m_writer.declLocale( "volumetric"
					, m_shadowModel.computeVolumetric( shadows
						, lightSurface
						, *m_directionalTransform
						, *m_directionalCascadeIndex
						, *m_directionalCascadeCount ) );

				if ( multiply )
				{
					output *= vec3( volumetric * lightIntensity.x() );
				}
				else
				{
					output = vec3( volumetric * lightIntensity.x() );
				}
			}
			FI
		}
	}

	void LightingModel::doApplyShadows( DirectionalShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec2 const & lightIntensity
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, DirectLighting & output
		, bool withDiffuse )
	{
		m_directionalCascadeIndex = castor::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
			, 0_u ) );
		m_directionalCascadeCount = castor::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
			, 0_u ) );
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i ) )
		{
			// Get cascade index for the current fragment's view position
			auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
				, m_lights.getCascadeFactors( shadows
					, lightSurface.viewPosition().value()
					, m_shadowModel.getMaxCascadeCount() ) );
			*m_directionalCascadeIndex = m_writer.cast< sdw::UInt >( cascadeFactors.x() );
			*m_directionalCascadeCount = shadows.cascadeCount();
			m_directionalTransform = castor::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
				, shadows.transforms()[*m_directionalCascadeIndex] ) );

			IF( m_writer, receivesShadows != 0_u )
			{
				auto firstScale = m_writer.declLocale( "firstScale"
					, shadows.splitScales()[0][0] );
				auto filterScale = m_writer.declLocale( "filterScale"
					, abs( firstScale / shadows.splitScales()[*m_directionalCascadeIndex / 4u][*m_directionalCascadeIndex % 4u] ) );
				auto filterSize = m_writer.declLocale( "filterSize"
					, baseShadows.pcfFilterSize() );
				baseShadows.pcfFilterSize() = clamp( filterSize * filterScale
					, 1.0_f
					, sdw::Float{ float( MaxPcfFilterSize ) } );
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, cascadeFactors.y()
						* m_shadowModel.computeDirectional( baseShadows
							, lightSurface
							, *m_directionalTransform
							, *m_directionalCascadeIndex
							, shadows.cascadeCount() ) );

				IF( m_writer, cascadeFactors.z() > 0.0_f )
				{
					auto filterIndex = m_writer.declLocale( "filterIndex"
						, *m_directionalCascadeIndex + 1u );
					filterScale = abs( firstScale / shadows.splitScales()[filterIndex / 4u][filterIndex % 4u] );
					baseShadows.pcfFilterSize() = clamp( filterSize * filterScale
						, 1.0_f
						, sdw::Float{ float( MaxPcfFilterSize ) } );
					shadowFactor += cascadeFactors.z()
						* m_shadowModel.computeDirectional( baseShadows
							, lightSurface
							, shadows.transforms()[filterIndex]
							, filterIndex
							, shadows.cascadeCount() );
				}
				FI

				output.attenuate( shadowFactor, false, withDiffuse );
			}
			FI

#if C3D_DebugCascades
			auto cascadeColours = m_writer.declLocaleArray( "cascadeColours"
				, 6u
				, std::vector< sdw::Vec3 >{vec3( 1.0_f, 0.25f, 0.25f )
					, vec3( 0.25f, 1.0_f, 0.25f )
					, vec3( 0.25f, 0.25f, 1.0_f )
					, vec3( 1.0_f, 1.0f, 0.25f )
					, vec3( 1.0_f, 0.25f, 1.0f )
					, vec3( 0.25_f, 1.0f, 1.0f ) } );
			auto cascadeColour = m_writer.declLocale( "cascadeColour"
				, ( cascadeColours[*m_directionalCascadeIndex] * cascadeFactors.y() ) );

			IF( m_writer, cascadeFactors.z() > 0.0_f )
			{
				cascadeColour += cascadeColours[*m_directionalCascadeIndex + 1_u] * cascadeFactors.z();
			}
			FI

			output.diffuse().rgb() *= cascadeColour;
			output.specular().rgb() *= cascadeColour;
			output.coating().rgb() *= cascadeColour;

#endif
		}
		FI
	}

	void LightingModel::doApplyShadows( PointShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, DirectLighting & output
		, bool withDiffuse )
	{
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i
				&& receivesShadows != 0_u ) )
		{
			auto shadowFactor = m_writer.declLocale( "shadowFactor"
				, m_shadowModel.computePoint( baseShadows
					, shadowMapIndex
					, lightSurface
					, 1.0_f - ( lightSurface.lengthL().value() / lightRange ) ) );
			output.attenuate( shadowFactor, false, withDiffuse );
		}
		FI
	}

	void LightingModel::doApplyShadows( SpotShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, DirectLighting & output
		, bool withDiffuse )
	{
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i
				&& receivesShadows != 0_u ) )
		{
			auto shadowFactor = m_writer.declLocale( "shadowFactor"
				, m_shadowModel.computeSpot( baseShadows
					, shadowMapIndex
					, lightSurface
					, shadows.transform()
					, lightSurface.lengthL().value() / lightRange ) );
			output.attenuate( shadowFactor, false, withDiffuse );
		}
		FI
	}

	void LightingModel::doApplyShadowsDiffuse( DirectionalShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );
		m_directionalCascadeIndex = castor::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
			, 0_u ) );
		m_directionalCascadeCount = castor::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
			, 0_u ) );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i ) )
		{
			*m_directionalCascadeIndex = shadows.cascadeCount() - 1_u;
			*m_directionalCascadeCount = shadows.cascadeCount();
			m_directionalTransform = castor::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
				, shadows.transforms()[*m_directionalCascadeIndex] ) );

			IF( m_writer, receivesShadows != 0_u )
			{
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, m_shadowModel.computeDirectional( baseShadows
						, lightSurface
						, *m_directionalTransform
						, *m_directionalCascadeIndex
						, shadows.cascadeCount() ) );
				output *= shadowFactor;
			}
			FI
		}
		FI
	}

	void LightingModel::doApplyShadowsDiffuse( PointShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i
				&& receivesShadows != 0_u ) )
		{
			auto shadowFactor = m_writer.declLocale( "shadowFactor"
				, m_shadowModel.computePoint( baseShadows
					, shadowMapIndex
					, lightSurface
					, 1.0_f - ( lightSurface.lengthL().value() / lightRange ) ) );
			output *= shadowFactor;
		}
		FI
	}

	void LightingModel::doApplyShadowsDiffuse( SpotShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i
				&& receivesShadows != 0_u ) )
		{
			auto shadowFactor = m_writer.declLocale( "shadowFactor"
				, m_shadowModel.computeSpot( baseShadows
					, shadowMapIndex
					, lightSurface
					, shadows.transform()
					, lightSurface.lengthL().value() / lightRange ) );
			output *= shadowFactor;
		}
		FI
	}

	void LightingModel::doInitLightSpecifics( LightSurface const & lightSurface
		, BlendComponents const & components )
	{
	}

	DerivFloat LightingModel::doGetNdotL( LightSurface const & lightSurface
		, BlendComponents const & components )
	{
		return lightSurface.NdotL();
	}

	DerivFloat LightingModel::doGetNdotH( LightSurface const & lightSurface
		, BlendComponents const & components )
	{
		return lightSurface.NdotH();
	}

	void LightingModel::doComputeLight( Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & attenuation
		, sdw::Vec3 & radiance
		, DirectLighting & output )
	{
		radiance = m_scattering->computeRadiance( light, lightSurface.L().value() );
		doInitLightSpecifics( lightSurface, components );
		doInternalComputeLightDiffuse( light, components, lightSurface
			, attenuation, radiance
			, output.diffuse );
		auto lightIntensity = doInternalComputeLightSpecular( light, components, lightSurface
			, attenuation, radiance
			, output );
		doInternalComputeLayers( components, lightSurface, lightIntensity, output );
	}

	sdw::Vec3 LightingModel::doComputeLightDiffuse( Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & attenuation
		, sdw::Vec3 & radiance )
	{
		radiance = m_scattering->computeRadiance( light, lightSurface.L().value() );
		doInitLightSpecifics( lightSurface, components );
		auto result = m_writer.declLocale( "result", vec3( 0.0_f ) );
		doInternalComputeLightDiffuse( light, components, lightSurface
			, attenuation, radiance
			, result );
		return result;
	}

	void LightingModel::doComputeLightAllButDiffuse( Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & attenuation
		, sdw::Vec3 & radiance
		, DirectLighting & output )
	{
		radiance = m_scattering->computeRadiance( light, lightSurface.L().value() );
		doInitLightSpecifics( lightSurface, components );
		auto lightIntensity = doInternalComputeLightSpecular( light, components, lightSurface
			, attenuation, radiance
			, output );
		doInternalComputeLayers( components, lightSurface, lightIntensity, output );
	}
	
	void LightingModel::doInternalComputeLightDiffuse( Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & attenuation
		, sdw::Vec3 const & radiance
		, sdw::Vec3 & result )
	{
		auto lightIntensity = m_writer.declLocale( "diffuseLightIntensity"
			, attenuation * light.intensity().x() );
		result = m_diffuse->compute( components
			, lightSurface
			, radiance
			, lightIntensity
			, doGetNdotL( lightSurface, components ).value() );
		result *= doGetNdotL( lightSurface, components ).value() * components.baseColour;

		if ( components.hasMember( "diffuseTransmissionFactor" ) )
		{
			lightSurface.updateN( -lightSurface.N() );
			auto diffuseBtdf = m_writer.declLocale( "diffuseBtdf"
				, doGetNdotL( lightSurface, components ).value()
					* lightIntensity
					* m_diffuse->compute( components
						, lightSurface
						, components.diffuseTransmissionColour
						, light.intensity().x()
						, doGetNdotL( lightSurface, components ).value() ) );

			if ( components.hasMember( "thicknessFactor" )
				&& components.hasMember( "attenuationDistance" ) )
			{
				diffuseBtdf *= ReflectionModel::applyVolumeAttenuation( components.thicknessFactor
					, components.attenuationColour
					, components.attenuationDistance );
			}

			result = mix( result, diffuseBtdf, vec3( components.diffuseTransmissionFactor ) );
			lightSurface.updateN( -lightSurface.N() );
		}

		if ( components.hasMember( "transmissionFactor" ) )
		{
			// If the light ray travels through the geometry, use the point it exits the geometry again.
			// That will change the angle to the light source, if the material refracts the light ray.
			auto transmissionRay = m_writer.declLocale( "transmissionRay"
				, ReflectionModel::getVolumeTransmissionRay( lightSurface.N().value()
					, lightSurface.V().value()
					, components.thicknessFactor
					, components.ior ) );
			lightSurface.updateL( lightSurface.vertexToLight() - transmissionRay );

			auto transmissionRougness = m_writer.declLocale( "transmissionRougness"
				, ReflectionModel::applyIorToRoughness( components.alphaRoughness, components.ior ) );
			// Mirror light reflection vector on surface
			auto mirrorL = m_writer.declLocale( "mirrorL"
				, normalize( lightSurface.L().value() + 2.0_f * lightSurface.N().value() * dot( -lightSurface.L().value(), lightSurface.N().value() ) ) );
			// Halfway vector between transmission light vector and v
			auto mirrorH = m_writer.declLocale( "mirrorL"
				, normalize( mirrorL + lightSurface.V().value() ) );

			auto D = m_writer.declLocale( "D"
				, m_brdfHelpers.distributionGGX( clamp( dot( lightSurface.N().value(), mirrorH ), 0.0_f, 1.0_f )
					, transmissionRougness ) );
			auto F = m_writer.declLocale( "F"
				, m_utils.conductorFresnel( clamp( dot( lightSurface.V().value(), mirrorH ), 0.0_f, 1.0_f )
					, components.dielectricF0
					, components.f90 ) );
			auto Vis = m_writer.declLocale( "Vis"
				, m_brdfHelpers.visibilityGGX( lightSurface.NdotV().value()
					, clamp( dot( lightSurface.N().value(), mirrorL ), 0.0_f, 1.0_f )
					, transmissionRougness ) );

			// Transmission BTDF
			auto transmittedLight = m_writer.declLocale( "transmittedLight"
				, lightIntensity * ( 1.0_f - F ) * components.baseColour * D * Vis );

			if ( components.hasMember( "thicknessFactor" )
				&& components.hasMember( "attenuationDistance" ) )
			{
				transmittedLight *= ReflectionModel::applyVolumeAttenuation( length( transmissionRay )
					, components.attenuationColour
					, components.attenuationDistance );
			}

			result = mix( result, transmittedLight, vec3( components.transmissionFactor ) );

			lightSurface.updateL( lightSurface.vertexToLight() + transmissionRay );
		}
	}

	sdw::Vec3 LightingModel::doInternalComputeLightSpecular( Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & attenuation
		, sdw::Vec3 const & radiance
		, DirectLighting & output )
	{
		auto lightIntensity = m_writer.declLocale( "specularLightIntensity"
			, radiance * attenuation * light.intensity().y() );
		auto specular = m_writer.declLocale( "specular"
			, doGetNdotL( lightSurface, components ).value()
				* lightIntensity
				* m_specular->compute( components
					, lightSurface.N().value()
					, lightSurface.L().value()
					, lightSurface.H().value()
					, lightSurface.V().value()
					, doGetNdotL( lightSurface, components ).value()
					, doGetNdotH( lightSurface, components ).value() ) );
		auto dielectricFresnel = m_writer.declLocale( "dielectricFresnel"
			, m_utils.conductorFresnel( abs( lightSurface.HdotV().value() ), components.dielectricF0 * components.specularWeight, components.dielectricF90 ) );
		auto metalFresnel = m_writer.declLocale( "metalFresnel"
			, m_utils.conductorFresnel( abs( lightSurface.HdotV().value() ), components.baseColour, vec3( 1.0_f ) ) );
		output.metal = metalFresnel * specular;
		output.dielectric = mix( output.diffuse, specular, dielectricFresnel );
		return lightIntensity;
	}

	void LightingModel::doInternalComputeLayers( BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & lightIntensity
		, DirectLighting & output )
	{
		if ( m_clearcoat
			&& components.hasMember( "clearcoatFactor" ) )
		{
			lightSurface.updateN( derivVec3( components.clearcoatNormal ) );
			output.coating = m_clearcoat->compute( components
				, lightSurface.N().value()
				, lightSurface.L().value()
				, lightSurface.H().value()
				, lightSurface.V().value()
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );
			output.coating *= doGetNdotL( lightSurface, components ).value() * lightIntensity;
		}

		if ( m_sheen
			&& components.hasMember( "sheenColour" ) )
		{
			output.sheen = m_sheen->compute( m_utils
				, components
				, lightSurface
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );
			output.sheen.xyz() *= doGetNdotL( lightSurface, components ).value() * lightIntensity;
		}
	}

	//*********************************************************************************************
}
