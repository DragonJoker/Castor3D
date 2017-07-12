#include "GlslCookTorranceLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String CookTorranceLightingModel::Name = cuT( "pbr" );

	CookTorranceLightingModel::CookTorranceLightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: LightingModel{ p_shadows, p_writer }
	{
	}

	std::shared_ptr< LightingModel > CookTorranceLightingModel::Create( ShadowType p_shadows, GlslWriter & p_writer )
	{
		return std::make_shared< CookTorranceLightingModel >( p_shadows, p_writer );
	}

	Vec3 CookTorranceLightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
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

	Vec3 CookTorranceLightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
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

	Vec3 CookTorranceLightingModel::ComputePointLight( PointLight const & p_light
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

	Vec3 CookTorranceLightingModel::ComputeSpotLight( SpotLight const & p_light
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

	Vec3 CookTorranceLightingModel::ComputeOneDirectionalLight( DirectionalLight const & p_light
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

	Vec3 CookTorranceLightingModel::ComputeOnePointLight( PointLight const & p_light
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

	Vec3 CookTorranceLightingModel::ComputeOneSpotLight( SpotLight const & p_light
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

	void CookTorranceLightingModel::DoDeclareModel()
	{
		DoDeclare_Distribution();
		DoDeclare_Geometry();
		DoDeclare_FresnelShlick();
		DoDeclare_ComputeLight();
	}

	void CookTorranceLightingModel::Declare_ComputeDirectionalLight()
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
				PbrMaterials l_materials{ m_writer };
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

	void CookTorranceLightingModel::Declare_ComputePointLight()
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
				PbrMaterials l_materials{ m_writer };
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

	void CookTorranceLightingModel::Declare_ComputeSpotLight()
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
				PbrMaterials l_materials{ m_writer };
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

	void CookTorranceLightingModel::Declare_ComputeOneDirectionalLight()
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
				PbrMaterials l_materials{ m_writer };
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

	void CookTorranceLightingModel::Declare_ComputeOnePointLight()
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
				PbrMaterials l_materials{ m_writer };
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

	void CookTorranceLightingModel::Declare_ComputeOneSpotLight()
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
				PbrMaterials l_materials{ m_writer };
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
	
	void CookTorranceLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents l_output{ m_writer };
		auto l_compute = [this]( Light const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_direction
			, Vec3 const & p_albedo
			, Float const & p_metalness
			, Float const & p_roughness
			, Float const & p_shadowFactor
			, FragmentInput const & p_fragmentIn )
		{
			// From https://learnopengl.com/#!PBR/Lighting
			auto constexpr PI = 3.1415926535897932384626433832795028841968;
			auto l_L = m_writer.DeclLocale( cuT( "l_L" )
				, normalize( p_direction ) );
			auto l_V = m_writer.DeclLocale( cuT( "l_V" )
				, normalize( normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) ) );
			auto l_H = m_writer.DeclLocale( cuT( "l_H" )
				, normalize( l_L + l_V ) );
			auto l_N = m_writer.DeclLocale( cuT( "l_N" )
				, normalize( p_fragmentIn.m_v3Normal ) );
			//l_N.x() = -l_N.x();
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

			auto l_f0 = m_writer.DeclLocale( cuT( "l_f0" )
				, mix( vec3( 0.04_f ), p_albedo, p_metalness ) );
			auto l_specfresnel = m_writer.DeclLocale( cuT( "l_specfresnel" )
				, m_schlickFresnel( l_HdotV, l_f0 ) );
			
			auto l_NDF = m_writer.DeclLocale( cuT( "l_NDF" )
				, m_distributionGGX( l_NdotH, p_roughness ) );
			auto l_G = m_writer.DeclLocale( cuT( "l_G" )
				, m_geometrySmith( l_NdotV, l_NdotL, p_roughness ) );

			auto l_nominator = m_writer.DeclLocale( cuT( "l_nominator" )
				, l_specfresnel * l_NDF * l_G );
			auto l_denominator = m_writer.DeclLocale( cuT( "l_denominator" )
				, 4.0_f * l_NdotV * l_NdotL + 0.001_f );
			auto l_specular = m_writer.DeclLocale( cuT( "l_specular" )
				, l_nominator / l_denominator );
			auto l_kS = m_writer.DeclLocale( cuT( "l_kS" )
				, l_specfresnel );
			auto l_kD = m_writer.DeclLocale( cuT( "l_kD" )
				, vec3( 1.0_f ) - l_kS );

			l_kD *= 1.0_f - p_metalness;

			m_writer.Return( m_writer.Paren( l_kD * p_albedo / PI + l_specular ) * l_radiance * l_NdotL );
		};
		m_writer.ImplementFunction< Vec3 >( cuT( "DoComputeLight" )
			, l_compute
			, InLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_direction" ) )
			, InVec3( &m_writer, cuT( "p_albedo" ) )
			, InFloat( &m_writer, cuT( "p_metalness" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) )
			, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer } );
	}

	void CookTorranceLightingModel::DoDeclare_Distribution()
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
	
	void CookTorranceLightingModel::DoDeclare_Geometry()
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
	
	void CookTorranceLightingModel::DoDeclare_FresnelShlick()
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
	}
	
	Vec3 CookTorranceLightingModel::DoComputeLight( Light const & p_light
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
