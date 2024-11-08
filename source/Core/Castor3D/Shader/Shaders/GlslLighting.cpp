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

#define C3D_DebugCascades 0

namespace castor3d::shader
{
	//*********************************************************************************************

	LightingModel::LightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdfHelpers
		, DiffuseBRDFPtr diffuse
		, SpecularBRDFPtr specular
		, SheenBRDFPtr sheen
		, ClearcoatBRDFPtr clearcoat
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
		, m_diffuse{ std::move( diffuse ) }
		, m_specular{ std::move( specular ) }
		, m_sheen{ std::move( sheen ) }
		, m_clearcoat{ std::move( clearcoat ) }
		, m_hasBackgroundReflectionsSupport{ hasBackgroundReflectionsSupport }
		, m_hasBackgroundRefractionSupport{ hasBackgroundRefractionSupport }
		, m_hasIblSupport{ hasIblSupport }
		, m_enableVolumetric{ enableVolumetric }
		, m_prefix{ castor::move( prefix ) }
	{
	}

	void LightingModel::finish( PassShaders const & passShaders
		, DerivSurfaceBase const & surface
		, Utils & utils
		, sdw::Vec3 const worldEye
		, BlendComponents & components )
	{
		components.finish( passShaders
			, surface
			, utils
			, worldEye );
		doFinish( passShaders
			, components );
	}

	sdw::Vec3 LightingModel::combine( DebugOutput & debugOutput
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & incident
		, DirectLighting directLighting
		, IndirectLighting indirectLighting
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, ReflectionRefraction reflRefr )
	{
		auto maxSheenColour = m_writer.declLocale( "maxSheenColour"
			, max( max( components.sheenColour.r(), components.sheenColour.g() ), components.sheenColour.b() ) );

		IF( m_writer, maxSheenColour != 0.0_f )
		{
			auto albedoSheenScaling = m_writer.declLocale( "albedoSheenScaling"
				, 1.0_f - reflRefr.reflSheen.w() * maxSheenColour );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Albedo Sheen Scaling" ), albedoSheenScaling );
			reflRefr.reflDiffuse *= albedoSheenScaling;
			reflRefr.reflSpecular *= albedoSheenScaling;
		}
		ELSE
		{
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Albedo Sheen Scaling" ), 1.0_f );
		}
		FI

		auto combineResult = combine( debugOutput
			, components
			, lightSurface
			, incident
			, directLighting
			, std::move( indirectLighting )
			, ambientOcclusion
			, emissive
			, castor::move( reflRefr.reflDiffuse )
			, castor::move( reflRefr.reflSpecular )
			, castor::move( reflRefr.refrColour ) );

		IF( m_writer, !all( components.sheenColour == vec3( 0.0_f ) ) )
		{
			combineResult += directLighting.sheen.xyz();
			combineResult += ( reflRefr.reflSheen.xyz() * ambientOcclusion );
		}
		FI

		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			auto clearcoatNdotV = m_writer.declLocale( "clearcoatNdotV"
				, max( dot( components.clearcoatNormal, -incident ), 0.0_f ) );
			auto clearcoatFresnel = m_writer.declLocale( "clearcoatFresnel"
				, pow( 0.04_f + ( 1.0_f - 0.04_f ) * ( 1.0_f - clearcoatNdotV ), 5.0_f ) );
			combineResult = combineResult * ( 1.0_f - vec3( components.clearcoatFactor * clearcoatFresnel ) )
				+ ( reflRefr.reflCoating * ambientOcclusion ) + directLighting.coating;
		}
		FI

