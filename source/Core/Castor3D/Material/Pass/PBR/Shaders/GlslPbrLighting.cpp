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
	namespace
	{
		void modifyMaterial( sdw::ShaderWriter & writer
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

		void updateMaterial( sdw::ShaderWriter & writer
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, PbrLightMaterial & pbrLightMat
			, sdw::Vec3 & emissive )
		{
			if ( pbrLightMat.isSpecularGlossiness() )
			{
				if ( !checkFlag( textureFlags, TextureFlag::eMetalness )
					&& ( checkFlag( textureFlags, TextureFlag::eSpecular ) || checkFlag( textureFlags, TextureFlag::eAlbedo ) ) )
				{
					pbrLightMat.metalness = LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
				}
			}
			else
			{
				if ( !checkFlag( textureFlags, TextureFlag::eSpecular  )
					&& ( checkFlag( textureFlags, TextureFlag::eMetalness ) || checkFlag( textureFlags, TextureFlag::eAlbedo ) ) )
				{
					pbrLightMat.specular = LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
				}
			}

			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !checkFlag( textureFlags, castor3d::TextureFlag::eEmissive ) )
			{
				emissive *= pbrLightMat.albedo;
			}
		}
	}

	PbrLightingModel::PbrLightingModel( bool isSpecularGlossiness
		, sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram )
		: LightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram
			, isSpecularGlossiness ? std::string{ "c3d_pbrsg_" } : std::string{ "c3d_pbrmr_" } }
		, m_isSpecularGlossiness{ isSpecularGlossiness }
		, m_cookTorrance{ writer, utils }
	{
	}

	sdw::Vec3 PbrLightingModel::combine( sdw::Vec3 const & directDiffuse
		, sdw::Vec3 const & indirectDiffuse
		, sdw::Vec3 const & directSpecular
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
			+ ( reflected * ambient * indirectAmbient * ambientOcclusion );
	}

	ReflectionModelPtr PbrLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< PbrReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void PbrLightingModel::compute( TiledDirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeTiledDirectional( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PbrLightingModel::compute( DirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeDirectional( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PbrLightingModel::compute( PointLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computePoint( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PbrLightingModel::compute( SpotLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeSpot( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PbrLightingModel::computeMapContributions( PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
		, sdw::Vec3 & texCoords
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
				auto sampled = config.computeCommonMapContribution( m_utils
					, passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texCoords
					, emissive
					, opacity
					, occlusion
					, transmittance
					, normal
					, tangent
					, bitangent
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
				modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( TiledDirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeTiledDirectionalDiffuse( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( DirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeDirectionalDiffuse( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( PointLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computePointDiffuse( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PbrLightingModel::computeDiffuse( SpotLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeSpotDiffuse( light
			, static_cast< PbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	void PbrLightingModel::computeMapDiffuseContributions( PassFlags const & passFlags
		, TextureFlagsArray const & textures
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::UVec4 const & textures0
		, sdw::UVec4 const & textures1
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, LightMaterial & lightMat )
	{
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
				auto sampled = config.computeCommonMapVoxelContribution( passFlags
					, textureFlags
					, name
					, anim
					, maps[id - 1_u]
					, texCoords
					, emissive
					, opacity
					, occlusion );
				modifyMaterial( m_writer
					, name
					, passFlags
					, textureFlags
					, sampled
					, config
					, pbrLightMat );
			}
			FI;
		}

		updateMaterial( m_writer
			, passFlags
			, textureFlags
			, pbrLightMat
			, emissive );
	}

	void PbrLightingModel::doDeclareModel()
	{
	}

	void PbrLightingModel::doDeclareComputeTiledDirectionalLight()
	{
		OutputComponents outputs{ m_writer };
		m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( TiledDirectionalLight const & light
				, PbrLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, OutputComponents & parentOutput )
			{
				OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.direction ) );
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
								, m_getTileFactors( sdw::Vec3{ surface.viewPosition }
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

						IF( m_writer, receivesShadows != 0_i )
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

						if ( m_isOpaqueProgram )
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
			}
			, InTiledDirectionalLight( m_writer, "light" )
			, InPbrLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PbrLightingModel::doDeclareComputeDirectionalLight()
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
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.direction ) );
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

						if ( m_isOpaqueProgram )
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
			}
			, InDirectionalLight( m_writer, "light" )
			, InPbrLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PbrLightingModel::doDeclareComputePointLight()
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
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto vertexToLight = m_writer.declLocale( "vertexToLight"
					, light.position - surface.worldPosition );
				auto distance = m_writer.declLocale( "distance"
					, length( vertexToLight ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( vertexToLight ) );
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
				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, InPointLight( m_writer, "light" )
			, InPbrLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PbrLightingModel::doDeclareComputeSpotLight()
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

				IF( m_writer, spotFactor > light.cutOff )
				{
					auto distance = m_writer.declLocale( "distance"
						, length( vertexToLight ) );
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
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
					spotFactor = 1.0_f - ( 1.0_f - spotFactor ) * ( 1.0_f / ( 1.0_f - light.cutOff ) );
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
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
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

	void PbrLightingModel::doDeclareDiffuseModel()
	{
	}

	void PbrLightingModel::doDeclareComputeTiledDirectionalLightDiffuse()
	{
		m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( TiledDirectionalLight light
				, PbrLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.direction ) );
				auto diffuse = m_writer.declLocale( "diffuse"
					, m_cookTorrance.computeDiffuse( light.base
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, surface ) );

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
			, InOutTiledDirectionalLight( m_writer, "light" )
			, InPbrLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void PbrLightingModel::doDeclareComputeDirectionalLightDiffuse()
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

	void PbrLightingModel::doDeclareComputePointLightDiffuse()
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

	void PbrLightingModel::doDeclareComputeSpotLightDiffuse()
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

				IF( m_writer, spotFactor > light.cutOff )
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
					spotFactor = 1.0_f - ( 1.0_f - spotFactor ) * ( 1.0_f / ( 1.0_f - light.cutOff ) );
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

	//***********************************************************************************************

	PbrMRLightingModel::PbrMRLightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram )
		: PbrLightingModel{ false
			, writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram }
	{
	}

	const castor::String PbrMRLightingModel::getName()
	{
		return cuT( "c3d.pbrmr" );
	}

	LightingModelPtr PbrMRLightingModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram )
	{
		return std::make_unique< PbrMRLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram );
	}

	std::unique_ptr< LightMaterial > PbrMRLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< LightMaterial, PbrMRLightMaterial >( name );
	}

	//***********************************************************************************************

	PbrSGLightingModel::PbrSGLightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram )
		: PbrLightingModel{ true
			, writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram }
	{
	}

	const castor::String PbrSGLightingModel::getName()
	{
		return cuT( "c3d.pbrsg" );
	}

	LightingModelPtr PbrSGLightingModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram )
	{
		return std::make_unique< PbrSGLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram );
	}

	std::unique_ptr< LightMaterial > PbrSGLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< LightMaterial, PbrSGLightMaterial >( name );
	}

	//***********************************************************************************************
}
