#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

using namespace castor;
using namespace sdw;

#define C3D_DebugCascades 0

namespace castor3d
{
	namespace shader
	{
		const String MetallicBrdfLightingModel::Name = cuT( "pbr_mr" );

		MetallicBrdfLightingModel::MetallicBrdfLightingModel( ShaderWriter & m_writer
			, Utils & utils )
			: LightingModel{ m_writer, utils }
		{
		}

		void MetallicBrdfLightingModel::computeCombined( Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			auto c3d_lightsCount = m_writer.getVariable< IVec4 >( "c3d_lightsCount" );
			auto begin = m_writer.declLocale( "begin"
				, 0_i );
			auto end = m_writer.declLocale( "end"
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, i, begin, i < end, ++i )
			{
				m_computeDirectional( getDirectionalLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, i, begin, i < end, ++i )
			{
				m_computePoint( getPointLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, i, begin, i < end, ++i )
			{
				m_computeSpot( getSpotLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;
		}

		void MetallicBrdfLightingModel::compute( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void MetallicBrdfLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeOnePoint( PointLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void MetallicBrdfLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeOneSpot( SpotLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		std::shared_ptr< MetallicBrdfLightingModel > MetallicBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & index
			, uint32_t maxCascades )
		{
			auto result = std::make_shared< MetallicBrdfLightingModel >( writer, utils );
			result->declareModel( index, maxCascades );
			return result;
		}

		std::shared_ptr< MetallicBrdfLightingModel > MetallicBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, ShadowType shadows
			, bool volumetric
			, uint32_t & index
			, uint32_t maxCascades )
		{
			auto result = std::make_shared< MetallicBrdfLightingModel >( writer, utils );
			result->declareDirectionalModel( shadows
				, volumetric
				, index
				, maxCascades );
			return result;
		}

		std::shared_ptr< MetallicBrdfLightingModel > MetallicBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, LightType lightType
			, ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			auto result = std::make_shared< MetallicBrdfLightingModel >( writer, utils );

			switch ( lightType )
			{
			case LightType::eDirectional:
				CU_Failure( "Directional light model should use the other overload" );
				break;

			case LightType::ePoint:
				result->declarePointModel( shadows, volumetric, index );
				break;

			case LightType::eSpot:
				result->declareSpotModel( shadows, volumetric, index );
				break;

			default:
				CU_Failure( "Invalid light type" );
				break;
			}

			return result;
		}

		void MetallicBrdfLightingModel::computeMapContributions( sdw::ShaderWriter & writer
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
			, sdw::Vec3 & albedo
			, sdw::Float & metallic
			, sdw::Vec3 & emissive
			, sdw::Float & roughness
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
					auto name = string::stringCast< char >( string::toString( i, std::locale{ "C" } ) );
					auto config = writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( writer.cast< UInt >( textureConfig[i / 4][i % 4] ) ) );
					auto sampled = writer.declLocale< Vec4 >( "sampled" + name
						, texture( maps[i], texCoords.xy() ) );

					if ( checkFlag( flags.textures, TextureFlag::eAlbedo ) )
					{
						albedo = config.getAlbedo( writer, sampled, albedo, gamma );
					}

					if ( checkFlag( flags.textures, TextureFlag::eMetalness ) )
					{
						metallic = config.getMetalness( writer, sampled, metallic );
					}

					if ( checkFlag( flags.textures, TextureFlag::eRoughness ) )
					{
						roughness = config.getRoughness( writer, sampled, roughness );
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

					if ( checkFlag( flags.textures, TextureFlag::eTransmittance ) )
					{
						transmittance = config.getTransmittance( writer, sampled, transmittance );
					}

					if ( checkFlag( flags.textures, TextureFlag::eNormal ) )
					{
						normal = config.getNormal( writer, sampled, normal, tangent, bitangent );
					}
				}
			}
		}

		void MetallicBrdfLightingModel::doDeclareModel()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeLight();
		}

		void MetallicBrdfLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.m_direction ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, UInt( 0u ) );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						// Get cascade index for the current fragment's view position
						auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

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
								, -lightDirection
								, cascadeIndex
								, fragmentIn.m_worldNormal ) );
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn
						, output );

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
							, -lightDirection
							, cascadeIndex
							, light.m_lightBase.m_colour
							, light.m_lightBase.m_intensity
							, light.m_lightBase.m_volumetricSteps
							, light.m_lightBase.m_volumetricScattering
							, output );
					}
					FI;

