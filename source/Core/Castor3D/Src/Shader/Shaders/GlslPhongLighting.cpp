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
						IF( m_writer, light.m_lightBase().m_index() >= 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computePointShadow( fragmentIn.m_vertex
									, light.m_position().xyz()
									, fragmentIn.m_normal
									, light.m_lightBase().m_farPlane()
									, light.m_lightBase().m_index() ) );
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
							IF( m_writer, light.m_lightBase().m_index() >= 0_i )
							{
								shadowFactor = 1.0_f - min( receivesShadows
									, m_shadowModel->computeSpotShadow( light.m_transform()
										, fragmentIn.m_vertex
										, lightToVertex
										, fragmentIn.m_normal
										, light.m_lightBase().m_index() ) );
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
							, glsl::fma( light.m_attenuation().z()
								, distance * distance
								, glsl::fma( light.m_attenuation().y()
									, distance
									, light.m_attenuation().x() ) ) );
						spotFactor = glsl::fma( m_writer.paren( spotFactor - 1.0_f )
							, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() )
							, 1.0_f );
						auto factor = m_writer.declLocale( cuT( "factor" )
							, vec3( spotFactor ) );
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
	}
}
