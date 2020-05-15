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
			, bool isOpaqueProgram )
			: LightingModel{ m_writer, utils, isOpaqueProgram }
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
				m_computeDirectional( getDirectionalLight( dir )
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
				m_computePoint( getPointLight( point )
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
				m_computeSpot( getSpotLight( spot )
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
			m_computeDirectional( DirectionalLight{ light }
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
			m_computeOnePoint( PointLight{ light }
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
			m_computeOneSpot( SpotLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		std::shared_ptr< PhongLightingModel > PhongLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, bool rsm
			, uint32_t & index
			, bool isOpaqueProgram )
		{
			auto result = std::make_shared< PhongLightingModel >( writer, utils, isOpaqueProgram );
			result->declareModel( rsm, index );
			return result;
		}

		std::shared_ptr< PhongLightingModel > PhongLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowType shadows
			, bool volumetric
			, bool rsm
			, uint32_t & index )
		{
			auto result = std::make_shared< PhongLightingModel >( writer, utils, true );
			result->declareDirectionalModel( shadows
				, volumetric
				, rsm
				, index );
			return result;
		}

		std::shared_ptr< PhongLightingModel > PhongLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, LightType lightType
			, ShadowType shadows
			, bool volumetric
			, bool rsm
			, uint32_t & index )
		{
			auto result = std::make_shared< PhongLightingModel >( writer, utils, true );

			switch ( lightType )
			{
			case LightType::eDirectional:
				CU_Failure( "Directional light model should use the other overload" );
				break;

			case LightType::ePoint:
				result->declarePointModel( shadows, volumetric, rsm, index );
				break;

			case LightType::eSpot:
				result->declareSpotModel( shadows, volumetric, rsm, index );
				break;

			default:
				CU_Failure( "Invalid light type" );
				break;
			}

			return result;
		}

		void PhongLightingModel::computeMapContributions( sdw::ShaderWriter & writer
			, shader::Utils const & utils
			, PipelineFlags const & flags
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & normal
			, sdw::Vec3 & diffuse
			, sdw::Vec3 & specular
			, sdw::Vec3 & emissive
			, sdw::Float & shininess
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance )
		{
			if ( ( flags.texturesCount > 1
					&& checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMapping )
					&& checkFlag( flags.textures, TextureFlag::eHeight ) )
				|| ( flags.texturesCount > 0
					&& ( !checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMapping )
						|| !checkFlag( flags.textures, TextureFlag::eHeight ) ) ) )
			{
				for ( uint32_t i = 0u; i < flags.texturesCount; ++i )
				{
					auto name = string::stringCast< char >( string::toString( i ) );
					auto config = writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( writer.cast< UInt >( textureConfig[i / 4u][i % 4u] ) ) );
					auto sampled = writer.declLocale< Vec4 >( "sampled" + name
						, texture( maps[i], config.convertUV( writer, texCoords.xy() ) ) );

					if ( checkFlag( flags.textures, TextureFlag::eDiffuse ) )
					{
						diffuse = config.getDiffuse( writer, sampled, diffuse, gamma );
					}

					if ( checkFlag( flags.textures, TextureFlag::eSpecular ) )
					{
						specular = config.getSpecular( writer, sampled, specular );
					}

					if ( checkFlag( flags.textures, TextureFlag::eShininess ) )
					{
						shininess = config.getShininess( writer, sampled, shininess );
					}

					if ( checkFlag( flags.textures, TextureFlag::eOpacity ) )
					{
						opacity = config.getOpacity( writer, sampled, opacity );
					}

					if ( checkFlag( flags.textures, TextureFlag::eEmissive ) )
					{
						emissive = config.getEmissive( writer, sampled, emissive, gamma );
					}

					if ( checkFlag( flags.textures, TextureFlag::eOcclusion ) )
					{
						occlusion = config.getOcclusion( writer, sampled, occlusion );
					}

					if ( checkFlag( flags.textures, TextureFlag::eNormal ) )
					{
						normal = config.getNormal( writer, sampled, normal, tangent, bitangent );
					}

					if ( checkFlag( flags.textures, TextureFlag::eTransmittance ) )
					{
						transmittance = config.getTransmittance( writer, sampled, transmittance );
					}
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
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, 0_u );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

						// Get cascade index for the current fragment's view position
						FOR( m_writer, UInt, i, 0u, i < maxCount, ++i )
						{
							IF( m_writer, -fragmentIn.m_viewVertex.z() < light.m_splitDepths[i] )
							{
								cascadeIndex = i + 1_u;
							}
							FI;
						}
						ROF;

						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeDirectionalShadow( light.m_lightBase.m_shadowType
								, light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, light.m_transforms[cascadeIndex]
								, fragmentIn.m_worldVertex
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount
								, fragmentIn.m_worldNormal ) );
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );

					if ( m_isOpaqueProgram )
					{
						IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) )
							&& light.m_lightBase.m_volumetricSteps != 0_u )
						{
							m_shadowModel->computeVolumetric( light.m_lightBase.m_shadowType
								, light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
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
					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
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
					}
					FI;
#endif

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
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
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computePointShadow( light.m_lightBase.m_shadowType
									, light.m_lightBase.m_shadowOffsets
									, light.m_lightBase.m_shadowVariance
									, fragmentIn.m_worldVertex
									, light.m_position.xyz()
									, fragmentIn.m_worldNormal
									, light.m_lightBase.m_farPlane
									, light.m_lightBase.m_index ) );
						}
						FI;
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
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
					parentOutput.m_diffuse += output.m_diffuse / attenuation;
					parentOutput.m_specular += output.m_specular / attenuation;
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
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, light.m_direction ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f - step( spotFactor, light.m_cutOff ) );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, light.m_lightBase.m_index >= 0_i )
						{
							shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpotShadow( light.m_lightBase.m_shadowType
									, light.m_lightBase.m_shadowOffsets
									, light.m_lightBase.m_shadowVariance
									, light.m_transform
									, fragmentIn.m_worldVertex
									, lightToVertex
									, fragmentIn.m_worldNormal
									, light.m_lightBase.m_index ) );
						}
						FI;
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					spotFactor = sdw::fma( ( spotFactor - 1.0_f )
						, 1.0_f / ( 1.0_f - light.m_cutOff )
						, 1.0_f );
					parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
					parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
				}
				, InSpotLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOneDirectionalLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this, shadowType, volumetric]( DirectionalLight const & light
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
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, 0_u
						, shadowType != ShadowType::eNone );

					if ( shadowType != ShadowType::eNone )
					{
						auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

						// Get cascade index for the current fragment's view position
						FOR( m_writer, UInt, i, 0u, i < maxCount, ++i )
						{
							IF( m_writer, fragmentIn.m_viewVertex.z() < light.m_splitDepths[i] )
							{
								cascadeIndex = i + 1_u;
							}
							FI;
						}
						ROF;

						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeDirectionalShadow( light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, light.m_transforms[cascadeIndex]
								, fragmentIn.m_worldVertex
								, lightDirection
								, cascadeIndex
								, light.m_cascadeCount
								, fragmentIn.m_worldNormal ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );

					if ( shadowType != ShadowType::eNone && volumetric )
					{
						m_shadowModel->computeVolumetric( light.m_lightBase.m_shadowOffsets
							, light.m_lightBase.m_shadowVariance
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

#if C3D_DebugCascades
					if ( shadowType != ShadowType::eNone )
					{
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
					}
#endif

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOnePointLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this, shadowType]( PointLight const & light
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
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );

					if ( shadowType != ShadowType::eNone )
					{
						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computePointShadow( light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, fragmentIn.m_worldVertex
								, light.m_position.xyz()
								, fragmentIn.m_worldNormal
								, light.m_lightBase.m_farPlane
								, light.m_lightBase.m_index ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					parentOutput.m_diffuse += output.m_diffuse / attenuation;
					parentOutput.m_specular += output.m_specular / attenuation;
				}
				, InPointLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InFloat( m_writer, "shininess" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOneSpotLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this, shadowType]( SpotLight const & light
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
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f - step( spotFactor, light.m_cutOff ) );

					if ( shadowType != ShadowType::eNone )
					{
#if C3D_DebugSpotShadows

						shadowFactor = m_shadowModel->computeSpotShadow( light.m_lightBase.m_shadowOffsets
							, light.m_lightBase.m_shadowVariance
							, light.m_transform
							, fragmentIn.m_worldVertex
							, lightToVertex
							, fragmentIn.m_worldNormal
							, light.m_lightBase.m_index );

#else

						shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeSpotShadow( light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, light.m_transform
								, fragmentIn.m_worldVertex
								, lightToVertex
								, fragmentIn.m_worldNormal
								, light.m_lightBase.m_index ) );

#endif
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
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
					parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
					parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
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
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					// Diffuse term.
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, dot( fragmentIn.m_worldNormal, -lightDirection ) );
					auto stepFactor = m_writer.declLocale( "stepFactor"
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					output.m_diffuse = stepFactor * shadowFactor * light.m_colour * light.m_intensity.x() * diffuseFactor;

					// Specular term.
					auto vertexToEye = m_writer.declLocale( "vertexToEye"
						, normalize( worldEye - fragmentIn.m_worldVertex ) );
					auto lightReflect = m_writer.declLocale( "lightReflect"
						, normalize( reflect( lightDirection, fragmentIn.m_worldNormal ) ) );
					auto specularFactor = m_writer.declLocale( "specularFactor"
						, max( dot( vertexToEye, lightReflect ), 0.0_f ) );
					output.m_specular = stepFactor * shadowFactor * light.m_colour * light.m_intensity.y() * pow( specularFactor, max( shininess, 0.1_f ) );
				}
				, InLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "lightDirection" )
				, InFloat( m_writer, "shininess" )
				, InFloat( m_writer, "shadowFactor" )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & lightDirection
			, Float const & shininess
			, Float const & p_shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_computeLight( light
				, worldEye
				, lightDirection
				, shininess
				, p_shadowFactor
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}
	}
}
