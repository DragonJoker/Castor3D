#include "GlslMetallicBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"

using namespace Castor;

namespace GLSL
{
	const String MetallicBrdfLightingModel::Name = cuT( "pbr_mr" );

	MetallicBrdfLightingModel::MetallicBrdfLightingModel( ShadowType shadows, GlslWriter & writer )
		: LightingModel{ shadows, writer }
	{
	}

	std::shared_ptr< LightingModel > MetallicBrdfLightingModel::Create( ShadowType shadows, GlslWriter & writer )
	{
		return std::make_shared< MetallicBrdfLightingModel >( shadows, writer );
	}

	Vec3 MetallicBrdfLightingModel::ComputeCombinedLighting( Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		auto c3d_lightsCount = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
		auto begin = m_writer.DeclLocale( cuT( "begin" )
			, 0_i );
		auto end = m_writer.DeclLocale( cuT( "end" )
			, m_writer.Cast< Int >( c3d_lightsCount.x() ) );
		auto result = m_writer.DeclLocale( cuT( "result" )
			, vec3( 0.0_f ) );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputeDirectionalLight( GetDirectionalLight( i )
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, fragmentIn );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_lightsCount.y() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputePointLight( GetPointLight( i )
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, fragmentIn );
		}
		ROF;

		begin = end;
		end += m_writer.Cast< Int >( c3d_lightsCount.z() );

		FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
		{
			result += ComputeSpotLight( GetSpotLight( i )
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, fragmentIn );
		}
		ROF;

