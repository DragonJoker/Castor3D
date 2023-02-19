#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
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

CU_ImplementCUSmartPtr( castor3d::shader, LightingModel )

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

	sdw::Vec3 LightingModel::combine( BlendComponents const & components
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
		IF( m_writer, components.refractionRatio != 0.0_f
			&& components.hasTransmission == 0_u )
		{
			auto fresnelFactor = m_writer.declLocale( "fresnelFactor"
				, m_utils.fresnelMix( incident
					, components.normal
					, components.roughness
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

		return doCombine( components
			, incident
			, directDiffuse
			, indirectDiffuse
			, directSpecular
			, directScattering
			, directCoatingSpecular
			, directSheen
			, indirectSpecular
			, directAmbient
			, indirectAmbient
			, ambientOcclusion
			, emissive
			, std::move( reflectedDiffuse )
			, std::move( reflectedSpecular )
			, std::move( refracted )
			, std::move( coatReflected )
			, std::move( sheenReflected ) );
	}

	sdw::Vec3 LightingModel::combine( BlendComponents const & components
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
		, sdw::Vec3 reflectedDiffuse
		, sdw::Vec3 reflectedSpecular
		, sdw::Vec3 refracted
		, sdw::Vec3 coatReflected
		, sdw::Vec3 sheenReflected )
	{
		return combine( components
			, incident
			, directDiffuse
			, indirectDiffuse
			, directSpecular
			, directScattering
			, directCoatingSpecular
			, directSheen
			, indirectSpecular
			, directAmbient
			, indirectAmbient
			, components.occlusion
			, components.emissiveColour * components.emissiveFactor
			, std::move( reflectedDiffuse )
			, std::move( reflectedSpecular )
			, std::move( refracted )
			, std::move( coatReflected )
			, std::move( sheenReflected ) );
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
						, components.specular
						, components );
					doComputeLight( light.base()
						, components
						, lightSurface
						, radiance
						, output );
					doApplyShadows( light
						, lightSurface
						, radiance
						, receivesShadows
						, output );
					doComputeScatteringTerm( radiance
						, light.base()
						, components
						, lightSurface
						, output.m_scattering );
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
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
						, components.specular
						, components );
					doComputeLight( light.base()
						, components
						, lightSurface
						, radiance
						, output );
					doApplyShadows( light
						, lightSurface
						, radiance
						, receivesShadows
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( lightSurface.lengthL() ) );
					doComputeAttenuation( attenuation, output );
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
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
						, components.specular
						, components );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( -lightSurface.L(), light.direction() ) );

					IF( m_writer, spotFactor > light.outerCutOff() )
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
						spotFactor = clamp( ( spotFactor - light.outerCutOff() ) / light.cutOffsDiff(), 0.0_f, 1.0_f );
						output.m_diffuse = spotFactor * output.m_diffuse;
						output.m_specular = spotFactor * output.m_specular;
						output.m_scattering = spotFactor * output.m_scattering;
						output.m_coatingSpecular = spotFactor * output.m_coatingSpecular;
						output.m_sheen.x() = spotFactor * output.m_sheen.x();

#if !C3D_DisableSSSTransmittance
						if ( m_shadowModel.isEnabled() && m_sssTransmittance )
						{
							IF( m_writer
								, ( light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) ) )
								&& ( light.base().index() >= 0_i )
								&& ( receivesShadows != 0_u )
								&& ( material.sssProfileIndex != 0.0_f ) )
							{
								output.m_diffuse += ( spotFactor * rawDiffuse / attenuation )
									* m_sssTransmittance->compute( material
										, light
										, surface );
							}
							FI;
						}
