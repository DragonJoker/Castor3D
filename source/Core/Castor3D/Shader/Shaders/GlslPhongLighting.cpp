#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

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

		void PhongLightingModel::computeCombined( Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			auto c3d_lightsCount = m_writer.getVariable< IVec4 >( "c3d_lightsCount" );
			auto begin = m_writer.declLocale( "begin"
				, 0_i );
			auto end = m_writer.declLocale( "end"
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, dir, begin, dir < end, ++dir )
			{
				compute( getDirectionalLight( dir )
					, worldEye
					, shininess
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, point, begin, point < end, ++point )
			{
				compute( getPointLight( point )
					, worldEye
					, shininess
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, spot, begin, spot < end, ++spot )
			{
				compute( getSpotLight( spot )
					, worldEye
					, shininess
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;
		}

		void PhongLightingModel::compute( DirectionalLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeDirectional( light
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void PhongLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computePoint( light
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void PhongLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeSpot( light
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		std::shared_ptr< PhongLightingModel > PhongLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, SceneFlags sceneFlags
			, bool rsm
			, uint32_t & index
			, bool isOpaqueProgram )
		{
			auto result = std::make_shared< PhongLightingModel >( writer
				, utils
				, ShadowOptions{ ( sceneFlags & SceneFlag::eShadowAny ), rsm }
				, isOpaqueProgram );
			result->declareModel( index );
			return result;
		}

		std::shared_ptr< PhongLightingModel > PhongLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, LightType lightType
			, bool lightUbo
			, bool shadows
			, bool rsm
			, uint32_t & index )
		{
			auto result = std::make_shared< PhongLightingModel >( writer
				, utils
				, ShadowOptions
				{
					( shadows
						? SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << int( lightType ) )
						: SceneFlags( 0u ) ),
					rsm
				}
				, true );

			switch ( lightType )
			{
			case LightType::eDirectional:
				result->declareDirectionalModel( lightUbo, index );
				break;

			case LightType::ePoint:
				result->declarePointModel( lightUbo, index );
				break;

			case LightType::eSpot:
				result->declareSpotModel( lightUbo, index );
				break;

			default:
				CU_Failure( "Invalid light type" );
				break;
			}

			return result;
		}

		void PhongLightingModel::computeMapContributions( PipelineFlags const & flags
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, sdw::Vec3 & diffuse
			, sdw::Vec3 & specular
			, sdw::Float & shininess
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			for ( uint32_t i = 0u; i < flags.textures.size(); ++i )
			{
				auto name = string::stringCast< char >( string::toString( i ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( flags.textures[i].id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapContribution( flags.textures[i].flags
						, flags.passFlags
						, name
						, config
						, maps[i]
						, gamma
						, texCoords
						, normal
						, tangent
						, bitangent
						, emissive
						, opacity
						, occlusion
						, transmittance
						, tangentSpaceViewPosition
						, tangentSpaceFragPosition ) );

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eDiffuse ) )
				{
					diffuse = config.getDiffuse( m_writer, sampled, diffuse, gamma );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eSpecular ) )
				{
					specular = config.getSpecular( m_writer, sampled, specular );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eShininess ) )
				{
					shininess = config.getShininess( m_writer, sampled, shininess );
				}
			}
		}

		void PhongLightingModel::doDeclareModel()
		{
			doDeclareComputeLight();
		}

		void PhongLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
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
									, m_getCascadeFactors( Vec3{ fragmentIn.m_viewVertex }
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
									, m_shadowModel->computeDirectional( light.m_lightBase.m_shadowType
										, light.m_lightBase.m_rawShadowOffsets
										, light.m_lightBase.m_pcfShadowOffsets
										, light.m_lightBase.m_vsmShadowVariance
										, light.m_transforms[cascadeIndex]
										, fragmentIn.m_worldVertex
										, lightDirection
										, cascadeIndex
										, light.m_cascadeCount
										, fragmentIn.m_worldNormal ) );

							IF( m_writer, cascadeIndex > 0_u )
							{
								shadowFactor += cascadeFactors.z()
									* max( 1.0_f - m_writer.cast< Float >( receivesShadows )
										, m_shadowModel->computeDirectional( light.m_lightBase.m_shadowType
											, light.m_lightBase.m_rawShadowOffsets
											, light.m_lightBase.m_pcfShadowOffsets
											, light.m_lightBase.m_vsmShadowVariance
											, light.m_transforms[cascadeIndex - 1u]
											, fragmentIn.m_worldVertex
											, -lightDirection
											, cascadeIndex - 1u
											, light.m_cascadeCount
											, fragmentIn.m_worldNormal ) );
							}
							FI;

							IF( m_writer, shadowFactor > 0.0_f )
							{
								doComputeLight( light.m_lightBase
									, worldEye
									, lightDirection
									, shininess
									, fragmentIn
									, output );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;

							if ( m_isOpaqueProgram )
							{
								IF( m_writer, light.m_lightBase.m_volumetricSteps != 0_u )
								{
									m_shadowModel->computeVolumetric( light.m_lightBase.m_shadowType
										, light.m_lightBase.m_rawShadowOffsets
										, light.m_lightBase.m_pcfShadowOffsets
										, light.m_lightBase.m_vsmShadowVariance
										, fragmentIn.m_clipVertex
										, fragmentIn.m_worldVertex
										, worldEye
										, light.m_transforms[cascadeIndex]
										, light.m_direction
										, cascadeIndex
										, light.m_cascadeCount
										, light.m_lightBase.m_colour
										, light.m_lightBase.m_intensity
										, light.m_lightBase.m_volumetricSteps
										, light.m_lightBase.m_volumetricScattering
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
								, worldEye
								, lightDirection
								, shininess
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, fragmentIn
						, output );
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, fragmentIn.m_worldVertex - light.m_position.xyz() );
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
									, m_shadowModel->computePoint( light.m_lightBase.m_shadowType
										, light.m_lightBase.m_rawShadowOffsets
										, light.m_lightBase.m_pcfShadowOffsets
										, light.m_lightBase.m_vsmShadowVariance
										, fragmentIn.m_worldVertex
										, light.m_position.xyz()
										, fragmentIn.m_worldNormal
										, light.m_lightBase.m_farPlane
										, light.m_lightBase.m_index ) );
							}
							FI;

							IF( m_writer, shadowFactor > 0.0_f )
							{
								doComputeLight( light.m_lightBase
									, worldEye
									, lightDirection
									, shininess
									, fragmentIn
									, output );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;
						}
						ELSE
						{
							doComputeLight( light.m_lightBase
								, worldEye
								, lightDirection
								, shininess
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
						doComputeLight( light.m_lightBase
							, worldEye
							, lightDirection
							, shininess
							, fragmentIn
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
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, fragmentIn.m_worldVertex - light.m_position.xyz() );
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
									, fragmentIn.m_worldVertex
									, lightToVertex
									, fragmentIn.m_worldNormal
									, light.m_lightBase.m_index );

#else

								shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeSpot( light.m_lightBase.m_shadowType
										, light.m_lightBase.m_rawShadowOffsets
										, light.m_lightBase.m_pcfShadowOffsets
										, light.m_lightBase.m_vsmShadowVariance
										, light.m_transform
										, fragmentIn.m_worldVertex
										, lightToVertex
										, fragmentIn.m_worldNormal
										, light.m_lightBase.m_index ) );

#endif
							}
							FI;

							IF( m_writer, shadowFactor > 0.0_f )
							{
								doComputeLight( light.m_lightBase
									, worldEye
									, lightDirection
									, shininess
									, fragmentIn
									, output );
								output.m_diffuse *= shadowFactor;
								output.m_specular *= shadowFactor;
							}
							FI;
						}
						ELSE
						{
							doComputeLight( light.m_lightBase
								, worldEye
								, lightDirection
								, shininess
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
						doComputeLight( light.m_lightBase
							, worldEye
							, lightDirection
							, shininess
							, fragmentIn
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
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< sdw::Void >( "doComputeLight"
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & lightDirection
					, Float const & shininess
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( fragmentIn.m_worldNormal, -lightDirection ) );
					auto isLit = m_writer.declLocale( "isLit"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					output.m_diffuse = isLit
						* light.m_colour
						* light.m_intensity.x()
						* diffuseFactor;

					// Specular term.
					auto vertexToEye = m_writer.declLocale( "vertexToEye"
						, normalize( worldEye - fragmentIn.m_worldVertex ) );
					// Blinn Phong
					auto halfwayDir = m_writer.declLocale( "halfwayDir"
						, normalize( vertexToEye - lightDirection ) );
					auto specularFactor = m_writer.declLocale( "specularFactor"
						, max( dot( fragmentIn.m_worldNormal, halfwayDir ), 0.0_f ) );
					// Phong
					//auto lightReflect = m_writer.declLocale( "lightReflect"
					//	, normalize( reflect( lightDirection, fragmentIn.m_worldNormal ) ) );
					//auto specularFactor = m_writer.declLocale( "specularFactor"
					//	, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
					output.m_specular = isLit
						* light.m_colour
						* light.m_intensity.y()
						* pow( specularFactor, clamp( shininess, 1.0_f, 256.0_f ) );
				}
				, InLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "lightDirection" )
				, InFloat( m_writer, "shininess" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & lightDirection
			, Float const & shininess
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_computeLight( light
				, worldEye
				, lightDirection
				, shininess
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}
	}
}
