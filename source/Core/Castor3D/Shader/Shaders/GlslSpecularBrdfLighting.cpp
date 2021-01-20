#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		const String SpecularBrdfLightingModel::Name = cuT( "pbr_sg" );

		SpecularBrdfLightingModel::SpecularBrdfLightingModel( ShaderWriter & writer
			, Utils & utils
			, ShadowOptions shadowOptions
			, bool isOpaqueProgram )
			: LightingModel{ writer
				, utils
				, std::move( shadowOptions )
				, isOpaqueProgram }
			, m_cookTorrance{ writer }
		{
		}

		void SpecularBrdfLightingModel::computeCombined( Vec3 const & worldEye
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

			FOR( m_writer, Int, dir, begin, dir < end, ++dir )
			{
				compute( getDirectionalLight( dir )
					, worldEye
					, specular
					, glossiness
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
					, specular
					, glossiness
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
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void SpecularBrdfLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computePoint( PointLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		void SpecularBrdfLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_computeSpot( SpotLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
		}

		Vec3 SpecularBrdfLightingModel::computeCombinedDiffuse( Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )const
		{
			auto c3d_lightsCount = m_writer.getVariable< IVec4 >( "c3d_lightsCount" );
			auto begin = m_writer.declLocale( "begin"
				, 0_i );
			auto end = m_writer.declLocale( "end"
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );
			auto result = m_writer.declLocale( "result"
				, vec3( 0.0_f ) );

			FOR( m_writer, Int, dir, begin, dir < end, ++dir )
			{
				result += computeDiffuse( getDirectionalLight( dir )
					, worldEye
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn } );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, point, begin, point < end, ++point )
			{
				result += computeDiffuse( getPointLight( point )
					, worldEye
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn } );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, spot, begin, spot < end, ++spot )
			{
				result += computeDiffuse( getSpotLight( spot )
					, worldEye
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn } );
			}
			ROF;

			return result;
		}

		Vec3 SpecularBrdfLightingModel::computeDiffuse( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )const
		{
			return m_computeDirectionalDiffuse( DirectionalLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn } );
		}

		Vec3 SpecularBrdfLightingModel::computeDiffuse( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )const
		{
			return m_computePointDiffuse( PointLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn } );
		}

		Vec3 SpecularBrdfLightingModel::computeDiffuse( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )const
		{
			return m_computeSpotDiffuse( SpotLight{ light }
				, worldEye
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn } );
		}

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, SceneFlags sceneFlags
			, bool rsm
			, uint32_t & index
			, bool isOpaqueProgram )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer
				, utils
				, ShadowOptions{ ( sceneFlags & SceneFlag::eShadowAny ), rsm }
				, isOpaqueProgram );
			result->declareModel( index );
			return result;
		}

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createModel( sdw::ShaderWriter & writer
			, Utils & utils
			, LightType lightType
			, bool lightUbo
			, bool shadows
			, bool rsm
			, uint32_t & index )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer
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

		std::shared_ptr< SpecularBrdfLightingModel > SpecularBrdfLightingModel::createDiffuseModel( sdw::ShaderWriter & writer
			, Utils & utils
			, SceneFlags sceneFlags
			, bool rsm
			, uint32_t & index
			, bool isOpaqueProgram )
		{
			auto result = std::make_shared< SpecularBrdfLightingModel >( writer
				, utils
				, ShadowOptions{ ( sceneFlags & SceneFlag::eShadowAny ), rsm }
				, isOpaqueProgram );
			result->declareDiffuseModel( index );
			return result;
		}

		void SpecularBrdfLightingModel::computeMapContributions( PipelineFlags const & flags
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
			, sdw::Vec3 & albedo
			, sdw::Vec3 & specular
			, sdw::Float & glossiness
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition )
		{
			for ( uint32_t i = 0u; i < flags.textures.size(); ++i )
			{
				auto name = string::stringCast< char >( string::toString( i, std::locale{ "C" } ) );
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

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eAlbedo ) )
				{
					albedo = config.getAlbedo( m_writer, sampled, albedo, gamma );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eSpecular ) )
				{
					specular = config.getSpecular( m_writer, sampled, specular );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eGlossiness ) )
				{
					glossiness = config.getGlossiness( m_writer, sampled, glossiness );
				}
			}
		}

		void SpecularBrdfLightingModel::computeMapVoxelContributions( PipelineFlags const & flags
			, sdw::Float const & gamma
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Vec3 & albedo
			, sdw::Vec3 & specular
			, sdw::Float & glossiness )
		{
			for ( uint32_t i = 0u; i < flags.textures.size(); ++i )
			{
				auto name = string::stringCast< char >( string::toString( i, std::locale{ "C" } ) );
				auto config = m_writer.declLocale( "config" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( flags.textures[i].id ) ) );
				auto sampled = m_writer.declLocale( "sampled" + name
					, m_utils.computeCommonMapVoxelContribution( flags.textures[i].flags
						, flags.passFlags
						, name
						, config
						, maps[i]
						, gamma
						, texCoords
						, emissive
						, opacity
						, occlusion ) );

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eAlbedo ) )
				{
					albedo = config.getAlbedo( m_writer, sampled, albedo, gamma );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eSpecular ) )
				{
					specular = config.getSpecular( m_writer, sampled, specular );
				}

				if ( checkFlag( flags.textures[i].flags, TextureFlag::eGlossiness ) )
				{
					glossiness = config.getGlossiness( m_writer, sampled, glossiness );
				}
			}
		}

		void SpecularBrdfLightingModel::doDeclareModel()
		{
			m_cookTorrance.declare();
		}

		void SpecularBrdfLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< sdw::Void >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
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
					PbrSGMaterials materials{ m_writer };
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
											, -lightDirection
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
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, 1.0_f - glossiness
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
							m_cookTorrance.compute( light.m_lightBase
								, worldEye
								, lightDirection
								, specular
								, 1.0_f - glossiness
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, 1.0_f - glossiness
							, fragmentIn
							, output );
					}

					parentOutput.m_diffuse += max( vec3( 0.0_f ), output.m_diffuse );
					parentOutput.m_specular += max( vec3( 0.0_f ), output.m_specular );
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
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
					PbrSGMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
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

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, 1.0_f - glossiness
									, fragmentIn
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
								, specular
								, 1.0_f - glossiness
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, 1.0_f - glossiness
							, fragmentIn
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
				, InVec3( m_writer, "worldEye" )
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
					PbrSGMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
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
								, m_shadowModel->computeSpot( light.m_lightBase.m_shadowType
									, light.m_lightBase.m_rawShadowOffsets
									, light.m_lightBase.m_pcfShadowOffsets
									, light.m_lightBase.m_vsmShadowVariance
									, light.m_transform
									, fragmentIn.m_worldVertex
									, -lightToVertex
									, fragmentIn.m_worldNormal
									, light.m_lightBase.m_index ) );

							IF( m_writer, shadowFactor )
							{
								m_cookTorrance.compute( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, 1.0_f - glossiness
									, fragmentIn
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
								, specular
								, 1.0_f - glossiness
								, fragmentIn
								, output );
						}
						FI;
					}
					else
					{
						m_cookTorrance.compute( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, 1.0_f - glossiness
							, fragmentIn
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
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareDiffuseModel()
		{
			m_cookTorrance.declareDiffuse();
		}

		void SpecularBrdfLightingModel::doDeclareComputeDirectionalLightDiffuse()
		{
			m_computeDirectionalDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeDirectionalLight"
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & specular
					, Float const & glossiness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrSGMaterials materials{ m_writer };
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
											, -lightDirection
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
							}
							FI;

							IF( m_writer, shadowFactor )
							{
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, fragmentIn );
							}
							FI;

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
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, specular
								, fragmentIn );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, fragmentIn );
					}

					m_writer.returnStmt( max( vec3( 0.0_f ), diffuse ) );
				}
				, InDirectionalLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer } );
		}

		void SpecularBrdfLightingModel::doDeclareComputePointLightDiffuse()
		{
			m_computePointDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computePointLight"
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & specular
					, Float const & glossiness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrSGMaterials materials{ m_writer };
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
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

							IF( m_writer, shadowFactor )
							{
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, fragmentIn );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, specular
								, fragmentIn );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, fragmentIn );
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
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer } );
		}

		void SpecularBrdfLightingModel::doDeclareComputeSpotLightDiffuse()
		{
			m_computeSpotDiffuse = m_writer.implementFunction< sdw::Vec3 >( "computeSpotLight"
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & specular
					, Float const & glossiness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrSGMaterials materials{ m_writer };
					auto diffuse = m_writer.declLocale( "diffuse"
						, vec3( 0.0_f ) );
					auto lightToVertex = m_writer.declLocale( "lightToVertex"
						, light.m_position.xyz() - fragmentIn.m_worldVertex );
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
								, m_shadowModel->computeSpot( light.m_lightBase.m_shadowType
									, light.m_lightBase.m_rawShadowOffsets
									, light.m_lightBase.m_pcfShadowOffsets
									, light.m_lightBase.m_vsmShadowVariance
									, light.m_transform
									, fragmentIn.m_worldVertex
									, -lightToVertex
									, fragmentIn.m_worldNormal
									, light.m_lightBase.m_index ) );

							IF( m_writer, shadowFactor )
							{
								diffuse = shadowFactor * m_cookTorrance.computeDiffuse( light.m_lightBase
									, worldEye
									, lightDirection
									, specular
									, fragmentIn );
							}
							FI;
						}
						ELSE
						{
							diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
								, worldEye
								, lightDirection
								, specular
								, fragmentIn );
						}
						FI;
					}
					else
					{
						diffuse = m_cookTorrance.computeDiffuse( light.m_lightBase
							, worldEye
							, lightDirection
							, specular
							, fragmentIn );
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
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "specular" )
				, InFloat( m_writer, "glossiness" )
				, InInt( m_writer, "receivesShadows" )
				, FragmentInput{ m_writer } );
		}

		//***********************************************************************************************
	}
}
