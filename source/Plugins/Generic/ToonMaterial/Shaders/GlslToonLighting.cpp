#include "ToonMaterial/Shaders/GlslToonLighting.hpp"

#include "ToonMaterial/Shaders/GlslToonMaterial.hpp"
#include "ToonMaterial/Shaders/GlslToonReflection.hpp"

#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslOutputComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslShadow.hpp>
#include <Castor3D/Shader/Shaders/GlslSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureAnimation.hpp>
#include <Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
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
				, castor3d::TextureFlags const & flags
				, sdw::Vec4 const & sampled
				, c3d::TextureConfigData const & config
				, bool & hasEmissive
				, ToonPhongLightMaterial & phongLightMat )
			{
				if ( checkFlag( flags, castor3d::TextureFlag::eDiffuse ) )
				{
					phongLightMat.albedo = config.getDiffuse( sampled, phongLightMat.albedo );
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eSpecular ) )
				{
					phongLightMat.specular = config.getSpecular( sampled, phongLightMat.specular );
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eShininess ) )
				{
					phongLightMat.shininess = config.getShininess( sampled, phongLightMat.shininess );
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eEmissive ) )
				{
					hasEmissive = true;
				}
			}

			void updateMaterial( castor3d::PassFlags const & passFlags
				, bool hasEmissive
				, ToonPhongLightMaterial & phongLightMat
				, sdw::Vec3 & emissive )
			{
				if ( checkFlag( passFlags, castor3d::PassFlag::eLighting )
					&& !hasEmissive )
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
		, bool isOpaqueProgram
		, bool isBlinnPhong )
		: c3d::PhongLightingModel{ m_writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram
			, isBlinnPhong }
	{
		m_prefix = m_prefix + "toon_";
	}

	c3d::LightingModelPtr ToonPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
	{
		return std::make_unique< ToonPhongLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram
			, false );
	}

	sdw::Vec3 ToonPhongLightingModel::combine( sdw::Vec3 const & directDiffuse
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

	std::unique_ptr< c3d::LightMaterial > ToonPhongLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< c3d::LightMaterial, ToonPhongLightMaterial >( name );
	}

	c3d::ReflectionModelPtr ToonPhongLightingModel::getReflectionModel( castor3d::PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< ToonPhongReflectionModel >( m_writer
			, m_utils
			, passFlags
			, envMapBinding
			, envMapSet );
	}

	c3d::ReflectionModelPtr ToonPhongLightingModel::getReflectionModel( uint32_t envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< ToonPhongReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void ToonPhongLightingModel::compute( c3d::TiledDirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeTiledDirectional( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPhongLightingModel::compute( c3d::DirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeDirectional( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPhongLightingModel::compute( c3d::PointLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computePoint( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPhongLightingModel::compute( c3d::SpotLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeSpot( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPhongLightingModel::computeMapContributions( castor3d::PassFlags const & passFlags
		, castor3d::FilteredTextureFlags const & textures
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 & texCoords
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
		bool hasEmissive = false;
		m_utils.computeGeometryMapsContributions( textures
			, passFlags
			, textureConfigs
			, textureAnims
			, maps
			, texCoords
			, opacity
			, normal
			, tangent
			, bitangent
			, tangentSpaceViewPosition
			, tangentSpaceFragPosition );

		for ( auto & textureIt : textures )
		{
			if ( !c3d::Utils::isGeometryOnlyMap( textureIt.second.flags, passFlags ) )
			{
				auto i = textureIt.first;
				auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapContribution( textureIt.second.flags
						, passFlags
						, name
						, config
						, anim
						, maps[i]
						, texCoords
						, emissive
						, opacity
						, occlusion
						, transmittance
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition ) );
				phong::modifyMaterial( m_writer
					, name
					, passFlags
					, textureIt.second.flags
					, sampled
					, config
					, hasEmissive
					, phongLightMat );
			}
		}

		phong::updateMaterial( passFlags
			, hasEmissive
			, phongLightMat
			, emissive );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::TiledDirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeTiledDirectionalDiffuse( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::DirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeDirectionalDiffuse( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::PointLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computePointDiffuse( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPhongLightingModel::computeDiffuse( c3d::SpotLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeSpotDiffuse( light
			, static_cast< ToonPhongLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	void ToonPhongLightingModel::computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
		, castor3d::FilteredTextureFlags const & textures
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, c3d::LightMaterial & lightMat )
	{
		auto & phongLightMat = static_cast< ToonPhongLightMaterial & >( lightMat );
		bool hasEmissive = false;

		for ( auto & textureIt : textures )
		{
			auto i = textureIt.first;
			auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
			auto config = m_writer.declLocale( "config" + name
				, textureConfigs.getTextureConfiguration( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
			auto anim = m_writer.declLocale( "anim" + name
				, textureAnims.getTextureAnimation( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
			auto sampled = m_writer.declLocale( "sampled" + name
				, m_utils.computeCommonMapVoxelContribution( textureIt.second.flags
					, passFlags
					, name
					, config
					, anim
					, maps[i]
					, texCoords
					, emissive
					, opacity
					, occlusion ) );
			phong::modifyMaterial( m_writer
				, name
				, passFlags
				, textureIt.second.flags
				, sampled
				, config
				, hasEmissive
				, phongLightMat );
		}

		phong::updateMaterial( passFlags
			, hasEmissive
			, phongLightMat
			, emissive );
	}

	void ToonPhongLightingModel::doDeclareModel()
	{
		doDeclareComputeLight();
	}

	void ToonPhongLightingModel::doDeclareComputeTiledDirectionalLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::TiledDirectionalLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );
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
						shadowFactor = cascadeFactors.y()
							* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeDirectional( light.m_lightBase
									, surface
									, light.m_transforms[cascadeIndex]
									, lightDirection
									, cascadeIndex
									, light.m_cascadeCount ) );

						IF( m_writer, cascadeIndex > 0_u )
						{
							shadowFactor += cascadeFactors.z()
								* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex - 1u]
										, -lightDirection
										, cascadeIndex - 1u
										, light.m_cascadeCount ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							doComputeLight( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection
								, output );
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
					}
					ELSE
					{
						doComputeLight( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection
							, output );
					}
					FI;
				}
				else
				{
					doComputeLight( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection
						, output );
				}

				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InTiledDirectionalLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPhongLightingModel::doDeclareComputeDirectionalLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::DirectionalLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );
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
						shadowFactor = cascadeFactors.y()
							* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeDirectional( light.m_lightBase
									, surface
									, light.m_transforms[cascadeIndex]
									, lightDirection
									, cascadeIndex
									, light.m_cascadeCount ) );

						IF( m_writer, cascadeIndex > 0_u )
						{
							shadowFactor += cascadeFactors.z()
								* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex - 1u]
										, -lightDirection
										, cascadeIndex - 1u
										, light.m_cascadeCount ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							doComputeLight( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection
								, output );
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
					}
					ELSE
					{
						doComputeLight( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection
							, output );
					}
					FI;
				}
				else
				{
					doComputeLight( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection
						, output );
				}

				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InDirectionalLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPhongLightingModel::doDeclareComputePointLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
			, [this]( c3d::PointLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computePoint( light.m_lightBase
									, surface
									, light.m_position.xyz() ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							doComputeLight( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection
								, output );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;
					}
					ELSE
					{
						doComputeLight( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection
							, output );
					}
					FI;
				}
				else
				{
					doComputeLight( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection
						, output );
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
			, c3d::InPointLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPhongLightingModel::doDeclareComputeSpotLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
			, [this]( c3d::SpotLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distLightToVertex = m_writer.declLocale( "distLightToVertex"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f - step( spotFactor, light.m_cutOff ) );

						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor *= max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, lightToVertex ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							doComputeLight( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection
								, output );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;
					}
					ELSE
					{
						doComputeLight( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection
							, output );
					}
					FI;
				}
				else
				{
					doComputeLight( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection
						, output );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distLightToVertex * distLightToVertex
						, sdw::fma( light.m_attenuation.y()
							, distLightToVertex
							, light.m_attenuation.x() ) ) );
				spotFactor = sdw::fma( ( spotFactor - 1.0_f )
					, 1.0_f / ( 1.0_f - light.m_cutOff )
					, 1.0_f );
				output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
				output.m_specular = spotFactor * output.m_specular / attenuation;
				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InSpotLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPhongLightingModel::doDeclareComputeLight()
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
				output.m_diffuse = light.m_colour
					* light.m_intensity.x()
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
					* light.m_colour
					* light.m_intensity.y();
			}
			, c3d::InLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "lightDirection" )
			, outputs );
	}

	void ToonPhongLightingModel::doComputeLight( c3d::Light const & light
		, ToonPhongLightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & lightDirection
		, c3d::OutputComponents & parentOutput )
	{
		m_computeLight( light
			, material
			, surface
			, worldEye
			, lightDirection
			, parentOutput );
	}

	void ToonPhongLightingModel::doDeclareDiffuseModel()
	{
		doDeclareComputeLightDiffuse();
	}

	void ToonPhongLightingModel::doDeclareComputeTiledDirectionalLightDiffuse()
	{
		m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::TiledDirectionalLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
							, m_shadowModel->computeDirectional( light.m_lightBase
								, surface
								, light.m_transforms[cascadeIndex]
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount ) );

						IF( m_writer, shadowFactor > 0.0_f )
						{
							diffuse = shadowFactor * doComputeLightDiffuse( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection );
						}
						FI;
					}
					ELSE
					{
						diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
							, surface
							, worldEye
							, lightDirection );
					}
					FI;
				}
				else
				{
					diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection );
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, c3d::InTiledDirectionalLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPhongLightingModel::doDeclareComputeDirectionalLightDiffuse()
	{
		m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::DirectionalLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
							, m_shadowModel->computeDirectional( light.m_lightBase
								, surface
								, light.m_transforms[cascadeIndex]
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount ) );

						IF( m_writer, shadowFactor > 0.0_f )
						{
							diffuse = shadowFactor * doComputeLightDiffuse( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection );
						}
						FI;
					}
					ELSE
					{
						diffuse = doComputeLightDiffuse( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection );
					}
					FI;
				}
				else
				{
					diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection );
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, c3d::InDirectionalLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPhongLightingModel::doDeclareComputePointLightDiffuse()
	{
		m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
			, [this]( c3d::PointLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computePoint( light.m_lightBase
									, surface
									, light.m_position.xyz() ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							diffuse = shadowFactor * doComputeLightDiffuse( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection );
						}
						FI;
					}
					ELSE
					{
						diffuse = doComputeLightDiffuse( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection );
					}
					FI;
				}
				else
				{
					diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse / attenuation ) );
			}
			, c3d::InPointLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPhongLightingModel::doDeclareComputeSpotLightDiffuse()
	{
		m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
			, [this]( c3d::SpotLight const & light
				, ToonPhongLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, surface.worldPosition - light.m_position.xyz() );
				auto distLightToVertex = m_writer.declLocale( "distLightToVertex"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f - step( spotFactor, light.m_cutOff ) );

						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor *= max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, lightToVertex ) );
						}
						FI;

						IF( m_writer, shadowFactor > 0.0_f )
						{
							diffuse = shadowFactor * doComputeLightDiffuse( light.m_lightBase
								, material
								, surface
								, worldEye
								, lightDirection );
						}
						FI;
					}
					ELSE
					{
						diffuse = doComputeLightDiffuse( light.m_lightBase
							, material
							, surface
							, worldEye
							, lightDirection );
					}
					FI;
				}
				else
				{
					diffuse = doComputeLightDiffuse( light.m_lightBase
						, material
						, surface
						, worldEye
						, lightDirection );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distLightToVertex * distLightToVertex
						, sdw::fma( light.m_attenuation.y()
							, distLightToVertex
							, light.m_attenuation.x() ) ) );
				spotFactor = sdw::fma( ( spotFactor - 1.0_f )
					, 1.0_f / ( 1.0_f - light.m_cutOff )
					, 1.0_f );
				m_writer.returnStmt( max( vec3( 0.0_f ), spotFactor * diffuse / attenuation ) );
			}
			, c3d::InSpotLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPhongLightingModel::doDeclareComputeLightDiffuse()
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
					* light.m_colour
					* light.m_intensity.x() );
			}
			, c3d::InLight( m_writer, "light" )
			, InToonPhongLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "lightDirection" ) );
	}

	sdw::Vec3 ToonPhongLightingModel::doComputeLightDiffuse( c3d::Light const & light
		, ToonPhongLightMaterial const & material
		, c3d::Surface const & surface
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

	ToonBlinnPhongLightingModel::ToonBlinnPhongLightingModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
		: ToonPhongLightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram
			, true }
	{
	}

	c3d::LightingModelPtr ToonBlinnPhongLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
	{
		return std::make_unique< ToonBlinnPhongLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram );
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
				bool hasAlbedo;
				bool hasMetalness;
				bool hasSpecular;
				bool hasEmissive;
			};

			void modifyMaterial( sdw::ShaderWriter & writer
				, castor::String const & configName
				, castor3d::PassFlags const & passFlags
				, castor3d::TextureFlags const & flags
				, sdw::Vec4 const & sampled
				, c3d::TextureConfigData const & config
				, MaterialTextureMods & mods
				, ToonPbrLightMaterial & pbrLightMat )
			{
				if ( checkFlag( flags, castor3d::TextureFlag::eAlbedo ) )
				{
					pbrLightMat.albedo = config.getAlbedo( sampled, pbrLightMat.albedo );
					mods.hasAlbedo = true;
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eSpecular ) )
				{
					pbrLightMat.specular = config.getSpecular( sampled, pbrLightMat.specular );
					mods.hasSpecular = true;
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eGlossiness ) )
				{
					auto gloss = writer.declLocale( "gloss" + configName
						, c3d::LightMaterial::computeRoughness( pbrLightMat.roughness ) );
					gloss = config.getGlossiness( sampled, gloss );
					pbrLightMat.roughness = c3d::LightMaterial::computeRoughness( gloss );
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eMetalness ) )
				{
					pbrLightMat.metalness = config.getMetalness( sampled, pbrLightMat.metalness );
					mods.hasMetalness = true;
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eRoughness ) )
				{
					pbrLightMat.roughness = config.getRoughness( sampled, pbrLightMat.roughness );
				}

				if ( checkFlag( flags, castor3d::TextureFlag::eEmissive ) )
				{
					mods.hasEmissive = true;
				}
			}

			void updateMaterial( castor3d::PassFlags const & passFlags
				, MaterialTextureMods const & mods
				, ToonPbrLightMaterial & pbrLightMat
				, sdw::Vec3 & emissive )
			{
				if ( pbrLightMat.isSpecularGlossiness() )
				{
					if ( !mods.hasMetalness && ( mods.hasSpecular || mods.hasAlbedo ) )
					{
						pbrLightMat.metalness = c3d::LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
					}
				}
				else
				{
					if ( !mods.hasSpecular && ( mods.hasMetalness || mods.hasAlbedo ) )
					{
						pbrLightMat.specular = c3d::LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
					}
				}

				if ( checkFlag( passFlags, castor3d::PassFlag::eLighting )
					&& !mods.hasEmissive )
				{
					emissive *= pbrLightMat.albedo;
				}
			}
		}
	}

	ToonPbrLightingModel::ToonPbrLightingModel( bool isSpecularGlossiness
		, sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
		: c3d::LightingModel{ writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram
			, isSpecularGlossiness ? std::string{ "c3d_pbrsg_toon_" } : std::string{ "c3d_pbrmr_toon_" } }
		, m_isSpecularGlossiness{ isSpecularGlossiness }
		, m_cookTorrance{ writer, utils }
	{
	}

	sdw::Vec3 ToonPbrLightingModel::combine( sdw::Vec3 const & directDiffuse
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

	c3d::ReflectionModelPtr ToonPbrLightingModel::getReflectionModel( castor3d::PassFlags const & passFlags
		, uint32_t & envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< ToonPbrReflectionModel >( m_writer
			, m_utils
			, passFlags
			, envMapBinding
			, envMapSet );
	}

	c3d::ReflectionModelPtr ToonPbrLightingModel::getReflectionModel( uint32_t envMapBinding
		, uint32_t envMapSet )const
	{
		return std::make_unique< ToonPbrReflectionModel >( m_writer
			, m_utils
			, envMapBinding
			, envMapSet );
	}

	void ToonPbrLightingModel::compute( c3d::TiledDirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeTiledDirectional( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPbrLightingModel::compute( c3d::DirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeDirectional( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPbrLightingModel::compute( c3d::PointLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computePoint( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPbrLightingModel::compute( c3d::SpotLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows
		, c3d::OutputComponents & parentOutput )const
	{
		m_computeSpot( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows
			, parentOutput );
	}

	void ToonPbrLightingModel::computeMapContributions( castor3d::PassFlags const & passFlags
		, castor3d::FilteredTextureFlags const & textures
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 & texCoords
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
		pbr::MaterialTextureMods mods{};
		m_utils.computeGeometryMapsContributions( textures
			, passFlags
			, textureConfigs
			, textureAnims
			, maps
			, texCoords
			, opacity
			, normal
			, tangent
			, bitangent
			, tangentSpaceViewPosition
			, tangentSpaceFragPosition );

		for ( auto & textureIt : textures )
		{
			if ( !c3d::Utils::isGeometryOnlyMap( textureIt.second.flags, passFlags ) )
			{
				auto name = castor::string::stringCast< char >( castor::string::toString( textureIt.first, std::locale{ "C" } ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
				auto anim = m_writer.declLocale( "anim" + name
					, textureAnims.getTextureAnimation( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapContribution( textureIt.second.flags
						, passFlags
						, name
						, config
						, anim
						, maps[textureIt.first]
						, texCoords
						, emissive
						, opacity
						, occlusion
						, transmittance
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition ) );
				pbr::modifyMaterial( m_writer
					, name
					, passFlags
					, textureIt.second.flags
					, sampled
					, config
					, mods
					, pbrLightMat );
			}
		}

		pbr::updateMaterial( passFlags
			, mods
			, pbrLightMat
			, emissive );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::TiledDirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeTiledDirectionalDiffuse( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::DirectionalLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeDirectionalDiffuse( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::PointLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computePointDiffuse( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	sdw::Vec3 ToonPbrLightingModel::computeDiffuse( c3d::SpotLight const & light
		, c3d::LightMaterial const & material
		, c3d::Surface const & surface
		, sdw::Vec3 const & worldEye
		, sdw::Int const & receivesShadows )const
	{
		return m_computeSpotDiffuse( light
			, static_cast< ToonPbrLightMaterial const & >( material )
			, surface
			, worldEye
			, receivesShadows );
	}

	void ToonPbrLightingModel::computeMapDiffuseContributions( castor3d::PassFlags const & passFlags
		, castor3d::FilteredTextureFlags const & textures
		, c3d::TextureConfigurations const & textureConfigs
		, c3d::TextureAnimations const & textureAnims
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, c3d::LightMaterial & lightMat )
	{
		auto & pbrLightMat = static_cast< ToonPbrLightMaterial & >( lightMat );
		pbr::MaterialTextureMods mods{};

		for ( auto & textureIt : textures )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( textureIt.first, std::locale{ "C" } ) );
			auto config = m_writer.declLocale( "config" + name
				, textureConfigs.getTextureConfiguration( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
			auto anim = m_writer.declLocale( "anim" + name
				, textureAnims.getTextureAnimation( m_writer.cast< sdw::UInt >( textureIt.second.id ) ) );
			auto sampled = m_writer.declLocale( "sampled" + name
				, m_utils.computeCommonMapVoxelContribution( textureIt.second.flags
					, passFlags
					, name
					, config
					, anim
					, maps[textureIt.first]
					, texCoords
					, emissive
					, opacity
					, occlusion ) );
			pbr::modifyMaterial( m_writer
				, name
				, passFlags
				, textureIt.second.flags
				, sampled
				, config
				, mods
				, pbrLightMat );
		}

		pbr::updateMaterial( passFlags
			, mods
			, pbrLightMat
			, emissive );
	}

	void ToonPbrLightingModel::doDeclareModel()
	{
	}

	void ToonPbrLightingModel::doDeclareComputeTiledDirectionalLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::TiledDirectionalLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, 0_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
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
							shadowFactor = cascadeFactors.y()
								* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, -lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
										, m_shadowModel->computeDirectional( light.m_lightBase
											, surface
											, light.m_transforms[cascadeIndex - 1u]
											, -lightDirection
											, cascadeIndex - 1u
											, light.m_cascadeCount ) );
							}
							FI;
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							m_cookTorrance.computeAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, material.smoothBand
								, surface
								, output );
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
					ELSE
					{
						m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, material.smoothBand
							, surface
							, output );
					}
					FI;
				}
				else
				{
					m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
						, output );
				}

				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InTiledDirectionalLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPbrLightingModel::doDeclareComputeDirectionalLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeDirectional = m_writer.implementFunction< sdw::Void >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::DirectionalLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, 0_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
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
							shadowFactor = cascadeFactors.y()
								* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, -lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
										, m_shadowModel->computeDirectional( light.m_lightBase
											, surface
											, light.m_transforms[cascadeIndex - 1u]
											, -lightDirection
											, cascadeIndex - 1u
											, light.m_cascadeCount ) );
							}
							FI;
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							m_cookTorrance.computeAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, material.smoothBand
								, surface
								, output );
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
					ELSE
					{
						m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, material.smoothBand
							, surface
							, output );
					}
					FI;
				}
				else
				{
					m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
						, output );
				}

				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InDirectionalLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPbrLightingModel::doDeclareComputePointLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computePoint = m_writer.implementFunction< sdw::Void >( m_prefix + "computePointLight"
			, [this]( c3d::PointLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - surface.worldPosition );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computePoint( light.m_lightBase
									, surface
									, light.m_position.xyz() ) );
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							m_cookTorrance.computeAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, material.smoothBand
								, surface
								, output );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;
					}
					ELSE
					{
						m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, material.smoothBand
							, surface
							, output );
					}
					FI;
				}
				else
				{
					m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
						, output );
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
			, c3d::InPointLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPbrLightingModel::doDeclareComputeSpotLight()
	{
		c3d::OutputComponents outputs{ m_writer };
		m_computeSpot = m_writer.implementFunction< sdw::Void >( m_prefix + "computeSpotLight"
			, [this]( c3d::SpotLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows
				, c3d::OutputComponents & parentOutput )
			{
				c3d::OutputComponents output
				{
					m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
					m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
				};
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - surface.worldPosition );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f - step( spotFactor, light.m_cutOff ) );
						shadowFactor *= max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
							, m_shadowModel->computeSpot( light.m_lightBase
								, surface
								, light.m_transform
								, -lightToVertex ) );

						IF( m_writer, shadowFactor )
						{
							m_cookTorrance.computeAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, material.smoothBand
								, surface
								, output );
							output.m_diffuse *= shadowFactor;
							output.m_specular *= shadowFactor;
						}
						FI;
					}
					ELSE
					{
						m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, material.smoothBand
							, surface
							, output );
					}
					FI;
				}
				else
				{
					m_cookTorrance.computeAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.getRoughness()
						, material.smoothBand
						, surface
						, output );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				spotFactor = sdw::fma( ( spotFactor - 1.0_f )
					, 1.0_f / ( 1.0_f - light.m_cutOff )
					, 1.0_f );
				output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
				output.m_specular = spotFactor * output.m_specular / attenuation;
				parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
				parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
			}
			, c3d::InSpotLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" )
			, outputs );
	}

	void ToonPbrLightingModel::doDeclareDiffuseModel()
	{
	}

	void ToonPbrLightingModel::doDeclareComputeTiledDirectionalLightDiffuse()
	{
		m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::TiledDirectionalLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeDirectional( light.m_lightBase
									, surface
									, light.m_transforms[cascadeIndex]
									, -lightDirection
									, cascadeIndex
									, light.m_cascadeCount ) );
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							diffuse = shadowFactor * m_cookTorrance.computeDiffuseAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.smoothBand
								, surface );
						}
						FI;
					}
					ELSE
					{
						diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.smoothBand
							, surface );
					}
					FI;
				}
				else
				{
					diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.smoothBand
						, surface );
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, c3d::InTiledDirectionalLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPbrLightingModel::doDeclareComputeDirectionalLightDiffuse()
	{
		m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeDirectionalLight"
			, [this]( c3d::DirectionalLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
							, vec3( 0.0_f, 1.0_f, 0.0_f ) );
						auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
							, light.m_cascadeCount - 1_u );
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computeDirectional( light.m_lightBase
									, surface
									, light.m_transforms[cascadeIndex]
									, -lightDirection
									, cascadeIndex
									, light.m_cascadeCount ) );
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							diffuse = shadowFactor * m_cookTorrance.computeDiffuseAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.smoothBand
								, surface );
						}
						FI;
					}
					ELSE
					{
						diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.smoothBand
							, surface );
					}
					FI;
				}
				else
				{
					diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.smoothBand
						, surface );
				}

				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
			}
			, c3d::InDirectionalLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPbrLightingModel::doDeclareComputePointLightDiffuse()
	{
		m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computePointLight"
			, [this]( c3d::PointLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - surface.worldPosition );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f );

						IF( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
								, m_shadowModel->computePoint( light.m_lightBase
									, surface
									, light.m_position.xyz() ) );
						}
						FI;

						IF( m_writer, shadowFactor )
						{
							diffuse = shadowFactor * m_cookTorrance.computeDiffuseAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.smoothBand
								, surface );
						}
						FI;
					}
					ELSE
					{
						diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.smoothBand
							, surface );
					}
					FI;
				}
				else
				{
					diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.smoothBand
						, surface );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				m_writer.returnStmt( max( vec3( 0.0_f ), diffuse / attenuation ) );
			}
			, c3d::InPointLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	void ToonPbrLightingModel::doDeclareComputeSpotLightDiffuse()
	{
		m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( m_prefix + "computeSpotLight"
			, [this]( c3d::SpotLight const & light
				, ToonPbrLightMaterial const & material
				, c3d::Surface const & surface
				, sdw::Vec3 const & worldEye
				, sdw::Int const & receivesShadows )
			{
				auto diffuse = m_writer.declLocale( "diffuse"
					, vec3( 0.0_f ) );
				auto lightToVertex = m_writer.declLocale( "lightToVertex"
					, light.m_position.xyz() - surface.worldPosition );
				auto distance = m_writer.declLocale( "distance"
					, length( lightToVertex ) );
				auto lightDirection = m_writer.declLocale( "lightDirection"
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.declLocale( "spotFactor"
					, dot( lightDirection, -light.m_direction ) );

				if ( m_shadowModel->isEnabled() )
				{
					IF( m_writer, light.m_lightBase.m_shadowType != sdw::Int( int( castor3d::ShadowType::eNone ) ) )
					{
						auto shadowFactor = m_writer.declLocale( "shadowFactor"
							, 1.0_f - step( spotFactor, light.m_cutOff ) );
						shadowFactor *= max( 1.0_f - m_writer.cast< sdw::Float >( receivesShadows )
							, m_shadowModel->computeSpot( light.m_lightBase
								, surface
								, light.m_transform
								, -lightToVertex ) );

						IF( m_writer, shadowFactor )
						{
							diffuse = shadowFactor * m_cookTorrance.computeDiffuseAON( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.smoothBand
								, surface );
						}
						FI;
					}
					ELSE
					{
						diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.smoothBand
							, surface );
					}
					FI;
				}
				else
				{
					diffuse = m_cookTorrance.computeDiffuseAON( light.m_lightBase
						, worldEye
						, lightDirection
						, material.specular
						, material.getMetalness()
						, material.smoothBand
						, surface );
				}

				auto attenuation = m_writer.declLocale( "attenuation"
					, sdw::fma( light.m_attenuation.z()
						, distance * distance
						, sdw::fma( light.m_attenuation.y()
							, distance
							, light.m_attenuation.x() ) ) );
				spotFactor = sdw::fma( ( spotFactor - 1.0_f )
					, 1.0_f / ( 1.0_f - light.m_cutOff )
					, 1.0_f );
				m_writer.returnStmt( max( vec3( 0.0_f ), spotFactor * diffuse / attenuation ) );
			}
			, c3d::InSpotLight( m_writer, "light" )
			, InToonPbrLightMaterial{ m_writer, "material" }
			, c3d::InSurface{ m_writer, "surface" }
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InInt( m_writer, "receivesShadows" ) );
	}

	//***********************************************************************************************

	ToonPbrMRLightingModel::ToonPbrMRLightingModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
		: ToonPbrLightingModel{ false
			, writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram }
	{
	}

	const castor::String ToonPbrMRLightingModel::getName()
	{
		return cuT( "c3d.toon.pbrmr" );
	}

	c3d::LightingModelPtr ToonPbrMRLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
	{
		return std::make_unique< ToonPbrMRLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram );
	}

	std::unique_ptr< c3d::LightMaterial > ToonPbrMRLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< c3d::LightMaterial, ToonPbrMRLightMaterial >( name );
	}

	//***********************************************************************************************

	ToonPbrSGLightingModel::ToonPbrSGLightingModel( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
		: ToonPbrLightingModel{ true
		, writer
		, utils
		, std::move( shadowOptions )
		, isOpaqueProgram }
	{
	}

	const castor::String ToonPbrSGLightingModel::getName()
	{
		return cuT( "c3d.toon.pbrsg" );
	}

	c3d::LightingModelPtr ToonPbrSGLightingModel::create( sdw::ShaderWriter & writer
		, c3d::Utils & utils
		, c3d::ShadowOptions shadowOptions
		, bool isOpaqueProgram )
	{
		return std::make_unique< ToonPbrSGLightingModel >( writer
			, utils
			, std::move( shadowOptions )
			, isOpaqueProgram );
	}

	std::unique_ptr< c3d::LightMaterial > ToonPbrSGLightingModel::declMaterial( std::string const & name )
	{
		return m_writer.declDerivedLocale< c3d::LightMaterial, ToonPbrSGLightMaterial >( name );
	}

	//*********************************************************************************************
}
