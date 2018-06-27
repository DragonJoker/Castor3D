#include "GlslSpecularBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		const String SpecularBrdfLightingModel::Name = cuT( "pbr_sg" );

		SpecularBrdfLightingModel::SpecularBrdfLightingModel( GlslWriter & writer )
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
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" )
				, 0_i );
			auto end = m_writer.declLocale( cuT( "end" )
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				m_writer << m_computeDirectional( getDirectionalLight( i )
					, worldEye
					, diffuse
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
				m_writer << endi;
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				m_writer << m_computePoint( getPointLight( i )
					, worldEye
					, diffuse
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
				m_writer << endi;
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				m_writer << m_computeSpot( getSpotLight( i )
					, worldEye
					, diffuse
					, specular
					, glossiness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
				m_writer << endi;
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
			m_writer << m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
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
			m_writer << m_computeOnePoint( PointLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
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
			m_writer << m_computeOneSpot( SpotLight{ light }
				, worldEye
				, diffuse
				, specular
				, glossiness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
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
			m_computeDirectional = m_writer.implementFunction< Void >( cuT( "computeDirectionalLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( cuT( "cascadeIndex" )
						, UInt( 0u ) );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
					{
						IF ( m_writer, receivesShadows != 0_i )
						{
							// Get cascade index for the current fragment's view position
							FOR( m_writer, UInt, i, 0u, cuT( "i < uint( min( c3d_maxCascadeCount, uint( light.m_directionCount.w ) ) - 1u )" ), cuT( "++i" ) )
							{
								IF( m_writer, worldEye.z() < light.m_splitDepth( i ) )
								{
									cascadeIndex = i + 1;
								}
								FI;
							}
							ROF;

							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeDirectionalShadow( light.m_lightBase().m_shadowType()
									, light.m_transform( cascadeIndex )
									, fragmentIn.m_worldVertex
									, -lightDirection
									, cascadeIndex
									, fragmentIn.m_worldNormal ) );
						}
						FI;
					}
					FI;

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
						, shadowFactor
						, fragmentIn
						, output );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) )
						&& light.m_lightBase().m_volumetricSteps() != 0_ui )
					{
						m_shadowModel->computeVolumetric( light.m_lightBase().m_shadowType()
							, fragmentIn.m_worldVertex
							, worldEye
							, light.m_transform( cascadeIndex )
							, light.m_direction()
							, cascadeIndex
							, light.m_lightBase().m_colour()
							, light.m_lightBase().m_intensity()
							, light.m_lightBase().m_volumetricSteps()
							, light.m_lightBase().m_volumetricScattering()
							, output );
					}
					FI;

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computePointShadow( light.m_lightBase().m_shadowType()
									, fragmentIn.m_worldVertex
									, light.m_position().xyz()
									, fragmentIn.m_worldNormal
									, light.m_lightBase().m_farPlane()
									, light.m_lightBase().m_index() ) );
						}
						FI;
					}
					FI;

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, glsl::fma( light.m_attenuation().z()
							, distance * distance
							, glsl::fma( light.m_attenuation().y()
								, distance
								, light.m_attenuation().x() ) ) );
					parentOutput.m_diffuse += output.m_diffuse / attenuation;
					parentOutput.m_specular += output.m_specular / attenuation;
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
						{
							IF( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
									, m_shadowModel->computeSpotShadow( light.m_lightBase().m_shadowType()
										, light.m_transform()
										, fragmentIn.m_worldVertex
										, -lightToVertex
										, fragmentIn.m_worldNormal
										, light.m_lightBase().m_index() ) );
							}
							FI;
						}
						FI;

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, diffuse
							, specular
							, glossiness
							, shadowFactor
							, fragmentIn
							, output );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, glsl::fma( light.m_attenuation().z()
								, distance * distance
								, glsl::fma( light.m_attenuation().y()
									, distance
									, light.m_attenuation().x() ) ) );
						spotFactor = glsl::fma( m_writer.paren( spotFactor - 1.0_f )
							, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() )
							, 1.0_f );
						parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
						parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOneDirectionalLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< Void >( cuT( "computeDirectionalLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( cuT( "cascadeIndex" )
						, shadowType != ShadowType::eNone
						, UInt( 0u ) );

					if ( shadowType != ShadowType::eNone )
					{
						IF ( m_writer, receivesShadows != 0_i )
						{
							// Get cascade index for the current fragment's view position
							FOR( m_writer, UInt, i, 0u, cuT( "i < uint( min( c3d_maxCascadeCount, uint( light.m_directionCount.w ) ) - 1u )" ), cuT( "++i" ) )
							{
								IF( m_writer, worldEye.z() < light.m_splitDepth( i ) )
								{
									cascadeIndex = i + 1;
								}
								FI;
							}
							ROF;

							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeDirectionalShadow( light.m_transform( cascadeIndex )
									, fragmentIn.m_worldVertex
									, -lightDirection
									, cascadeIndex
									, fragmentIn.m_worldNormal ) );
						}
						FI;
					}

					doComputeLight( light.m_lightBase()
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
						m_shadowModel->computeVolumetric( fragmentIn.m_worldVertex
							, worldEye
							, light.m_transform( cascadeIndex )
							, light.m_direction()
							, cascadeIndex
							, light.m_lightBase().m_colour()
							, light.m_lightBase().m_intensity()
							, light.m_lightBase().m_volumetricSteps()
							, light.m_lightBase().m_volumetricScattering()
							, output );
					}

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOnePointLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( shadowType != ShadowType::eNone )
					{
						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computePointShadow( fragmentIn.m_worldVertex
								, light.m_position().xyz()
								, fragmentIn.m_worldNormal
								, light.m_lightBase().m_farPlane() ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, diffuse
						, specular
						, glossiness
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, glsl::fma( light.m_attenuation().z()
							, distance * distance
							, glsl::fma( light.m_attenuation().y()
								, distance
								, light.m_attenuation().x() ) ) );
					parentOutput.m_diffuse += output.m_diffuse / attenuation;
					parentOutput.m_specular += output.m_specular / attenuation;
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeOneSpotLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
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
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_worldVertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( shadowType != ShadowType::eNone )
						{
							shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpotShadow( light.m_transform()
									, fragmentIn.m_worldVertex
									, -lightToVertex
									, fragmentIn.m_worldNormal ) );
						}

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, diffuse
							, specular
							, glossiness
							, shadowFactor
							, fragmentIn
							, output );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, glsl::fma( light.m_attenuation().z()
								, distance * distance
								, glsl::fma( light.m_attenuation().y()
									, distance
									, light.m_attenuation().x() ) ) );
						spotFactor = glsl::fma( m_writer.paren( spotFactor - 1.0_f )
							, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() )
							, 1.0_f );
						parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
						parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< Void >( cuT( "doComputeLight" )
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
					auto roughness = m_writer.declLocale( cuT( "roughness" )
						, 1.0_f - glossiness );
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto L = m_writer.declLocale( cuT( "L" )
						, normalize( direction ) );
					auto V = m_writer.declLocale( cuT( "V" )
						, normalize( normalize( worldEye - fragmentIn.m_worldVertex ) ) );
					auto H = m_writer.declLocale( cuT( "H" )
						, normalize( L + V ) );
					auto N = m_writer.declLocale( cuT( "N" )
						, normalize( fragmentIn.m_worldNormal ) );
					auto radiance = m_writer.declLocale( cuT( "radiance" )
						, light.m_colour() );

					auto NdotL = m_writer.declLocale( cuT( "NdotL" )
						, max( 0.0_f, dot( N, L ) ) );
					auto NdotV = m_writer.declLocale( cuT( "NdotV" )
						, max( 0.0_f, dot( N, V ) ) );
					auto NdotH = m_writer.declLocale( cuT( "NdotH" )
						, max( 0.0_f, dot( N, H ) ) );
					auto HdotV = m_writer.declLocale( cuT( "HdotV" )
						, max( 0.0_f, dot( H, V ) ) );
					auto LdotV = m_writer.declLocale( cuT( "LdotV" )
						, max( 0.0_f, dot( L, V ) ) );

					auto f0 = m_writer.declLocale( cuT( "f0" )
						, specular );
					auto specfresnel = m_writer.declLocale( cuT( "specfresnel" )
						, m_schlickFresnel( HdotV, f0 ) );
			
					auto NDF = m_writer.declLocale( cuT( "NDF" )
						, m_distributionGGX( NdotH, roughness ) );
					auto G = m_writer.declLocale( cuT( "G" )
						, m_geometrySmith( NdotV, NdotL, roughness ) );

					auto nominator = m_writer.declLocale( cuT( "nominator" )
						, specfresnel * NDF * G );
					auto denominator = m_writer.declLocale( cuT( "denominator" )
						, glsl::fma( 4.0_f
							, NdotV * NdotL
							, 0.001_f ) );
					auto specReflectance = m_writer.declLocale( cuT( "specReflectance" )
						, nominator / denominator );
					auto kS = m_writer.declLocale( cuT( "kS" )
						, specfresnel );
					auto kD = m_writer.declLocale( cuT( "kD" )
						, vec3( 1.0_f ) - kS );

					kD *= 1.0_f - length( specular );

					output.m_diffuse = shadowFactor * m_writer.paren( radiance * NdotL * kD / PI );
					output.m_specular = shadowFactor * m_writer.paren( specReflectance * radiance * NdotL );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "direction" ) )
				, InVec3( &m_writer, cuT( "diffuse" ) )
				, InVec3( &m_writer, cuT( "specular" ) )
				, InFloat( &m_writer, cuT( "glossiness" ) )
				, InFloat( &m_writer, cuT( "shadowFactor" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void SpecularBrdfLightingModel::doDeclareDistribution()
		{
			// Distribution Function
			m_distributionGGX = m_writer.implementFunction< Float >( cuT( "Distribution" )
				, [this]( Float const & product
				, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = m_writer.declLocale( cuT( "a" )
						, roughness * roughness );
					auto a2 = m_writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH2 = m_writer.declLocale( cuT( "NdotH2" )
						, product * product );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, a2 );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, glsl::fma( NdotH2
							, a2 - 1.0
							, 1.0_f ) );
					denominator = Float( PI ) * denominator * denominator;

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void SpecularBrdfLightingModel::doDeclareGeometry()
		{
			// Geometry Functions
			m_geometrySchlickGGX = m_writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [this]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = m_writer.declLocale( cuT( "r" )
						, roughness + 1.0_f );
					auto k = m_writer.declLocale( cuT( "k" )
						, m_writer.paren( r * r ) / 8.0_f );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, product );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, glsl::fma( product
							, 1.0_f - k
							, k ) );

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );

			m_geometrySmith = m_writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [this]( Float const & NdotV
					, Float const & NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = m_writer.declLocale( cuT( "ggx2" )
						, m_geometrySchlickGGX( NdotV, roughness ) );
					auto ggx1 = m_writer.declLocale( cuT( "ggx1" )
						, m_geometrySchlickGGX( NdotL, roughness ) );

					m_writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &m_writer, cuT( "NdotV" ) )
				, InFloat( &m_writer, cuT( "NdotL" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void SpecularBrdfLightingModel::doDeclareFresnelShlick()
		{
			// Fresnel Function
			m_schlickFresnel = m_writer.implementFunction< Vec3 >( cuT( "FresnelShlick" )
				, [this]( Float const & product
					, Vec3 const & f0 )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					m_writer.returnStmt( glsl::fma( vec3( 1.0_f ) - f0
						, vec3( pow( 1.0_f - product, 5.0_f ) )
						, f0 ) );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InVec3( &m_writer, cuT( "f0" ) ) );
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
			m_writer << m_computeLight( light
				, worldEye
				, direction
				, diffuse
				, specular
				, glossiness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}

		//***********************************************************************************************
	}
}