#endif

						doApplyShadows( light
							, lightSurface
							, radiance
							, receivesShadows
							, output );
						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( lightSurface.lengthL() ) );
						doComputeAttenuation( attenuation, output );
						parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
						parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
						parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
						parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
						parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
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
		, LightSurface const & plightSurface
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
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
					doApplyShadowsDiffuse( light
						, lightSurface
						, radiance
						, receivesShadows
						, diffuse );
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
					doApplyShadowsDiffuse( light
						, lightSurface
						, radiance
						, receivesShadows
						, diffuse );
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
						, dot( -lightSurface.L(), light.direction() ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.outerCutOff() )
					{
						auto radiance = m_writer.declLocale( "radiance"
							, vec3( 0.0_f ) );
						diffuse = doComputeLightDiffuse( light.base()
							, components
							, lightSurface
							, radiance );
						spotFactor = clamp( ( spotFactor - light.outerCutOff() ) / light.cutOffsDiff(), 0.0_f, 1.0_f );
						diffuse = spotFactor * diffuse;
						doApplyShadowsDiffuse( light
							, lightSurface
							, radiance
							, receivesShadows
							, diffuse );
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
		output.m_diffuse = output.m_diffuse / attenuation;
		output.m_specular = output.m_specular / attenuation;
		output.m_scattering = output.m_scattering / attenuation;
		output.m_coatingSpecular = output.m_coatingSpecular / attenuation;
		output.m_sheen.x() = output.m_sheen.x() / attenuation;
	}

	void LightingModel::doApplyShadows( DirectionalLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			m_directionalCascadeIndex = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
				, 0_u ) );
			m_directionalCascadeCount = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
				, 0_u ) );

			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) ) )
			{
				auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
					, vec3( 0.0_f, 1.0_f, 0.0_f ) );
				auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
				auto maxCount = m_writer.declLocale( "maxCount"
					, m_writer.cast< sdw::UInt >( clamp( light.cascadeCount(), 1_u, c3d_maxCascadeCount ) - 1_u ) );

				// Get cascade index for the current fragment's view position
				FOR( m_writer, sdw::UInt, i, maxCount, i > 0u, --i )
				{
					auto factors = m_writer.declLocale( "factors"
						, m_lights.getCascadeFactors( lightSurface.viewPosition()
							, light.splitDepths()
							, i ) );

					IF( m_writer, factors.x() != 0.0_f )
					{
						cascadeFactors = factors;
					}
					FI;
				}
				ROF;

				*m_directionalCascadeIndex = m_writer.cast< sdw::UInt >( cascadeFactors.x() );
				*m_directionalCascadeCount = light.cascadeCount();
				m_directionalTransform = std::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
					, light.transforms()[*m_directionalCascadeIndex] ) );

				IF( m_writer, receivesShadows != 0_u )
				{
					auto filterScale = m_writer.declLocale( "filterScale"
						, abs( light.splitScales()[0] / light.splitScales()[*m_directionalCascadeIndex] ) );
					auto filterSize = m_writer.declLocale( "filterSize"
						, light.shadows().pcfFilterSize() );
					light.shadows().pcfFilterSize() = clamp( filterSize * filterScale
						, 1.0_f
						, sdw::Float{ float( MaxPcfFilterSize ) } );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, cascadeFactors.y()
							* m_shadowModel.computeDirectional( light.shadows()
								, lightSurface
								, *m_directionalTransform
								, *m_directionalCascadeIndex
								, light.cascadeCount() ) );

					IF( m_writer, cascadeFactors.z() > 0.0_f )
					{
						auto filterScale = m_writer.declLocale( "filterScale"
							, abs( light.splitScales()[0] / light.splitScales()[*m_directionalCascadeIndex - 1u] ) );
						light.shadows().pcfFilterSize() = clamp( filterSize * filterScale
							, 1.0_f
							, 64.0_f );
						shadowFactor += cascadeFactors.z()
							* m_shadowModel.computeDirectional( light.shadows()
								, lightSurface
								, light.transforms()[*m_directionalCascadeIndex - 1u]
								, *m_directionalCascadeIndex - 1u
								, light.cascadeCount() );
					}
					FI;

					output.m_diffuse *= shadowFactor;
					output.m_specular *= shadowFactor;
					output.m_coatingSpecular *= shadowFactor;
					output.m_sheen.x() *= shadowFactor;
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
	}

	void LightingModel::doApplyShadows( PointLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& light.shadows().shadowMapIndex() >= 0_i
				&& receivesShadows != 0_u )
			{
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, m_shadowModel.computePoint( light.shadows()
						, lightSurface
						, 1.0_f - ( length( -lightSurface.vertexToLight() ) / light.base().farPlane() ) ) );
				output.m_diffuse *= shadowFactor;
				output.m_specular *= shadowFactor;
				output.m_coatingSpecular *= shadowFactor;
				output.m_sheen.x() *= shadowFactor;
			}
			FI;
		}
	}

	void LightingModel::doApplyShadows( SpotLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, OutputComponents & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& light.shadows().shadowMapIndex() >= 0_i
				&& receivesShadows != 0_u )
			{
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, m_shadowModel.computeSpot( light.shadows()
						, lightSurface
						, light.transform()
						, length( -lightSurface.vertexToLight() ) / light.base().farPlane() ) );
				output.m_diffuse *= shadowFactor;
				output.m_specular *= shadowFactor;
				output.m_coatingSpecular *= shadowFactor;
				output.m_sheen.x() *= shadowFactor;
			}
			FI;
		}
	}

	void LightingModel::doApplyShadowsDiffuse( DirectionalLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			m_directionalCascadeIndex = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeIndex"
				, 0_u ) );
			m_directionalCascadeCount = std::make_unique< sdw::UInt >( m_writer.declLocale( "cascadeCount"
				, 0_u ) );

			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) ) )
			{
				*m_directionalCascadeIndex = light.cascadeCount() - 1_u;
				*m_directionalCascadeCount = light.cascadeCount();
				m_directionalTransform = std::make_unique< sdw::Mat4 >( m_writer.declLocale( "directionalTransform"
					, light.transforms()[*m_directionalCascadeIndex] ) );

				IF( m_writer, receivesShadows != 0_u )
				{
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, m_shadowModel.computeDirectional( light.shadows()
							, lightSurface
							, *m_directionalTransform
							, *m_directionalCascadeIndex
							, light.cascadeCount() ) );
					output *= shadowFactor;
				}
				FI;
			}
			FI;
		}
	}

	void LightingModel::doApplyShadowsDiffuse( PointLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& light.shadows().shadowMapIndex() >= 0_i
				&& receivesShadows != 0_u )
			{
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, m_shadowModel.computePoint( light.shadows()
						, lightSurface
						, 1.0_f - ( length( -lightSurface.vertexToLight() ) / light.base().farPlane() ) ) );
				output *= shadowFactor;
			}
			FI;
		}
	}

	void LightingModel::doApplyShadowsDiffuse( SpotLight const & light
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::UInt const & receivesShadows
		, sdw::Vec3 & output )
	{
		if ( m_shadowModel.isEnabled() )
		{
			IF( m_writer
				, light.shadows().shadowType() != sdw::UInt( int( ShadowType::eNone ) )
				&& light.shadows().shadowMapIndex() >= 0_i
				&& receivesShadows != 0_u )
			{
				auto shadowFactor = m_writer.declLocale( "shadowFactor"
					, m_shadowModel.computeSpot( light.shadows()
						, lightSurface
						, light.transform()
						, length( -lightSurface.vertexToLight() ) / light.base().farPlane() ) );
				output *= shadowFactor;
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

	void LightingModel::doComputeScatteringTerm( sdw::Vec3 const & radiance
		, Light const & light
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 & output )
	{
		if ( m_enableVolumetric
			&& m_directionalTransform
			&& m_directionalCascadeIndex
			&& m_directionalCascadeCount )
		{
			IF( m_writer, light.shadows().volumetricSteps() != 0_u )
			{
				auto volumetric = m_writer.declLocale( "volumetric"
					, m_shadowModel.computeVolumetric( light.shadows()
						, lightSurface
						, *m_directionalTransform
						, *m_directionalCascadeIndex
						, *m_directionalCascadeCount ) );
				output = volumetric
					* radiance
					* light.intensity().x();
			}
			FI;
		}
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
		auto rawDiffuse = doComputeDiffuseTerm( radiance
			, light.intensity().x()
			, components
			, lightSurface
			, isLit
			, output.m_diffuse );
		doComputeSpecularTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.m_specular );
		doComputeSheenTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.m_sheen );
		doComputeCoatingTerm( radiance
			, light.intensity().y()
			, components
			, lightSurface
			, isLit
			, output.m_coatingSpecular );
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
