#include "GlslMetallicBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		const String MetallicBrdfLightingModel::Name = cuT( "pbr_mr" );

		MetallicBrdfLightingModel::MetallicBrdfLightingModel( ShadowType shadows, GlslWriter & writer )
			: LightingModel{ shadows, writer }
		{
		}

		Vec3 MetallicBrdfLightingModel::computeCombinedLighting( Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )
		{
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" )
				, 0_i );
			auto end = m_writer.declLocale( cuT( "end" )
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );
			auto result = m_writer.declLocale( cuT( "result" )
				, vec3( 0.0_f ) );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				result += computeDirectionalLight( getDirectionalLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, fragmentIn );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.y() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				result += computePointLight( getPointLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, fragmentIn );
			}
			ROF;

			begin = end;
			end += m_writer.cast< Int >( c3d_lightsCount.z() );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				result += computeSpotLight( getSpotLight( i )
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

		Vec3 MetallicBrdfLightingModel::computeDirectionalLight( DirectionalLight const & light
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

		Vec3 MetallicBrdfLightingModel::computePointLight( PointLight const & light
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

		Vec3 MetallicBrdfLightingModel::computeSpotLight( SpotLight const & light
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

		Vec3 MetallicBrdfLightingModel::computeOneDirectionalLight( DirectionalLight const & light
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

		Vec3 MetallicBrdfLightingModel::computeOnePointLight( PointLight const & light
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

		Vec3 MetallicBrdfLightingModel::computeOneSpotLight( SpotLight const & light
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

		void MetallicBrdfLightingModel::doDeclareModel()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeLight();
		}

		void MetallicBrdfLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< Vec3 >( cuT( "computeDirectionalLight" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						IF ( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computeDirectionalShadow( light.m_transform()
									, fragmentIn.m_vertex
									, -lightDirection
									, fragmentIn.m_normal ) );
						}
						FI;
					}

					m_writer.returnStmt( doComputeLight( light.m_lightBase()
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

		void MetallicBrdfLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< Vec3 >( cuT( "computePointLight" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						IF ( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computePointShadow( fragmentIn.m_vertex
									, light.m_position().xyz()
									, fragmentIn.m_normal
									, light.m_index() ) );
						}
						FI;
					}

					auto result = m_writer.declLocale( cuT( "result" )
						, doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
							, shadowFactor
							, fragmentIn ) );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, light.m_attenuation().x() + light.m_attenuation().y() * distance + light.m_attenuation().z() * distance * distance );
					m_writer.returnStmt( result / attenuation );
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< Vec3 >( cuT( "computeSpotLight" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );
					auto result = m_writer.declLocale( cuT( "result" )
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							IF ( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = 1.0_f - min( receivesShadows
									, m_shadowModel->computeSpotShadow( light.m_transform()
										, fragmentIn.m_vertex
										, -lightToVertex
										, fragmentIn.m_normal
										, light.m_index() ) );
							}
							FI;
						}

						result = doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
							, shadowFactor
							, fragmentIn );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );

						result = spotFactor * result / attenuation;
					}
					FI;

					m_writer.returnStmt( result );
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOneDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeOneDirectional = m_writer.implementFunction< Vec3 >( cuT( "computeDirectionalLight" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->computeDirectionalShadow( light.m_transform()
								, fragmentIn.m_vertex
								, -lightDirection
								, fragmentIn.m_normal ) );
					}

					m_writer.returnStmt( doComputeLight( light.m_lightBase()
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

		void MetallicBrdfLightingModel::doDeclareComputeOnePointLight()
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< Vec3 >( cuT( "computePointLight" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
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
								, fragmentIn.m_normal ) );
					}

					auto result = m_writer.declLocale( cuT( "result" )
						, doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
							, shadowFactor
							, fragmentIn ) );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, light.m_attenuation().x()
						+ light.m_attenuation().y() * distance
						+ light.m_attenuation().z() * distance * distance );
					m_writer.returnStmt( result / attenuation );
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOneSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< Vec3 >( cuT( "computeSpotLight" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );
					auto result = m_writer.declLocale( cuT( "result" )
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							shadowFactor = 1.0_f - min( receivesShadows
								, m_shadowModel->computeSpotShadow( light.m_transform()
									, fragmentIn.m_vertex
									, -lightToVertex
									, fragmentIn.m_normal ) );
						}

						result = doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
							, shadowFactor
							, fragmentIn );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );
						result = spotFactor * result / attenuation;
					}
					FI;

					m_writer.returnStmt( result );
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}
	
		void MetallicBrdfLightingModel::doDeclareComputeLight()
		{
			m_computeLight = m_writer.implementFunction< Vec3 >( cuT( "doComputeLight" )
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
					auto L = m_writer.declLocale( cuT( "L" )
						, normalize( direction ) );
					auto V = m_writer.declLocale( cuT( "V" )
						, normalize( normalize( worldEye - fragmentIn.m_vertex ) ) );
					auto H = m_writer.declLocale( cuT( "H" )
						, normalize( L + V ) );
					auto N = m_writer.declLocale( cuT( "N" )
						, normalize( fragmentIn.m_normal ) );
					auto radiance = m_writer.declLocale( cuT( "radiance" )
						, light.m_colour() );

					auto NdotL = m_writer.declLocale( cuT( "NdotL" )
						, max( 0.0_f, dot( N, L ) ) );
					auto NdotV = m_writer.declLocale( cuT( "NdotV" )
						, max( 0.0_f, dot( N, V ) ) );
					auto NdotH = m_writer.declLocale( cuT( "NdotH" )
						, max( 0.0_f, dot( N, H ) ) );
					auto HdotV = m_writer.declLocale( cuT( "HdotV" )
						, max( 0.0_f, dot( H, V ) ) );
					auto LdotV = m_writer.declLocale( cuT( "LdotV" )
						, max( 0.0_f, dot( L, V ) ) );

					auto f0 = m_writer.declLocale( cuT( "f0" )
						, mix( vec3( 0.04_f ), albedo, metallic ) );
					auto specfresnel = m_writer.declLocale( cuT( "specfresnel" )
						, m_schlickFresnel( HdotV, f0 ) );
			
					auto NDF = m_writer.declLocale( cuT( "NDF" )
						, m_distributionGGX( NdotH, roughness ) );
					auto G = m_writer.declLocale( cuT( "G" )
						, m_geometrySmith( NdotV, NdotL, roughness ) );

					auto nominator = m_writer.declLocale( cuT( "nominator" )
						, specfresnel * NDF * G );
					auto denominator = m_writer.declLocale( cuT( "denominator" )
						, 4.0_f * NdotV * NdotL + 0.001_f );
					auto specular = m_writer.declLocale( cuT( "specular" )
						, nominator / denominator );
					auto kS = m_writer.declLocale( cuT( "kS" )
						, specfresnel );
					auto kD = m_writer.declLocale( cuT( "kD" )
						, vec3( 1.0_f ) - kS );

					kD *= 1.0_f - metallic;

					m_writer.returnStmt( shadowFactor * m_writer.paren( m_writer.paren( kD * albedo / PI + specular ) * radiance * NdotL ) );
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

		void MetallicBrdfLightingModel::doDeclareDistribution()
		{
			// Distribution Function
			m_distributionGGX = m_writer.implementFunction< Float >( cuT( "Distribution" )
				, [this]( Float const & product
				, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = m_writer.declLocale( cuT( "a" )
						, roughness * roughness );
					auto a2 = m_writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH2 = m_writer.declLocale( cuT( "NdotH2" )
						, product * product );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, a2 );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, NdotH2 * m_writer.paren( a2 - 1.0 ) + 1.0 );
					denominator = Float( PI ) * denominator * denominator;

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void MetallicBrdfLightingModel::doDeclareGeometry()
		{
			// Geometry Functions
			m_geometrySchlickGGX = m_writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [this]( Float const & product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = m_writer.declLocale( cuT( "r" )
						, roughness + 1.0_f );
					auto k = m_writer.declLocale( cuT( "k" )
						, m_writer.paren( r * r ) / 8.0_f );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, product );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, product * m_writer.paren( 1.0_f - k ) + k );

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );

			m_geometrySmith = m_writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [this]( Float const & NdotV
					, Float const & NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = m_writer.declLocale( cuT( "ggx2" )
						, m_geometrySchlickGGX( NdotV, roughness ) );
					auto ggx1 = m_writer.declLocale( cuT( "ggx1" )
						, m_geometrySchlickGGX( NdotL, roughness ) );

					m_writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &m_writer, cuT( "NdotV" ) )
				, InFloat( &m_writer, cuT( "NdotL" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void MetallicBrdfLightingModel::doDeclareFresnelShlick()
		{
			// Fresnel Function
			m_schlickFresnel = m_writer.implementFunction< Vec3 >( cuT( "FresnelShlick" )
				, [this]( Float const & product
					, Vec3 const & f0 )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					m_writer.returnStmt( f0 + m_writer.paren( vec3( 1.0_f ) - f0 ) * pow( 1.0_f - product, 5.0 ) );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InVec3( &m_writer, cuT( "p_f0" ) ) );
		}
	
		Vec3 MetallicBrdfLightingModel::doComputeLight( Light const & light
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
}
