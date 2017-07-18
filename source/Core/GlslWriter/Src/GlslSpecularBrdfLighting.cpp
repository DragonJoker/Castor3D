#include "GlslSpecularBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String SpecularBrdfLightingModel::Name = cuT( "pbr_sg" );

	SpecularBrdfLightingModel::SpecularBrdfLightingModel( ShadowType p_shadows, GlslWriter & p_writer )
		: LightingModel{ p_shadows, p_writer }
	{
	}

	std::shared_ptr< LightingModel > SpecularBrdfLightingModel::Create( ShadowType p_shadows, GlslWriter & p_writer )
	{
		return std::make_shared< SpecularBrdfLightingModel >( p_shadows, p_writer );
	}

	Vec3 SpecularBrdfLightingModel::ComputeCombinedLighting( Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		auto c3d_iLightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_iLightsCount" ) );
		auto begin = m_writer.DeclLocale( cuT( "begin" ), 0_i );
		auto end = m_writer.DeclLocale( cuT( "end" ), m_writer.Cast< Int >( c3d_iLightsCount.x() ) );
		auto result = m_writer.DeclLocale( cuT( "result" ), vec3( 0.0_f ) );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputeDirectionalLight( GetDirectionalLight( i )
				, p_worldEye
				, p_diffuse
				, p_specular
				, p_glossiness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_iLightsCount.y() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputePointLight( GetPointLight( i )
				, p_worldEye
				, p_diffuse
				, p_specular
				, p_glossiness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_iLightsCount.z() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputeSpotLight( GetSpotLight( i )
				, p_worldEye
				, p_diffuse
				, p_specular
				, p_glossiness
				, p_receivesShadows
				, p_fragmentIn );
		}
		ROF;

		return result;
	}

	Vec3 SpecularBrdfLightingModel::ComputeDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 SpecularBrdfLightingModel::ComputePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computePoint( PointLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 SpecularBrdfLightingModel::ComputeSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeSpot( SpotLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 SpecularBrdfLightingModel::ComputeOneDirectionalLight( DirectionalLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOneDirectional( DirectionalLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 SpecularBrdfLightingModel::ComputeOnePointLight( PointLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOnePoint( PointLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	Vec3 SpecularBrdfLightingModel::ComputeOneSpotLight( SpotLight const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Int const & p_receivesShadows
		, FragmentInput const & p_fragmentIn )
	{
		return m_computeOneSpot( SpotLight{ p_light }
			, p_worldEye
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_receivesShadows
			, FragmentInput{ p_fragmentIn } );
	}

	void SpecularBrdfLightingModel::DoDeclareModel()
	{
		DoDeclare_Distribution();
		DoDeclare_Geometry();
		DoDeclare_FresnelShlick();
		DoDeclare_ComputeLight();
	}

	void SpecularBrdfLightingModel::Declare_ComputeDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( p_light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					Shadow shadows{ m_writer };

					IF ( m_writer, p_receivesShadows != 0_i )
					{
						shadowFactor = shadows.ComputeDirectionalShadow( p_light.m_transform(), p_fragmentIn.m_v3Vertex, lightDirection, p_fragmentIn.m_v3Normal );
					}
					FI;
				}

				m_writer.Return( DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_diffuse
					, p_specular
					, p_glossiness
					, shadowFactor
					, p_fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::Declare_ComputePointLight()
	{
		OutputComponents output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					Shadow shadows{ m_writer };

					IF ( m_writer, p_receivesShadows != 0_i )
					{
						shadowFactor = shadows.ComputePointShadow( p_fragmentIn.m_v3Vertex, lightDirection, p_fragmentIn.m_v3Normal, 0_i );
					}
					FI;
				}

				auto result = m_writer.DeclLocale( cuT( "result" )
					, DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, lightDirection
						, p_diffuse
						, p_specular
						, p_glossiness
						, shadowFactor
						, p_fragmentIn ) );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" ), p_light.m_attenuation().x() + p_light.m_attenuation().y() * distance + p_light.m_attenuation().z() * distance * distance );
				m_writer.Return( result / attenuation );
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_fragmentIn.m_v3Vertex - p_light.m_position().xyz() );
				auto distance = m_writer.DeclLocale( cuT( "distance" ), length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" ), dot( lightDirection, p_light.m_direction() ) );

				IF( m_writer, spotFactor > p_light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						Shadow shadows{ m_writer };

						IF ( m_writer, p_receivesShadows != 0_i )
						{
							shadowFactor = shadows.ComputeSpotShadow( p_light.m_transform(), p_fragmentIn.m_v3Vertex, lightToVertex, p_fragmentIn.m_v3Normal, Int( 0 ) );
						}
						FI;
					}

					auto result = m_writer.DeclLocale( cuT( "result" )
						, DoComputeLight( p_light.m_lightBase()
							, p_worldEye
							, lightDirection
							, p_diffuse
							, p_specular
							, p_glossiness
							, shadowFactor
							, p_fragmentIn ) );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * distance
						+ p_light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );

					m_writer.Return( spotFactor * result / attenuation );
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
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

				m_writer.Return( DoComputeLight( p_light.m_lightBase()
					, p_worldEye
					, lightDirection
					, p_diffuse
					, p_specular
					, p_glossiness
					, shadowFactor
					, p_fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_light.m_position().xyz() - p_fragmentIn.m_v3Vertex );
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

				auto result = m_writer.DeclLocale( cuT( "result" )
					, DoComputeLight( p_light.m_lightBase()
						, p_worldEye
						, lightDirection
						, p_diffuse
						, p_specular
						, p_glossiness
						, shadowFactor
						, p_fragmentIn ) );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, p_light.m_attenuation().x()
					+ p_light.m_attenuation().y() * distance
					+ p_light.m_attenuation().z() * distance * distance );
				m_writer.Return( result / attenuation );
			}
			, PointLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & p_light
				, Vec3 const & p_worldEye
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Int const & p_receivesShadows
				, FragmentInput const & p_fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" ), p_light.m_position().xyz() - p_fragmentIn.m_v3Vertex );
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

					auto result = m_writer.DeclLocale( cuT( "result" )
						, DoComputeLight( p_light.m_lightBase()
							, p_worldEye
							, lightDirection
							, p_diffuse
							, p_specular
							, p_glossiness
							, shadowFactor
							, p_fragmentIn ) );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, p_light.m_attenuation().x()
						+ p_light.m_attenuation().y() * distance
						+ p_light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - p_light.m_cutOff() ) );
					m_writer.Return( spotFactor * result / attenuation );
				}
				FI;
			}
			, SpotLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InInt( &m_writer, cuT( "p_receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}
	
	void SpecularBrdfLightingModel::DoDeclare_ComputeLight()
	{
		OutputComponents output{ m_writer };
		auto compute = [this]( Light const & p_light
			, Vec3 const & p_worldEye
			, Vec3 const & p_direction
			, Vec3 const & p_diffuse
			, Vec3 const & p_specular
			, Float const & p_glossiness
			, Float const & p_shadowFactor
			, FragmentInput const & p_fragmentIn )
		{
			// From https://learnopengl.com/#!PBR/Lighting
			auto constexpr PI = 3.1415926535897932384626433832795028841968;
			auto L = m_writer.DeclLocale( cuT( "L" )
				, normalize( p_direction ) );
			auto V = m_writer.DeclLocale( cuT( "V" )
				, normalize( normalize( p_worldEye - p_fragmentIn.m_v3Vertex ) ) );
			auto H = m_writer.DeclLocale( cuT( "H" )
				, normalize( L + V ) );
			auto N = m_writer.DeclLocale( cuT( "N" )
				, normalize( p_fragmentIn.m_v3Normal ) );
			auto radiance = m_writer.DeclLocale( cuT( "radiance" )
				, p_light.m_colour() );
			auto roughness = m_writer.DeclLocale( cuT( "roughness" )
				, 1.0_f - p_glossiness );

			auto NdotL = m_writer.DeclLocale( cuT( "NdotL" )
				, max( 0.0_f, dot( N, L ) ) );
			auto NdotV = m_writer.DeclLocale( cuT( "NdotV" )
				, max( 0.0_f, dot( N, V ) ) );
			auto NdotH = m_writer.DeclLocale( cuT( "NdotH" )
				, max( 0.0_f, dot( N, H ) ) );
			auto HdotV = m_writer.DeclLocale( cuT( "HdotV" )
				, max( 0.0_f, dot( H, V ) ) );
			auto LdotV = m_writer.DeclLocale( cuT( "LdotV" )
				, max( 0.0_f, dot( L, V ) ) );

			auto f0 = m_writer.DeclLocale( cuT( "f0" )
				, p_specular );
			auto specfresnel = m_writer.DeclLocale( cuT( "specfresnel" )
				, m_schlickFresnel( HdotV, f0 ) );
			
			auto NDF = m_writer.DeclLocale( cuT( "NDF" )
				, m_distributionGGX( NdotH, roughness ) );
			auto G = m_writer.DeclLocale( cuT( "G" )
				, m_geometrySmith( NdotV, NdotL, roughness ) );

			auto nominator = m_writer.DeclLocale( cuT( "nominator" )
				, specfresnel * NDF * G );
			auto denominator = m_writer.DeclLocale( cuT( "denominator" )
				, 4.0_f * NdotV * NdotL + 0.001_f );
			auto specular = m_writer.DeclLocale( cuT( "specular" )
				, nominator / denominator );
			auto kS = m_writer.DeclLocale( cuT( "kS" )
				, specfresnel );
			auto kD = m_writer.DeclLocale( cuT( "kD" )
				, vec3( 1.0_f ) - kS );

			kD *= 1.0_f - length( p_specular );

			m_writer.Return( m_writer.Paren( kD * p_diffuse / PI + specular ) * radiance * NdotL );
		};
		m_writer.ImplementFunction< Vec3 >( cuT( "DoComputeLight" )
			, compute
			, InLight( &m_writer, cuT( "p_light" ) )
			, InVec3( &m_writer, cuT( "p_worldEye" ) )
			, InVec3( &m_writer, cuT( "p_direction" ) )
			, InVec3( &m_writer, cuT( "p_diffuse" ) )
			, InVec3( &m_writer, cuT( "p_specular" ) )
			, InFloat( &m_writer, cuT( "p_glossiness" ) )
			, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
			, FragmentInput{ m_writer } );
	}

	void SpecularBrdfLightingModel::DoDeclare_Distribution()
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
				auto NdotH2 = m_writer.DeclLocale( cuT( "NdotH2" )
					, p_NdotH * p_NdotH );

				auto nominator = m_writer.DeclLocale( cuT( "num" )
					, a2 );
				auto denominator = m_writer.DeclLocale( cuT( "denom" )
					, NdotH2 * m_writer.Paren( a2 - 1.0 ) + 1.0 );
				denominator = Float( PI ) * denominator * denominator;

				m_writer.Return( nominator / denominator );
			}
			, InFloat( &m_writer, cuT( "p_NdotH" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );
	}
	
	void SpecularBrdfLightingModel::DoDeclare_Geometry()
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

				auto nominator = m_writer.DeclLocale( cuT( "num" )
					, p_product );
				auto denominator = m_writer.DeclLocale( cuT( "denom" )
					, p_product * m_writer.Paren( 1.0_f - k ) + k );

				m_writer.Return( nominator / denominator );
			}
			, InFloat( &m_writer, cuT( "p_product" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );

		m_geometrySmith = m_writer.ImplementFunction< Float >( cuT( "GeometrySmith" )
			, [this]( Float const & p_NdotV
				, Float const & p_NdotL
				, Float const & p_roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto ggx2 = m_writer.DeclLocale( cuT( "ggx2" )
					, m_geometrySchlickGGX( p_NdotV, p_roughness ) );
				auto ggx1 = m_writer.DeclLocale( cuT( "ggx1" )
					, m_geometrySchlickGGX( p_NdotL, p_roughness ) );

				m_writer.Return( ggx1 * ggx2 );
			}
			, InFloat( &m_writer, cuT( "p_NdotV" ) )
			, InFloat( &m_writer, cuT( "p_NdotL" ) )
			, InFloat( &m_writer, cuT( "p_roughness" ) ) );
	}
	
	void SpecularBrdfLightingModel::DoDeclare_FresnelShlick()
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
	
	Vec3 SpecularBrdfLightingModel::DoComputeLight( Light const & p_light
		, Vec3 const & p_worldEye
		, Vec3 const & p_direction
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Float const & p_shadowFactor
		, FragmentInput const & p_fragmentIn )
	{
		return WriteFunctionCall< Vec3 >( &m_writer, cuT( "DoComputeLight" )
			, p_light
			, p_worldEye
			, p_direction
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_shadowFactor
			, p_fragmentIn );
	}

	//***********************************************************************************************
}
