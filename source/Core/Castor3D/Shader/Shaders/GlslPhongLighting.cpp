#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

#define C3D_UnrollLightsLoop 0

namespace castor3d
{
	namespace shader
	{
		const String PhongLightingModel::Name = cuT( "phong" );

		PhongLightingModel::PhongLightingModel( ShaderWriter & m_writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram )
			: LightingModel{ m_writer
				, utils
				, std::move( shadowOptions )
				, isOpaqueProgram }
		{
		}

		void PhongLightingModel::compute( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, FilteredTextureFlags const & textures
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
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
			bool hasEmissive = false;
			m_utils.computeGeometryMapsContributions( textures
				, passFlags
				, textureConfigs
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
				if ( textureIt.second.flags != TextureFlag::eOpacity
					&& textureIt.second.flags != TextureFlag::eNormal
					&& textureIt.second.flags != TextureFlag::eHeight )
				{
					auto i = textureIt.first;
					auto name = string::stringCast< char >( string::toString( i ) );
					auto config = m_writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureIt.second.id ) ) );
					auto sampled = m_writer.declLocale( "sampled" + name
						, m_utils.computeCommonMapContribution( textureIt.second.flags
							, passFlags
							, name
							, config
							, maps[i]
							, gamma
							, texCoords
							, emissive
							, opacity
							, occlusion
							, transmittance
							, tangentSpaceViewPosition
							, tangentSpaceFragPosition ) );

