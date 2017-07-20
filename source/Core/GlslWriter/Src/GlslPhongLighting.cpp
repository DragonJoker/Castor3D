#include "GlslPhongLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String PhongLightingModel::Name = cuT( "phong" );

	PhongLightingModel::PhongLightingModel( ShadowType shadows, GlslWriter & writer )
		: LightingModel{ shadows, writer }
	{
	}

	std::shared_ptr< LightingModel > PhongLightingModel::Create( ShadowType shadows, GlslWriter & writer )
	{
		return std::make_shared< PhongLightingModel >( shadows, writer );
	}

	void PhongLightingModel::ComputeCombinedLighting( Vec3 const & worldEye
		, Float const & shininess
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn
		, OutputComponents & parentOutput )
	{
		auto c3d_lightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
		auto begin = m_writer.DeclLocale( cuT( "begin" ), 0_i );
		auto end = m_writer.DeclLocale( cuT( "end" ), m_writer.Cast< Int >( c3d_lightsCount.x() ) );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputeDirectionalLight( GetDirectionalLight( i )
				, worldEye
				, shininess
				, receivesShadows
				, fragmentIn
				, parentOutput );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_lightsCount.y() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputePointLight( GetPointLight( i )
				, worldEye
				, shininess
				, receivesShadows
				, fragmentIn
				, parentOutput );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_lightsCount.z() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputeSpotLight( GetSpotLight( i )
				, worldEye
				, shininess
				, receivesShadows
				, fragmentIn
				, parentOutput );
		}
		ROF;
	}

	void PhongLightingModel::ComputeDirectionalLight( DirectionalLight const & light
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

	void PhongLightingModel::ComputePointLight( PointLight const & light
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

	void PhongLightingModel::ComputeSpotLight( SpotLight const & light
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

	void PhongLightingModel::ComputeOneDirectionalLight( DirectionalLight const & light
		, Vec3 const & worldEye
		, Float const & shininess
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn
		, OutputComponents & parentOutput )
	{
		m_writer << m_computeOneDirectional( DirectionalLight{ light }
			, worldEye
			, shininess
			, receivesShadows
			, FragmentInput{ fragmentIn }
		, parentOutput );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputeOnePointLight( PointLight const & light
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

	void PhongLightingModel::ComputeOneSpotLight( SpotLight const & light
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

	void PhongLightingModel::DoDeclareModel()
	{
		DoDeclare_ComputeLight();
	}

	void PhongLightingModel::Declare_ComputeDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( light.m_transform()
							, fragmentIn.m_v3Vertex
							, lightDirection
							, fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, shininess
					, shadowFactor
					, fragmentIn
					, output );
				parentOutput.m_v3Diffuse += output.m_v3Diffuse;
				parentOutput.m_v3Specular += output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InFloat( &m_writer, cuT( "shininess" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputePointLight()
	{
		OutputComponents output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), fragmentIn.m_v3Vertex - light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF( m_writer, light.m_index() >= 0_i )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel.ComputePointShadow( fragmentIn.m_v3Vertex
								, light.m_position().xyz()
								, fragmentIn.m_v3Normal
								, light.m_index() ) );
					}
					FI;
				}

				DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, shininess
					, shadowFactor
					, fragmentIn
					, output );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, light.m_attenuation().x()
					+ light.m_attenuation().y() * distance
					+ light.m_attenuation().z() * distance * distance );
				parentOutput.m_v3Diffuse += output.m_v3Diffuse / attenuation;
				parentOutput.m_v3Specular += output.m_v3Specular / attenuation;
			}
			, PointLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InFloat( &m_writer, cuT( "shininess" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), fragmentIn.m_v3Vertex - light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" ), dot( lightDirection, light.m_direction() ) );

				IF( m_writer, spotFactor > light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						IF( m_writer, light.m_index() >= 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel.ComputeSpotShadow( light.m_transform()
									, fragmentIn.m_v3Vertex
									, lightToVertex
									, fragmentIn.m_v3Normal
									, light.m_index() ) );
						}
						FI;
					}

					DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - light.m_cutOff() ) );
					parentOutput.m_v3Diffuse += spotFactor * output.m_v3Diffuse / attenuation;
					parentOutput.m_v3Specular += spotFactor * output.m_v3Specular / attenuation;
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
	
	void PhongLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( light.m_transform()
							, fragmentIn.m_v3Vertex
							, lightDirection
							, fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, shininess
					, shadowFactor
					, fragmentIn
					, output );
				parentOutput.m_v3Diffuse += output.m_v3Diffuse;
				parentOutput.m_v3Specular += output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InFloat( &m_writer, cuT( "shininess" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), fragmentIn.m_v3Vertex - light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( receivesShadows
						, m_shadowModel.ComputePointShadow( fragmentIn.m_v3Vertex
							, light.m_position().xyz()
							, fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, shininess
					, shadowFactor
					, fragmentIn
					, output );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, light.m_attenuation().x()
					+ light.m_attenuation().y() * distance
					+ light.m_attenuation().z() * distance * distance );
				parentOutput.m_v3Diffuse += output.m_v3Diffuse / attenuation;
				parentOutput.m_v3Specular += output.m_v3Specular / attenuation;
			}
			, PointLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InFloat( &m_writer, cuT( "shininess" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & light
				, Vec3 const & worldEye
				, Float const & shininess
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), fragmentIn.m_v3Vertex - light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" ), dot( lightDirection, light.m_direction() ) );

				IF( m_writer, spotFactor > light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel.ComputeSpotShadow( light.m_transform()
								, fragmentIn.m_v3Vertex
								, lightToVertex
								, fragmentIn.m_v3Normal ) );
					}

					DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, shininess
						, shadowFactor
						, fragmentIn
						, output );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - light.m_cutOff() ) );
					parentOutput.m_v3Diffuse += spotFactor * output.m_v3Diffuse / attenuation;
					parentOutput.m_v3Specular += spotFactor * output.m_v3Specular / attenuation;
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

	void PhongLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents output{ m_writer };
		m_computeLight = m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" )
			, [this]( Light const & light
				, Vec3 const & worldEye
				, Vec3 const & lightDirection
				, Float const & shininess
				, Float const & p_shadowFactor
				, FragmentInput const & fragmentIn
				, OutputComponents & parentOutput )
			{
				auto diffuseFactor = m_writer.DeclLocale( cuT( "diffuseFactor" ), dot( fragmentIn.m_v3Normal, -lightDirection ) );

				IF( m_writer, diffuseFactor > 0.0_f )
				{
					auto vertexToEye = m_writer.DeclLocale( cuT( "vertexToEye" ), normalize( worldEye - fragmentIn.m_v3Vertex ) );
					auto lightReflect = m_writer.DeclLocale( cuT( "lightReflect" ), normalize( reflect( lightDirection, fragmentIn.m_v3Normal ) ) );
					auto specularFactor = m_writer.DeclLocale( cuT( "specularFactor" ), max( dot( vertexToEye, lightReflect ), 0.0 ) );
					parentOutput.m_v3Diffuse = p_shadowFactor * light.m_colour() * light.m_intensity().x() * diffuseFactor;
					parentOutput.m_v3Specular = p_shadowFactor * light.m_colour() * light.m_intensity().y() * pow( specularFactor, max( shininess, 0.1_f ) );
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

	void PhongLightingModel::DoComputeLight( Light const & light
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
		m_writer << Endi{};
	}
}
