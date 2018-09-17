#include "GlslPhongLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

#define C3D_DebugCascades 0
#define C3D_DebugSpotShadows 0

namespace castor3d
{
	namespace shader
	{
		const String PhongLightingModel::Name = cuT( "phong" );

		PhongLightingModel::PhongLightingModel( GlslWriter & writer )
			: LightingModel{ writer }
		{
		}

		void PhongLightingModel::computeCombined( Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" ), 0_i );
			auto end = m_writer.declLocale( cuT( "end" ), m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				m_writer << m_computeDirectional( getDirectionalLight( i )
					, worldEye
					, shininess
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
					, shininess
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
					, shininess
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
				m_writer << endi;
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
			m_writer << m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}

		void PhongLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_writer << m_computeOnePoint( PointLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}

		void PhongLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_writer << m_computeOneSpot( SpotLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}

		void PhongLightingModel::doDeclareModel()
		{
			doDeclareComputeLight();
		}

		void PhongLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< Void >( cuT( "computeDirectionalLight" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( cuT( "cascadeIndex" )
						, UInt( 0u ) );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
					{
						// Get cascade index for the current fragment's view position
						FOR( m_writer, UInt, i, 0u, cuT( "i < uint( max( min( c3d_maxCascadeCount, uint( light.m_directionCount.w ) ), 1u ) - 1u )" ), cuT( "++i" ) )
						{
							IF( m_writer, -fragmentIn.m_viewVertex.z() < light.m_splitDepth( i ) )
							{
								cascadeIndex = i + 1_ui;
							}
							FI;
						}
						ROF;

						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeDirectionalShadow( light.m_lightBase().m_shadowType()
								, light.m_transform( cascadeIndex )
								, fragmentIn.m_worldVertex
								, lightDirection
								, cascadeIndex
								, fragmentIn.m_worldNormal ) );
					}
					FI;

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
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

#if C3D_DebugCascades
					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
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

						output.m_specular.rgb() = output.m_diffuse.rgb();
					}
					FI;
#endif

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_worldVertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, light.m_lightBase().m_index() >= 0_i )
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
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, glsl::fma( light.m_attenuation().z()
							, distance * distance
							, glsl::fma( light.m_attenuation().y()
								, distance
								, light.m_attenuation().x() ) ) );
#if C3D_DebugSpotShadows
					parentOutput.m_diffuse += shadowFactor;
					parentOutput.m_specular += shadowFactor;
#else
					parentOutput.m_diffuse += output.m_diffuse / attenuation;
					parentOutput.m_specular += output.m_specular / attenuation;
#endif
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_worldVertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f - step( spotFactor, light.m_cutOff() ) );

					IF( m_writer, light.m_lightBase().m_shadowType() != Int( int( ShadowType::eNone ) ) )
					{
						IF( m_writer, light.m_lightBase().m_index() >= 0_i )
						{
							shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpotShadow( light.m_lightBase().m_shadowType()
									, light.m_transform()
									, fragmentIn.m_worldVertex
									, lightToVertex
									, fragmentIn.m_worldNormal
									, light.m_lightBase().m_index() ) );
						}
						FI;
					}
					FI;

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
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
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOneDirectionalLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< Void >( cuT( "computeDirectionalLight" )
				, [this, shadowType, volumetric]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );
					auto cascadeIndex = m_writer.declLocale( cuT( "cascadeIndex" )
						, shadowType != ShadowType::eNone
						, UInt( 0u ) );

					if ( shadowType != ShadowType::eNone )
					{
						// Get cascade index for the current fragment's view position
						FOR( m_writer, UInt, i, 0u, cuT( "i < uint( min( c3d_maxCascadeCount, uint( light.m_directionCount.w ) ) - 1u )" ), cuT( "++i" ) )
						{
							IF( m_writer, fragmentIn.m_viewVertex.z() < light.m_splitDepth( i ) )
							{
								cascadeIndex = i + 1_ui;
							}
							FI;
						}
						ROF;

						shadowFactor = max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeDirectionalShadow( light.m_transform( cascadeIndex )
								, fragmentIn.m_worldVertex
								, lightDirection
								, cascadeIndex
								, fragmentIn.m_worldNormal ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
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

#if C3D_DebugCascades
					if ( shadowType != ShadowType::eNone )
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

						output.m_specular.rgb() = output.m_diffuse.rgb();
					}
#endif

					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOnePointLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
				, [this, shadowType]( PointLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_worldVertex - light.m_position().xyz() );
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
						, shininess
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
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOneSpotLight( ShadowType shadowType
			, bool volumetric )
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
				, [this, shadowType]( SpotLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_worldVertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, light.m_direction() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f - step( spotFactor, light.m_cutOff() ) );

					if ( shadowType != ShadowType::eNone )
					{
						shadowFactor *= max( 1.0_f - m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeSpotShadow( light.m_transform()
								, fragmentIn.m_worldVertex
								, lightToVertex
								, fragmentIn.m_worldNormal ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
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
#if C3D_DebugSpotShadows
					parentOutput.m_diffuse += shadowFactor;
					parentOutput.m_specular += shadowFactor;
#else
					parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
					parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
#endif
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< Void >( cuT( "doComputeLight" )
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & lightDirection
					, Float const & shininess
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					auto diffuseFactor = m_writer.declLocale( cuT( "diffuseFactor" ), dot( fragmentIn.m_worldNormal, -lightDirection ) );
					auto stepFactor = m_writer.declLocale( cuT( "stepFactor" )
						, 1.0_f - step( diffuseFactor, 0.0_f ) );
					auto vertexToEye = m_writer.declLocale( cuT( "vertexToEye" )
						, normalize( worldEye - fragmentIn.m_worldVertex ) );
					auto lightReflect = m_writer.declLocale( cuT( "lightReflect" )
						, normalize( reflect( lightDirection, fragmentIn.m_worldNormal ) ) );
					auto specularFactor = m_writer.declLocale( cuT( "specularFactor" )
						, max( dot( vertexToEye, lightReflect ), 0.0 ) );
					output.m_diffuse = stepFactor * shadowFactor * light.m_colour() * light.m_intensity().x() * diffuseFactor;
					output.m_specular = stepFactor * shadowFactor * light.m_colour() * light.m_intensity().y() * pow( specularFactor, max( shininess, 0.1_f ) );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InFloat( &m_writer, cuT( "shadowFactor" ) )
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
			m_writer << m_computeLight( light
				, worldEye
				, lightDirection
				, shininess
				, p_shadowFactor
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}
	}
}
