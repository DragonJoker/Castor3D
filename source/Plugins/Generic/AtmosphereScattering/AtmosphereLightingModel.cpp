#include "AtmosphereScattering/AtmosphereLightingModel.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Material/Pass/Phong/Shaders/GlslPhongReflection.hpp>
#include <Castor3D/Material/Pass/PBR/Shaders/GlslPbrReflection.hpp>
#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslShadow.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	AtmospherePhongLightingModel::AtmospherePhongLightingModel( sdw::ShaderWriter & m_writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric
		, bool isBlinnPhong )
		: c3d::PhongLightingModel{ m_writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, isBlinnPhong }
	{
		m_prefix = m_prefix + "atmosphere_";
	}

	const castor::String AtmospherePhongLightingModel::getName()
	{
		return c3d::concatModelNames( c3d::PhongLightingModel::getName(), AtmosphereBackgroundModel::Name );
	}

	c3d::LightingModelPtr AtmospherePhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< AtmospherePhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, false );
	}

	void AtmospherePhongLightingModel::compute( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, c3d::BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_atmosphereBackground )
		{
			m_atmosphereBackground = &static_cast< AtmosphereBackgroundModel & >( pbackground );
			m_atmosphereBackground->atmosphere.luminanceSettings.shadowMapEnabled = true;
			m_atmosphereBackground->atmosphere.shadows = m_shadowModel;
		}

		if ( !m_computeDirectional )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) ) };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.direction ) );
					auto radiance = m_writer.declLocale( "lightSpecular"
						, m_atmosphereBackground->getSunRadiance( light.direction ) );
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.normal, -lightDirection ) );
					auto isLit = m_writer.declLocale( "isLit"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					auto result = m_writer.declLocale( "result"
						, radiance * light.base.intensity.x() );
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
						* radiance
						* light.base.intensity.y()
						* pow( specularFactor, clamp( components.shininess, 1.0_f, 256.0_f ) );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, 0_u );
					auto maxCount = m_writer.declLocale( "maxCount"
						, 1_u );

					auto targetSize = vec2( sdw::Float{ float( m_atmosphereBackground->getTargetSize().width ) }
					, float( m_atmosphereBackground->getTargetSize().height ) );
					auto luminance = m_writer.declLocale< sdw::Vec4 >( "luminance" );
					auto transmittance = m_writer.declLocale< sdw::Vec4 >( "transmittance" );
					m_atmosphereBackground->getPixelTransLum( vec2( surface.clipPosition.x(), 1.0_f - surface.clipPosition.y() )
						, targetSize
						, surface.clipPosition.z()
						, transmittance
						, luminance );
					output.m_scattering = luminance.xyz() / luminance.a();

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
							maxCount = m_writer.cast< sdw::UInt >( clamp( light.cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u );

							// Get cascade index for the current fragment's view position
							FOR( m_writer, sdw::UInt, i, 0u, i < maxCount, ++i )
							{
								auto factors = m_writer.declLocale( "factors"
									, m_getCascadeFactors( sdw::Vec3{ surface.viewPosition.xyz() }
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
							}
							FI;
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
										, light.cascadeCount
										, 1.0_f ) );
								output.m_scattering *= volumetric;
							}
							FI;
						}
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
				}
				, c3d::InDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "components", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	//*********************************************************************************************

	AtmosphereBlinnPhongLightingModel::AtmosphereBlinnPhongLightingModel( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: AtmospherePhongLightingModel{ writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, true }
	{
	}

	c3d::LightingModelPtr AtmosphereBlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< AtmosphereBlinnPhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	castor::String AtmosphereBlinnPhongLightingModel::getName()
	{
		return c3d::concatModelNames( c3d::BlinnPhongLightingModel::getName(), AtmosphereBackgroundModel::Name );
	}

	//*********************************************************************************************

	AtmospherePbrLightingModel::AtmospherePbrLightingModel( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: c3d::PbrLightingModel{ writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric }
	{
		m_prefix = "c3d_pbr_atmosphere_";
	}

	const castor::String AtmospherePbrLightingModel::getName()
	{
		return c3d::concatModelNames( c3d::PbrLightingModel::getName(), AtmosphereBackgroundModel::Name );
	}

	c3d::LightingModelPtr AtmospherePbrLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< AtmospherePbrLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	void AtmospherePbrLightingModel::compute( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, c3d::BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_atmosphereBackground )
		{
			m_atmosphereBackground = &static_cast< AtmosphereBackgroundModel & >( pbackground );
			m_atmosphereBackground->atmosphere.luminanceSettings.shadowMapEnabled = true;
			m_atmosphereBackground->atmosphere.shadows = m_shadowModel;
		}

		if ( !m_computeDirectional )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) ) };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					auto radiance = m_writer.declLocale( "lightSpecular"
						, m_atmosphereBackground->getSunRadiance( lightDirection ) );

					//Direct lighting
					m_cookTorrance.compute( radiance
						, light.base.intensity
						, worldEye
						, lightDirection
						, components.specular
						, components.metalness
						, components.roughness
						, surface
						, output );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, 0_u );
					auto maxCount = m_writer.declLocale( "maxCount"
						, 1_u );

					auto targetSize = vec2( sdw::Float{ float( m_atmosphereBackground->getTargetSize().width ) }
					, float( m_atmosphereBackground->getTargetSize().height ) );
					auto luminance = m_writer.declLocale< sdw::Vec4 >( "luminance" );
					auto transmittance = m_writer.declLocale< sdw::Vec4 >( "transmittance" );
					m_atmosphereBackground->getPixelTransLum( vec2( surface.clipPosition.x(), 1.0_f - surface.clipPosition.y() )
						, targetSize
						, surface.clipPosition.z()
						, transmittance
						, luminance );
					output.m_scattering = luminance.xyz() / luminance.a();

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, ( light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
							maxCount = m_writer.cast< sdw::UInt >( clamp( light.cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u );

							// Get cascade index for the current fragment's view position
							FOR( m_writer, sdw::UInt, i, 0u, i < maxCount, ++i )
							{
								auto factors = m_writer.declLocale( "factors"
									, m_getCascadeFactors( sdw::Vec3{ surface.viewPosition.xyz() }
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
										, -lightDirection
										, cascadeIndex
										, light.cascadeCount ) );

								IF( m_writer, cascadeIndex > 0_u )
								{
									shadowFactor += cascadeFactors.z()
										* m_shadowModel->computeDirectional( light.base
											, surface
											, light.transforms[cascadeIndex - 1u]
											, -lightDirection
											, cascadeIndex - 1u
											, light.cascadeCount );
								}
								FI;

								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
								output.m_coatingSpecular *= shadowFactor;
							}
							FI;
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
										, light.cascadeCount
										, 1.0_f ) );
								output.m_scattering *= volumetric;
							}
							FI;
						}
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
				}
				, c3d::InDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "components", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	//*********************************************************************************************
}
