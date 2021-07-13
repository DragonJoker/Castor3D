#include "Castor3D/Shader/Shaders/GlslPbrLighting.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPbrMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		const String PbrLightingModel::Name = cuT( "pbr" );

		PbrLightingModel::PbrLightingModel( ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram )
			: LightingModel{ writer
				, utils
				, std::move( shadowOptions )
				, isOpaqueProgram }
			, m_cookTorrance{ writer, utils }
		{
		}

		void PbrLightingModel::compute( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			, Vec3 const & worldEye
			, Int const & receivesShadows
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
			auto & pbrLightMat = static_cast< PbrLightMaterial & >( lightMat );
			bool hasEmissive = false;
			bool hasAlbedo = false;
			bool hasMetalness = false;
			bool hasSpecular = false;
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
					auto name = string::stringCast< char >( string::toString( textureIt.first, std::locale{ "C" } ) );
					auto config = m_writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureIt.second.id ) ) );
					auto sampled = m_writer.declLocale( "sampled" + name
						, m_utils.computeCommonMapContribution( textureIt.second.flags
							, passFlags
							, name
							, config
							, maps[textureIt.first]
							, gamma
							, texCoords
							, emissive
							, opacity
							, occlusion
							, transmittance
							, tangentSpaceViewPosition
							, tangentSpaceFragPosition ) );

					if ( checkFlag( textureIt.second.flags, TextureFlag::eAlbedo ) )
					{
						pbrLightMat.albedo = config.getAlbedo( m_writer, sampled, pbrLightMat.albedo, gamma );
						hasAlbedo = true;
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eSpecular ) )
					{
						pbrLightMat.specular = config.getSpecular( m_writer, sampled, pbrLightMat.specular );
						hasSpecular = true;
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eGlossiness ) )
					{
						auto gloss = m_writer.declLocale( "gloss" + name
							, LightMaterial::computeRoughness( pbrLightMat.roughness ) );
						gloss = config.getGlossiness( m_writer
							, sampled
							, gloss );
						pbrLightMat.roughness = LightMaterial::computeRoughness( gloss );
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eMetalness ) )
					{
						pbrLightMat.metalness = config.getMetalness( m_writer, sampled, pbrLightMat.metalness );
						hasMetalness = true;
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eRoughness ) )
					{
						pbrLightMat.roughness = config.getRoughness( m_writer
							, sampled
							, pbrLightMat.roughness );
					}

					if ( checkFlag( textureIt.second.flags, TextureFlag::eEmissive ) )
					{
						hasEmissive = true;
					}
				}
			}

			if ( checkFlag( passFlags, PassFlag::eSpecularGlossiness ) )
			{
				if ( !hasMetalness && ( hasSpecular || hasAlbedo ) )
				{
					pbrLightMat.metalness = LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
				}
			}
			else
			{
				if ( !hasSpecular && ( hasMetalness || hasAlbedo ) )
				{
					pbrLightMat.specular = LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
				}
			}

			if ( checkFlag( passFlags, PassFlag::eLighting )
				&& !hasEmissive )
			{
				emissive *= pbrLightMat.albedo;
			}
		}

		Vec3 PbrLightingModel::computeDiffuse( TiledDirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeTiledDirectionalDiffuse( light
				, static_cast< PbrLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PbrLightingModel::computeDiffuse( DirectionalLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeDirectionalDiffuse( light
				, static_cast< PbrLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PbrLightingModel::computeDiffuse( PointLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computePointDiffuse( light
				, static_cast< PbrLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		Vec3 PbrLightingModel::computeDiffuse( SpotLight const & light
			, LightMaterial const & material
			, Surface const & surface
			, Vec3 const & worldEye
			, Int const & receivesShadows )const
		{
			return m_computeSpotDiffuse( light
				, static_cast< PbrLightMaterial const & >( material )
				, surface
				, worldEye
				, receivesShadows );
		}

		void PbrLightingModel::computeMapDiffuseContributions( PassFlags const & passFlags
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
			auto & pbrLightMat = static_cast< PbrLightMaterial & >( lightMat );
			bool hasEmissive = false;
			bool hasAlbedo = false;
			bool hasMetalness = false;
			bool hasSpecular = false;

			for ( auto & textureIt : textures )
			{
				auto name = string::stringCast< char >( string::toString( textureIt.first, std::locale{ "C" } ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureIt.second.id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapVoxelContribution( textureIt.second.flags
						, passFlags
						, name
						, config
						, maps[textureIt.first]
						, gamma
						, texCoords
						, emissive
						, opacity
						, occlusion ) );

				if ( checkFlag( textureIt.second.flags, TextureFlag::eAlbedo ) )
				{
					pbrLightMat.albedo = config.getAlbedo( m_writer, sampled, pbrLightMat.albedo, gamma );
					hasAlbedo = true;
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eSpecular ) )
				{
					pbrLightMat.specular = config.getSpecular( m_writer, sampled, pbrLightMat.specular );
					hasSpecular = true;
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eGlossiness ) )
				{
					auto gloss = m_writer.declLocale( "gloss" + name
						, LightMaterial::computeRoughness( pbrLightMat.roughness ) );
					gloss = config.getGlossiness( m_writer
						, sampled
						, gloss );
					pbrLightMat.roughness = LightMaterial::computeRoughness( gloss );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eMetalness ) )
				{
					pbrLightMat.metalness = config.getMetalness( m_writer, sampled, pbrLightMat.metalness );
					hasMetalness = true;
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eRoughness ) )
				{
					pbrLightMat.roughness = config.getRoughness( m_writer
						, sampled
						, pbrLightMat.roughness );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eEmissive ) )
				{
					hasEmissive = true;
				}
			}

			if ( checkFlag( passFlags, PassFlag::eSpecularGlossiness ) )
			{
				if ( !hasMetalness && ( hasSpecular || hasAlbedo ) )
				{
					pbrLightMat.metalness = LightMaterial::computeMetalness( pbrLightMat.albedo, pbrLightMat.specular );
				}
			}
			else
			{
				if ( !hasSpecular && ( hasMetalness || hasAlbedo ) )
				{
					pbrLightMat.specular = LightMaterial::computeF0( pbrLightMat.albedo, pbrLightMat.metalness );
				}
			}

			if ( checkFlag( passFlags, PassFlag::eLighting ) 
				&& !hasEmissive )
			{
				emissive *= pbrLightMat.albedo;
			}
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

		void PbrLightingModel::doDeclareModel()
		{
			m_cookTorrance.declare();
		}

		void PbrLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
#if C3D_UseTiledDirectionalShadowMap
			m_computeTiledDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( TiledDirectionalLight const & light
					, PbrLightMaterial const & material
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
						, normalize( -light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, 0_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF ( m_writer, receivesShadows != 0_i )
							{
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
											, -lightDirection
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
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.metalness
									, material.roughness
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
							m_cookTorrance.compute( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.metalness
								, material.roughness
								, surface
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.metalness
							, material.roughness
							, surface
							, output );
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
				}
				, InTiledDirectionalLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
#else
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, PbrLightMaterial const & material
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
						, normalize( -light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, 0_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF ( m_writer, receivesShadows != 0_i )
							{
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
											, -lightDirection
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
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, material.getRoughness()
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
							m_cookTorrance.compute( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, surface
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
							, surface
							, output );
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
				}
				, InDirectionalLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
#endif
		}

		void PbrLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this]( PointLight const & light
					, PbrLightMaterial const & material
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
						, light.m_position.xyz() - surface.worldPosition );
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

							IF( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computePoint( light.m_lightBase
										, surface
										, light.m_position.xyz() ) );
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, material.getRoughness()
									, surface
									, output );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;
						}
						ELSE
						{
							m_cookTorrance.compute( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, surface
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
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
				, InPointLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
		}

		void PbrLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this]( SpotLight const & light
					, PbrLightMaterial const & material
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
						, light.m_position.xyz() - surface.worldPosition );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f - step( spotFactor, light.m_cutOff ) );
							shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, -lightToVertex ) );

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, material.getRoughness()
									, surface
									, output );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;
						}
						ELSE
						{
							m_cookTorrance.compute( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, material.getRoughness()
								, surface
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, material.getRoughness()
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
				, InSpotLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" )
				, output );
		}

		void PbrLightingModel::doDeclareDiffuseModel()
		{
			m_cookTorrance.declareDiffuse();
		}

		void PbrLightingModel::doDeclareComputeDirectionalLightDiffuse()
		{
#if C3D_UseTiledDirectionalShadowMap
			m_computeTiledDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeDirectionalLight"
				, [this]( TiledDirectionalLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.m_cascadeCount - 1_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF ( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.metalness
									, surface );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.metalness
								, surface );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.metalness
							, surface );
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InTiledDirectionalLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
#else
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
				{
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.m_direction ) );

					if ( m_shadowModel->isEnabled() )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto cascadeFactors = m_writer.declLocale( "cascadeFactors"
								, vec3( 0.0_f, 1.0_f, 0.0_f ) );
							auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
								, light.m_cascadeCount - 1_u );
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF ( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
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
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, surface );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, surface );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
							, surface );
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InDirectionalLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
#endif
		}

		void PbrLightingModel::doDeclareComputePointLightDiffuse()
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computePointLight"
				, [this]( PointLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f );

							IF( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computePoint( light.m_lightBase
										, surface
										, light.m_position.xyz() ) );
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, surface );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, surface );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
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
				, InPointLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
		}

		void PbrLightingModel::doDeclareComputeSpotLightDiffuse()
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeSpotLight"
				, [this]( SpotLight const & light
					, PbrLightMaterial const & material
					, Surface const & surface
					, Vec3 const & worldEye
					, Int const & receivesShadows )
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
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
						{
							auto shadowFactor = m_writer.declLocale( "shadowFactor"
								, 1.0_f - step( spotFactor, light.m_cutOff ) );
							shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpot( light.m_lightBase
									, surface
									, light.m_transform
									, -lightToVertex ) );

							IF( m_writer, shadowFactor )
							{
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, material.specular
									, material.getMetalness()
									, surface );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, material.specular
								, material.getMetalness()
								, surface );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, material.specular
							, material.getMetalness()
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
				, InSpotLight( m_writer, "light" )
				, InPbrLightMaterial{ m_writer, "material" }
				, InSurface{ m_writer, "surface" }
				, InVec3( m_writer, "worldEye" )
				, InInt( m_writer, "receivesShadows" ) );
		}

		//***********************************************************************************************
	}
}