		return combineResult + directLighting.scattering;
	}

	sdw::Vec3 LightingModel::combine( DebugOutput & debugOutput
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & incident
		, DirectLighting directLighting
		, IndirectLighting indirectLighting
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 reflectedDiffuse
		, sdw::Vec3 reflectedSpecular
		, sdw::Vec3 refracted )
	{
		auto fresnelFactor = m_writer.hasVariable( "fresnelFactor", true )
			? m_writer.getVariable< sdw::Float >( "fresnelFactor"
				, ( components.hasMember( "specularFactor" )
					|| components.hasMember( "refractionRatio" )
					|| components.hasMember( "hasRefraction" )
					|| components.hasMember( "hasTransmission" ) ) )
			: m_writer.declLocale( "fresnelFactor"
				, m_utils.fresnelMix( incident
					, components.getRawNormal()
					, components.refractionRatio )
				, ( components.hasMember( "specularFactor" )
					|| components.hasMember( "refractionRatio" )
					|| components.hasMember( "hasRefraction" )
					|| components.hasMember( "hasTransmission" ) ) );

		if ( fresnelFactor.isEnabled() )
		{
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Fresnel Factor" ), fresnelFactor );

			IF( m_writer, components.refractionRatio != 0.0_f
				&& components.hasRefraction != 0_u
				&& components.hasTransmission == 0_u )
			{
				reflectedDiffuse = mix( vec3( 0.0_f )
					, reflectedDiffuse
					, vec3( fresnelFactor ) );
				reflectedSpecular = mix( vec3( 0.0_f )
					, reflectedSpecular
					, vec3( fresnelFactor ) );
				refracted = mix( refracted
					, vec3( 0.0_f )
					, vec3( fresnelFactor ) );
			}
			FI
		}

		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Final Ambient" ), directLighting.ambient );
		 // Fresnel already included in both diffuse and specular.
		auto diffuseResult = m_writer.declLocale( "c3d_diffuseResult"
			, doGetDiffuseResult( components
				, directLighting, indirectLighting
				, ambientOcclusion
				, reflectedDiffuse ) );
		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Diffuse Result" ), diffuseResult );
		auto specularResult = m_writer.declLocale( "c3d_specularResult"
			, doGetSpecularResult( components
				, directLighting, indirectLighting
				, ambientOcclusion
				, reflectedSpecular ) );
		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Specular Result" ), specularResult );

		IF( m_writer, components.hasTransmission )
		{
			if ( components.hasMember( "metalness" ) )
			{
				refracted *= 1.0_f - components.metalness;
			}

			auto specularBtdf = m_writer.declLocale( "c3d_specularBtdf"
				, refracted );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Specular BTDF" ), specularBtdf );
			diffuseResult = mix( diffuseResult, specularBtdf, vec3( components.transmission ) );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Transmission" ), components.transmission );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Transmission Result" ), diffuseResult );
		}
		ELSE
		{
			diffuseResult += refracted;
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Specular BTDF" ), 0.0_f );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Transmission" ), 0.0_f );
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Transmission Result" ), 0.0_f );
		}
		FI

		if ( components.hasMember( "specularFactor" ) )
		{
			auto specularFactor = m_writer.declLocale( "c3d_specularFactor"
				, clamp( components.getMember< sdw::Float >( "specularFactor" ), 0.0_f, 1.0_f ) );
			specularResult *= specularFactor * fresnelFactor;
			diffuseResult *= 1.0_f - specularFactor * fresnelFactor;
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Specular Factor" ), specularFactor );
		}
		else
		{
			debugOutput.registerOutput( cuT( "Combine" ), cuT( "Specular Factor" ), 0.0_f );
		}

		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Final Specular" ), specularResult );
		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Final Diffuse" ), diffuseResult );
		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Emissive" ), emissive );
		auto combineResult = m_writer.declLocale( "c3d_combineResult"
			, emissive + specularResult + diffuseResult );
		debugOutput.registerOutput( cuT( "Combine" ), cuT( "Combine Result" ), combineResult );
		return combineResult;
	}

	void LightingModel::compute( DebugOutput & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeDirectional )
		{
			doInitialiseBackground( background );
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
					lightSurface.updateLAndF( m_utils
						, derivVec3( -light.direction() )
						, components.f0
						, components );
					doComputeLight( light.base()
						, components
						, lightSurface
						, radiance
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

					doComputeScatteringTerm( shadows.base()
						, light.shadowMapIndex()
						, radiance
						, light.base().intensity()
						, components
						, lightSurface
						, output.scattering );
					output.sheenAlbedoScale( components );
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

	void LightingModel::compute( DebugOutput & debugOutput
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
					lightSurface.updateLAndF( m_utils
						, derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() )
						, components.f0
						, components );
					doComputeLight( light.base()
						, components
						, lightSurface
						, radiance
						, output );
					auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
					light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
								parentOutput.diffuse += ( output.diffuse * attenuation )
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

					output.attenuate( attenuation, true );
					output.sheenAlbedoScale( components );
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

	void LightingModel::compute( DebugOutput & debugOutput
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
					lightSurface.updateLAndF( m_utils
						, derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() )
						, components.f0
						, components );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto output = m_writer.declLocale( "output"
							, DirectLighting{ m_writer } );
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						doComputeLight( light.base()
							, components
							, lightSurface
							, radiance
							, output );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						output *= spotFactor;
						auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
						light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
									parentOutput.diffuse += ( output.diffuse * attenuation )
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

						output.attenuate( attenuation, true );
						output.sheenAlbedoScale( components );
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

	sdw::Vec3 LightingModel::computeDiffuse( DebugOutput & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			doInitialiseBackground( background );
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( castor::toUtf8( m_prefix ) + "computeDirectionalLightDiffuse"
				, [this, &debugOutput]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( derivVec3( -light.direction() ) );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					auto diffuse = doComputeLightDiffuse( light.base()
						, components
						, lightSurface
						, radiance );

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

	sdw::Vec3 LightingModel::computeDiffuse( DebugOutput & debugOutput
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
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					auto diffuse = doComputeLightDiffuse( light.base()
						, components
						, lightSurface
						, radiance );
					auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
					light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
								diffuse += ( diffuse * attenuation )
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

					diffuse *= attenuation;
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
	
	sdw::Vec3 LightingModel::computeDiffuse( DebugOutput & debugOutput
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
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						diffuse = doComputeLightDiffuse( light.base()
							, components
							, lightSurface
							, radiance );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						diffuse = spotFactor * diffuse;
						auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
						light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
									diffuse += ( diffuse * attenuation )
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

						diffuse *= attenuation;
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

	void LightingModel::computeAllButDiffuse( DebugOutput & debugOutput
		, DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, DirectLighting & pparentOutput )
	{
		if ( !m_computeDirectionalAllButDiffuse )
		{
			doInitialiseBackground( background );
			m_computeDirectionalAllButDiffuse = m_writer.implementFunction< sdw::Void >( castor::toUtf8( m_prefix ) + "computeDirectionalLightAllButDiffuse"
				, [this, &debugOutput]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface const & lightSurface
					, sdw::UInt const & receivesShadows
					, DirectLighting parentOutput )
				{
					auto output = m_writer.declLocale< DirectLighting >( "output"
						, DirectLighting{ m_writer } );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateLAndF( m_utils
						, derivVec3( -light.direction() )
						, components.f0
						, components );
					doComputeLightAllButDiffuse( light.base()
						, components
						, lightSurface
						, radiance
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
								parentOutput.diffuse += m_lights.computeSssTransmittance(debugOutput
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

					doComputeScatteringTerm( shadows.base()
						, light.shadowMapIndex()
						, radiance
						, light.base().intensity()
						, components
						, lightSurface
						, output.scattering );
					output.sheenAlbedoScale( components, false );
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

	void LightingModel::computeAllButDiffuse( DebugOutput & debugOutput
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
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateLAndF( m_utils
						, derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() )
						, components.f0
						, components );
					doComputeLightAllButDiffuse( light.base()
						, components
						, lightSurface
						, radiance
						, output );
					auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
					light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
								parentOutput.diffuse += attenuation
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

					output.attenuate( attenuation, true, false );
					output.sheenAlbedoScale( components, false );
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

	void LightingModel::computeAllButDiffuse( DebugOutput & debugOutput
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
					lightSurface.updateLAndF( m_utils
						, derivVec3( light.position() ) - getXYZ( lightSurface.worldPosition() )
						, components.f0
						, components );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L().value(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						auto output = m_writer.declLocale< DirectLighting >( "output"
							, DirectLighting{ m_writer } );
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						doComputeLightAllButDiffuse( light.base()
							, components
							, lightSurface
							, radiance
							, output );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						output *= spotFactor;
						auto attenuation = m_writer.declLocale( "attenuation", 1.0_f );
						light.getAttenuationFactor( lightSurface.lengthL().value(), attenuation );

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
									parentOutput.diffuse += attenuation
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

						output.attenuate( attenuation, true, false );
						output.sheenAlbedoScale( components, false );
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

	void LightingModel::doApplyVolumetric( ShadowData const & shadows
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

	void LightingModel::doInitialiseBackground( BackgroundModel & pbackground )
	{
	}

	sdw::Vec3 LightingModel::doComputeRadiance( Light const & light
		, sdw::Vec3 const & lightDirection )const
	{
		return light.colour();
	}

	void LightingModel::doComputeScatteringTerm( ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Vec2 const & lightIntensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 output )
	{
		doApplyVolumetric( shadows
			, shadowMapIndex
			, lightIntensity
			, lightSurface
			, output
			, false /*multiply*/ );
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

	void LightingModel::doComputeLight( Light light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & radiance
		, DirectLighting & output )
	{
		radiance = doComputeRadiance( light, lightSurface.L().value() );
		doInitLightSpecifics( lightSurface, components );
		output.diffuse = doGetNdotL( lightSurface, components ).value()
			* m_diffuse->compute( components
				, lightSurface
				, radiance
				, light.intensity().x()
				, doGetNdotL( lightSurface, components ).value() );
		output.specular = doGetNdotL( lightSurface, components ).value()
			* m_specular->compute( components
				, lightSurface
				, radiance
				, light.intensity().y()
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );

		if ( m_sheen )
		{
			IF( m_writer, !all( components.sheenColour == vec3( 0.0_f ) ) )
			{
				output.sheen = m_sheen->compute( m_utils
					, components
					, lightSurface
					, doGetNdotL( lightSurface, components ).value()
					, doGetNdotH( lightSurface, components ).value() );
			}
			FI
		}

		IF( m_writer, components.hasTransmission != 0_u
			&& components.thicknessFactor != 0.0_f
			&& components.refractionRatio != 0.0_f )
		{
			// If the light ray travels through the geometry, use the point it exits the geometry again.
			// That will change the angle to the light source, if the material refracts the light ray.
			auto transmissionRay = m_writer.declLocale( "transmissionRay"
				, ReflectionModel::getVolumeTransmissionRay( lightSurface.N().value()
					, lightSurface.V().value()
					, components.thicknessFactor
					, components.refractionRatio ) );
			lightSurface.updateL( lightSurface.vertexToLight() - transmissionRay );

			auto transmittedLight = m_writer.declLocale( "transmittedLight"
				, light.intensity().x() * doComputeLightTransmission( components, lightSurface ) );
			transmittedLight = ReflectionModel::applyVolumeAttenuation( transmittedLight, length( transmissionRay ), components.attenuationColour, components.attenuationDistance );

			output.diffuse = mix( output.diffuse, transmittedLight, vec3( components.transmission ) );

			lightSurface.updateL( lightSurface.vertexToLight() + transmissionRay );
		}
		FI

		if ( m_clearcoat )
		{
			IF( m_writer, components.clearcoatFactor != 0.0_f )
			{
				lightSurface.updateN( derivVec3( components.clearcoatNormal ) );
				output.coating = m_clearcoat->compute( components
					, lightSurface
					, radiance
					, light.intensity().y()
					, doGetNdotL( lightSurface, components ).value()
					, doGetNdotH( lightSurface, components ).value() );
			}
			FI
		}
	}
	
	sdw::Vec3 LightingModel::doComputeLightTransmission( BlendComponents const & components
		, LightSurface const & lightSurface )
	{
		auto transmissionRougness = m_writer.declLocale( "transmissionRougness"
			, ReflectionModel::applyIorToRoughness( components.roughness * components.roughness
				, components.refractionRatio ) );
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
				, components.f0
				, components.f90 ) );
		auto Vis = m_writer.declLocale( "Vis"
			, m_brdfHelpers.visibilitySmithGGXCorrelated( clamp( dot( lightSurface.N().value(), lightSurface.V().value() ), 0.0_f, 1.0_f )
				, clamp( dot( lightSurface.N().value(), mirrorL ), 0.0_f, 1.0_f )
				, transmissionRougness ) );

		// Transmission BTDF
		return ( 1.0_f - F ) * components.colour * D * Vis;
	}
	
	sdw::Vec3 LightingModel::doComputeLightDiffuse( Light light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & radiance )
	{
		auto result = m_writer.declLocale( "result", vec3( 0.0_f ) );
		radiance = doComputeRadiance( light, lightSurface.L().value() );
		doInitLightSpecifics( lightSurface, components );
		result = doGetNdotL( lightSurface, components ).value()
			* m_diffuse->compute( components
				, lightSurface
				, radiance
				, light.intensity().x()
				, doGetNdotL( lightSurface, components ).value() );
		return result;
	}

	void LightingModel::doComputeLightAllButDiffuse( Light light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & radiance
		, DirectLighting & output )
	{
		radiance = doComputeRadiance( light, lightSurface.L().value() );
		auto isLit = m_writer.declLocale( "isLit", 0.0_f );
		doInitLightSpecifics( lightSurface, components );
		output.specular = doGetNdotL( lightSurface, components ).value()
			* m_specular->compute( components
				, lightSurface
				, radiance
				, light.intensity().y()
				, doGetNdotL( lightSurface, components ).value()
				, doGetNdotH( lightSurface, components ).value() );
		if ( m_sheen )
		{
			IF( m_writer, !all( components.sheenColour == vec3( 0.0_f ) ) )
			{
				output.sheen = doGetNdotL( lightSurface, components ).value()
					* m_sheen->compute( m_utils
						, components
						, lightSurface
						, doGetNdotL( lightSurface, components ).value()
						, doGetNdotH( lightSurface, components ).value() );
			}
			FI
		}
		if ( m_clearcoat )
		{
			IF( m_writer, components.clearcoatFactor != 0.0_f )
			{
				lightSurface.updateN( derivVec3( components.clearcoatNormal ) );
				output.coating = doGetNdotL( lightSurface, components ).value()
					* m_clearcoat->compute( components
						, lightSurface
						, radiance
						, light.intensity().y()
						, doGetNdotL( lightSurface, components ).value()
						, doGetNdotH( lightSurface, components ).value() );
			}
			FI
		}
	}

	//*********************************************************************************************
}
