#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongReflection.hpp"
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
	//*********************************************************************************************

	castor::String PhongLightingModel::getName()
	{
		return cuT( "c3d.phong" );
	}

	PhongLightingModel::PhongLightingModel( sdw::ShaderWriter & m_writer
		, Materials const & materials
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric
		, bool isBlinnPhong )
		: LightingModel{ m_writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, isBlinnPhong ? std::string{ "c3d_blinnphong_" } : std::string{ "c3d_phong_" } }
		, m_isBlinnPhong{ isBlinnPhong }
	{
	}

	LightingModelPtr PhongLightingModel::create( sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< PhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, false );
	}

	sdw::Float PhongLightingModel::getFinalTransmission( BlendComponents const & components
		, sdw::Vec3 const & incident )
	{
		return components.transmission;
	}

	sdw::Vec3 PhongLightingModel::adjustDirectAmbient( BlendComponents const & components
		, sdw::Vec3 const & directAmbient )const
	{
		return  directAmbient * components.colour;
	}

	sdw::Vec3 PhongLightingModel::adjustDirectSpecular( BlendComponents const & components
		, sdw::Vec3 const & directSpecular )const
	{
		auto specular = components.getMember( "specular", vec3( 1.0_f ) );
		return directSpecular * specular;
	}

	ReflectionModelPtr PhongLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< PhongReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void PhongLightingModel::compute( DirectionalLight const & plight
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
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.direction ) );
					doComputeLight( light.base
						, components
						, surface
						, worldEye
						, lightDirection
						, output );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
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

							IF( m_writer, receivesShadows != 0_u )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, cascadeFactors.y()
										* m_shadowModel->computeDirectional( light.base
											, surface
											, light.transforms[cascadeIndex]
											, lightDirection
											, cascadeIndex
											, light.cascadeCount ) );

								IF( m_writer, cascadeIndex > 0_u )
								{
									shadowFactor += cascadeFactors.z()
										* m_shadowModel->computeDirectional( light.base
											, surface
											, light.transforms[cascadeIndex - 1u]
											, lightDirection
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

	void PhongLightingModel::compute( PointLight const & plight
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
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition.xyz() - light.position );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					doComputeLight( light.base
						, components
						, surface
						, worldEye
						, lightDirection
						, output );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& light.base.index >= 0_i
								&& receivesShadows != 0_u )
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

	void PhongLightingModel::compute( SpotLight const & plight
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
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition.xyz() - light.position );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distLightToVertex"
							, length( lightToVertex ) );
						OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
						auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
							, doComputeLight( light.base
								, components
								, surface
								, worldEye
								, lightDirection
								, output ) );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
									&& light.base.index >= 0_i
									&& receivesShadows != 0_u )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, m_shadowModel->computeSpot( light.base
										, surface
										, light.transform
										, lightToVertex ) );
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

	sdw::Vec3 PhongLightingModel::computeDiffuse( DirectionalLight const & plight
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
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.direction ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, doComputeLightDiffuse( light.base
							, components
							, surface
							, worldEye
							, lightDirection ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& receivesShadows != 0_u )
						{
							light.base.updateShadowType( castor3d::ShadowType::eRaw );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.cascadeCount - 1_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computeDirectional( light.base
									, surface
									, light.transforms[cascadeIndex]
									, lightDirection
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

	sdw::Vec3 PhongLightingModel::computeDiffuse( PointLight const & plight
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
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition.xyz() - light.position );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, doComputeLightDiffuse( light.base
							, components
							, surface
							, worldEye
							, lightDirection ) );

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

	sdw::Vec3 PhongLightingModel::computeDiffuse( SpotLight const & plight
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
						, surface.worldPosition.xyz() - light.position );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, light.direction ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( lightToVertex ) );
						diffuse = doComputeLightDiffuse( light.base
							, components
							, surface
							, worldEye
							, lightDirection );

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
										, lightToVertex ) );
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

	sdw::RetVec3 PhongLightingModel::doComputeLight( Light const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Vec3 const & plightDirection
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeLight )
		{
			OutputComponents outputs{ m_writer };
			m_computeLight = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
				, [this]( Light const & light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection
					, OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.normal, -lightDirection ) );
					auto isLit = m_writer.declLocale( "isLit"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					auto result = m_writer.declLocale( "result"
						, light.colour
						* light.intensity.x() );
					output.m_diffuse = isLit
						* result
						* diffuseFactor;

					// Specular term.
					auto vertexToEye = m_writer.declLocale( "vertexToEye"
						, normalize( worldEye - surface.worldPosition.xyz() ) );

					if ( m_isBlinnPhong )
					{
						auto halfwayDir = m_writer.declLocale( "halfwayDir"
							, normalize( vertexToEye - lightDirection ) );
						m_writer.declLocale( "specularFactor"
							, max( dot( surface.normal, halfwayDir ), 0.0_f ) );
					}
					else
					{
						auto lightReflect = m_writer.declLocale( "lightReflect"
							, normalize( reflect( lightDirection, surface.normal ) ) );
						m_writer.declLocale( "specularFactor"
							, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
					}

					auto specularFactor = m_writer.getVariable< sdw::Float >( "specularFactor" );
					output.m_specular = isLit
						* light.colour
						* light.intensity.y()
						* pow( specularFactor, clamp( components.shininess, 1.0_f, 256.0_f ) );

					IF( m_writer, components.clearcoatFactor != 0.0_f )
					{
						if ( m_isBlinnPhong )
						{
							auto halfwayDir = m_writer.declLocale( "coatHalfwayDir"
								, normalize( vertexToEye - lightDirection ) );
							m_writer.declLocale( "coatSpecularFactor"
								, max( dot( surface.normal, halfwayDir ), 0.0_f ) );
						}
						else
						{
							auto lightReflect = m_writer.declLocale( "coatLightReflect"
								, normalize( reflect( lightDirection, surface.normal ) ) );
							m_writer.declLocale( "coatSpecularFactor"
								, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
						}

						auto coatSpecularFactor = m_writer.getVariable< sdw::Float >( "coatSpecularFactor" );
						output.m_coatingSpecular = isLit
							* light.colour
							* light.intensity.y()
							* pow( coatSpecularFactor, clamp( components.shininess, 1.0_f, 256.0_f ) );
					}
					FI;

					m_writer.returnStmt( result );
				}
				, InLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" )
				, outputs );
		}

		return m_computeLight( plight
			, pcomponents
			, psurface
			, pworldEye
			, plightDirection
			, pparentOutput );
	}

	sdw::Vec3 PhongLightingModel::doComputeLightDiffuse( Light const & plight
		, BlendComponents const & pcomponents
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Vec3 const & plightDirection )
	{
		if ( !m_computeLightDiffuse )
		{
			m_computeLightDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
				, [this]( Light const & light
					, BlendComponents const & components
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.normal, -lightDirection ) );
					auto isLit = m_writer.declLocale( "isLit"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					m_writer.returnStmt( isLit
						* light.colour
						* light.intensity.x()
						* diffuseFactor );
				}
				, InLight( m_writer, "light" )
				, InBlendComponents{ m_writer, "components", m_materials }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" ) );
		}

		return m_computeLightDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, plightDirection );
	}

	sdw::Vec3 PhongLightingModel::doCombine( BlendComponents const & components
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
		return m_writer.ternary( components.hasTransmission != 0_u
			, components.colour * components.transmission * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
				+ ( adjustDirectSpecular( components, directSpecular ) + ( indirectSpecular * ambientOcclusion ) )
				+ ( adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
				+ emissive
				+ refracted
				+ reflected * ambientOcclusion
				+ directScattering
			, components.colour * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
				+ ( adjustDirectSpecular( components, directSpecular ) + ( indirectSpecular * ambientOcclusion ) )
				+ ( adjustDirectAmbient( components, directAmbient ) * indirectAmbient * ambientOcclusion )
				+ emissive
				+ refracted
				+ reflected * ambientOcclusion
				+ directScattering );
	}

	//*********************************************************************************************

	BlinnPhongLightingModel::BlinnPhongLightingModel( sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: PhongLightingModel{ writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, true }
	{
	}

	LightingModelPtr BlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< BlinnPhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	castor::String BlinnPhongLightingModel::getName()
	{
		return cuT( "c3d.blinn_phong" );
	}

	//*********************************************************************************************
}
