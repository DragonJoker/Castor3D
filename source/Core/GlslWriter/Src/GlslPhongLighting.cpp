#include "GlslPhongLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String PhongLightingModel::Name = cuT( "phong" );

	PhongLightingModel::PhongLightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: LightingModel{ p_shadows, p_writer }
	{
	}

	std::shared_ptr< LightingModel > PhongLightingModel::Create( ShadowType p_shadows, GlslWriter & p_writer )
	{
		return std::make_shared< PhongLightingModel >( p_shadows, p_writer );
	}

	void PhongLightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		auto c3d_iLightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_iLightsCount" ) );
		auto begin = m_writer.DeclLocale( cuT( "begin" ), 0_i );
		auto end = m_writer.DeclLocale( cuT( "end" ), m_writer.Cast< Int >( c3d_iLightsCount.x() ) );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputeDirectionalLight( GetDirectionalLight( i )
				, p_worldEye
				, p_shininess
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_iLightsCount.y() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputePointLight( GetPointLight( i )
				, p_worldEye
				, p_shininess
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_iLightsCount.z() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			ComputeSpotLight( GetSpotLight( i )
				, p_worldEye
				, p_shininess
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;
	}

	void PhongLightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computePoint( PointLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputeSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeSpot( SpotLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputeOneDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeOneDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
		, p_output );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputeOnePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeOnePoint( PointLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi();
	}

	void PhongLightingModel::ComputeOneSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Float const & p_shininess
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeOneSpot( SpotLight{ p_light }
			, p_worldEye
			, p_shininess
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn }
			, p_output );
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
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( p_light.m_transform()
							, p_fragmentIn.m_v3Vertex
							, lightDirection
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_shininess
					, shadowFactor
					, p_fragmentIn
					, output );
				p_output.m_v3Diffuse += output.m_v3Diffuse;
				p_output.m_v3Specular += output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputePointLight()
	{
		OutputComponents output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF( m_writer, p_light.m_index() >= 0_i )
					{
						shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
								, p_light.m_position().xyz()
								, p_fragmentIn.m_v3Normal
								, p_light.m_index() ) );
					}
					FI;
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_shininess
					, shadowFactor
					, p_fragmentIn
					, output );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * distance
					+ p_light.m_attenuation().z() * distance * distance );
				p_output.m_v3Diffuse += output.m_v3Diffuse / attenuation;
				p_output.m_v3Specular += output.m_v3Specular / attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" ), dot( lightDirection, p_light.m_direction() ) );

				IF( m_writer, spotFactor > p_light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						IF( m_writer, p_light.m_index() >= 0_i )
						{
							shadowFactor = 1.0_f - min( p_receivesShadows
								, m_shadowModel.ComputeSpotShadow( p_light.m_transform()
									, p_fragmentIn.m_v3Vertex
									, lightToVertex
									, p_fragmentIn.m_v3Normal
									, p_light.m_index() ) );
						}
						FI;
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, lightDirection
						, p_shininess
						, shadowFactor
						, p_fragmentIn
						, output );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * distance
						+ p_light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					p_output.m_v3Diffuse += spotFactor * output.m_v3Diffuse / attenuation;
					p_output.m_v3Specular += spotFactor * output.m_v3Specular / attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}
	
	void PhongLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( p_light.m_transform()
							, p_fragmentIn.m_v3Vertex
							, lightDirection
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_shininess
					, shadowFactor
					, p_fragmentIn
					, output );
				p_output.m_v3Diffuse += output.m_v3Diffuse;
				p_output.m_v3Specular += output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
							, p_light.m_position().xyz()
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_shininess
					, shadowFactor
					, p_fragmentIn
					, output );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * distance
					+ p_light.m_attenuation().z() * distance * distance );
				p_output.m_v3Diffuse += output.m_v3Diffuse / attenuation;
				p_output.m_v3Specular += output.m_v3Specular / attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents output
				{
					m_writer.DeclLocale( cuT( "diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" ), dot( lightDirection, p_light.m_direction() ) );

				IF( m_writer, spotFactor > p_light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputeSpotShadow( p_light.m_transform()
								, p_fragmentIn.m_v3Vertex
								, lightToVertex
								, p_fragmentIn.m_v3Normal ) );
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, lightDirection
						, p_shininess
						, shadowFactor
						, p_fragmentIn
						, output );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * distance
						+ p_light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					p_output.m_v3Diffuse += spotFactor * output.m_v3Diffuse / attenuation;
					p_output.m_v3Specular += spotFactor * output.m_v3Specular / attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents output{ m_writer };
		m_computeLight = m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" )
			, [this]( Light const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_lightDirection
				, Float const & p_shininess
				, Float const & p_shadowFactor
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				auto diffuseFactor = m_writer.DeclLocale( cuT( "diffuseFactor" ), dot( p_fragmentIn.m_v3Normal, -p_lightDirection ) );

				IF( m_writer, diffuseFactor > 0.0_f )
				{
					auto vertexToEye = m_writer.DeclLocale( cuT( "vertexToEye" ), normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) );
					auto lightReflect = m_writer.DeclLocale( cuT( "lightReflect" ), normalize( reflect( p_lightDirection, p_fragmentIn.m_v3Normal ) ) );
					auto specularFactor = m_writer.DeclLocale( cuT( "specularFactor" ), max( dot( vertexToEye, lightReflect ), 0.0 ) );
					p_output.m_v3Diffuse = p_shadowFactor * p_light.m_colour() * p_light.m_intensity().x() * diffuseFactor;
					p_output.m_v3Specular = p_shadowFactor * p_light.m_colour() * p_light.m_intensity().y() * pow( specularFactor, max( p_shininess, 0.1_f ) );
				}
				FI;
			}
			, InLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer }
			, output );
	}

	void PhongLightingModel::DoComputeLight( Light const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_lightDirection
		, Float const & p_shininess
		, Float const & p_shadowFactor
		, FragmentInput const & p_fragmentIn
		, OutputComponents & p_output )
	{
		m_writer << m_computeLight( p_light
			, p_worldEye
			, p_lightDirection
			, p_shininess
			, p_shadowFactor
			, FragmentInput{ p_fragmentIn }
			, p_output );
		m_writer << Endi{};
	}
}
