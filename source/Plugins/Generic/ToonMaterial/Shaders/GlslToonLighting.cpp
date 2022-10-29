#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

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

namespace toon::shader
{
	//*********************************************************************************************

	const castor::String ToonPhongLightingModel::getName()
	{
		return cuT( "c3d.toon.phong" );
	}

	ToonPhongLightingModel::ToonPhongLightingModel( sdw::ShaderWriter & m_writer
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
		m_prefix = m_prefix + "toon_";
	}

	c3d::LightingModelPtr ToonPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonPhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, false );
	}

	c3d::ReflectionModelPtr ToonPhongLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< c3d::PhongReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void ToonPhongLightingModel::compute( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, c3d::BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
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
											, -lightDirection
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
						}
						FI;
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					parentOutput.m_coatingSpecular += max( vec3( 0.0_f ), output.m_coatingSpecular );
					parentOutput.m_sheen += max( vec2( 0.0_f ), output.m_sheen );
				}
				, c3d::InDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
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

	void ToonPhongLightingModel::compute( c3d::PointLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_computePoint )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InPointLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	void ToonPhongLightingModel::compute( c3d::SpotLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition.xyz() - light.position );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( lightToVertex ) );
						c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightCoatingSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSheen", vec2( 0.0_f ) ) };
						doComputeLight( light.base
							, components
							, surface
							, worldEye
							, lightDirection
							, output );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
						spotFactor = ( spotFactor - light.outerCutOff ) / light.cutOffsDiff;
						output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
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
				, c3d::InSpotLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InOutDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::PointLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InOutPointLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::SpotLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
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
								, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
						spotFactor = ( spotFactor - light.outerCutOff ) / light.cutOffsDiff;
						diffuse = max( vec3( 0.0_f ), spotFactor * diffuse / attenuation );
					}
					FI;

					m_writer.returnStmt( diffuse );
				}
				, c3d::InOutSpotLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	void ToonPhongLightingModel::doComputeLight( c3d::Light const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Vec3 const & plightDirection
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_computeLight )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computeLight = m_writer.implementFunction< sdw::Void >( m_prefix + "doComputeLight"
				, [this]( c3d::Light const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection
					, c3d::OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.normal, -lightDirection ) );
					auto delta = m_writer.declLocale( "delta"
						, fwidth( diffuseFactor ) * components.getMember< sdw::Float >( "smoothBand", true ) );
					diffuseFactor = smoothStep( 0.0_f, delta, diffuseFactor );
					output.m_diffuse = light.colour
						* light.intensity.x()
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
					specularFactor = pow( specularFactor * diffuseFactor, clamp( components.shininess, 1.0_f, 256.0_f ) );
					specularFactor = smoothStep( 0.0_f, 0.01_f * components.getMember< sdw::Float >( "smoothBand", true ), specularFactor );
					output.m_specular = specularFactor
						* light.colour
						* light.intensity.y();
				}
				, c3d::InLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" )
				, outputs );
		}

		m_computeLight( plight
			, pcomponents
			, psurface
			, pworldEye
			, plightDirection
			, pparentOutput );
	}

	sdw::Vec3 ToonPhongLightingModel::doComputeLightDiffuse( c3d::Light const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Vec3 const & plightDirection )
	{
		if ( !m_computeLightDiffuse )
		{
			m_computeLightDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
				, [this]( c3d::Light const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.normal, -lightDirection ) );
					auto delta = m_writer.declLocale( "delta"
						, fwidth( diffuseFactor ) * components.getMember< sdw::Float >( "smoothBand", true ) );
					diffuseFactor = smoothStep( 0.0_f, delta, diffuseFactor );
					m_writer.returnStmt( diffuseFactor
						* light.colour
						* light.intensity.x() );
				}
				, c3d::InLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" ) );
		}

		return m_computeLightDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, plightDirection );
	}

	//*********************************************************************************************

	ToonBlinnPhongLightingModel::ToonBlinnPhongLightingModel( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: ToonPhongLightingModel{ writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, true }
	{
	}

	c3d::LightingModelPtr ToonBlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonBlinnPhongLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	castor::String ToonBlinnPhongLightingModel::getName()
	{
		return cuT( "c3d.toon.blinn_phong" );
	}

	//*********************************************************************************************

	ToonPbrLightingModel::ToonPbrLightingModel( sdw::ShaderWriter & writer
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
		m_prefix = m_prefix + "toon_";
	}

	const castor::String ToonPbrLightingModel::getName()
	{
		return cuT( "c3d.toon.pbr" );
	}

	c3d::LightingModelPtr ToonPbrLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Materials const & materials
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonPbrLightingModel >( writer
			, materials
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	c3d::ReflectionModelPtr ToonPbrLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< c3d::PbrReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void ToonPbrLightingModel::compute( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, c3d::BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
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

					m_cookTorrance.computeAON( light.base
						, HdotV
						, NdotH
						, NdotV
						, NdotL
						, components.specular
						, components.metalness
						, components.roughness
						, components.getMember< sdw::Float >( "smoothBand", true )
						, surface
						, output );

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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
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
				, c3d::InDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
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

	void ToonPbrLightingModel::compute( c3d::PointLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_computePoint )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
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

					m_cookTorrance.computeAON( light.base
						, HdotV
						, NdotH
						, NdotV
						, NdotL
						, components.specular
						, components.metalness
						, components.roughness
						, components.getMember< sdw::Float >( "smoothBand", true )
						, surface
						, output );

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
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InPointLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	void ToonPbrLightingModel::compute( c3d::SpotLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows
		, c3d::OutputComponents & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			c3d::OutputComponents outputs{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight const & light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, light.position - surface.worldPosition.xyz() );
					auto L = m_writer.declLocale( "lightDirection"
						, normalize( vertexToLight ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( L, -light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( vertexToLight ) );
						c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
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

						m_cookTorrance.computeAON( light.base
							, HdotV
							, NdotH
							, NdotV
							, NdotL
							, components.specular
							, components.metalness
							, components.roughness
							, components.getMember< sdw::Float >( "smoothBand", true )
							, surface
							, output );

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
								, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
								&& light.base.index >= 0_i
								&& receivesShadows != 0_u )
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
				, c3d::InSpotLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
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

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::DirectionalLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, m_cookTorrance.computeDiffuseAON( light.base
							, worldEye
							, lightDirection
							, components.specular
							, components.metalness
							, components.getMember< sdw::Float >( "smoothBand", true )
							, surface ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InOutDirectionalLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::PointLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if (!m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition.xyz() );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, m_cookTorrance.computeDiffuseAON( light.base
							, worldEye
							, lightDirection
							, components.specular
							, components.metalness
							, components.getMember< sdw::Float >( "smoothBand", true )
							, surface ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InOutPointLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::SpotLight const & plight
		, c3d::BlendComponents const & pcomponents
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight light
					, c3d::BlendComponents const & components
					, c3d::Surface const & surface
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
						diffuse = m_cookTorrance.computeDiffuseAON( light.base
							, worldEye
							, lightDirection
							, components.specular
							, components.metalness
							, components.getMember< sdw::Float >( "smoothBand", true )
							, surface );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, light.base.shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) )
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
				, c3d::InOutSpotLight( m_writer, "light" )
				, c3d::InBlendComponents{ m_writer, "component", m_materials }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, pcomponents
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	//*********************************************************************************************
}
