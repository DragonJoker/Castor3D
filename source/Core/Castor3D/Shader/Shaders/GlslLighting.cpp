#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderAST/Expr/ExprComma.hpp>
#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d::shader, LightingModel )

namespace castor3d::shader
{
	//*********************************************************************************************

	LightingModel::LightingModel( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool hasIblSupport
		, bool enableVolumetric
		, std::string prefix )
		: m_lightingModelId{ lightingModelId }
		, m_writer{ writer }
		, m_materials{ materials }
		, m_utils{ utils }
		, m_shadowModel{ shadowModel }
		, m_lights{ lights }
		, m_hasIblSupport{ hasIblSupport }
		, m_enableVolumetric{ enableVolumetric }
		, m_prefix{ std::move( prefix ) }
	{
	}

	void LightingModel::finish( PassShaders const & passShaders
		, SurfaceBase const & surface
		, Utils & utils
		, sdw::Vec3 const worldEye
		, BlendComponents & components )
	{
		components.finish( passShaders
			, surface
			, utils
			, worldEye );
		doFinish( components );
	}

	sdw::Vec3 LightingModel::combine( DebugOutput & debugOutput
		, BlendComponents const & components
		, sdw::Vec3 const & incident
		, sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & directScattering
		, sdw::Vec3 const & directCoatingSpecular
		, sdw::Vec2 const & directSheen
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 reflectedDiffuse
		, sdw::Vec3 reflectedSpecular
		, sdw::Vec3 refracted
		, sdw::Vec3 coatReflected
		, sdw::Vec3 sheenReflected )
	{
		auto combineResult = combine( debugOutput
			, components
			, incident
			, directDiffuse
			, indirectDiffuse
			, directSpecular
			, indirectSpecular
			, directAmbient
			, indirectAmbient
			, ambientOcclusion
			, emissive
			, std::move( reflectedDiffuse )
			, std::move( reflectedSpecular )
			, std::move( refracted ) );

		IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
		{
			combineResult = ( sheenReflected * ambientOcclusion )
				+ ( components.colour * directSheen.x() )
				+ combineResult * directSheen.y() * max( max( components.colour.r(), components.colour.g() ), components.colour.b() );
		}
		FI;

		IF( m_writer, components.clearcoatFactor != 0.0_f )
		{
			auto clearcoatNdotV = m_writer.declLocale( "clearcoatNdotV"
				, max( dot( components.clearcoatNormal, -incident ), 0.0_f ) );
			auto clearcoatFresnel = m_writer.declLocale( "clearcoatFresnel"
				, pow( 0.04_f + ( 1.0_f - 0.04_f ) * ( 1.0_f - clearcoatNdotV ), 5.0_f ) );
			combineResult = combineResult * ( 1.0_f - vec3( components.clearcoatFactor * clearcoatFresnel ) )
				+ ( coatReflected * ambientOcclusion ) + directCoatingSpecular;
		}
		FI;

		return combineResult + directScattering;
	}

