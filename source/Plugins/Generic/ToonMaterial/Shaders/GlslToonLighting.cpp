#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"

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

	namespace phong
	{
		namespace
		{
			void modifyMaterial( sdw::ShaderWriter & writer
				, castor::String const & configName
				, castor3d::PassFlags const & passFlags
				, castor3d::TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, c3d::TextureConfigData const & config
				, ToonPhongLightMaterial & phongLightMat )
			{
				config.applyAlbedo( textureFlags, sampled, phongLightMat.albedo );
				config.applySpecular( textureFlags, sampled, phongLightMat.specular );
				config.applyShininess( textureFlags, sampled, phongLightMat.shininess );
			}

			void updateMaterial( sdw::ShaderWriter & writer
				, castor3d::PassFlags const & passFlags
				, castor3d::TextureFlags const & textureFlags
				, ToonPhongLightMaterial & phongLightMat
				, sdw::Vec3 & emissive )
			{
				if ( checkFlag( passFlags, castor3d::PassFlag::eLighting )
					&& !checkFlag( textureFlags, castor3d::TextureFlag::eEmissive ) )
				{
					emissive *= phongLightMat.albedo;
				}
			}
		}
	}

	//*********************************************************************************************

	const castor::String ToonPhongLightingModel::getName()
	{
		return cuT( "c3d.toon.phong" );
	}

	ToonPhongLightingModel::ToonPhongLightingModel( sdw::ShaderWriter & m_writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric
		, bool isBlinnPhong )
		: c3d::PhongLightingModel{ m_writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, isBlinnPhong }
	{
		m_prefix = m_prefix + "toon_";
	}

	c3d::LightingModelPtr ToonPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonPhongLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, false );
	}

	sdw::Vec3 ToonPhongLightingModel::combine( sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & directScattering
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & ambient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 const & reflected
		, sdw::Vec3 const & refracted
		, sdw::Vec3 const & materialAlbedo )
	{
		return materialAlbedo * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
			+ ( directSpecular + ( indirectSpecular * ambientOcclusion ) )
			+ ( ambient * indirectAmbient * ambientOcclusion )
			+ emissive
			+ refracted
			+ reflected * ambientOcclusion
			+ directScattering;
	}

	std::unique_ptr< c3d::LightMaterial > ToonPhongLightingModel::declMaterial( std::string const & name
		, bool enabled )
	{
		return m_writer.declDerivedLocale< c3d::LightMaterial, ToonPhongLightMaterial >( name, enabled );
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
		, c3d::LightMaterial const & pmaterial
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
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.direction ) );
					doComputeLight( light.base
						, material
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
									, m_getCascadeFactors( sdw::Vec3{ surface.viewPosition }
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
								}
								FI;
							}
						}
						FI;
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
				}
				, c3d::InDirectionalLight( m_writer, "light" )
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeDirectional( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPhongLightingModel::compute( c3d::PointLight const & plight
		, c3d::LightMaterial const & pmaterial
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
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition - light.position );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					doComputeLight( light.base
						, material
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
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					output.m_diffuse = output.m_diffuse / attenuation;
					output.m_specular = output.m_specular / attenuation;
					output.m_scattering = output.m_scattering / attenuation;
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
				}
				, c3d::InPointLight( m_writer, "light" )
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computePoint( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPhongLightingModel::compute( c3d::SpotLight const & plight
		, c3d::LightMaterial const & pmaterial
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
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition - light.position );
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
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
						doComputeLight( light.base
							, material
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
							}
							FI;
						}

						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( distance ) );
						spotFactor = ( spotFactor - light.outerCutOff ) / light.cutOffsDiff;
						output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
						output.m_specular = spotFactor * output.m_specular / attenuation;
						output.m_scattering = spotFactor * output.m_scattering / attenuation;
						parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
						parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
						parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					}
					FI;
				}
				, c3d::InSpotLight( m_writer, "light" )
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeSpot( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPhongLightingModel::computeMapContributions( castor3d::PassFlags const & passFlags
		, castor3d::TextureFlags const & textureFlags
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, c3d::LightMaterial & lightMat
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto & phongLightMat = static_cast< ToonPhongLightMaterial & >( lightMat );

		if ( !textureConfigs.isEnabled() )
		{
			phong::updateMaterial( m_writer
				, passFlags
				, textureFlags
				, phongLightMat
				, emissive );
			return;
		}

		for ( uint32_t index = 0u; index < textureFlags.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, material.getTexture( index ) );

			IF( m_writer, id > 0_u )
			{
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( id ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( id ) );
				auto texcoord = m_writer.declLocale( "tex" + name
					, textureConfigs.getTexcoord( config
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 ) );
				auto sampled = config.computeCommonMapContribution( m_utils
					, passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texcoord
					, emissive
					, opacity
					, occlusion
					, transmittance
					, normal
					, tangent
					, bitangent
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
				textureConfigs.setTexcoord( config
					, texcoord
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
				phong::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, phongLightMat );
			}
			FI;
		}

		phong::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, phongLightMat
			, emissive );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::DirectionalLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight light
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.direction ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, doComputeLightDiffuse( light.base
							, material
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
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::PointLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight light
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition - light.position );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, doComputeLightDiffuse( light.base
							, material
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
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::SpotLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight light
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, surface.worldPosition - light.position );
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
							, material
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
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, static_cast< ToonPhongLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	void ToonPhongLightingModel::computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
		, castor3d::TextureFlags const & textureFlags
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, c3d::LightMaterial & lightMat )
	{
		auto & phongLightMat = static_cast< ToonPhongLightMaterial & >( lightMat );

		if ( !textureConfigs.isEnabled() )
		{
			phong::updateMaterial( m_writer
				, passFlags
				, textureFlags
				, phongLightMat
				, emissive );
			return;
		}

		for ( uint32_t index = 0u; index < textureFlags.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, material.getTexture( index ) );

			IF( m_writer, id > 0_u )
			{
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( id ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( id ) );
				auto texcoord = m_writer.declLocale( "tex" + name
					, textureConfigs.getTexcoord( config
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 ) );
				auto sampled = config.computeCommonMapVoxelContribution( m_utils
					, passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texcoord
					, emissive
					, opacity
					, occlusion );
				textureConfigs.setTexcoord( config
					, texcoord
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
				phong::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, phongLightMat );
			}
			FI;
		}

		phong::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, phongLightMat
			, emissive );
	}

	void ToonPhongLightingModel::doComputeLight( c3d::Light const & plight
		, ToonPhongLightMaterial const & pmaterial
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
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection
					, c3d::OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.worldNormal, -lightDirection ) );
					auto delta = m_writer.declLocale( "delta"
						, fwidth( diffuseFactor ) * material.smoothBand );
					diffuseFactor = smoothStep( 0.0_f, delta, diffuseFactor );
					output.m_diffuse = light.colour
						* light.intensity.x()
						* diffuseFactor;

					// Specular term.
					auto vertexToEye = m_writer.declLocale( "vertexToEye"
						, normalize( worldEye - surface.worldPosition ) );

					if ( m_isBlinnPhong )
					{
						auto halfwayDir = m_writer.declLocale( "halfwayDir"
							, normalize( vertexToEye - lightDirection ) );
						m_writer.declLocale( "specularFactor"
							, max( dot( surface.worldNormal, halfwayDir ), 0.0_f ) );
					}
					else
					{
						auto lightReflect = m_writer.declLocale( "lightReflect"
							, normalize( reflect( lightDirection, surface.worldNormal ) ) );
						m_writer.declLocale( "specularFactor"
							, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
					}

					auto specularFactor = m_writer.getVariable< sdw::Float >( "specularFactor" );
					specularFactor = pow( specularFactor * diffuseFactor, clamp( material.shininess, 1.0_f, 256.0_f ) );
					specularFactor = smoothStep( 0.0_f, 0.01_f * material.smoothBand, specularFactor );
					output.m_specular = specularFactor
						* light.colour
						* light.intensity.y();
				}
				, c3d::InLight( m_writer, "light" )
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" )
				, outputs );
		}

		m_computeLight( plight
			, pmaterial
			, psurface
			, pworldEye
			, plightDirection
			, pparentOutput );
	}

	sdw::Vec3 ToonPhongLightingModel::doComputeLightDiffuse( c3d::Light const & plight
		, ToonPhongLightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Vec3 const & plightDirection )
	{
		if ( !m_computeLightDiffuse )
		{
			m_computeLightDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
				, [this]( c3d::Light const & light
					, ToonPhongLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Vec3 const & lightDirection )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.worldNormal, -lightDirection ) );
					auto delta = m_writer.declLocale( "delta"
						, fwidth( diffuseFactor ) * material.smoothBand );
					diffuseFactor = smoothStep( 0.0_f, delta, diffuseFactor );
					m_writer.returnStmt( diffuseFactor
						* light.colour
						* light.intensity.x() );
				}
				, c3d::InLight( m_writer, "light" )
				, InToonPhongLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InVec3( m_writer, "lightDirection" ) );
		}

		return m_computeLightDiffuse( plight
			, pmaterial
			, psurface
			, pworldEye
			, plightDirection );
	}

	//*********************************************************************************************

	ToonBlinnPhongLightingModel::ToonBlinnPhongLightingModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: ToonPhongLightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, true }
	{
	}

	c3d::LightingModelPtr ToonBlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonBlinnPhongLightingModel >( writer
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

	namespace pbr
	{
		namespace
		{
			struct MaterialTextureMods
			{
				sdw::Boolean hasAlbedo;
				sdw::Boolean hasMetalness;
				sdw::Boolean hasSpecular;
				sdw::Boolean hasEmissive;
			};

			void modifyMaterial( sdw::ShaderWriter & writer
				, castor::String const & configName
				, castor3d::PassFlags const & passFlags
				, castor3d::TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, c3d::TextureConfigData const & config
				, ToonPbrLightMaterial & pbrLightMat )
			{
				config.applyAlbedo( textureFlags, sampled, pbrLightMat.albedo );
				config.applySpecular( textureFlags, sampled, pbrLightMat.specular );

				if ( checkFlag( textureFlags, castor3d::TextureFlag::eGlossiness ) )
				{
					IF( writer, config.isGlossiness() )
					{
						auto gloss = writer.declLocale( "gloss" + configName
							, castor3d::shader::LightMaterial::computeRoughness( pbrLightMat.roughness ) );
						gloss = config.getGlossiness( sampled, gloss );
						pbrLightMat.roughness = castor3d::shader::LightMaterial::computeRoughness( gloss );
					}
					FI;
				}

				config.applyMetalness( textureFlags, sampled, pbrLightMat.metalness );
				config.applyRoughness( textureFlags, sampled, pbrLightMat.roughness );
			}

			void updateMaterial( sdw::ShaderWriter & writer
				, castor3d::PassFlags const & passFlags
				, castor3d::TextureFlags const & textureFlags
				, ToonPbrLightMaterial & pbrLightMat
				, sdw::Vec3 & emissive )
			{
				if ( !checkFlag( textureFlags, castor3d::TextureFlag::eMetalness )
					&& ( checkFlag( textureFlags, castor3d::TextureFlag::eSpecular ) || checkFlag( textureFlags, castor3d::TextureFlag::eAlbedo ) ) )
				{
					pbrLightMat.metalness = castor3d::shader::LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
				}

				if ( !checkFlag( textureFlags, castor3d::TextureFlag::eSpecular )
					&& ( checkFlag( textureFlags, castor3d::TextureFlag::eMetalness ) || checkFlag( textureFlags, castor3d::TextureFlag::eAlbedo ) ) )
				{
					pbrLightMat.specular = castor3d::shader::LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
				}

				if ( checkFlag( passFlags, castor3d::PassFlag::eLighting )
					&& !checkFlag( textureFlags, castor3d::TextureFlag::eEmissive ) )
				{
					emissive *= pbrLightMat.albedo;
				}
			}
		}
	}

	ToonPbrLightingModel::ToonPbrLightingModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: c3d::LightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, std::string{ "c3d_pbr_toon_" } }
		, m_cookTorrance{ writer, utils }
	{
	}

	const castor::String ToonPbrLightingModel::getName()
	{
		return cuT( "c3d.toon.pbr" );
	}

	c3d::LightingModelPtr ToonPbrLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, c3d::SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< ToonPbrLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	std::unique_ptr< c3d::LightMaterial > ToonPbrLightingModel::declMaterial( std::string const & name
		, bool enabled )
	{
		return m_writer.declDerivedLocale< c3d::LightMaterial, ToonPbrLightMaterial >( name, enabled );
	}

	sdw::Vec3 ToonPbrLightingModel::combine( sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
		, sdw::Vec3 const & directScattering
		, sdw::Vec3 const & indirectSpecular
		, sdw::Vec3 const & ambient
		, sdw::Vec3 const & indirectAmbient
		, sdw::Float const & ambientOcclusion
		, sdw::Vec3 const & emissive
		, sdw::Vec3 const & reflected
		, sdw::Vec3 const & refracted
		, sdw::Vec3 const & materialAlbedo )
	{
		return materialAlbedo * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
			+ directSpecular + ( indirectSpecular * ambientOcclusion )
			+ emissive
			+ refracted
			+ ( reflected * ambient * indirectAmbient * ambientOcclusion )
			+ directScattering;
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
		, c3d::LightMaterial const & pmaterial
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
					, ToonPbrLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					m_cookTorrance.computeAON( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
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
									, m_getCascadeFactors( sdw::Vec3{ surface.viewPosition }
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
								}
								FI;
							}

#if C3D_DebugCascades
							IF( m_writer, cascadeIndex == 0_u )
							{
								output.m_diffuse.rgb() *= vec3( 1.0_f, 0.25f, 0.25f );
								output.m_specular.rgb() *= vec3( 1.0_f, 0.25f, 0.25f );
							}
							ELSEIF( cascadeIndex == 1_u )
							{
								output.m_diffuse.rgb() *= vec3( 0.25_f, 1.0f, 0.25f );
								output.m_specular.rgb() *= vec3( 0.25_f, 1.0f, 0.25f );
							}
							ELSEIF( cascadeIndex == 2_u )
							{
								output.m_diffuse.rgb() *= vec3( 0.25_f, 0.25f, 1.0f );
								output.m_specular.rgb() *= vec3( 0.25_f, 0.25f, 1.0f );
							}
							ELSE
							{
								output.m_diffuse.rgb() *= vec3( 1.0_f, 1.0f, 0.25f );
								output.m_specular.rgb() *= vec3( 1.0_f, 1.0f, 0.25f );
							}
							FI;
#endif
						}
						FI;
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
				}
				, c3d::InDirectionalLight( m_writer, "light" )
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeDirectional( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPbrLightingModel::compute( c3d::PointLight const & plight
		, c3d::LightMaterial const & pmaterial
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
					, ToonPbrLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					m_cookTorrance.computeAON( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
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
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, light.getAttenuationFactor( distance ) );
					output.m_diffuse = output.m_diffuse / attenuation;
					output.m_specular = output.m_specular / attenuation;
					output.m_scattering = output.m_scattering / attenuation;
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
					parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
				}
				, c3d::InPointLight( m_writer, "light" )
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computePoint( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPbrLightingModel::compute( c3d::SpotLight const & plight
		, c3d::LightMaterial const & pmaterial
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
					, ToonPbrLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows
					, c3d::OutputComponents & parentOutput )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( lightToVertex ) );
						c3d::OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
						m_cookTorrance.computeAON( light.base
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, material.smoothBand
							, surface
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
										, -lightToVertex ) );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;
						}

						auto attenuation = m_writer.declLocale( "attenuation"
							, light.getAttenuationFactor( distance ) );
						spotFactor = clamp( ( spotFactor - light.outerCutOff ) / light.cutOffsDiff, 0.0_f, 1.0_f );
						output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
						output.m_specular = spotFactor * output.m_specular / attenuation;
						output.m_scattering = spotFactor * output.m_scattering / attenuation;
						parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
						parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
						parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					}
					FI;
				}
				, c3d::InSpotLight( m_writer, "light" )
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeSpot( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void ToonPbrLightingModel::computeMapContributions( castor3d::PassFlags const & passFlags
		, castor3d::TextureFlags const & textureFlags
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, c3d::LightMaterial & lightMat
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto & pbrLightMat = static_cast< ToonPbrLightMaterial & >( lightMat );

		if ( !textureConfigs.isEnabled() )
		{
			pbr::updateMaterial( m_writer
				, passFlags
				, textureFlags
				, pbrLightMat
				, emissive );
			return;
		}

		pbr::MaterialTextureMods mods{ m_writer.declLocale( "hasAlbedo", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasMetalness", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasSpecular", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasEmissive", sdw::Boolean{ false } ) };

		for ( uint32_t index = 0u; index < textureFlags.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, material.getTexture( index ) );

			IF( m_writer, id > 0_u )
			{
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( id ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( id ) );
				auto texcoord = m_writer.declLocale( "tex" + name
					, textureConfigs.getTexcoord( config
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 ) );
				auto sampled = config.computeCommonMapContribution( m_utils
					, passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texcoord
					, emissive
					, opacity
					, occlusion
					, transmittance
					, normal
					, tangent
					, bitangent
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
				textureConfigs.setTexcoord( config
					, texcoord
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
				pbr::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		pbr::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::DirectionalLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( c3d::DirectionalLight light
					, ToonPbrLightMaterial const & material
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
							, material.specular
							, material.getMetalness()
							, material.smoothBand
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
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::PointLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if (!m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( c3d::PointLight light
					, ToonPbrLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto diffuse = m_writer.declLocale( "diffuse"
						, m_cookTorrance.computeDiffuseAON( light.base
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.smoothBand
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
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::SpotLight const & plight
		, c3d::LightMaterial const & pmaterial
		, c3d::Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::UInt const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( c3d::SpotLight light
					, ToonPbrLightMaterial const & material
					, c3d::Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::UInt const & receivesShadows )
				{
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition );
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
							, material.specular
							, material.getMetalness()
							, material.smoothBand
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
				, InToonPbrLightMaterial{ m_writer, "material" }
				, c3d::InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InUInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, static_cast< ToonPbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	void ToonPbrLightingModel::computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
		, castor3d::TextureFlags const & textureFlags
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, c3d::Material const & material
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, c3d::LightMaterial & lightMat )
	{
		auto & pbrLightMat = static_cast< ToonPbrLightMaterial & >( lightMat );

		if ( !textureConfigs.isEnabled() )
		{
			pbr::updateMaterial( m_writer
				, passFlags
				, textureFlags
				, pbrLightMat
				, emissive );
			return;
		}

		pbr::MaterialTextureMods mods{ m_writer.declLocale( "hasAlbedo", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasMetalness", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasSpecular", sdw::Boolean{ false } )
			, m_writer.declLocale( "hasEmissive", sdw::Boolean{ false } ) };

		for ( uint32_t index = 0u; index < textureFlags.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, material.getTexture( index ) );

			IF( m_writer, id > 0_u )
			{
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( id ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( id ) );
				auto texcoord = m_writer.declLocale( "tex" + name
					, textureConfigs.getTexcoord( config
						, texCoords0
						, texCoords1
						, texCoords2
						, texCoords3 ) );
				auto sampled = config.computeCommonMapVoxelContribution( m_utils
					, passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texcoord
					, emissive
					, opacity
					, occlusion );
				textureConfigs.setTexcoord( config
					, texcoord
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
				pbr::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		pbr::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	//*********************************************************************************************
}
