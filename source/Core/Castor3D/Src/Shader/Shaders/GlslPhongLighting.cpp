#include "GlslPhongLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		const String PhongLightingModel::Name = cuT( "phong" );

		PhongLightingModel::PhongLightingModel( ShadowType shadows, GlslWriter & writer )
			: LightingModel{ shadows, writer }
		{
		}

		void PhongLightingModel::computeCombinedLighting( Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" ), 0_i );
			auto end = m_writer.declLocale( cuT( "end" ), m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				computeDirectionalLight( getDirectionalLight( i )
					, worldEye
					, shininess
					, receivesShadows
					, fragmentIn
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				computePointLight( getPointLight( i )
					, worldEye
					, shininess
					, receivesShadows
					, fragmentIn
					, parentOutput );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				computeSpotLight( getSpotLight( i )
					, worldEye
					, shininess
					, receivesShadows
					, fragmentIn
					, parentOutput );
			}
			ROF;
		}

		void PhongLightingModel::computeDirectionalLight( DirectionalLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		void PhongLightingModel::computePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_writer << m_computePoint( PointLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		void PhongLightingModel::computeSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeSpot( SpotLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		void PhongLightingModel::computeOnePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeOnePoint( PointLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		void PhongLightingModel::computeOneSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )
		{
			m_writer << m_computeOneSpot( SpotLight{ light }
				, worldEye
				, shininess
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		Vec3 PhongLightingModel::computeDirectionalLightBackLit( DirectionalLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, FragmentInput const & fragmentIn )
		{
			return m_computeDirectionalBackLit( DirectionalLight{ light }
				, worldEye
				, shininess
				, FragmentInput{ fragmentIn } );
		}

		Vec3 PhongLightingModel::computePointLightBackLit( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, FragmentInput const & fragmentIn )
		{
			return m_computePointBackLit( PointLight{ light }
				, worldEye
				, shininess
				, FragmentInput{ fragmentIn } );
		}

		Vec3 PhongLightingModel::computeSpotLightBackLit( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, FragmentInput const & fragmentIn )
		{
			return m_computeSpotBackLit( SpotLight{ light }
				, worldEye
				, shininess
				, FragmentInput{ fragmentIn } );
		}

		void PhongLightingModel::doDeclareModel()
		{
			doDeclareComputeLight();
			doDeclareComputeLightBackLit();
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
						, normalize( light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->computeDirectionalShadow( light.m_transform()
								, fragmentIn.m_vertex
								, lightDirection
								, fragmentIn.m_normal ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
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
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						IF( m_writer, light.m_index() >= 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computePointShadow( fragmentIn.m_vertex
									, light.m_position().xyz()
									, fragmentIn.m_normal
									, light.m_farPlane()
									, light.m_index() ) );
						}
						FI;
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
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
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							IF( m_writer, light.m_index() >= 0_i )
							{
								shadowFactor = 1.0_f - min( receivesShadows
									, m_shadowModel->computeSpotShadow( light.m_transform()
										, fragmentIn.m_vertex
										, lightToVertex
										, fragmentIn.m_normal
										, light.m_index() ) );
							}
							FI;
						}

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, shininess
							, shadowFactor
							, fragmentIn
							, output );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );
						parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
						parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void PhongLightingModel::doDeclareComputeOnePointLight()
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
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
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->computePointShadow( fragmentIn.m_vertex
								, light.m_position().xyz()
								, fragmentIn.m_normal
								, light.m_farPlane() ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
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

		void PhongLightingModel::doDeclareComputeOneSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
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
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computeSpotShadow( light.m_transform()
									, fragmentIn.m_vertex
									, lightToVertex
									, fragmentIn.m_normal ) );
						}

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, shininess
							, shadowFactor
							, fragmentIn
							, output );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );
						parentOutput.m_diffuse += spotFactor * output.m_diffuse / attenuation;
						parentOutput.m_specular += spotFactor * output.m_specular / attenuation;
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}
		
		void PhongLightingModel::doDeclareComputeDirectionalLightBackLit()
		{
			m_computeDirectionalBackLit = m_writer.implementFunction< Vec3 >( cuT( "computeDirectionalLightBackLit" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, FragmentInput const & fragmentIn )
				{
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( light.m_direction().xyz() ) );
					m_writer.returnStmt( doComputeLightBackLit( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, fragmentIn ) );
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, FragmentInput{ m_writer } );
		}

		void PhongLightingModel::doDeclareComputePointLightBackLit()
		{
			m_computePointBackLit = m_writer.implementFunction< Vec3 >( cuT( "computePointLightBackLit" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, FragmentInput const & fragmentIn )
				{
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto backLit = m_writer.declLocale( cuT( "backLit" )
						, doComputeLightBackLit( light.m_lightBase()
							, worldEye
							, lightDirection
							, shininess
							, fragmentIn ) );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					m_writer.returnStmt( backLit / attenuation );
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, FragmentInput{ m_writer } );
		}

		void PhongLightingModel::doDeclareComputeSpotLightBackLit()
		{
			m_computeSpotBackLit = m_writer.implementFunction< Vec3 >( cuT( "computeSpotLightBackLit" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Float const & shininess
					, FragmentInput const & fragmentIn )
				{
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, light.m_direction() ) );
					auto backLit = m_writer.declLocale( cuT( "backLit" )
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						backLit = doComputeLightBackLit( light.m_lightBase()
							, worldEye
							, lightDirection
							, shininess
							, fragmentIn );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );
						backLit = spotFactor * backLit / attenuation;
					}
					FI;

					m_writer.returnStmt( backLit );
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, FragmentInput{ m_writer } );
		}

		void PhongLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< Void >( cuT( "doComputeLight" )
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & lightDirection
					, Float const & shininess
					, Float const & p_shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					auto diffuseFactor = m_writer.declLocale( cuT( "diffuseFactor" ), dot( fragmentIn.m_normal, -lightDirection ) );

					IF( m_writer, diffuseFactor > 0.0_f )
					{
						auto vertexToEye = m_writer.declLocale( cuT( "vertexToEye" )
							, normalize( worldEye - fragmentIn.m_vertex ) );
						auto lightReflect = m_writer.declLocale( cuT( "lightReflect" )
							, normalize( reflect( lightDirection, fragmentIn.m_normal ) ) );
						auto specularFactor = m_writer.declLocale( cuT( "specularFactor" )
							, max( dot( vertexToEye, lightReflect ), 0.0 ) );
						output.m_diffuse = p_shadowFactor * light.m_colour() * light.m_intensity().x() * diffuseFactor;
						output.m_specular = p_shadowFactor * light.m_colour() * light.m_intensity().y() * pow( specularFactor, max( shininess, 0.1_f ) );
					}
					FI;
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
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

		void PhongLightingModel::doDeclareComputeLightBackLit()
		{
			m_computeLightBackLit = m_writer.implementFunction< Vec3 >( cuT( "doComputeLightBackLit" )
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & lightDirection
					, Float const & shininess
					, FragmentInput const & fragmentIn )
				{
					auto diffuseFactor = m_writer.declLocale( cuT( "diffuseFactor" )
						, dot( fragmentIn.m_normal, -lightDirection ) );
					auto backLit = m_writer.declLocale( cuT( "backLit" )
						, vec3( 0.0_f ) );

					IF( m_writer, diffuseFactor > 0.0_f )
					{
						auto vertexToEye = m_writer.declLocale( cuT( "vertexToEye" )
							, normalize( worldEye - fragmentIn.m_vertex ) );
						auto lightReflect = m_writer.declLocale( cuT( "lightReflect" )
							, normalize( reflect( lightDirection, fragmentIn.m_normal ) ) );
						auto specularFactor = m_writer.declLocale( cuT( "specularFactor" )
							, max( dot( vertexToEye, lightReflect ), 0.0 ) );
						backLit = light.m_colour() * light.m_intensity().x() * diffuseFactor;
					}
					FI;

					m_writer.returnStmt( backLit );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "lightDirection" ) )
				, InFloat( &m_writer, cuT( "shininess" ) )
				, FragmentInput{ m_writer } );
		}

		Vec3 PhongLightingModel::doComputeLightBackLit( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & lightDirection
			, Float const & shininess
			, FragmentInput const & fragmentIn )
		{
			return m_computeLightBackLit( light
				, worldEye
				, lightDirection
				, shininess
				, FragmentInput{ fragmentIn } );
		}
	}
}
