#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Material/Pass/PBR/Shaders/GlslPbrMaterial.hpp"
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
	namespace pbrlgt
	{
		static void modifyMaterial( sdw::ShaderWriter & writer
			, castor::String const & configName
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, TextureConfigData const & config
			, PbrLightMaterial & pbrLightMat )
		{
			config.applyAlbedo( textureFlags, sampled, pbrLightMat.albedo );
			config.applySpecular( textureFlags, sampled, pbrLightMat.specular );

			if ( checkFlag( textureFlags, TextureFlag::eGlossiness ) )
			{
				IF( writer, config.isGlossiness() )
				{
					auto gloss = writer.declLocale( "gloss" + configName
						, LightMaterial::computeRoughness( pbrLightMat.roughness ) );
					gloss = config.getGlossiness( sampled, gloss );
					pbrLightMat.roughness = LightMaterial::computeRoughness( gloss );
				}
				FI;
			}

			config.applyMetalness( textureFlags, sampled, pbrLightMat.metalness );
			config.applyRoughness( textureFlags, sampled, pbrLightMat.roughness );
		}

		static void updateMaterial( sdw::ShaderWriter & writer
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, PbrLightMaterial & pbrLightMat
			, sdw::Vec3 & emissive )
		{
			if ( checkFlag( textureFlags, TextureFlag::eMetalness )
				&& !checkFlag( textureFlags, TextureFlag::eSpecular ) )
			{
				pbrLightMat.specular = LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
			}

			if ( checkFlag( textureFlags, TextureFlag::eSpecular )
				&& !checkFlag( textureFlags, TextureFlag::eMetalness ) )
			{
				pbrLightMat.metalness = LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
			}

			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !checkFlag( textureFlags, castor3d::TextureFlag::eEmissive ) )
			{
				emissive *= pbrLightMat.albedo;
			}
		}
	}

	PbrLightingModel::PbrLightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
		: LightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric
			, "c3d_pbr_" }
		, m_cookTorrance{ writer, utils }
	{
	}

	const castor::String PbrLightingModel::getName()
	{
		return cuT( "c3d.pbr" );
	}

	LightingModelPtr PbrLightingModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return std::make_unique< PbrLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, enableVolumetric );
	}

	std::unique_ptr< LightMaterial > PbrLightingModel::declMaterial( std::string const & name
		, bool enabled )
	{
		return m_writer.declDerivedLocale< LightMaterial, PbrLightMaterial >( name, enabled );
	}

	sdw::Vec3 PbrLightingModel::combine( sdw::Vec3 const & directDiffuse
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

	ReflectionModelPtr PbrLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< PbrReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void PbrLightingModel::compute( DirectionalLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, BackgroundModel & pbackground
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeDirectional )
		{
			OutputComponents outputs{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					m_cookTorrance.compute( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, surface
						, output );

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

							IF ( m_writer, receivesShadows != 0_i )
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
									m_shadowModel->computeVolumetric( light.base
										, surface
										, worldEye
										, light.transforms[cascadeIndex]
										, light.direction
										, cascadeIndex
										, light.cascadeCount
										, output );
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
				, InDirectionalLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeDirectional( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void PbrLightingModel::compute( PointLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computePoint )
		{
			OutputComponents outputs{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
				, [this]( PointLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, light.position - surface.worldPosition );
					auto distance = m_writer.declLocale( "distance"
						, length( vertexToLight ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( vertexToLight ) );
					m_cookTorrance.compute( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, surface
						, output );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
								&& ( light.base.index >= 0_i )
								&& ( receivesShadows != 0_i ) )
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
				, InPointLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computePoint( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void PbrLightingModel::compute( SpotLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows
		, OutputComponents & pparentOutput )
	{
		if ( !m_computeSpot )
		{
			OutputComponents outputs{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
				, [this]( SpotLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					auto vertexToLight = m_writer.declLocale( "vertexToLight"
						, light.position - surface.worldPosition );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( vertexToLight ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.direction ) );

					IF( m_writer, spotFactor > light.outerCutOff )
					{
						auto distance = m_writer.declLocale( "distance"
							, length( vertexToLight ) );
						OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
							, m_writer.declLocale( "lightScattering", vec3( 0.0_f ) ) };
						auto rawDiffuse = m_writer.declLocale( "rawDiffuse"
							, m_cookTorrance.compute( light.base
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, surface
								, output ) );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, ( light.base.shadowType != sdw::Int( int( ShadowType::eNone ) ) )
									&& ( light.base.index >= 0_i )
									&& ( receivesShadows != 0_i ) )
							{
								auto shadowFactor = m_writer.declLocale( "shadowFactor"
									, m_shadowModel->computeSpot( light.base
									, surface
									, light.transform
									, -vertexToLight ) );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
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
						parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
						parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
						parentOutput.m_scattering += max( vec3( 0.0_f ), output.m_scattering );
					}
					FI;
				}
				, InSpotLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" )
				, outputs );
		}

		m_computeSpot( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows
			, pparentOutput );
	}

	void PbrLightingModel::computeMapContributions( PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
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
		, LightMaterial & lightMat
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		if ( !textureConfigs.isEnabled() )
		{
			return;
		}

		auto & pbrLightMat = static_cast< PbrLightMaterial & >( lightMat );
		auto textureFlags = merge( textures );

		for ( uint32_t index = 0u; index < textures.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, ModelIndices::getTexture( textures0, textures1, index ) );

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
				pbrlgt::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		pbrlgt::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( DirectionalLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows )
	{
		if ( !m_computeDirectionalDiffuse )
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
				, [this]( DirectionalLight light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.direction ) );
					diffuse = m_cookTorrance.computeDiffuse( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, surface );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& receivesShadows != 0_i )
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
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" ) );
		}

		return m_computeDirectionalDiffuse( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( PointLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows )
	{
		if ( !m_computePointDiffuse )
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
				, [this]( PointLight light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.position - surface.worldPosition );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					diffuse = m_cookTorrance.computeDiffuse( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, surface );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& light.base.index >= 0_i
								&& receivesShadows != 0_i )
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
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" ) );
		}

		return m_computePointDiffuse( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( SpotLight const & plight
		, LightMaterial const & pmaterial
		, Surface const & psurface
		, sdw::Vec3 const & pworldEye
		, sdw::Int const & preceivesShadows )
	{
		if ( !m_computeSpotDiffuse )
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
				, [this]( SpotLight light
					, PbrLightMaterial const & material
					, Surface const & surface
					, sdw::Vec3 const & worldEye
					, sdw::Int const & receivesShadows )
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
						diffuse = m_cookTorrance.computeDiffuse( light.base
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, surface );

						if ( m_shadowModel->isEnabled() )
						{
							IF( m_writer
								, light.base.shadowType != sdw::Int( int( ShadowType::eNone ) )
									&& light.base.index >= 0_i
									&& receivesShadows != 0_i )
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
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3( m_writer, "worldEye" )
				, sdw::InInt( m_writer, "receivesShadows" ) );
		}

		return m_computeSpotDiffuse( plight
			, static_cast< PbrLightMaterial const & >( pmaterial )
			, psurface
			, pworldEye
			, preceivesShadows );
	}

	void PbrLightingModel::computeMapDiffuseContributions( PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
		, sdw::Vec3 & texCoords0
		, sdw::Vec3 & texCoords1
		, sdw::Vec3 & texCoords2
		, sdw::Vec3 & texCoords3
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, LightMaterial & lightMat )
	{
		if ( !textureConfigs.isEnabled() )
		{
			return;
		}

		auto & pbrLightMat = static_cast< PbrLightMaterial & >( lightMat );
		auto textureFlags = merge( textures );

		for ( uint32_t index = 0u; index < textures.size(); ++index )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( index ) );
			auto id = m_writer.declLocale( "c3d_id" + name
				, ModelIndices::getTexture( textures0, textures1, index ) );

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
				auto sampled = config.computeCommonMapVoxelContribution( passFlags
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
				pbrlgt::modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		pbrlgt::updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	//***********************************************************************************************
}
