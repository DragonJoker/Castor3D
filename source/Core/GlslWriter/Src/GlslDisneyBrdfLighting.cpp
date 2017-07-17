#include "GlslDisneyBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String DisneyBrdfLightingModel::Name = cuT( "pbr_mr" );

	DisneyBrdfLightingModel::DisneyBrdfLightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: LightingModel{ p_shadows, p_writer }
	{
	}

	std::shared_ptr< LightingModel > DisneyBrdfLightingModel::Create( ShadowType p_shadows, GlslWriter & p_writer )
	{
		return std::make_shared< DisneyBrdfLightingModel >( p_shadows, p_writer );
	}

	Vec3 DisneyBrdfLightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		auto c3d_iLightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_iLightsCount" ) );
		auto l_begin = m_writer.DeclLocale( cuT( "l_begin" ), 0_i );
		auto l_end = m_writer.DeclLocale( cuT( "l_end" ), m_writer.Cast< Int >( c3d_iLightsCount.x() ) );
		auto l_result = m_writer.DeclLocale( cuT( "l_result" ), vec3( 0.0_f ) );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			l_result += ComputeDirectionalLight( GetDirectionalLight( i )
				, p_worldEye
				, p_albedo
				, p_metallic
				, p_roughness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.y() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			l_result += ComputePointLight( GetPointLight( i )
				, p_worldEye
				, p_albedo
				, p_metallic
				, p_roughness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		l_begin = l_end;
		l_end += m_writer.Cast< Int >( c3d_iLightsCount.z() );

		FOR( m_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
		{
			l_result += ComputeSpotLight( GetSpotLight( i )
				, p_worldEye
				, p_albedo
				, p_metallic
				, p_roughness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		return l_result;
	}

	Vec3 DisneyBrdfLightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 DisneyBrdfLightingModel::ComputePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computePoint( PointLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 DisneyBrdfLightingModel::ComputeSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeSpot( SpotLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 DisneyBrdfLightingModel::ComputeOneDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOneDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 DisneyBrdfLightingModel::ComputeOnePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOnePoint( PointLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 DisneyBrdfLightingModel::ComputeOneSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_albedo
		, Float const & p_metallic
		, Float const & p_roughness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOneSpot( SpotLight{ p_light }
			, p_worldEye
			, p_albedo
			, p_metallic
			, p_roughness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	void DisneyBrdfLightingModel::DoDeclareModel()
	{
		DoDeclare_Distribution();
		DoDeclare_Geometry();
		DoDeclare_FresnelShlick();
		DoDeclare_ComputeLight();
	}

	void DisneyBrdfLightingModel::Declare_ComputeDirectionalLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					Shadow l_shadows{ m_writer };

					IF ( m_writer, p_receivesShadows != 0_i )
					{
						l_shadowFactor = l_shadows.ComputeDirectionalShadow( p_light.m_transform(), p_fragmentIn.m_v3Vertex, l_lightDirection, p_fragmentIn.m_v3Normal );
					}
					FI;
				}

				m_writer.Return( DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, l_lightDirection
					, p_albedo
					, p_metallic
					, p_roughness
					, l_shadowFactor
					, p_fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::Declare_ComputePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					Shadow l_shadows{ m_writer };

					IF ( m_writer, p_receivesShadows != 0_i )
					{
						l_shadowFactor = l_shadows.ComputePointShadow( p_fragmentIn.m_v3Vertex, l_lightDirection, p_fragmentIn.m_v3Normal, 0_i );
					}
					FI;
				}

				auto l_result = m_writer.DeclLocale( cuT( "l_result" )
					, DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_albedo
						, p_metallic
						, p_roughness
						, l_shadowFactor
						, p_fragmentIn ) );
				auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" ), p_light.m_attenuation().x() + p_light.m_attenuation().y() * l_distance + p_light.m_attenuation().z() * l_distance * l_distance );
				m_writer.Return( l_result / l_attenuation );
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto l_distance = m_writer.DeclLocale( cuT( "l_distance" ), length( l_lightToVertex ) );
				auto l_lightDirection = m_writer.DeclLocale( cuT( "l_lightDirection" ), normalize( l_lightToVertex ) );
				auto l_spotFactor = m_writer.DeclLocale( cuT( "l_spotFactor" ), dot( l_lightDirection, p_light.m_direction() ) );

				IF( m_writer, l_spotFactor > p_light.m_cutOff() )
				{
					auto l_shadowFactor = m_writer.DeclLocale( cuT( "l_shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						Shadow l_shadows{ m_writer };

						IF ( m_writer, p_receivesShadows != 0_i )
						{
							l_shadowFactor = l_shadows.ComputeSpotShadow( p_light.m_transform(), p_fragmentIn.m_v3Vertex, l_lightToVertex, p_fragmentIn.m_v3Normal, Int( 0 ) );
						}
						FI;
					}

					auto l_result = m_writer.DeclLocale( cuT( "l_result" )
						, DoComputeLight( p_light.m_lightBase()
							, p_worldEye
							, l_lightDirection
							, p_albedo
							, p_metallic
							, p_roughness
							, l_shadowFactor
							, p_fragmentIn ) );
					auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * l_distance
						+ p_light.m_attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );

					m_writer.Return( l_spotFactor * l_result / l_attenuation );
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
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

				m_writer.Return( DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_albedo
						, p_metallic
						, p_roughness
						, l_shadowFactor
						, p_fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_light.m_position().xyz() - p_fragmentIn.m_v3Vertex );
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

				auto l_result = m_writer.DeclLocale( cuT( "l_result" )
					, DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, l_lightDirection
						, p_albedo
						, p_metallic
						, p_roughness
						, l_shadowFactor
						, p_fragmentIn ) );
				auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * l_distance
					+ p_light.m_attenuation().z() * l_distance * l_distance );
				m_writer.Return( l_result / l_attenuation );
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents l_output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_albedo
				, Float const & p_metallic
				, Float const & p_roughness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials l_materials{ m_writer };
				auto l_lightToVertex = m_writer.DeclLocale( cuT( "l_lightToVertex" ), p_light.m_position().xyz() - p_fragmentIn.m_v3Vertex );
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

					auto l_result = m_writer.DeclLocale( cuT( "l_result" )
						, DoComputeLight( p_light.m_lightBase()
							, p_worldEye
							, l_lightDirection
							, p_albedo
							, p_metallic
							, p_roughness
							, l_shadowFactor
							, p_fragmentIn ) );
					auto l_attenuation = m_writer.DeclLocale( cuT( "l_attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * l_distance
						+ p_light.m_attenuation().z() * l_distance * l_distance );
					l_spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - l_spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					m_writer.Return( l_spotFactor * l_result / l_attenuation );
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metallic" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}
	
	void DisneyBrdfLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents l_output{ m_writer };
		auto constexpr PI = 3.1415926535897932384626433832795028841968;
		auto l_sqr = m_writer.ImplementFunction< Float >( cuT( "sqr" )
			, [&]( Float const & p_value )
			{
				m_writer.Return( p_value * p_value );
			}
			, InFloat{ &m_writer, cuT( "p_value" ) } );

		auto l_gtr1 = m_writer.ImplementFunction< Float >( cuT( "GTR1" )
			, [&]( Float const & p_NdotH
				, Float const & p_a )
			{
				IF( m_writer, p_a >= 1.0_f )
				{
					m_writer.Return( 1.0_f / PI );
				}
				FI;

				auto a2 = m_writer.DeclLocale( cuT( "a2" )
					, p_a * p_a );
				auto t = m_writer.DeclLocale( cuT( "t" )
					, 1.0_f + m_writer.Paren( a2 - 1.0f ) * p_NdotH * p_NdotH );
				m_writer.Return( m_writer.Paren( a2 - 1.0f ) / m_writer.Paren( Float( PI ) * log( a2 ) * t) );
			}
			, InFloat{ &m_writer, cuT( "p_NdotH" ) }
			, InFloat{ &m_writer, cuT( "p_a" ) } );

		auto l_gtr2 = m_writer.ImplementFunction< Float >( cuT( "GTR2" )
			, [&]( Float const & p_NdotH
				, Float const & p_HdotX
				, Float const & p_HdotY
				, Float const & p_ax
				, Float const & p_ay )
			{
				m_writer.Return( 1.0f / m_writer.Paren( Float( PI ) * p_ax * p_ay * l_sqr( l_sqr( p_HdotX / p_ax ) + l_sqr( p_HdotY / p_ay ) + l_sqr( p_NdotH ) ) ) );
			}
			, InFloat{ &m_writer, cuT( "p_NdotH" ) }
			, InFloat{ &m_writer, cuT( "p_HdotX" ) }
			, InFloat{ &m_writer, cuT( "p_HdotY" ) }
			, InFloat{ &m_writer, cuT( "p_ax" ) }
			, InFloat{ &m_writer, cuT( "p_ay" ) } );

		auto l_smithGGX = m_writer.ImplementFunction< Float >( cuT( "SmithG_GGX" )
			, [&]( Float const & p_NdotV
				, Float const & p_alphaG )
			{
				auto a = m_writer.DeclLocale( cuT( "a" )
					, p_alphaG * p_alphaG );
				auto b = m_writer.DeclLocale( cuT( "b" )
					, p_NdotV * p_NdotV );
				m_writer.Return( 1.0_f / m_writer.Paren( p_NdotV + sqrt( a + b - a*b ) ) );
			}
			, InFloat{ &m_writer, cuT( "p_NdotV" ) }
			, InFloat{ &m_writer, cuT( "p_alphaG" ) } );

		m_writer.ImplementFunction< Vec3 >( cuT( "DoComputeLight" )
			, [&]( Light const & p_light
				, Vec3 const & p_worldEye
				, Vec2 const & p_tangentToWorld
				, Vec3 const & p_direction
				, Vec3 const & p_albedo
				, Float const & p_metalness
				, Float const & p_roughness
				, Float const & p_shadowFactor
				, FragmentInput const & p_fragmentIn )
			{
				// From Disney's BRDF explorer: https://github.com/wdas/brdf
				//float3 DisneyBRDF( float3 baseColor, out float3 specularColor, float3 normal, float roughness, float3 lightDir, float3 viewDir, float3 X, float3 Y, out float3 diffuse )
				auto c3d_specular = m_writer.GetBuiltin< Float >( cuT( "c3d_specular" ) );
				auto c3d_specularTint = m_writer.GetBuiltin< Float >( cuT( "c3d_specularTint" ) );
				auto c3d_sheen = m_writer.GetBuiltin< Float >( cuT( "c3d_sheen" ) );
				auto c3d_sheenTint = m_writer.GetBuiltin< Float >( cuT( "c3d_sheenTint" ) );
				auto c3d_anisotropic = m_writer.GetBuiltin< Float >( cuT( "c3d_anisotropic" ) );
				auto c3d_clearCoat = m_writer.GetBuiltin< Float >( cuT( "c3d_clearCoat" ) );
				auto c3d_subsurface = m_writer.GetBuiltin< Float >( cuT( "c3d_subsurface" ) );
				auto l_L = m_writer.DeclLocale( cuT( "l_L" )
					, normalize( p_direction ) );
				auto l_V = m_writer.DeclLocale( cuT( "l_V" )
					, normalize( normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) ) );
				auto l_H = m_writer.DeclLocale( cuT( "l_H" )
					, normalize( l_L + l_V ) );
				auto l_N = m_writer.DeclLocale( cuT( "l_N" )
					, normalize( p_fragmentIn.m_v3Normal ) );
				auto l_radiance = m_writer.DeclLocale( cuT( "l_radiance" )
					, p_light.m_colour() );

				auto l_NdotL = m_writer.DeclLocale( cuT( "l_NdotL" )
					, max( 0.0_f, dot( l_N, l_L ) ) );
				auto l_NdotV = m_writer.DeclLocale( cuT( "l_NdotV" )
					, max( 0.0_f, dot( l_N, l_V ) ) );
				auto l_NdotH = m_writer.DeclLocale( cuT( "l_NdotH" )
					, max( 0.0_f, dot( l_N, l_H ) ) );
				auto l_HdotV = m_writer.DeclLocale( cuT( "l_HdotV" )
					, max( 0.0_f, dot( l_H, l_V ) ) );
				auto l_LdotV = m_writer.DeclLocale( cuT( "l_LdotV" )
					, max( 0.0_f, dot( l_L, l_V ) ) );
				auto l_LdotH = m_writer.DeclLocale( cuT( "l_LdotH" )
					, max( 0.0_f, dot( l_L, l_H ) ) );

				auto a = m_writer.DeclLocale( cuT( "a" )
					, max( 0.001_f, l_sqr( p_roughness ) ) );
				auto l_luminance = m_writer.DeclLocale( cuT( "l_luminance" )
					, 0.3_f * p_light.m_colour().x() + 0.6f * p_light.m_colour().y() + 0.1f * p_light.m_colour().z() );

				auto l_tint = m_writer.DeclLocale( cuT( "l_tint" )
					, m_writer.Ternary( l_luminance > 0.0, p_light.m_colour() / l_luminance, vec3( 1.0_f ) ) ); // Normalize luminance to isolate hue+sat.
				auto l_specularColour = m_writer.DeclLocale( cuT( "l_specularColour" )
					, mix( c3d_specular * 0.08_f * mix( vec3( 1.0_f ), l_tint, c3d_specularTint ), p_albedo, p_metalness ) );
				auto l_csheen = m_writer.DeclLocale( cuT( "l_csheen" )
					, mix( vec3( 1.0_f ), l_tint, c3d_sheenTint ) );

				// Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
				// and mix in diffuse retro-reflection based on roughness
				auto FL = m_writer.DeclLocale( cuT( "FL" )
					, m_schlickFresnel1( l_NdotL ) );
				auto FV = m_writer.DeclLocale( cuT( "FV" )
					, m_schlickFresnel1( l_NdotV ) );
				auto Fd90 = m_writer.DeclLocale( cuT( "Fd90" )
					, 0.5_f + 2.0f * l_LdotH * l_LdotH * a );
				auto Fd = m_writer.DeclLocale( cuT( "Fd" )
					, mix( 1.0_f, Fd90, FL ) * mix( 1.0_f, Fd90, FV ) );

				// Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
				// 1.25 scale is used to (roughly) preserve albedo
				// Fss90 used to "flatten" retroreflection based on roughness
				auto Fss90 = m_writer.DeclLocale( cuT( "Fss90" )
					, l_LdotH * l_LdotH * a );
				auto Fss = m_writer.DeclLocale( cuT( "Fss" )
					, mix( 1.0_f, Fss90, FL ) * mix( 1.0_f, Fss90, FV ) );
				auto ss = m_writer.DeclLocale( cuT( "ss" )
					, 1.25_f * m_writer.Paren( Fss * m_writer.Paren( 1.0f / m_writer.Paren( l_NdotL + l_NdotV + 0.0001f ) - 0.5f ) + 0.5f ) );

				// Specular
				auto aspect = m_writer.DeclLocale( cuT( "aspect" )
					, sqrt( 1.0_f - c3d_anisotropic * 0.9f ) );
				auto ax = m_writer.DeclLocale( cuT( "ax" )
					, max( 0.001_f, m_writer.Paren( a * a ) / aspect ) );
				auto ay = m_writer.DeclLocale( cuT( "ay" )
					, max( 0.001_f, m_writer.Paren( a * a ) * aspect ) );
				auto Ds = m_writer.DeclLocale( cuT( "Ds" )
					, l_gtr2( l_NdotH, dot( l_H, p_tangentToWorld.x() ), dot( l_H, p_tangentToWorld.y() ), ax, ay ) );
				auto FH = m_writer.DeclLocale( cuT( "FH" )
					, m_schlickFresnel1( l_LdotH ) );
				auto Fs = m_writer.DeclLocale( cuT( "Fs" )
					, mix( l_specularColour, vec3( 1.0_f ), FH ) );
				auto roughg = m_writer.DeclLocale( cuT( "roughg" )
					, l_sqr( a * 0.5f + 0.5f ) );
				auto Gs = m_writer.DeclLocale( cuT( "Gs" )
					, l_smithGGX( l_NdotL, roughg ) * l_smithGGX( l_NdotV, roughg ) );

				// Sheen
				auto l_fsheen = m_writer.DeclLocale( cuT( "l_fsheen" )
					, FH * c3d_sheen * l_csheen );

				// Clearcoat (ior = 1.5 -> F0 = 0.04)
				auto Dr = m_writer.DeclLocale( cuT( "Dr" )
					, l_gtr1( l_NdotH, mix( 0.1_f, 0.001_f, c3d_clearCoat ) ) );
				auto Fr = m_writer.DeclLocale( cuT( "Fr" )
					, mix( 0.04_f, 1.0f, FH ) );
				auto Gr = m_writer.DeclLocale( cuT( "Gr" )
					, l_smithGGX( l_NdotL, 0.25_f ) * l_smithGGX( l_NdotV, 0.25_f ) );
				auto l_diffuse = m_writer.DeclLocale( cuT( "Gr" )
					, m_writer.Paren( m_writer.Paren( 1.0f / Float( PI ) ) * mix( Fd, ss, c3d_subsurface ) * p_albedo + l_fsheen ) * m_writer.Paren( 1.0f - p_metalness ) );
				m_writer.Return( m_writer.Paren( l_diffuse + Gs * Fs * Ds + 0.25f * c3d_clearCoat * Gr * Fr * Dr ) * l_NdotL );
			}
			, InLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec2( &m_writer, cuT( "p_tangentToWorld" ) )
			, InVec3( &m_writer, cuT( "p_direction" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metalness" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer } );
	}

	void DisneyBrdfLightingModel::DoDeclare_Distribution()
	{
		// Distribution Function
		m_distributionGGX = m_writer.ImplementFunction< Float >( cuT( "Distribution" )
			, [this]( Float const & p_NdotH
			, Float const & p_roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				auto a = m_writer.DeclLocale( cuT( "a" )
					, p_roughness * p_roughness );
				auto a2 = m_writer.DeclLocale( cuT( "a2" )
					, a * a );
				auto l_NdotH2 = m_writer.DeclLocale( cuT( "l_NdotH2" )
					, p_NdotH * p_NdotH );

				auto l_nominator = m_writer.DeclLocale( cuT( "l_num" )
					, a2 );
				auto l_denominator = m_writer.DeclLocale( cuT( "l_denom" )
					, l_NdotH2 * m_writer.Paren( a2 - 1.0 ) + 1.0 );
				l_denominator = Float( PI ) * l_denominator * l_denominator;

				m_writer.Return( l_nominator / l_denominator );
			}
			, InFloat( &m_writer, cuT( "p_NdotH" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );
	}
	
	void DisneyBrdfLightingModel::DoDeclare_Geometry()
	{
		// Geometry Functions
		m_geometrySchlickGGX = m_writer.ImplementFunction< Float >( cuT( "GeometrySchlickGGX" )
			, [this]( Float const & p_product
				, Float const & p_roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto r = m_writer.DeclLocale( cuT( "r" )
					, p_roughness + 1.0_f );
				auto k = m_writer.DeclLocale( cuT( "k" )
					, m_writer.Paren( r * r ) / 8.0_f );

				auto l_nominator = m_writer.DeclLocale( cuT( "l_num" )
					, p_product );
				auto l_denominator = m_writer.DeclLocale( cuT( "l_denom" )
					, p_product * m_writer.Paren( 1.0_f - k ) + k );

				m_writer.Return( l_nominator / l_denominator );
			}
			, InFloat( &m_writer, cuT( "p_product" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );

		m_geometrySmith = m_writer.ImplementFunction< Float >( cuT( "GeometrySmith" )
			, [this]( Float const & p_NdotV
				, Float const & p_NdotL
				, Float const & p_roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto l_ggx2 = m_writer.DeclLocale( cuT( "ggx2" )
					, m_geometrySchlickGGX( p_NdotV, p_roughness ) );
				auto l_ggx1 = m_writer.DeclLocale( cuT( "ggx1" )
					, m_geometrySchlickGGX( p_NdotL, p_roughness ) );

				m_writer.Return( l_ggx1 * l_ggx2 );
			}
			, InFloat( &m_writer, cuT( "p_NdotV" ) )
			, InFloat( &m_writer, cuT( "p_NdotL" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );
	}
	
	void DisneyBrdfLightingModel::DoDeclare_FresnelShlick()
	{
		// Fresnel Function
		m_schlickFresnel = m_writer.ImplementFunction< Vec3 >( cuT( "FresnelShlick" )
			, [this]( Float const & p_product
				, Vec3 const & p_f0 )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				m_writer.Return( p_f0 + m_writer.Paren( vec3( 1.0_f ) - p_f0 ) * pow( 1.0_f - p_product, 5.0 ) );
			}
			, InFloat( &m_writer, cuT( "p_product" ) )
			, InVec3( &m_writer, cuT( "p_f0" ) ) );
		m_schlickFresnel1 = m_writer.ImplementFunction< Vec3 >( cuT( "FresnelShlick1" )
			, [this]( Float const & p_product )
			{
				auto m = m_writer.DeclLocale( cuT( "m" )
					, clamp( 1.0f - p_product, 0.0_f, 1.0_f ) );
				auto m2 = m_writer.DeclLocale( cuT( "m2" )
					, m * m );
				m_writer.Return( m2 *m2 * m );
			}
			, InFloat( &m_writer, cuT( "p_product" ) ) );
	}
	
	Vec3 DisneyBrdfLightingModel::DoComputeLight( Light const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_direction
		, Vec3 const & p_albedo
		, Float const & p_metalness
		, Float const & p_roughness
		, Float const & p_shadowFactor
		, FragmentInput const & p_fragmentIn )
	{
		return WriteFunctionCall< Vec3 >( &m_writer, cuT( "DoComputeLight" )
			, p_light
			, p_worldEye
			, p_direction
			, p_albedo
			, p_metalness
			, p_roughness
			, p_shadowFactor
			, p_fragmentIn );
	}

	//***********************************************************************************************
}