	sdw::Vec3 LightingModel::combine( DebugOutput & debugOutput
		, BlendComponents const & components
		, sdw::Vec3 const & incident
		, sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & directAmbient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 reflectedDiffuse
		, sdw::Vec3 reflectedSpecular
		, sdw::Vec3 refracted )
	{
		IF( m_writer, components.refractionRatio != 0.0_f
			&& components.hasRefraction != 0_u
			&& components.hasTransmission == 0_u )
		{
			auto fresnelFactor = m_writer.declLocale( "fresnelFactor"
				, m_utils.fresnelMix( incident
					, components.normal
					, components.refractionRatio ) );
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
		FI;

		auto finalAmbient = m_writer.declLocale( "c3d_directAmbient"
			, adjustDirectAmbient( components, directAmbient ) );
		debugOutput.registerOutput( "Final Ambient", finalAmbient );
		 // Fresnel already included in both diffuse and specular.
		auto diffuseBrdf = m_writer.declLocale( "c3d_diffuseBrdf"
			, doGetDiffuseBrdf( components
				, directDiffuse, indirectDiffuse
				, finalAmbient, indirectAmbient, ambientOcclusion
				, reflectedDiffuse ) );
		debugOutput.registerOutput( "Diffuse BRDF", diffuseBrdf );
		auto specularBrdf = m_writer.declLocale( "c3d_specularBrdf"
			, doGetSpecularBrdf( components
				, directSpecular, indirectSpecular
				, finalAmbient, indirectAmbient, ambientOcclusion
				, reflectedSpecular ) );
		debugOutput.registerOutput( "Specular BRDF", specularBrdf );

		IF( m_writer, components.hasTransmission )
		{
			auto specularBtdf = m_writer.declLocale( "c3d_specularBtdf"
				, adjustRefraction( components, refracted ) );
			debugOutput.registerOutput( "Specular BTDF", specularBtdf );
			diffuseBrdf = mix( diffuseBrdf, specularBtdf, vec3( components.transmission ) );
			debugOutput.registerOutput( "Transmission", components.transmission );
			debugOutput.registerOutput( "Transmission BRDF", diffuseBrdf );
		}
		ELSE
		{
			diffuseBrdf += refracted;
			debugOutput.registerOutput( "Specular BTDF", 0.0_f );
			debugOutput.registerOutput( "Transmission BRDF", 0.0_f );
			debugOutput.registerOutput( "Transmission", 0.0_f );
		}
		FI;

		debugOutput.registerOutput( "Emissive", emissive );
		auto combineResult = m_writer.declLocale( "c3d_combineResult"
			, emissive + specularBrdf + diffuseBrdf );
		debugOutput.registerOutput( "Combine Result", combineResult );
		return combineResult;
	}

	void LightingModel::compute( DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeDirectional )
		{
			doInitialiseBackground( background );
			OutputComponents outputs{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( m_utils
						, -light.direction()
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
					parentOutput.diffuse += max( vec3( 0.0_f ), output.diffuse );
					parentOutput.specular += max( vec3( 0.0_f ), output.specular );
					parentOutput.scattering += max( vec3( 0.0_f ), output.scattering );
					parentOutput.coatingSpecular += max( vec3( 0.0_f ), output.coatingSpecular );
					parentOutput.sheen += max( vec2( 0.0_f ), output.sheen );
				}
				, PDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeDirectional( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::compute( PointLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computePoint )
		{
			OutputComponents outputs{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
				, [this]( PointLight light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					lightSurface.updateL( m_utils
						, light.position() - lightSurface.worldPosition()
						, components.f0
						, components );
					doComputeLight( light.base()
						, components
						, lightSurface
						, radiance
						, output );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getPointShadows( light.shadowMapIndex() ) );
						doApplyShadows( shadows
							, light.shadowMapIndex()
							, light.radius()
							, lightSurface
							, radiance
							, receivesShadows
							, output );
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( lightSurface.lengthL() ) );
					doComputeAttenuation( attenuation, output );
					parentOutput.diffuse += max( vec3( 0.0_f ), output.diffuse );
					parentOutput.specular += max( vec3( 0.0_f ), output.specular );
					parentOutput.scattering += max( vec3( 0.0_f ), output.scattering );
					parentOutput.coatingSpecular += max( vec3( 0.0_f ), output.coatingSpecular );
					parentOutput.sheen += max( vec2( 0.0_f ), output.sheen );
				}
				, PPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computePoint( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	void LightingModel::compute( SpotLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			OutputComponents outputs{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
				, [this]( SpotLight light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					lightSurface.updateL( m_utils
						, light.position() - lightSurface.worldPosition()
						, components.f0
						, components );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOffCos() )
					{
						OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						auto rawDiffuse = doComputeLight( light.base()
							, components
							, lightSurface
							, radiance
							, output );
						spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
						output.diffuse = spotFactor * output.diffuse;
						output.specular = spotFactor * output.specular;
						output.scattering = spotFactor * output.scattering;
						output.coatingSpecular = spotFactor * output.coatingSpecular;
						output.sheen.x() = spotFactor * output.sheen.x();
						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( lightSurface.lengthL() ) );

						if ( m_shadowModel.isEnabled() )
						{
							auto shadows = m_writer.declLocale( "shadows"
								, m_shadowModel.getSpotShadows( light.shadowMapIndex() ) );

#if !C3D_DisableSSSTransmittance
							if ( m_sssTransmittance )
							{
								IF( m_writer
									, ( light.shadowMapIndex() >= 0_i )
									&& ( shadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) ) )
									&& ( receivesShadows != 0_u )
									&& ( material.sssProfileIndex != 0.0_f ) )
								{
									parentOutput.diffuse += ( output.diffuse / attenuation )
										* m_sssTransmittance->compute( material
											, light
											, surface );
								}
								FI;
							}
#endif

							doApplyShadows( shadows
								, light.shadowMapIndex()
								, light.radius()
								, lightSurface
								, radiance
								, receivesShadows
								, output );
						}

						doComputeAttenuation( attenuation, output );
						parentOutput.diffuse += max( vec3( 0.0_f ), output.diffuse );
						parentOutput.specular += max( vec3( 0.0_f ), output.specular );
						parentOutput.scattering += max( vec3( 0.0_f ), output.scattering );
						parentOutput.coatingSpecular += max( vec3( 0.0_f ), output.coatingSpecular );
						parentOutput.sheen += max( vec2( 0.0_f ), output.sheen );
					}
					FI;
				}
				, PSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InLightSurface{ m_writer, "lightSurface", plightSurface.getType() }
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeSpot( plight
			, pcomponents
			, plightSurface
			, preceivesShadows
			, pparentOutput );
	}

	sdw::Vec3 LightingModel::computeDiffuse( DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, BackgroundModel & background
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			doInitialiseBackground( background );
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( -light.direction() );
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

	sdw::Vec3 LightingModel::computeDiffuse( PointLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( PointLight light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( light.position() - lightSurface.worldPosition() );
					auto radiance = m_writer.declLocale( "radiance"
						, vec3( 0.0_f ) );
					auto diffuse = doComputeLightDiffuse( light.base()
						, components
						, lightSurface
						, radiance );

					if ( m_shadowModel.isEnabled() )
					{
						auto shadows = m_writer.declLocale( "shadows"
							, m_shadowModel.getPointShadows( light.shadowMapIndex() ) );
						doApplyShadowsDiffuse( shadows
							, light.shadowMapIndex()
							, light.radius()
							, lightSurface
							, radiance
							, receivesShadows
							, diffuse );
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( lightSurface.lengthL() ) );
					diffuse /= attenuation;
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
	
	sdw::Vec3 LightingModel::computeDiffuse( SpotLight const & plight
		, BlendComponents const & pcomponents
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( SpotLight light
					, BlendComponents const & components
					, LightSurface lightSurface
					, sdw::UInt const & receivesShadows )
				{
					lightSurface.updateL( light.position() - lightSurface.worldPosition() );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightSurface.L(), light.direction() ) );
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

						if ( m_shadowModel.isEnabled() )
						{
							auto shadows = m_writer.declLocale( "shadows"
								, m_shadowModel.getSpotShadows( light.shadowMapIndex() ) );
							doApplyShadowsDiffuse( shadows
								, light.shadowMapIndex()
								, light.radius()
								, lightSurface
								, radiance
								, receivesShadows
								, diffuse );
						}

						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( lightSurface.lengthL() ) );
						diffuse /= attenuation;
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

	void LightingModel::doComputeAttenuation( sdw::Float const attenuation
		, OutputComponents & output )
	{
		output.diffuse = output.diffuse / attenuation;
		output.specular = output.specular / attenuation;
		output.scattering = output.scattering / attenuation;
		output.coatingSpecular = output.coatingSpecular / attenuation;
		output.sheen.x() = output.sheen.x() / attenuation;
	}

	void LightingModel::doApplyShadows( DirectionalShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec2 const & lightIntensity
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		m_directionalCascadeIndex = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
			, 0_u ) );
		m_directionalCascadeCount = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
			, 0_u ) );
		auto baseShadows = m_writer.declLocale( "baseShadows"
			, shadows.base() );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i ) )
		{
			auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
				, m_lights.getCascadeFactors( shadows
					, lightSurface.viewPosition()
					, m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" ) ) );
			*m_directionalCascadeIndex = m_writer.cast< sdw::UInt >( cascadeFactors.x() );
			*m_directionalCascadeCount = shadows.cascadeCount();
			m_directionalTransform = std::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
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
						, *m_directionalCascadeIndex - 1u );
					filterScale = abs( firstScale / shadows.splitScales()[filterIndex / 4u][filterIndex % 4u] );
					baseShadows.pcfFilterSize() = clamp( filterSize * filterScale
						, 1.0_f
						, 64.0_f );
					shadowFactor += cascadeFactors.z()
						* m_shadowModel.computeDirectional( baseShadows
							, lightSurface
							, shadows.transforms()[*m_directionalCascadeIndex - 1u]
							, *m_directionalCascadeIndex - 1u
							, shadows.cascadeCount() );
				}
				FI;

				output.diffuse *= shadowFactor;
				output.specular *= shadowFactor;
				output.coatingSpecular *= shadowFactor;
				output.sheen.x() *= shadowFactor;
			}
			FI;

