#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Material/Pass/Phong/Shaders/GlslPhongMaterial.hpp"
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

	namespace
	{
		void modifyMaterial( sdw::ShaderWriter & writer
			, castor::String const & configName
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, sdw::Vec4 const & sampled
			, TextureConfigData const & config
			, PhongLightMaterial & phongLightMat )
		{
			config.applyAlbedo( textureFlags, sampled, phongLightMat.albedo );
			config.applySpecular( textureFlags, sampled, phongLightMat.specular );
			config.applyShininess( textureFlags, sampled, phongLightMat.shininess );
		}

		void updateMaterial( sdw::ShaderWriter & writer
			, PassFlags const & passFlags
			, TextureFlags const & textureFlags
			, PhongLightMaterial & phongLightMat
			, sdw::Vec3 & emissive )
		{
			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !checkFlag( textureFlags, castor3d::TextureFlag::eEmissive ) )
			{
				emissive *= phongLightMat.albedo;
			}
		}
	}

	//*********************************************************************************************

	castor::String PhongLightingModel::getName()
	{
		return cuT( "c3d.phong" );
	}

	PhongLightingModel::PhongLightingModel( sdw::ShaderWriter & m_writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram
		, bool hasSsbo
		, bool isBlinnPhong )
		: LightingModel{ m_writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram
			, hasSsbo
			, isBlinnPhong ? std::string{ "c3d_blinnphong_" } : std::string{ "c3d_phong_" } }
		, m_isBlinnPhong{ isBlinnPhong }
	{
	}

	LightingModelPtr PhongLightingModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram
		, bool hasSsbo )
	{
		return std::make_unique< PhongLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram
			, hasSsbo
			, false );
	}

	sdw::Vec3 PhongLightingModel::combine( sdw::Vec3 const & directDiffuse
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
			+ ( directSpecular + ( indirectSpecular * ambientOcclusion ) )
			+ ( ambient * indirectAmbient * ambientOcclusion )
			+ emissive
			+ refracted
			+ reflected * ambientOcclusion;
	}

	std::unique_ptr< LightMaterial > PhongLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< LightMaterial, PhongLightMaterial >( name );
	}

	ReflectionModelPtr PhongLightingModel::getReflectionModel( uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< PhongReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void PhongLightingModel::compute( TiledDirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeTiledDirectional( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PhongLightingModel::compute( DirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeDirectional( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PhongLightingModel::compute( PointLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computePoint( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PhongLightingModel::compute( SpotLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, OutputComponents & parentOutput )const
	{
		m_computeSpot( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void PhongLightingModel::computeMapContributions( PassFlags const & passFlags
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
		auto & phongLightMat = static_cast< PhongLightMaterial & >( lightMat );
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
					, phongLightMat );
			}
			FI;
		}

		updateMaterial( m_writer
			, passFlags
			, textureFlags
			, phongLightMat
			, emissive );
	}

	sdw::Vec3 PhongLightingModel::computeDiffuse( TiledDirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeTiledDirectionalDiffuse( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PhongLightingModel::computeDiffuse( DirectionalLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeDirectionalDiffuse( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PhongLightingModel::computeDiffuse( PointLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computePointDiffuse( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 PhongLightingModel::computeDiffuse( SpotLight const & light
		, LightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeSpotDiffuse( light
			, static_cast< PhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	void PhongLightingModel::computeMapDiffuseContributions( PassFlags const & passFlags
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
		auto & phongLightMat = static_cast< PhongLightMaterial & >( lightMat );
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
					, phongLightMat );
			}
			FI;
		}

		updateMaterial( m_writer
			, passFlags
			, textureFlags
			, phongLightMat
			, emissive );
	}

	void PhongLightingModel::doDeclareModel()
	{
		doDeclareComputeLight();
	}

	void PhongLightingModel::doDeclareComputeTiledDirectionalLight()
	{
		OutputComponents outputs{ m_writer };
		m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( TiledDirectionalLight const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, OutputComponents & parentOutput )
			{
				OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );
				doComputeLight( light.m_lightBase
					, material
					, surface
					, worldEye
					, lightDirection
					, output );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, 0_u );
						auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< sdw::UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

						// Get cascade index for the current fragment's view position
						FOR( m_writer, sdw::UInt, i, 0u, i < maxCount, ++i )
						{
							auto factors = m_writer.declLocale( "factors"
								, m_getTileFactors( sdw::Vec3{ surface.viewPosition }
									, light.m_splitDepths
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
									* m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex - 1u]
										, -lightDirection
										, cascadeIndex - 1u
										, light.m_cascadeCount );
							}
							FI;

							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;

						if ( m_isOpaqueProgram )
						{
							IF( m_writer, light.m_lightBase.m_volumetricSteps != 0_u )
							{
								m_shadowModel->computeVolumetric( light.m_lightBase
									, surface
									, worldEye
									, light.m_transforms[cascadeIndex]
									, light.m_direction
									, cascadeIndex
									, light.m_cascadeCount
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
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PhongLightingModel::doDeclareComputeDirectionalLight()
	{
		OutputComponents outputs{ m_writer };
		m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( DirectionalLight const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, OutputComponents & parentOutput )
			{
				OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );
				doComputeLight( light.m_lightBase
					, material
					, surface
					, worldEye
					, lightDirection
					, output );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, 0_u );
						auto c3d_maxCascadeCount = m_writer.getVariable< sdw::UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< sdw::UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

						// Get cascade index for the current fragment's view position
						FOR( m_writer, sdw::UInt, i, 0u, i < maxCount, ++i )
						{
							auto factors = m_writer.declLocale( "factors"
								, m_getCascadeFactors( sdw::Vec3{ surface.viewPosition }
									, light.m_splitDepths
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
									* m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex - 1u]
										, -lightDirection
										, cascadeIndex - 1u
										, light.m_cascadeCount );
							}
							FI;

							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;

						if ( m_isOpaqueProgram )
						{
							IF( m_writer, light.m_lightBase.m_volumetricSteps != 0_u )
							{
								m_shadowModel->computeVolumetric( light.m_lightBase
									, surface
									, worldEye
									, light.m_transforms[cascadeIndex]
									, light.m_direction
									, cascadeIndex
									, light.m_cascadeCount
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
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PhongLightingModel::doDeclareComputePointLight()
	{
		OutputComponents outputs{ m_writer };
		m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
			, [this]( PointLight const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, OutputComponents & parentOutput )
			{
				OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
					, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				doComputeLight( light.m_lightBase
					, material
					, surface
					, worldEye
					, lightDirection
					, output );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
							&& light.m_lightBase.m_index >= 0_i
							&& receivesShadows != 0_i )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, m_shadowModel->computePoint( light.m_lightBase
								, surface
								, light.m_position.xyz() ) );
						output.m_diffuse *= shadowFactor;
						output.m_specular *= shadowFactor;
					}
					FI;
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				output.m_diffuse = output.m_diffuse / attenuation;
				output.m_specular = output.m_specular / attenuation;
				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, InPointLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PhongLightingModel::doDeclareComputeSpotLight()
	{
		OutputComponents outputs{ m_writer };
		m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
			, [this]( SpotLight const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, OutputComponents & parentOutput )
			{
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, light.m_direction ) );

				IF( m_writer, spotFactor > light.m_cutOff )
				{
					auto distance = m_writer.declLocale( "distLightToVertex"
						, length( lightToVertex ) );
					OutputComponents output{ m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) )
						, m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) ) };
					auto rawDiffuse = doComputeLight( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection
						, output );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& light.m_lightBase.m_index >= 0_i
								&& receivesShadows != 0_i )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, lightToVertex ) );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					spotFactor = 1.0_f - ( 1.0_f - spotFactor ) * ( 1.0_f / ( 1.0_f - light.m_cutOff ) );
					output.m_diffuse = spotFactor * output.m_diffuse / attenuation;

#if !C3D_DisableSSSTransmittance
					if ( m_shadowModel->isEnabled() && m_sssTransmittance )
					{
						IF( m_writer
							, ( light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) ) )
							&& ( light.m_lightBase.m_index >= 0_i )
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
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void PhongLightingModel::doDeclareComputeLight()
	{
		OutputComponents outputs{ m_writer };
		m_computeLight = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
			, [this]( Light const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & lightDirection
				, OutputComponents & output )
			{
				// Diffuse term.
				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, dot( surface.worldNormal, -lightDirection ) );
				auto isLit = m_writer.declLocale( "isLit"
					, 1.0_f - step( diffuseFactor, 0.0_f ) );
				auto result = m_writer.declLocale( "result"
					, light.m_colour
					* light.m_intensity.x() );
				output.m_diffuse = isLit
					* result
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
				output.m_specular = isLit
					* light.m_colour
					* light.m_intensity.y()
					* pow( specularFactor, clamp( material.shininess, 1.0_f, 256.0_f ) );

				m_writer.returnStmt( result );
			}
			, InLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "lightDirection" )
			, outputs );
	}

	sdw::Vec3 PhongLightingModel::doComputeLight( Light const & light
		, PhongLightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & lightDirection
		, OutputComponents & parentOutput )
	{
		return m_computeLight( light
			, material
			, surface
			, worldEye
			, lightDirection
			, parentOutput );
	}

	void PhongLightingModel::doDeclareDiffuseModel()
	{
		doDeclareComputeLightDiffuse();
	}

	void PhongLightingModel::doDeclareComputeTiledDirectionalLightDiffuse()
	{
		m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( TiledDirectionalLight light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );
				auto diffuse = m_writer.declLocale( "diffuse"
					, doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
							&& receivesShadows != 0_i )
					{
						light.m_lightBase.m_intensityFarPlane.w() = sdw::Float( float( castor3d::ShadowType::eRaw ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, m_shadowModel->computeDirectional( light.m_lightBase
								, surface
								, light.m_transforms[cascadeIndex]
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount ) );
						diffuse *= shadowFactor;
					}
					FI;
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, InOutTiledDirectionalLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void PhongLightingModel::doDeclareComputeDirectionalLightDiffuse()
	{
		m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( DirectionalLight light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );
				auto diffuse = m_writer.declLocale( "diffuse"
					, doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
							&& receivesShadows != 0_i )
					{
						light.m_lightBase.m_intensityFarPlane.w() = sdw::Float( float( castor3d::ShadowType::eRaw ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, m_shadowModel->computeDirectional( light.m_lightBase
								, surface
								, light.m_transforms[cascadeIndex]
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount ) );
						diffuse *= shadowFactor;
					}
					FI;
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, InOutDirectionalLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void PhongLightingModel::doDeclareComputePointLightDiffuse()
	{
		m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
			, [this]( PointLight light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto diffuse = m_writer.declLocale( "diffuse"
					, doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer
						, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
							&& light.m_lightBase.m_index >= 0_i
							&& receivesShadows != 0_i )
					{
						light.m_lightBase.m_intensityFarPlane.w() = sdw::Float( float( castor3d::ShadowType::eRaw ) );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, m_shadowModel->computePoint( light.m_lightBase
								, surface
								, light.m_position.xyz() ) );
						diffuse *= shadowFactor;
					}
					FI;
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse / attenuation ) );
			}
			, InOutPointLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void PhongLightingModel::doDeclareComputeSpotLightDiffuse()
	{
		m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
			, [this]( SpotLight light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, light.m_direction ) );
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );

				IF( m_writer, spotFactor > light.m_cutOff )
				{
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer
							, light.m_lightBase.m_shadowType != sdw::Int( int( ShadowType::eNone ) )
								&& light.m_lightBase.m_index >= 0_i
								&& receivesShadows != 0_i )
						{
							light.m_lightBase.m_intensityFarPlane.w() = sdw::Float( float( castor3d::ShadowType::eRaw ) );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, lightToVertex ) );
							diffuse *= shadowFactor;
						}
						FI;
					}

					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					spotFactor = 1.0_f - ( 1.0_f - spotFactor ) * ( 1.0_f / ( 1.0_f - light.m_cutOff ) );
					diffuse = max( vec3( 0.0_f ), spotFactor * diffuse / attenuation );
				}
				FI;

				m_writer.returnStmt( diffuse );
			}
			, InOutSpotLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void PhongLightingModel::doDeclareComputeLightDiffuse()
	{
		m_computeLightDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "doComputeLight"
			, [this]( Light const & light
				, PhongLightMaterial const & material
				, Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & lightDirection )
			{
				// Diffuse term.
				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, dot( surface.worldNormal, -lightDirection ) );
				auto isLit = m_writer.declLocale( "isLit"
					, 1.0_f - step( diffuseFactor, 0.0_f ) );
				m_writer.returnStmt( isLit
					* light.m_colour
					* light.m_intensity.x()
					* diffuseFactor );
			}
			, InLight( m_writer, "light" )
			, InPhongLightMaterial{ m_writer, "material" }
			, InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "lightDirection" ) );
	}

	sdw::Vec3 PhongLightingModel::doComputeLightDiffuse( Light const & light
		, PhongLightMaterial const & material
		, Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & lightDirection )
	{
		return m_computeLightDiffuse( light
			, material
			, surface
			, worldEye
			, lightDirection );
	}

	//*********************************************************************************************

	BlinnPhongLightingModel::BlinnPhongLightingModel( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram
		, bool hasSsbo )
		: PhongLightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram
			, hasSsbo
			, true }
	{
	}

	LightingModelPtr BlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowOptions
		, SssProfiles const * sssProfiles
		, bool isOpaqueProgram
		, bool hasSsbo )
	{
		return std::make_unique< BlinnPhongLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, sssProfiles
			, isOpaqueProgram
			, hasSsbo );
	}

	castor::String BlinnPhongLightingModel::getName()
	{
		return cuT( "c3d.blinn_phong" );
	}

	//*********************************************************************************************
}
