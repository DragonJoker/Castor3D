#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	PbrLightingModel::PbrLightingModel( sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: LightingModel{ writer
			, materials
			, utils
			, brdf
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, "c3d_pbr_" }
		, m_cookTorrance{ writer, utils, brdf }
		, m_sheen{ writer, brdf }
	{
	}

	const castor::String PbrLightingModel::getName()
	{
		return cuT( "c3d.pbr" );
	}

	LightingModelPtr PbrLightingModel::create( sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< PbrLightingModel >( writer
			, materials
			, utils
			, brdf
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	sdw::Float PbrLightingModel::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return mix( components.transmission, 0.0_f, components.metalness );
	}

	sdw::Vec3 PbrLightingModel::adjustDirectAmbient( BlendComponents const & components
		, sdw::Vec3 const & directAmbient )const
	{
		return directAmbient;
	}

	sdw::Vec3 PbrLightingModel::adjustDirectSpecular( BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		return directSpecular;
	}

	sdw::Vec3 PbrLightingModel::adjustRefraction( BlendComponents const & components
		, sdw::Vec3 const & refraction )const
	{
		if ( components.hasMember( "metalness" ) )
		{
			return refraction * ( 1.0_f - components.getMember< sdw::Float >( "metalness" ) );
		}

		return refraction;
	}

	ReflectionModelPtr PbrLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< PbrReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void PbrLightingModel::compute( DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeDirectional )
		{
			OutputComponents outputs{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };

					auto L = m_writer.declLocale( "L"
						, normalize( -light.direction ) );
					auto V = m_writer.declLocale( "V"
						, normalize( worldEye - surface.worldPosition.xyz() ) );
					auto H = m_writer.declLocale( "H"
						, normalize( L + V ) );
					auto N = m_writer.declLocale( "N"
						, normalize( surface.normal ) );

					auto NdotL = m_writer.declLocale( "NdotL"
						, max( 0.0_f, dot( N, L ) ) );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( 0.0_f, dot( N, V ) ) );
					auto NdotH = m_writer.declLocale( "NdotH"
						, max( 0.0_f, dot( N, H ) ) );
					auto HdotV = m_writer.declLocale( "HdotV"
						, max( 0.0_f, dot( H, V ) ) );

					IF( m_writer, components.iridescenceFactor != 0.0_f )
					{
						m_cookTorrance.compute( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.roughness
							, components.iridescenceFresnel
							, components.iridescenceFactor
							, surface
							, output );
					}
					ELSE
					{
						m_cookTorrance.compute( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.roughness
							, surface
							, output );
					}
					FI;

					IF( m_writer, components.clearcoatFactor != 0.0_f )
					{
						output.m_coatingSpecular = m_cookTorrance.computeSpecular( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.clearcoatRoughness
							, surface.worldPosition.xyz()
							, components.clearcoatNormal );
					}
					FI;

					IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
					{
						output.m_sheen = m_sheen.compute( NdotH
							, NdotV
							, NdotL
							, components.sheenRoughness );
					}
					FI;

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, 0_u );
							auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
							auto maxCount = m_writer.declLocale( "maxCount"
								, m_writer.cast< sdw::UInt >( clamp( light.cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

							// Get cascade index for the current fragment's view position
							FOR( m_writer, sdw::UInt, i, 0u, i < maxCount, ++i )
							{
								auto factors = m_writer.declLocale( "factors"
									, m_getCascadeFactors( surface.viewPosition.xyz()
										, light.splitDepths
										, i ) );

								IF( m_writer, factors.x() != 0.0_f )
								{
									cascadeFactors = factors;
								}
								FI;
							}
							ROF;

							cascadeIndex = m_writer.cast< sdw::UInt >( cascadeFactors.x() );

							IF ( m_writer, receivesShadows != 0_u )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, cascadeFactors.y()
										* m_shadowModel->computeDirectional( light.base
											, surface
											, light.transforms[cascadeIndex]
											, -L
											, cascadeIndex
											, light.cascadeCount ) );

								IF( m_writer, cascadeIndex > 0_u )
								{
									shadowFactor += cascadeFactors.z()
										* m_shadowModel->computeDirectional( light.base
											, surface
											, light.transforms[cascadeIndex - 1u]
											, -L
											, cascadeIndex - 1u
											, light.cascadeCount );
								}
								FI;

								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
								output.m_coatingSpecular *= shadowFactor;
								output.m_sheen.x() *= shadowFactor;
							}
							FI;

							if ( m_enableVolumetric )
							{
								IF( m_writer, light.base.volumetricSteps != 0_u )
								{
									auto volumetric = m_writer.declLocale( "volumetric"
										, m_shadowModel->computeVolumetric( light.base
											, surface
											, worldEye
											, light.transforms[cascadeIndex]
											, light.direction
											, cascadeIndex
											, light.cascadeCount ) );
									output.m_diffuse += volumetric * light.base.intensity.x() * light.base.colour;
									output.m_specular += volumetric * light.base.intensity.y() * light.base.colour;
									output.m_coatingSpecular += volumetric * light.base.intensity.y() * light.base.colour;
								}
								FI;
							}

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

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
				}
				, InDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeDirectional( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void PbrLightingModel::compute( PointLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computePoint )
		{
			OutputComponents outputs{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
				, [this]( PointLight const & light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, light.position - surface.worldPosition.xyz() );
					auto distance = m_writer.declLocale( "distance"
						, length( vertexToLight ) );

					auto L = m_writer.declLocale( "L"
						, normalize( vertexToLight ) );
					auto V = m_writer.declLocale( "V"
						, normalize( worldEye - surface.worldPosition.xyz() ) );
					auto H = m_writer.declLocale( "H"
						, normalize( L + V ) );
					auto N = m_writer.declLocale( "N"
						, normalize( surface.normal ) );

					auto NdotL = m_writer.declLocale( "NdotL"
						, max( 0.0_f, dot( N, L ) ) );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( 0.0_f, dot( N, V ) ) );
					auto NdotH = m_writer.declLocale( "NdotH"
						, max( 0.0_f, dot( N, H ) ) );
					auto HdotV = m_writer.declLocale( "HdotV"
						, max( 0.0_f, dot( H, V ) ) );

					IF( m_writer, components.iridescenceFactor != 0.0_f )
					{
						m_cookTorrance.compute( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.roughness
							, components.iridescenceFresnel
							, components.iridescenceFactor
							, surface
							, output );
					}
					ELSE
					{
						m_cookTorrance.compute( light.base
						, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.roughness
							, surface
							, output );
					}
					FI;

					IF( m_writer, components.clearcoatFactor != 0.0_f )
					{
						output.m_coatingSpecular = m_cookTorrance.computeSpecular( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.clearcoatRoughness
							, surface.worldPosition.xyz()
							, components.clearcoatNormal );
					}
					FI;

					IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
					{
						output.m_sheen = m_sheen.compute( NdotH
							, NdotV
							, NdotL
							, components.sheenRoughness );
					}
					FI;

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
								&& ( light.base.index >= 0_i )
								&& ( receivesShadows != 0_u ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computePoint( light.base
									, surface
									, light.position ) );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
							output.m_coatingSpecular *= shadowFactor;
							output.m_sheen.x() *= shadowFactor;
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					output.m_diffuse = output.m_diffuse / attenuation;
					output.m_specular = output.m_specular / attenuation;
					output.m_scattering = output.m_scattering / attenuation;
					output.m_coatingSpecular = output.m_coatingSpecular / attenuation;
					output.m_sheen.x() = output.m_sheen.x() / attenuation;
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
				}
				, InPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computePoint( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void PbrLightingModel::compute( SpotLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			OutputComponents outputs{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
				, [this]( SpotLight const & light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, OutputComponents & parentOutput )
				{
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, light.position - surface.worldPosition.xyz() );
					auto L = m_writer.declLocale( "L"
						, normalize( vertexToLight ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( L, -light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( vertexToLight ) );
						OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };

						auto V = m_writer.declLocale( "V"
							, normalize( worldEye - surface.worldPosition.xyz() ) );
						auto H = m_writer.declLocale( "H"
							, normalize( L + V ) );
						auto N = m_writer.declLocale( "N"
							, normalize( surface.normal ) );

						auto NdotL = m_writer.declLocale( "NdotL"
							, max( 0.0_f, dot( N, L ) ) );
						auto NdotV = m_writer.declLocale( "NdotV"
							, max( 0.0_f, dot( N, V ) ) );
						auto NdotH = m_writer.declLocale( "NdotH"
							, max( 0.0_f, dot( N, H ) ) );
						auto HdotV = m_writer.declLocale( "HdotV"
							, max( 0.0_f, dot( H, V ) ) );
						auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
							, vec3( 0.0_f ) );

						IF( m_writer, components.iridescenceFactor != 0.0_f )
						{
							rawDiffuse = m_cookTorrance.compute( light.base
								, HdotV
								, NdotH
								, NdotV
								, NdotL
								, components.specular
								, components.metalness
								, components.roughness
								, components.iridescenceFresnel
								, components.iridescenceFactor
								, surface
								, output );
						}
						ELSE
						{
							rawDiffuse = m_cookTorrance.compute( light.base
								, HdotV
								, NdotH
								, NdotV
								, NdotL
								, components.specular
								, components.metalness
								, components.roughness
								, surface
								, output );
						}
						FI;

						IF( m_writer, components.clearcoatFactor != 0.0_f )
						{
							output.m_coatingSpecular = m_cookTorrance.computeSpecular( light.base
								, HdotV
								, NdotH
								, NdotV
								, NdotL
								, components.specular
								, components.metalness
								, components.clearcoatRoughness
								, surface.worldPosition.xyz()
								, components.clearcoatNormal );
						}
						FI;

						IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
						{
							output.m_sheen = m_sheen.compute( NdotH
								, NdotV
								, NdotL
								, components.sheenRoughness );
						}
						FI;

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
									&& ( light.base.index >= 0_i )
									&& ( receivesShadows != 0_u ) )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, m_shadowModel->computeSpot( light.base
									, surface
									, light.transform
									, -vertexToLight ) );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
								output.m_coatingSpecular *= shadowFactor;
								output.m_sheen.x() *= shadowFactor;
							}
							FI;
						}

						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( distance ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOff ) / light.cutOffsDiff, 0.0_f, 1.0_f );
						output.m_diffuse = spotFactor * output.m_diffuse / attenuation;