#if C3D_DebugCascades
					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, cascadeIndex == 0 )
						{
							output.m_diffuse.rgb() = vec3( 1.0_f, 0.25f, 0.25f );
						}
						ELSEIF( m_writer, cascadeIndex == 1 )
						{
							output.m_diffuse.rgb() = vec3( 0.25_f, 1.0f, 0.25f );
						}
						ELSEIF( m_writer, cascadeIndex == 2 )
						{
							output.m_diffuse.rgb() = vec3( 0.25_f, 0.25f, 1.0f );
						}
						ELSE
						{
							output.m_diffuse.rgb() = vec3( 1.0_f, 1.0f, 0.25f );
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
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
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

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
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
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.m_direction ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f - step( spotFactor, light.m_cutOff ) );

					IF( m_writer, light.m_lightBase.m_shadowType != Int( int( ShadowType::eNone ) ) )
					{
						shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeSpotShadow( light.m_lightBase.m_shadowType
								, light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, light.m_transform
								, fragmentIn.m_worldVertex
								, -lightToVertex
								, fragmentIn.m_worldNormal
								, light.m_lightBase.m_index ) );
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					spotFactor = sdw::fma( m_writer.paren( spotFactor - 1.0_f )
						, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff )
						, 1.0_f );
					parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
					parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
				}
				, InSpotLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOneDirectionalLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this, shadowType, volumetric]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( -light.m_direction ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( "cascadeIndex"
						, 0_u
						, shadowType != ShadowType::eNone );

					if ( shadowType != ShadowType::eNone )
					{
						// Get cascade index for the current fragment's view position
						auto c3d_maxCascadeCount = m_writer.getVariable< UInt >( "c3d_maxCascadeCount" );
						auto maxCount = m_writer.declLocale( "maxCount"
							, m_writer.cast< UInt >( clamp( light.m_cascadeCount, 1_u, c3d_maxCascadeCount ) - 1_u ) );

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
								, -lightDirection
								, cascadeIndex
								, fragmentIn.m_worldNormal ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
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
							, -lightDirection
							, cascadeIndex
							, light.m_lightBase.m_colour
							, light.m_lightBase.m_intensity
							, light.m_lightBase.m_volumetricSteps
							, light.m_lightBase.m_volumetricScattering
							, output );
					}

#if C3D_DebugCascades
					if ( shadowType != ShadowType::eNone )
					{
						IF( m_writer, cascadeIndex == 0 )
						{
							output.m_diffuse.rgb() = vec3( 1.0_f, 0.25f, 0.25f );
							output.m_specular.rgb() = vec3( 1.0_f, 0.25f, 0.25f );
						}
						ELSEIF( m_writer, cascadeIndex == 1 )
						{
							output.m_diffuse.rgb() = vec3( 0.25_f, 1.0f, 0.25f );
							output.m_specular.rgb() = vec3( 0.25_f, 1.0f, 0.25f );
						}
						ELSEIF( m_writer, cascadeIndex == 2 )
						{
							output.m_diffuse.rgb() = vec3( 0.25_f, 0.25f, 1.0f );
							output.m_specular.rgb() = vec3( 0.25_f, 0.25f, 1.0f );
						}
						ELSE
						{
							output.m_diffuse.rgb() = vec3( 1.0_f, 1.0f, 0.25f );
							output.m_specular.rgb() = vec3( 1.0_f, 1.0f, 0.25f );
						}
						FI;
					}
