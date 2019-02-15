#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		const String SpecularBrdfLightingModel::Name = cuT( "pbr_sg" );

		SpecularBrdfLightingModel::SpecularBrdfLightingModel( ShaderWriter & writer )
			: LightingModel{ writer }
		{
		}

		void SpecularBrdfLightingModel::computeCombined( Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
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
					, diffuse
					, specular
					, glossiness
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
					, diffuse
					, specular
					, glossiness
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
					, diffuse
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
			}
			ROF;
		}

		void SpecularBrdfLightingModel::compute( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void SpecularBrdfLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeOnePoint( PointLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void SpecularBrdfLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeOneSpot( SpotLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, uint32_t & index
			, uint32_t maxCascades )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer );
			result->declareModel( index, maxCascades );
			return result;
		}

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, ShadowType shadows
			, bool volumetric
			, uint32_t & index
			, uint32_t maxCascades )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer );
			result->declareDirectionalModel( shadows
				, volumetric
				, index
				, maxCascades );
			return result;
		}

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, LightType lightType
			, ShadowType shadows
			, bool volumetric
			, uint32_t & index )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer );

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

		void SpecularBrdfLightingModel::computeMapContributions( sdw::ShaderWriter & writer
			, shader::Utils const & utils
			, sdw::Vec3 & normal
			, sdw::Vec3 & diffuse
			, sdw::Vec3 & specular
			, sdw::Vec3 & emissive
			, sdw::Float & glossiness
			, sdw::Float const & gamma
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, PassFlags const & passFlags )
		{
			using namespace sdw;
			auto texCoord( writer.getVariable< Vec3 >( "texCoord" ) );

			if ( checkFlag( textureFlags, TextureChannel::eNormal ) )
			{
				auto vtx_normal( writer.getVariable< Vec3 >( "vtx_normal" ) );
				auto vtx_tangent( writer.getVariable< Vec3 >( "vtx_tangent" ) );
				auto vtx_bitangent( writer.getVariable< Vec3 >( "vtx_bitangent" ) );
				auto c3d_mapNormal( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapNormal" ) );

				auto v3MapNormal = writer.declLocale( "v3MapNormal"
					, texture( c3d_mapNormal, texCoord.xy() ).xyz() );
				v3MapNormal = normalize( sdw::fma( v3MapNormal
					, vec3( 2.0_f )
					, vec3( -1.0_f ) ) );

				if ( checkFlag( textureFlags, TextureChannel::eHeight )
					&& !checkFlag( passFlags, PassFlag::eParallaxOcclusionMapping ) )
				{
					auto c3d_mapHeight( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapHeight" ) );
					auto mapHeight = writer.declLocale( "mapHeight"
						, texture( c3d_mapHeight, texCoord.xy() ).r() );
					v3MapNormal = mix( vec3( 0.0_f, 0.0_f, 1.0_f )
						, v3MapNormal
						, vec3( mapHeight ) );
				}

				auto tbn = writer.declLocale( "tbn"
					, mat3( normalize( vtx_tangent )
						, normalize( vtx_bitangent )
						, normal ) );

				if ( utils.hasInvertedNormals() )
				{
					normal = normalize( v3MapNormal * tbn );
				}
				else
				{
					normal = normalize( tbn * v3MapNormal );
				}
			}

			if ( checkFlag( textureFlags, TextureChannel::eSpecular ) )
			{
				auto c3d_mapSpecular( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapSpecular" ) );
				specular *= texture( c3d_mapSpecular, texCoord.xy() ).rgb();
			}

			if ( checkFlag( textureFlags, TextureChannel::eGloss ) )
			{
				auto c3d_mapGloss( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapGloss" ) );
				glossiness *= texture( c3d_mapGloss, texCoord.xy() ).r();
			}

			if ( checkFlag( textureFlags, TextureChannel::eAlbedo ) )
			{
				auto c3d_mapDiffuse( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapDiffuse" ) );
				diffuse *= utils.removeGamma( gamma
					, texture( c3d_mapDiffuse, texCoord.xy() ).xyz() );
			}

			emissive *= diffuse;

			if ( checkFlag( textureFlags, TextureChannel::eEmissive ) )
			{
				auto c3d_mapEmissive( writer.getVariable< SampledImage2DRgba32 >( "c3d_mapEmissive" ) );
				emissive *= utils.removeGamma( gamma
					, texture( c3d_mapEmissive, texCoord.xy() ).xyz() );
			}
		}

		void SpecularBrdfLightingModel::doDeclareModel()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeLight();
		}

		void SpecularBrdfLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
						IF ( m_writer, receivesShadows != 0_i )
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
					}
					FI;

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
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
							, light.m_direction
							, cascadeIndex
							, light.m_lightBase.m_colour
							, light.m_lightBase.m_intensity
							, light.m_lightBase.m_volumetricSteps
							, light.m_lightBase.m_volumetricScattering
							, output );
					}
					FI;

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
						IF( m_writer, receivesShadows != 0_i )
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
						, diffuse
						, specular
						, glossiness
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
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
						, diffuse
						, specular
						, glossiness
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
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOneDirectionalLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this, shadowType, volumetric]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
						, UInt( 0u )
						, shadowType != ShadowType::eNone );

					if ( shadowType != ShadowType::eNone )
					{
						IF ( m_writer, receivesShadows != 0_i )
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
						FI;
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
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
							, light.m_lightBase.m_colour
							, light.m_lightBase.m_intensity
							, light.m_lightBase.m_volumetricSteps
							, light.m_lightBase.m_volumetricScattering
							, output );
					}

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOnePointLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< sdw::Void >( "computePointLight"
				, [this, shadowType]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
								, light.m_lightBase.m_farPlane ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
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
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOneSpotLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< sdw::Void >( "computeSpotLight"
				, [this, shadowType]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
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
								, fragmentIn.m_worldNormal ) );
					}

					doComputeLight( light.m_lightBase
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
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
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< sdw::Void >( "doComputeLight"
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & direction
					, Vec3 const & diffuse
					, Vec3 const & specular
					, Float const & glossiness
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					auto roughness = m_writer.declLocale( "roughness"
						, 1.0_f - glossiness );
					// From https://learnopengl.com/#!PBR/Lighting
					auto L = m_writer.declLocale( "L"
						, normalize( direction ) );
					auto V = m_writer.declLocale( "V"
						, normalize( normalize( worldEye - fragmentIn.m_worldVertex ) ) );
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
						, specular );
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

					kD *= 1.0_f - length( specular );

					output.m_diffuse = shadowFactor * m_writer.paren( radiance * NdotL * kD / Float{ Pi< float > } );
					output.m_specular = shadowFactor * m_writer.paren( specReflectance * radiance * NdotL );
				}
				, InLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "direction" )
				, InVec3( m_writer, "diffuse" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InFloat( m_writer, "shadowFactor" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareDistribution()
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
							, a2 - 1.0
							, 1.0_f ) );
					denominator = Float{ Pi< float > } * denominator * denominator;

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( m_writer, "product" )
				, InFloat( m_writer, "roughness" ) );
		}
	
		void SpecularBrdfLightingModel::doDeclareGeometry()
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
	
		void SpecularBrdfLightingModel::doDeclareFresnelShlick()
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

		void SpecularBrdfLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & direction
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_computeLight( light
				, worldEye
				, direction
				, diffuse
				, specular
				, glossiness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		//***********************************************************************************************
	}
}