#if C3D_DebugCascades
			IF( m_writer, cascadeIndex == 0_u )
			{
				output.m_diffuse.rgb() *= vec3( 1.0_f, 0.25f, 0.25f );
				output.m_specular.rgb() *= vec3( 1.0_f, 0.25f, 0.25f );
				output.m_coatingSpecular.rgb() *= vec3( 1.0_f, 0.25f, 0.25f );
			}
			ELSEIF( cascadeIndex == 1_u )
			{
				output.m_diffuse.rgb() *= vec3( 0.25_f, 1.0f, 0.25f );
				output.m_specular.rgb() *= vec3( 0.25_f, 1.0f, 0.25f );
				output.m_coatingSpecular.rgb() *= vec3( 0.25_f, 1.0f, 0.25f );
			}
			ELSEIF( cascadeIndex == 2_u )
			{
				output.m_diffuse.rgb() *= vec3( 0.25_f, 0.25f, 1.0f );
				output.m_specular.rgb() *= vec3( 0.25_f, 0.25f, 1.0f );
				output.m_coatingSpecular.rgb() *= vec3( 0.25_f, 0.25f, 1.0f );
			}
			ELSE
			{
				output.m_diffuse.rgb() *= vec3( 1.0_f, 1.0f, 0.25f );
				output.m_specular.rgb() *= vec3( 1.0_f, 1.0f, 0.25f );
				output.m_coatingSpecular.rgb() *= vec3( 1.0_f, 1.0f, 0.25f );
			}
			FI;