#if !C3D_DisableSSSTransmittance
						if ( m_shadowModel->isEnabled() && m_sssTransmittance )
						{
							IF( m_writer
								, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
								&& ( light.base.index >= 0_i )
								&& ( receivesShadows != 0_i )
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

						output.m_specular = spotFactor * output.m_specular / attenuation;
						output.m_scattering = spotFactor * output.m_scattering / attenuation;
						output.m_coatingSpecular = spotFactor * output.m_coatingSpecular / attenuation;
						output.m_sheen.x() = spotFactor * output.m_sheen.x() / attenuation;
						parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
						parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
						parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
						parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
						parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
					}
					FI;
				}
				, InSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeSpot( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( DirectionalLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					diffuse = m_cookTorrance.computeDiffuse( light.base
						, worldEye
						, lightDirection
						, components.specular
						, components.metalness
						, surface );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& receivesShadows != 0_u )
						{
							light.base.updateShadowType( castor3d::ShadowType::eRaw );
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.cascadeCount - 1_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computeDirectional( light.base
									, surface
									, light.transforms[cascadeIndex]
									, -lightDirection
									, cascadeIndex
									, light.cascadeCount ) );
							diffuse *= shadowFactor;
						}
						FI;
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InOutDirectionalLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( PointLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( PointLight light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition.xyz() );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					diffuse = m_cookTorrance.computeDiffuse( light.base
						, worldEye
						, lightDirection
						, components.specular
						, components.metalness
						, surface );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& light.base.index >= 0_i
								&& receivesShadows != 0_u )
						{
							light.base.updateShadowType( castor3d::ShadowType::eRaw );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computePoint( light.base
									, surface
									, light.position ) );
							diffuse *= shadowFactor;
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse / attenuation ) );
				}
				, InOutPointLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( SpotLight const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( SpotLight light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition.xyz() );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.direction ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( lightToVertex ) );
						diffuse = m_cookTorrance.computeDiffuse( light.base
							, worldEye
							, lightDirection
							, components.specular
							, components.metalness
							, surface );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
									&& light.base.index >= 0_i
									&& receivesShadows != 0_u )
							{
								light.base.updateShadowType( castor3d::ShadowType::eRaw );
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, m_shadowModel->computeSpot( light.base
										, surface
										, light.transform
										, -lightToVertex ) );
								diffuse *= shadowFactor;
							}
							FI;
						}

						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( distance ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOff ) / light.cutOffsDiff, 0.0_f, 1.0_f );
						diffuse = max( vec3( 0.0_f ), spotFactor * diffuse / attenuation );
					}
					FI;

					m_writer.returnStmt( diffuse );
				}
				, InOutSpotLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 PbrLightingModel::doCombine( BlendComponents const & components
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
		, sdw::Vec3 const & reflected
		, sdw::Vec3 const & refracted
		, sdw::Vec3 const & coatReflected
		, sdw::Vec3 const & sheenReflected )
	{
		auto diffuseBrdf = m_writer.declLocale( "diffuseBrdf"
			, components.colour * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) ) );
		auto specularBrdf = m_writer.declLocale( "specularBrdf"
			, ( reflected * adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
			+ adjustDirectSpecular( components, directSpecular ) + ( indirectSpecular * ambientOcclusion ) );
		auto metal = m_writer.declLocale( "metal"
			, specularBrdf ); // Conductor Fresnel already included there.

		IF( m_writer, components.hasTransmission )
		{
			auto specularBtdf = m_writer.declLocale( "specularBtdf"
				, adjustRefraction( components, refracted ) );
			diffuseBrdf = mix( diffuseBrdf, specularBtdf, vec3( components.transmission ) );
		}
		ELSE
		{
			diffuseBrdf += refracted;
		}
		FI;

		auto dielectric = m_writer.declLocale( "dielectric"
			, specularBrdf + diffuseBrdf );
		auto combineResult = m_writer.declLocale( "combineResult"
			, mix( dielectric, metal, vec3( components.metalness ) ) );

		IF( m_writer, !all( components.sheenFactor == vec3( 0.0_f ) ) )
		{
			combineResult = sheenReflected
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
			combineResult = mix( combineResult
				, coatReflected + directCoatingSpecular
				, vec3( components.clearcoatFactor * clearcoatFresnel ) );
		}
		FI;

		return combineResult
			+ emissive
			+ directScattering;
	}

	//***********************************************************************************************
}
