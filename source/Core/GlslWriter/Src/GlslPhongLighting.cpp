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
		auto l_begin = m_writer.DeclLocale( cuT( "l_begin" ), 0_i );
		auto l_end = m_writer.DeclLocale( cuT( "l_end" ), m_writer.Cast< Int >( c3d_iLightsCount.x() ) );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			ComputeDirectionalLight( GetDirectionalLight( i )
				, p_worldEye
				, p_shininess
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.y() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			ComputePointLight( GetPointLight( i )
				, p_worldEye
				, p_shininess
				, p_receivesShadows
				, p_fragmentIn
				, p_output );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.z() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
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
		OutputComponents l_output{ m_writer };
		m_computeDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					l_shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( p_light.m_transform()
							, p_fragmentIn.m_v3Vertex
							, l_lightDirection
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				p_output.m_v3Diffuse += l_output.m_v3Diffuse;
				p_output.m_v3Specular += l_output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF( m_writer, p_light.m_index() >= 0_i )
					{
						l_shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
								, p_light.m_position().xyz()
								, p_fragmentIn.m_v3Normal
								, p_light.m_index() ) );
					}
					FI;
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * l_distance
					+ p_light.m_attenuation().z() * l_distance * l_distance );
				p_output.m_v3Diffuse += l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_output.m_v3Specular / l_attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_spotFactor = m_writer.DeclLocale( cuT( "l_spotFactor" ), dot( l_lightDirection, p_light.m_direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_cutOff() )
				{
					auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						IF( m_writer, p_light.m_index() >= 0_i )
						{
							l_shadowFactor = 1.0_f - min( p_receivesShadows
								, m_shadowModel.ComputeSpotShadow( p_light.m_transform()
									, p_fragmentIn.m_v3Vertex
									, l_lightToVertex
									, p_fragmentIn.m_v3Normal
									, p_light.m_index() ) );
						}
						FI;
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_shininess
						, l_shadowFactor
						, p_fragmentIn
						, l_output );
					auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * l_distance
						+ p_light.m_attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					p_output.m_v3Diffuse += l_spotFactor * l_output.m_v3Diffuse / l_attenuation;
					p_output.m_v3Specular += l_spotFactor * l_output.m_v3Specular / l_attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}
	
	void PhongLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Void >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					l_shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputeDirectionalShadow( p_light.m_transform()
							, p_fragmentIn.m_v3Vertex
							, l_lightDirection
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				p_output.m_v3Diffuse += l_output.m_v3Diffuse;
				p_output.m_v3Specular += l_output.m_v3Specular;
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Void >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					l_shadowFactor = 1.0_f - min( p_receivesShadows
						, m_shadowModel.ComputePointShadow( p_fragmentIn.m_v3Vertex
							, p_light.m_position().xyz()
							, p_fragmentIn.m_v3Normal ) );
				}

				DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_shininess
					, l_shadowFactor
					, p_fragmentIn
					, l_output );
				auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * l_distance
					+ p_light.m_attenuation().z() * l_distance * l_distance );
				p_output.m_v3Diffuse += l_output.m_v3Diffuse / l_attenuation;
				p_output.m_v3Specular += l_output.m_v3Specular / l_attenuation;
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Void >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Float const & p_shininess
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				OutputComponents l_output
				{
					m_writer.DeclLocale( cuT( "l_diffuse" ), vec3( 0.0_f, 0.0f, 0.0f ) ),
					m_writer.DeclLocale( cuT( "l_specular" ), vec3( 0.0_f, 0.0f, 0.0f ) )
				};
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_spotFactor = m_writer.DeclLocale( cuT( "l_spotFactor" ), dot( l_lightDirection, p_light.m_direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_cutOff() )
				{
					auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						l_shadowFactor = 1.0_f - min( p_receivesShadows
							, m_shadowModel.ComputeSpotShadow( p_light.m_transform()
								, p_fragmentIn.m_v3Vertex
								, l_lightToVertex
								, p_fragmentIn.m_v3Normal ) );
					}

					DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_shininess
						, l_shadowFactor
						, p_fragmentIn
						, l_output );
					auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * l_distance
						+ p_light.m_attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					p_output.m_v3Diffuse += l_spotFactor * l_output.m_v3Diffuse / l_attenuation;
					p_output.m_v3Specular += l_spotFactor * l_output.m_v3Specular / l_attenuation;
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer }
			, l_output );
	}

	void PhongLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeLight = m_writer.ImplementFunction< Void >( cuT( "DoComputeLight" )
			, [this]( Light const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_lightDirection
				, Float const & p_shininess
				, Float const & p_shadowFactor
				, FragmentInput const & p_fragmentIn
				, OutputComponents & p_output )
			{
				auto l_diffuseFactor = m_writer.DeclLocale( cuT( "l_diffuseFactor" ), dot( p_fragmentIn.m_v3Normal, -p_lightDirection ) );

				IF( m_writer, l_diffuseFactor > 0.0_f )
				{
					auto l_vertexToEye = m_writer.DeclLocale( cuT( "l_vertexToEye" ), normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) );
					auto l_lightReflect = m_writer.DeclLocale( cuT( "l_lightReflect" ), normalize( reflect( p_lightDirection, p_fragmentIn.m_v3Normal ) ) );
					auto l_specularFactor = m_writer.DeclLocale( cuT( "l_specularFactor" ), max( dot( l_vertexToEye, l_lightReflect ), 0.0 ) );
					p_output.m_v3Diffuse = p_shadowFactor * p_light.m_colour() * p_light.m_intensity().x() * l_diffuseFactor;
					p_output.m_v3Specular = p_shadowFactor * p_light.m_colour() * p_light.m_intensity().y() * pow( l_specularFactor, max( p_shininess, 0.1_f ) );
				}
				FI;
			}
			, InLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_lightDirection" ) )
			, InFloat( &m_writer, cuT( "p_shininess" ) )
			, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer }
			, l_output );
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