#endif
		}
		FI;
	}

	void LightingModel::doApplyShadows( PointShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
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
					, 1.0_f - ( length( -lightSurface.vertexToLight() ) / lightRange ) ) );
			output.diffuse *= shadowFactor;
			output.specular *= shadowFactor;
			output.coatingSpecular *= shadowFactor;
			output.sheen.x() *= shadowFactor;
		}
		FI;
	}

	void LightingModel::doApplyShadows( SpotShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Float const lightRange
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
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
					, length( -lightSurface.vertexToLight() ) / lightRange ) );
			output.diffuse *= shadowFactor;
			output.specular *= shadowFactor;
			output.coatingSpecular *= shadowFactor;
			output.sheen.x() *= shadowFactor;
		}
		FI;
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
		m_directionalCascadeIndex = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
			, 0_u ) );
		m_directionalCascadeCount = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
			, 0_u ) );

		IF( m_writer
			, ( baseShadows.shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& shadowMapIndex >= 0_i ) )
		{
			*m_directionalCascadeIndex = shadows.cascadeCount() - 1_u;
			*m_directionalCascadeCount = shadows.cascadeCount();
			m_directionalTransform = std::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
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
			FI;
		}
		FI;
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
					, 1.0_f - ( length( -lightSurface.vertexToLight() ) / lightRange ) ) );
			output *= shadowFactor;
		}
		FI;
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
					, length( -lightSurface.vertexToLight() ) / lightRange ) );
			output *= shadowFactor;
		}
		FI;
	}

	void LightingModel::doApplyVolumetric( ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec2 const & lightIntensity
		, LightSurface const & lightSurface
		, sdw::Vec3 & output
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
			FI;
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

	void LightingModel::doComputeSheenTerm( sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Float const & isLit
		, sdw::Vec2 & output )
	{
	}

	void LightingModel::doComputeScatteringTerm( ShadowData const & shadows
		, sdw::Int const shadowMapIndex
		, sdw::Vec3 const & radiance
		, sdw::Vec2 const & lightIntensity
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & output )
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

	sdw::Float LightingModel::doGetNdotL( LightSurface const & lightSurface
		, BlendComponents const & components )
	{
		return lightSurface.NdotL();
	}

	sdw::Float LightingModel::doGetNdotH( LightSurface const & lightSurface
		, BlendComponents const & components )
	{
		return lightSurface.NdotH();
	}

	sdw::Vec3 LightingModel::doComputeLight( Light light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & radiance
		, OutputComponents & output )
	{
		radiance = doComputeRadiance( light, lightSurface.L() );
		auto isLit = m_writer.declLocale( "isLit", 0.0_f );
		doInitLightSpecifics( lightSurface, components );
		auto rawDiffuse = doComputeDiffuseTerm( radiance
			, light.intensity().x()
			, components
			, lightSurface
			, isLit
			, output.diffuse );
		doComputeSpecularTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.specular );
		doComputeSheenTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.sheen );
		doComputeCoatingTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.coatingSpecular );
		return rawDiffuse;
	}
	
	sdw::Vec3 LightingModel::doComputeLightDiffuse( Light light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & radiance )
	{
		auto isLit = m_writer.declLocale( "isLit", 0.0_f );
		auto result = m_writer.declLocale( "result", vec3( 0.0_f ) );
		radiance = doComputeRadiance( light, lightSurface.L() );
		doInitLightSpecifics( lightSurface, components );
		doComputeDiffuseTerm( radiance
			, light.intensity().x()
			, components
			, lightSurface
			, isLit
			, result );
		return result;
	}

	//*********************************************************************************************
}