#endif

					parentOutput.m_diffuse = output.m_diffuse;
					parentOutput.m_specular = output.m_specular;
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOnePointLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this, shadowType]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
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
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					parentOutput.m_diffuse = output.m_diffuse / attenuation;
					parentOutput.m_specular = output.m_specular / attenuation;
				}
				, InPointLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOneSpotLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this, shadowType]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( "lightDiffuse", vec3( 0.0_f ) ),
						m_writer.declLocale( "lightSpecular", vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( "distance"
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( "lightDirection"
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( "spotFactor"
						, dot( lightDirection, -light.m_direction ) );
					auto shadowFactor = m_writer.declLocale( "shadowFactor"
						, 1.0_f - step( spotFactor, light.m_cutOff ) );

					if ( shadowType != ShadowType::eNone )
					{
						shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeSpotShadow( light.m_lightBase.m_shadowOffsets
								, light.m_lightBase.m_shadowVariance
								, light.m_transform
								, fragmentIn.m_worldVertex
								, -lightToVertex
								, fragmentIn.m_worldNormal
								, light.m_lightBase.m_index ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( "attenuation"
						, sdw::fma( light.m_attenuation.z()
							, distance * distance
							, sdw::fma( light.m_attenuation.y()
								, distance
								, light.m_attenuation.x() ) ) );
					spotFactor = sdw::fma( m_writer.paren( spotFactor - 1.0_f )
						, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff )
						, 1.0_f );
					parentOutput.m_diffuse = spotFactor * output.m_diffuse / attenuation;
					parentOutput.m_specular = spotFactor * output.m_specular / attenuation;
				}
				, InSpotLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< Void >( "doComputeLight"
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & direction
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto L = m_writer.declLocale( "L"
						, normalize( direction ) );
					auto V = m_writer.declLocale( "V"
						, normalize( worldEye - fragmentIn.m_worldVertex ) );
					auto H = m_writer.declLocale( "H"
						, normalize( L + V ) );
					auto N = m_writer.declLocale( "N"
						, normalize( fragmentIn.m_worldNormal ) );
					auto radiance = m_writer.declLocale( "radiance"
						, light.m_colour );

					auto NdotL = m_writer.declLocale( "NdotL"
						, max( 0.0_f, dot( N, L ) ) );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( 0.0_f, dot( N, V ) ) );
					auto NdotH = m_writer.declLocale( "NdotH"
						, max( 0.0_f, dot( N, H ) ) );
					auto HdotV = m_writer.declLocale( "HdotV"
						, max( 0.0_f, dot( H, V ) ) );
					auto LdotV = m_writer.declLocale( "LdotV"
						, max( 0.0_f, dot( L, V ) ) );

					auto f0 = m_writer.declLocale( "f0"
						, mix( vec3( 0.04_f ), albedo, vec3( metallic ) ) );
					auto specfresnel = m_writer.declLocale( "specfresnel"
						, m_schlickFresnel( HdotV, f0 ) );
			
					auto NDF = m_writer.declLocale( "NDF"
						, m_distributionGGX( NdotH, roughness ) );
					auto G = m_writer.declLocale( "G"
						, m_geometrySmith( NdotV, NdotL, roughness ) );

					auto nominator = m_writer.declLocale( "nominator"
						, specfresnel * NDF * G );
					auto denominator = m_writer.declLocale( "denominator"
						, sdw::fma( 4.0_f
							, NdotV * NdotL
							, 0.001_f ) );
					auto specReflectance = m_writer.declLocale( "specReflectance"
						, nominator / denominator );
					auto kS = m_writer.declLocale( "kS"
						, specfresnel );
					auto kD = m_writer.declLocale( "kD"
						, vec3( 1.0_f ) - kS );

					kD *= 1.0_f - metallic;

					output.m_diffuse = shadowFactor * m_writer.paren( radiance * NdotL * kD / Float{ Pi< float > } );
					output.m_specular = shadowFactor * m_writer.paren( specReflectance * radiance * NdotL );
				}
				, InLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "direction" )
				, InVec3( m_writer, "albedo" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InFloat( m_writer, "shadowFactor" )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareDistribution()
		{
			// Distribution Function
			m_distributionGGX = m_writer.implementFunction< Float >( "Distribution"
				, [this]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto a = m_writer.declLocale( "a"
						, roughness * roughness );
					auto a2 = m_writer.declLocale( "a2"
						, a * a );
					auto NdotH2 = m_writer.declLocale( "NdotH2"
						, product * product );

					auto nominator = m_writer.declLocale( "num"
						, a2 );
					auto denominator = m_writer.declLocale( "denom"
						, sdw::fma( NdotH2
							, a2 - 1.0_f
							, 1.0_f ) );
					denominator = Float{ Pi< float > } * denominator * denominator;

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( m_writer, "product" )
				, InFloat( m_writer, "roughness" ) );
		}
	
		void MetallicBrdfLightingModel::doDeclareGeometry()
		{
			// Geometry Functions
			m_geometrySchlickGGX = m_writer.implementFunction< Float >( "GeometrySchlickGGX"
				, [this]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = m_writer.declLocale( "r"
						, roughness + 1.0_f );
					auto k = m_writer.declLocale( "k"
						, m_writer.paren( r * r ) / 8.0_f );

					auto nominator = m_writer.declLocale( "num"
						, product );
					auto denominator = m_writer.declLocale( "denom"
						, sdw::fma( product
							, 1.0_f - k
							, k ) );

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( m_writer, "product" )
				, InFloat( m_writer, "roughness" ) );

			m_geometrySmith = m_writer.implementFunction< Float >( "GeometrySmith"
				, [this]( Float const & NdotV
					, Float const & NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = m_writer.declLocale( "ggx2"
						, m_geometrySchlickGGX( NdotV, roughness ) );
					auto ggx1 = m_writer.declLocale( "ggx1"
						, m_geometrySchlickGGX( NdotL, roughness ) );

					m_writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( m_writer, "NdotV" )
				, InFloat( m_writer, "NdotL" )
				, InFloat( m_writer, "roughness" ) );
		}
	
		void MetallicBrdfLightingModel::doDeclareFresnelShlick()
		{
			// Fresnel Function
			m_schlickFresnel = m_writer.implementFunction< Vec3 >( "FresnelShlick"
				, [this]( Float const & product
					, Vec3 const & f0 )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					m_writer.returnStmt( sdw::fma( vec3( 1.0_f ) - f0
						, vec3( pow( 1.0_f - product, 5.0_f ) )
						, f0 ) );
				}
				, InFloat( m_writer, "product" )
				, InVec3( m_writer, "f0" ) );
		}
	
		void MetallicBrdfLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & direction
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & output )const
		{
			m_computeLight( light
				, worldEye
				, direction
				, albedo
				, metallic
				, roughness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, output );
		}

		//***********************************************************************************************
	}
}