		return result;
	}

	Vec3 MetallicBrdfLightingModel::ComputeDirectionalLight( DirectionalLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computeDirectional( DirectionalLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	Vec3 MetallicBrdfLightingModel::ComputePointLight( PointLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computePoint( PointLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	Vec3 MetallicBrdfLightingModel::ComputeSpotLight( SpotLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computeSpot( SpotLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	Vec3 MetallicBrdfLightingModel::ComputeOneDirectionalLight( DirectionalLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computeOneDirectional( DirectionalLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	Vec3 MetallicBrdfLightingModel::ComputeOnePointLight( PointLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computeOnePoint( PointLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	Vec3 MetallicBrdfLightingModel::ComputeOneSpotLight( SpotLight const & light
		, Vec3 const & worldEye
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Int const & receivesShadows
		, FragmentInput const & fragmentIn )
	{
		return m_computeOneSpot( SpotLight{ light }
			, worldEye
			, albedo
			, metallic
			, roughness
			, receivesShadows
			, FragmentInput{ fragmentIn } );
	}

	void MetallicBrdfLightingModel::DoDeclareModel()
	{
		DoDeclare_Distribution();
		DoDeclare_Geometry();
		DoDeclare_FresnelShlick();
		DoDeclare_ComputeLight();
	}

	void MetallicBrdfLightingModel::Declare_ComputeDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( -light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF ( m_writer, receivesShadows != 0_i )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->ComputeDirectionalShadow( light.m_transform()
								, fragmentIn.m_v3Vertex
								, -lightDirection
								, fragmentIn.m_v3Normal ) );
					}
					FI;
				}

				m_writer.Return( DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, albedo
					, metallic
					, roughness
					, shadowFactor
					, fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::Declare_ComputePointLight()
	{
		OutputComponents output{ m_writer };
		m_computePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" )
					, light.m_position().xyz() - fragmentIn.m_v3Vertex );
				auto distance = m_writer.DeclLocale( cuT( "distance" )
					, length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					IF ( m_writer, receivesShadows != 0_i )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->ComputePointShadow( fragmentIn.m_v3Vertex
								, light.m_position().xyz()
								, fragmentIn.m_v3Normal
								, light.m_index() ) );
					}
					FI;
				}

				auto result = m_writer.DeclLocale( cuT( "result" )
					, DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn ) );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, light.m_attenuation().x() + light.m_attenuation().y() * distance + light.m_attenuation().z() * distance * distance );
				m_writer.Return( result / attenuation );
			}
			, PointLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::Declare_ComputeSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" )
					, light.m_position().xyz() - fragmentIn.m_v3Vertex );
				auto distance = m_writer.DeclLocale( cuT( "distance" )
					, length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" )
					, dot( lightDirection, -light.m_direction() ) );
				auto result = m_writer.DeclLocale( cuT( "result" )
					, vec3( 0.0_f ) );

				IF( m_writer, spotFactor > light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						IF ( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->ComputeSpotShadow( light.m_transform()
									, fragmentIn.m_v3Vertex
									, -lightToVertex
									, fragmentIn.m_v3Normal
									, light.m_index() ) );
						}
						FI;
					}

					result = DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - light.m_cutOff() ) );

					result = spotFactor * result / attenuation;
				}
				FI;

				m_writer.Return( result );
			}
			, SpotLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::Declare_ComputeOneDirectionalLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneDirectional = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeDirectionalLight" )
			, [this]( DirectionalLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( -light.m_direction().xyz() ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( receivesShadows
						, m_shadowModel->ComputeDirectionalShadow( light.m_transform()
							, fragmentIn.m_v3Vertex
							, -lightDirection
							, fragmentIn.m_v3Normal ) );
				}

				m_writer.Return( DoComputeLight( light.m_lightBase()
					, worldEye
					, lightDirection
					, albedo
					, metallic
					, roughness
					, shadowFactor
					, fragmentIn ) );
			}
			, DirectionalLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::Declare_ComputeOnePointLight()
	{
		OutputComponents output{ m_writer };
		m_computeOnePoint = m_writer.ImplementFunction< Vec3 >( cuT( "ComputePointLight" )
			, [this]( PointLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" )
					, light.m_position().xyz() - fragmentIn.m_v3Vertex );
				auto distance = m_writer.DeclLocale( cuT( "distance" )
					, length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( lightToVertex ) );
				auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" )
					, 1.0_f );

				if ( m_shadows != ShadowType::eNone )
				{
					shadowFactor = 1.0_f - min( receivesShadows
						, m_shadowModel->ComputePointShadow( fragmentIn.m_v3Vertex
							, light.m_position().xyz()
							, fragmentIn.m_v3Normal ) );
				}

				auto result = m_writer.DeclLocale( cuT( "result" )
					, DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn ) );
				auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
					, light.m_attenuation().x()
					+ light.m_attenuation().y() * distance
					+ light.m_attenuation().z() * distance * distance );
				m_writer.Return( result / attenuation );
			}
			, PointLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::Declare_ComputeOneSpotLight()
	{
		OutputComponents output{ m_writer };
		m_computeOneSpot = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeSpotLight" )
			, [this]( SpotLight const & light
				, Vec3 const & worldEye
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Int const & receivesShadows
				, FragmentInput const & fragmentIn )
			{
				PbrMRMaterials materials{ m_writer };
				auto lightToVertex = m_writer.DeclLocale( cuT( "lightToVertex" )
					, light.m_position().xyz() - fragmentIn.m_v3Vertex );
				auto distance = m_writer.DeclLocale( cuT( "distance" )
					, length( lightToVertex ) );
				auto lightDirection = m_writer.DeclLocale( cuT( "lightDirection" )
					, normalize( lightToVertex ) );
				auto spotFactor = m_writer.DeclLocale( cuT( "spotFactor" )
					, dot( lightDirection, -light.m_direction() ) );
				auto result = m_writer.DeclLocale( cuT( "result" )
					, vec3( 0.0_f ) );

				IF( m_writer, spotFactor > light.m_cutOff() )
				{
					auto shadowFactor = m_writer.DeclLocale( cuT( "shadowFactor" ), Float( 1 ) );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->ComputeSpotShadow( light.m_transform()
								, fragmentIn.m_v3Vertex
								, -lightToVertex
								, fragmentIn.m_v3Normal ) );
					}

					result = DoComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn );
					auto attenuation = m_writer.DeclLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					spotFactor = m_writer.Paren( 1.0_f - m_writer.Paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.Paren( 1.0_f - light.m_cutOff() ) );
					result = spotFactor * result / attenuation;
				}
				FI;

				m_writer.Return( result );
			}
			, SpotLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InInt( &m_writer, cuT( "receivesShadows" ) )
			, FragmentInput{ m_writer } );
	}
	
	void MetallicBrdfLightingModel::DoDeclare_ComputeLight()
	{
		m_computeLight = m_writer.ImplementFunction< Vec3 >( cuT( "DoComputeLight" )
			, [this]( Light const & light
				, Vec3 const & worldEye
				, Vec3 const & direction
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Float const & shadowFactor
				, FragmentInput const & fragmentIn )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				auto L = m_writer.DeclLocale( cuT( "L" )
					, normalize( direction ) );
				auto V = m_writer.DeclLocale( cuT( "V" )
					, normalize( normalize( worldEye - fragmentIn.m_v3Vertex ) ) );
				auto H = m_writer.DeclLocale( cuT( "H" )
					, normalize( L + V ) );
				auto N = m_writer.DeclLocale( cuT( "N" )
					, normalize( fragmentIn.m_v3Normal ) );
				auto radiance = m_writer.DeclLocale( cuT( "radiance" )
					, light.m_colour() );

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
					, mix( vec3( 0.04_f ), albedo, metallic ) );
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

				kD *= 1.0_f - metallic;

				m_writer.Return( shadowFactor * m_writer.Paren( m_writer.Paren( kD * albedo / PI + specular ) * radiance * NdotL ) );
			}
			, InLight( &m_writer, cuT( "light" ) )
			, InVec3( &m_writer, cuT( "worldEye" ) )
			, InVec3( &m_writer, cuT( "direction" ) )
			, InVec3( &m_writer, cuT( "albedo" ) )
			, InFloat( &m_writer, cuT( "metallic" ) )
			, InFloat( &m_writer, cuT( "roughness" ) )
			, InFloat( &m_writer, cuT( "shadowFactor" ) )
			, FragmentInput{ m_writer } );
	}

	void MetallicBrdfLightingModel::DoDeclare_Distribution()
	{
		// Distribution Function
		m_distributionGGX = m_writer.ImplementFunction< Float >( cuT( "Distribution" )
			, [this]( Float const & product
			, Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				auto a = m_writer.DeclLocale( cuT( "a" )
					, roughness * roughness );
				auto a2 = m_writer.DeclLocale( cuT( "a2" )
					, a * a );
				auto NdotH2 = m_writer.DeclLocale( cuT( "NdotH2" )
					, product * product );

				auto nominator = m_writer.DeclLocale( cuT( "num" )
					, a2 );
				auto denominator = m_writer.DeclLocale( cuT( "denom" )
					, NdotH2 * m_writer.Paren( a2 - 1.0 ) + 1.0 );
				denominator = Float( PI ) * denominator * denominator;

				m_writer.Return( nominator / denominator );
			}
			, InFloat( &m_writer, cuT( "product" ) )
			, InFloat( &m_writer, cuT( "roughness" ) ) );
	}
	
	void MetallicBrdfLightingModel::DoDeclare_Geometry()
	{
		// Geometry Functions
		m_geometrySchlickGGX = m_writer.ImplementFunction< Float >( cuT( "GeometrySchlickGGX" )
			, [this]( Float const & product
				, Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto r = m_writer.DeclLocale( cuT( "r" )
					, roughness + 1.0_f );
				auto k = m_writer.DeclLocale( cuT( "k" )
					, m_writer.Paren( r * r ) / 8.0_f );

				auto nominator = m_writer.DeclLocale( cuT( "num" )
					, product );
				auto denominator = m_writer.DeclLocale( cuT( "denom" )
					, product * m_writer.Paren( 1.0_f - k ) + k );

				m_writer.Return( nominator / denominator );
			}
			, InFloat( &m_writer, cuT( "product" ) )
			, InFloat( &m_writer, cuT( "roughness" ) ) );

		m_geometrySmith = m_writer.ImplementFunction< Float >( cuT( "GeometrySmith" )
			, [this]( Float const & NdotV
				, Float const & NdotL
				, Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto ggx2 = m_writer.DeclLocale( cuT( "ggx2" )
					, m_geometrySchlickGGX( NdotV, roughness ) );
				auto ggx1 = m_writer.DeclLocale( cuT( "ggx1" )
					, m_geometrySchlickGGX( NdotL, roughness ) );

				m_writer.Return( ggx1 * ggx2 );
			}
			, InFloat( &m_writer, cuT( "NdotV" ) )
			, InFloat( &m_writer, cuT( "NdotL" ) )
			, InFloat( &m_writer, cuT( "roughness" ) ) );
	}
	
	void MetallicBrdfLightingModel::DoDeclare_FresnelShlick()
	{
		// Fresnel Function
		m_schlickFresnel = m_writer.ImplementFunction< Vec3 >( cuT( "FresnelShlick" )
			, [this]( Float const & product
				, Vec3 const & f0 )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				m_writer.Return( f0 + m_writer.Paren( vec3( 1.0_f ) - f0 ) * pow( 1.0_f - product, 5.0 ) );
			}
			, InFloat( &m_writer, cuT( "product" ) )
			, InVec3( &m_writer, cuT( "p_f0" ) ) );
	}
	
	Vec3 MetallicBrdfLightingModel::DoComputeLight( Light const & light
		, Vec3 const & worldEye
		, Vec3 const & direction
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Float const & shadowFactor
		, FragmentInput const & fragmentIn )
	{
		return m_computeLight( light
			, worldEye
			, direction
			, albedo
			, metallic
			, roughness
			, shadowFactor
			, FragmentInput{ fragmentIn } );
	}

	//***********************************************************************************************
}