					if ( checkFlag( textureIt.second.flags, TextureFlag::eDiffuse ) )
					{
						phongLightMat.albedo = config.getDiffuse( m_writer, sampled, phongLightMat.albedo, gamma );
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eSpecular ) )
					{
						phongLightMat.specular = config.getSpecular( m_writer, sampled, phongLightMat.specular );
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eShininess ) )
					{
						phongLightMat.shininess = config.getShininess( m_writer, sampled, phongLightMat.shininess );
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eEmissive ) )
					{
						hasEmissive = true;
					}
				}
			}

			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !hasEmissive )
			{
				emissive *= phongLightMat.albedo;
			}
		}

		Vec3 PhongLightingModel::computeDiffuse( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeTiledDirectionalDiffuse( light
				, static_cast< PhongLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PhongLightingModel::computeDiffuse( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeDirectionalDiffuse( light
				, static_cast< PhongLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PhongLightingModel::computeDiffuse( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computePointDiffuse( light
				, static_cast< PhongLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PhongLightingModel::computeDiffuse( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeSpotDiffuse( light
				, static_cast< PhongLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		void PhongLightingModel::computeMapDiffuseContributions( PassFlags const & passFlags
			, FilteredTextureFlags const & textures
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, LightMaterial & lightMat )
		{
			auto & phongLightMat = static_cast< PhongLightMaterial & >( lightMat );
			bool hasEmissive = false;

			for ( auto & textureIt : textures )
			{
				auto i = textureIt.first;
				auto name = string::stringCast< char >( string::toString( i ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureIt.second.id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapVoxelContribution( textureIt.second.flags
						, passFlags
						, name
						, config
						, maps[i]
						, gamma
						, texCoords
						, emissive
						, opacity
						, occlusion ) );

				if ( checkFlag( textureIt.second.flags, TextureFlag::eDiffuse ) )
				{
					phongLightMat.albedo = config.getDiffuse( m_writer, sampled, phongLightMat.albedo, gamma );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eSpecular ) )
				{
					phongLightMat.specular = config.getSpecular( m_writer, sampled, phongLightMat.specular );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eShininess ) )
				{
					phongLightMat.shininess = config.getShininess( m_writer, sampled, phongLightMat.shininess );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eEmissive ) )
				{
					hasEmissive = true;
				}
			}

			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !hasEmissive )
			{
				emissive *= phongLightMat.albedo;
			}
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
			, sdw::Vec3 const & materialDiffuse )
		{
			return materialDiffuse * ( directDiffuse + ( indirectDiffuse * ambientOcclusion ) )
				+ ( directSpecular + ( indirectSpecular * ambientOcclusion ) )
				+ ( ambient * indirectAmbient * ambientOcclusion )
				+ emissive
				+ refracted
				+ reflected * ambientOcclusion;
		}

		void PhongLightingModel::doDeclareModel()
		{
			doDeclareComputeLight();
		}

		void PhongLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
#if C3D_UseTiledDirectionalShadowMap
			m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( TiledDirectionalLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, 0_u );
							auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
							auto maxCount = m_writer.declLocale( "maxCount"
								, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

							// Get cascade index for the current fragment's view position
							FOR( m_writer, UInt, i, 0u, i < maxCount, ++i )
							{
								auto factors = m_writer.declLocale( "factors"
									, m_getTileFactors( Vec3{ surface.viewPosition }
										, light.m_splitDepths
										, i ) );

								IF( m_writer, factors.x() != 0.0_f )
								{
									cascadeFactors = factors;
								}
								FI;
							}
							ROF;

							cascadeIndex = m_writer.cast< UInt >( cascadeFactors.x() );
							shadowFactor = cascadeFactors.y()
								* max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
				, InTiledDirectionalLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
#else
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, 0_u );
							auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
							auto maxCount = m_writer.declLocale( "maxCount"
								, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

							// Get cascade index for the current fragment's view position
							FOR( m_writer, UInt, i, 0u, i < maxCount, ++i )
							{
								auto factors = m_writer.declLocale( "factors"
									, m_getCascadeFactors( Vec3{ surface.viewPosition }
										, light.m_splitDepths
										, i ) );

								IF( m_writer, factors.x() != 0.0_f )
								{
									cascadeFactors = factors;
								}
								FI;
							}
							ROF;

							cascadeIndex = m_writer.cast< UInt >( cascadeFactors.x() );
							shadowFactor = cascadeFactors.y()
								* max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeDirectional( light.m_lightBase
										, surface
										, light.m_transforms[cascadeIndex]
										, lightDirection
										, cascadeIndex
										, light.m_cascadeCount ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
				, InDirectionalLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
#endif
		}

		void PhongLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this]( PointLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF( m_writer, light.m_lightBase.m_index >= 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
#if C3D_DebugSpotShadows
					parentOutput.m_diffuse += shadowFactor;
					parentOutput.m_specular += shadowFactor;
#else
					output.m_diffuse = output.m_diffuse / attenuation;
					output.m_specular = output.m_specular / attenuation;
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
#endif
				}
				, InPointLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
		}

		void PhongLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this]( SpotLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows
					, OutputComponents & parentOutput )
				{
					OutputComponents output
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f - step( spotFactor, light.m_cutOff ) );

							IF( m_writer, light.m_lightBase.m_index >= 0_i )
							{
#if C3D_DebugSpotShadows

								shadowFactor = m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, lightToVertex );

#else

								shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeSpot( light.m_lightBase
										, surface
										, light.m_transform
										, lightToVertex ) );

#endif
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
#if C3D_DebugSpotShadows
					parentOutput.m_diffuse += shadowFactor;
					parentOutput.m_specular += shadowFactor;
#else
					output.m_diffuse = spotFactor * output.m_diffuse / attenuation;
					output.m_specular = spotFactor * output.m_specular / attenuation;
					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
#endif
				}
				, InSpotLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
		}

		void PhongLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< sdw::Void >( "doComputeLight"
				, [this]( Light const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Vec3 const & lightDirection
					, OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( surface.worldNormal, -lightDirection ) );
					auto isLit = m_writer.declLocale( "isLit"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					output.m_diffuse = isLit
						* light.m_colour
						* light.m_intensity.x()
						* diffuseFactor;

					// Specular term.
					auto vertexToEye = m_writer.declLocale( "vertexToEye"
						, normalize( worldEye - surface.worldPosition ) );
					// Blinn Phong
					auto halfwayDir = m_writer.declLocale( "halfwayDir"
						, normalize( vertexToEye - lightDirection ) );
					auto specularFactor = m_writer.declLocale( "specularFactor"
						, max( dot( surface.worldNormal, halfwayDir ), 0.0_f ) );
					// Phong
					//auto lightReflect = m_writer.declLocale( "lightReflect"
					//	, normalize( reflect( lightDirection, surface.worldNormal ) ) );
					//auto specularFactor = m_writer.declLocale( "specularFactor"
					//	, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
					output.m_specular = isLit
						* light.m_colour
						* light.m_intensity.y()
						* pow( specularFactor, clamp( material.shininess, 1.0_f, 256.0_f ) );
				}
				, InLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "lightDirection" )
				, output );
		}

		void PhongLightingModel::doComputeLight( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Vec3 const & lightDirection
			, OutputComponents & parentOutput )
		{
			m_computeLight( light
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

		void PhongLightingModel::doDeclareComputeDirectionalLightDiffuse()
		{
#if C3D_UseTiledDirectionalShadowMap
			m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeDirectionalLight"
				, [this]( TiledDirectionalLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.m_cascadeCount - 1_u );
							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
				, InTiledDirectionalLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
#else
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.m_cascadeCount - 1_u );
							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
				, InDirectionalLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
#endif
		}

		void PhongLightingModel::doDeclareComputePointLightDiffuse()
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computePointLight"
				, [this]( PointLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF( m_writer, light.m_lightBase.m_index >= 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
#if C3D_DebugSpotShadows
					parentOutput.m_diffuse += shadowFactor;
					parentOutput.m_specular += shadowFactor;
#else
					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse / attenuation ) );
#endif
				}
				, InPointLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
		}

		void PhongLightingModel::doDeclareComputeSpotLightDiffuse()
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeSpotLight"
				, [this]( SpotLight const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f - step( spotFactor, light.m_cutOff ) );

							IF( m_writer, light.m_lightBase.m_index >= 0_i )
							{
#if C3D_DebugSpotShadows

								shadowFactor = m_shadowModel->computeSpot( light.m_lightBase.m_shadowType
									, light.m_lightBase.m_rawShadowOffsets
									, light.m_lightBase.m_pcfShadowOffsets
									, light.m_lightBase.m_vsmShadowVariance
									, light.m_transform
									, surface.worldPosition
									, lightToVertex
									, surface.worldNormal
									, light.m_lightBase.m_index );

#else

								shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeSpot( light.m_lightBase
										, surface
										, light.m_transform
										, lightToVertex ) );

#endif
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
#if C3D_DebugSpotShadows
					m_writer.returnStmt( vec3( shadowFactor ) );
#else
					m_writer.returnStmt( max( vec3( 0.0_f ), spotFactor * diffuse / attenuation ) );
#endif
				}
				, InSpotLight( m_writer, "light" )
				, InPhongLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
		}

		void PhongLightingModel::doDeclareComputeLightDiffuse()
		{
			m_computeLightDiffuse = m_writer.implementFunction< sdw::Vec3 >( "doComputeLight"
				, [this]( Light const & light
					, PhongLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Vec3 const & lightDirection )
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
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "lightDirection" ) );
		}

		Vec3 PhongLightingModel::doComputeLightDiffuse( Light const & light
			, PhongLightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Vec3 const & lightDirection )
		{
			return m_computeLightDiffuse( light
				, material
				, surface
				, worldEye
				, lightDirection );
		}
	}
}
