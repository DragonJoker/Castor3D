#include "GlslDisneyBrdfLighting.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		const String DisneyBrdfLightingModel::Name = cuT( "pbr_mr" );

		DisneyBrdfLightingModel::DisneyBrdfLightingModel( ShadowType shadows
			, GlslWriter & writer )
			: LightingModel{ shadows, writer }
		{
		}

		Vec3 DisneyBrdfLightingModel::computeCombinedLighting( Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn )
		{
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" ), 0_i );
			auto end = m_writer.declLocale( cuT( "end" ), m_writer.cast< Int >( c3d_lightsCount.x() ) );
			auto result = m_writer.declLocale( cuT( "result" ), vec3( 0.0_f ) );

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

		Vec3 DisneyBrdfLightingModel::computeDirectionalLight( DirectionalLight const & light
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

		Vec3 DisneyBrdfLightingModel::computePointLight( PointLight const & light
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

		Vec3 DisneyBrdfLightingModel::computeSpotLight( SpotLight const & light
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

		Vec3 DisneyBrdfLightingModel::computeOneDirectionalLight( DirectionalLight const & light
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

		Vec3 DisneyBrdfLightingModel::computeOnePointLight( PointLight const & light
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

		Vec3 DisneyBrdfLightingModel::computeOneSpotLight( SpotLight const & light
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

		void DisneyBrdfLightingModel::doDeclareModel()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeLight();
		}

		void DisneyBrdfLightingModel::doDeclareComputeDirectionalLight()
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
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						Shadow shadows{ m_writer };

						IF ( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = shadows.computeDirectionalShadow( light.m_transform()
								, fragmentIn.m_vertex
								, lightDirection
								, fragmentIn.m_normal );
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

		void DisneyBrdfLightingModel::doDeclareComputePointLight()
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
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" ), fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" ), length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						Shadow shadows{ m_writer };

						IF ( m_writer, receivesShadows != 0_i )
						{
							shadowFactor = shadows.computePointShadow( fragmentIn.m_vertex
								, lightDirection
								, fragmentIn.m_normal
								, light.m_farPlane()
								, light.m_index() );
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
					auto attenuation = m_writer.declLocale( cuT( "attenuation" ), light.m_attenuation().x() + light.m_attenuation().y() * distance + light.m_attenuation().z() * distance * distance );
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

		void DisneyBrdfLightingModel::doDeclareComputeSpotLight()
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
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" ), fragmentIn.m_vertex - light.m_position().xyz() );
					auto distance = m_writer.declLocale( cuT( "distance" ), length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" ), dot( lightDirection, light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							Shadow shadows{ m_writer };

							IF ( m_writer, receivesShadows != 0_i )
							{
								shadowFactor = shadows.computeSpotShadow( light.m_transform(), fragmentIn.m_vertex, lightToVertex, fragmentIn.m_normal, Int( 0 ) );
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
							, light.m_attenuation().x()
							+ light.m_attenuation().y() * distance
							+ light.m_attenuation().z() * distance * distance );
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );

						m_writer.returnStmt( spotFactor * result / attenuation );
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}

		void DisneyBrdfLightingModel::doDeclareComputeOneDirectionalLight()
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
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->computeDirectionalShadow( light.m_transform()
								, fragmentIn.m_vertex
								, lightDirection
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

		void DisneyBrdfLightingModel::doDeclareComputeOnePointLight()
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
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" ), light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" ), length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( receivesShadows
							, m_shadowModel->computePointShadow( fragmentIn.m_vertex
								, light.m_position().xyz()
								, fragmentIn.m_normal
								, light.m_farPlane() ) );
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

		void DisneyBrdfLightingModel::doDeclareComputeOneSpotLight()
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
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" ), light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" ), length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" ), normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" ), dot( lightDirection, light.m_direction() ) );

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
						spotFactor = m_writer.paren( 1.0_f - m_writer.paren( 1.0_f - spotFactor ) * 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() ) );
						m_writer.returnStmt( spotFactor * result / attenuation );
					}
					FI;
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer } );
		}
	
		void DisneyBrdfLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			auto constexpr PI = 3.1415926535897932384626433832795028841968;
			auto sqr = m_writer.implementFunction< Float >( cuT( "sqr" )
				, [&]( Float const & p_value )
				{
					m_writer.returnStmt( p_value * p_value );
				}
				, InFloat{ &m_writer, cuT( "p_value" ) } );

			auto gtr1 = m_writer.implementFunction< Float >( cuT( "GTR1" )
				, [&]( Float const & p_NdotH
					, Float const & p_a )
				{
					IF( m_writer, p_a >= 1.0_f )
					{
						m_writer.returnStmt( 1.0_f / PI );
					}
					FI;

					auto a2 = m_writer.declLocale( cuT( "a2" )
						, p_a * p_a );
					auto t = m_writer.declLocale( cuT( "t" )
						, 1.0_f + m_writer.paren( a2 - 1.0f ) * p_NdotH * p_NdotH );
					m_writer.returnStmt( m_writer.paren( a2 - 1.0f ) / m_writer.paren( Float( PI ) * log( a2 ) * t) );
				}
				, InFloat{ &m_writer, cuT( "p_NdotH" ) }
				, InFloat{ &m_writer, cuT( "p_a" ) } );

			auto gtr2 = m_writer.implementFunction< Float >( cuT( "GTR2" )
				, [&]( Float const & p_NdotH
					, Float const & p_HdotX
					, Float const & p_HdotY
					, Float const & p_ax
					, Float const & p_ay )
				{
					m_writer.returnStmt( 1.0f / m_writer.paren( Float( PI ) * p_ax * p_ay * sqr( sqr( p_HdotX / p_ax ) + sqr( p_HdotY / p_ay ) + sqr( p_NdotH ) ) ) );
				}
				, InFloat{ &m_writer, cuT( "p_NdotH" ) }
				, InFloat{ &m_writer, cuT( "p_HdotX" ) }
				, InFloat{ &m_writer, cuT( "p_HdotY" ) }
				, InFloat{ &m_writer, cuT( "p_ax" ) }
				, InFloat{ &m_writer, cuT( "p_ay" ) } );

			auto smithGGX = m_writer.implementFunction< Float >( cuT( "SmithG_GGX" )
				, [&]( Float const & p_NdotV
					, Float const & p_alphaG )
				{
					auto a = m_writer.declLocale( cuT( "a" )
						, p_alphaG * p_alphaG );
					auto b = m_writer.declLocale( cuT( "b" )
						, p_NdotV * p_NdotV );
					m_writer.returnStmt( 1.0_f / m_writer.paren( p_NdotV + sqrt( a + b - a*b ) ) );
				}
				, InFloat{ &m_writer, cuT( "p_NdotV" ) }
				, InFloat{ &m_writer, cuT( "p_alphaG" ) } );

			m_writer.implementFunction< Vec3 >( cuT( "doComputeLight" )
				, [&]( Light const & light
					, Vec3 const & worldEye
					, Vec2 const & p_tangentToWorld
					, Vec3 const & p_direction
					, Vec3 const & albedo
					, Float const & p_metalness
					, Float const & roughness
					, Float const & p_shadowFactor
					, FragmentInput const & fragmentIn )
				{
					// From Disney's BRDF explorer: https://github.com/wdas/brdf
					//float3 DisneyBRDF( float3 baseColor, out float3 specularColor, float3 normal, float roughness, float3 lightDir, float3 viewDir, float3 X, float3 Y, out float3 diffuse )
					auto c3d_specular = m_writer.getBuiltin< Float >( cuT( "c3d_specular" ) );
					auto c3d_specularTint = m_writer.getBuiltin< Float >( cuT( "c3d_specularTint" ) );
					auto c3d_sheen = m_writer.getBuiltin< Float >( cuT( "c3d_sheen" ) );
					auto c3d_sheenTint = m_writer.getBuiltin< Float >( cuT( "c3d_sheenTint" ) );
					auto c3d_anisotropic = m_writer.getBuiltin< Float >( cuT( "c3d_anisotropic" ) );
					auto c3d_clearCoat = m_writer.getBuiltin< Float >( cuT( "c3d_clearCoat" ) );
					auto c3d_subsurface = m_writer.getBuiltin< Float >( cuT( "c3d_subsurface" ) );
					auto L = m_writer.declLocale( cuT( "L" )
						, normalize( p_direction ) );
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
					auto LdotH = m_writer.declLocale( cuT( "LdotH" )
						, max( 0.0_f, dot( L, H ) ) );

					auto a = m_writer.declLocale( cuT( "a" )
						, max( 0.001_f, sqr( roughness ) ) );
					auto luminance = m_writer.declLocale( cuT( "luminance" )
						, 0.3_f * light.m_colour().x() + 0.6f * light.m_colour().y() + 0.1f * light.m_colour().z() );

					auto tint = m_writer.declLocale( cuT( "tint" )
						, m_writer.ternary( luminance > 0.0, light.m_colour() / luminance, vec3( 1.0_f ) ) ); // Normalize luminance to isolate hue+sat.
					auto specularColour = m_writer.declLocale( cuT( "specularColour" )
						, mix( c3d_specular * 0.08_f * mix( vec3( 1.0_f ), tint, c3d_specularTint ), albedo, p_metalness ) );
					auto csheen = m_writer.declLocale( cuT( "csheen" )
						, mix( vec3( 1.0_f ), tint, c3d_sheenTint ) );

					// Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
					// and mix in diffuse retro-reflection based on roughness
					auto FL = m_writer.declLocale( cuT( "FL" )
						, m_schlickFresnel1( NdotL ) );
					auto FV = m_writer.declLocale( cuT( "FV" )
						, m_schlickFresnel1( NdotV ) );
					auto Fd90 = m_writer.declLocale( cuT( "Fd90" )
						, 0.5_f + 2.0f * LdotH * LdotH * a );
					auto Fd = m_writer.declLocale( cuT( "Fd" )
						, mix( 1.0_f, Fd90, FL ) * mix( 1.0_f, Fd90, FV ) );

					// Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
					// 1.25 scale is used to (roughly) preserve albedo
					// Fss90 used to "flatten" retroreflection based on roughness
					auto Fss90 = m_writer.declLocale( cuT( "Fss90" )
						, LdotH * LdotH * a );
					auto Fss = m_writer.declLocale( cuT( "Fss" )
						, mix( 1.0_f, Fss90, FL ) * mix( 1.0_f, Fss90, FV ) );
					auto ss = m_writer.declLocale( cuT( "ss" )
						, 1.25_f * m_writer.paren( Fss * m_writer.paren( 1.0f / m_writer.paren( NdotL + NdotV + 0.0001f ) - 0.5f ) + 0.5f ) );

					// Specular
					auto aspect = m_writer.declLocale( cuT( "aspect" )
						, sqrt( 1.0_f - c3d_anisotropic * 0.9f ) );
					auto ax = m_writer.declLocale( cuT( "ax" )
						, max( 0.001_f, m_writer.paren( a * a ) / aspect ) );
					auto ay = m_writer.declLocale( cuT( "ay" )
						, max( 0.001_f, m_writer.paren( a * a ) * aspect ) );
					auto Ds = m_writer.declLocale( cuT( "Ds" )
						, gtr2( NdotH, dot( H, p_tangentToWorld.x() ), dot( H, p_tangentToWorld.y() ), ax, ay ) );
					auto FH = m_writer.declLocale( cuT( "FH" )
						, m_schlickFresnel1( LdotH ) );
					auto Fs = m_writer.declLocale( cuT( "Fs" )
						, mix( specularColour, vec3( 1.0_f ), FH ) );
					auto roughg = m_writer.declLocale( cuT( "roughg" )
						, sqr( a * 0.5f + 0.5f ) );
					auto Gs = m_writer.declLocale( cuT( "Gs" )
						, smithGGX( NdotL, roughg ) * smithGGX( NdotV, roughg ) );

					// Sheen
					auto fsheen = m_writer.declLocale( cuT( "fsheen" )
						, FH * c3d_sheen * csheen );

					// Clearcoat (ior = 1.5 -> F0 = 0.04)
					auto Dr = m_writer.declLocale( cuT( "Dr" )
						, gtr1( NdotH, mix( 0.1_f, 0.001_f, c3d_clearCoat ) ) );
					auto Fr = m_writer.declLocale( cuT( "Fr" )
						, mix( 0.04_f, 1.0f, FH ) );
					auto Gr = m_writer.declLocale( cuT( "Gr" )
						, smithGGX( NdotL, 0.25_f ) * smithGGX( NdotV, 0.25_f ) );
					auto diffuse = m_writer.declLocale( cuT( "Gr" )
						, m_writer.paren( m_writer.paren( 1.0f / Float( PI ) ) * mix( Fd, ss, c3d_subsurface ) * albedo + fsheen ) * m_writer.paren( 1.0f - p_metalness ) );
					m_writer.returnStmt( m_writer.paren( diffuse + Gs * Fs * Ds + 0.25f * c3d_clearCoat * Gr * Fr * Dr ) * NdotL );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec2( &m_writer, cuT( "p_tangentToWorld" ) )
				, InVec3( &m_writer, cuT( "p_direction" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "p_metalness" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InFloat( &m_writer, cuT( "p_shadowFactor" ) )
				, FragmentInput{ m_writer } );
		}

		void DisneyBrdfLightingModel::doDeclareDistribution()
		{
			// Distribution Function
			m_distributionGGX = m_writer.implementFunction< Float >( cuT( "Distribution" )
				, [this]( Float const & p_NdotH
				, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = m_writer.declLocale( cuT( "a" )
						, roughness * roughness );
					auto a2 = m_writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH2 = m_writer.declLocale( cuT( "NdotH2" )
						, p_NdotH * p_NdotH );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, a2 );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, NdotH2 * m_writer.paren( a2 - 1.0 ) + 1.0 );
					denominator = Float( PI ) * denominator * denominator;

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "p_NdotH" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void DisneyBrdfLightingModel::doDeclareGeometry()
		{
			// Geometry Functions
			m_geometrySchlickGGX = m_writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [this]( Float const & p_product
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = m_writer.declLocale( cuT( "r" )
						, roughness + 1.0_f );
					auto k = m_writer.declLocale( cuT( "k" )
						, m_writer.paren( r * r ) / 8.0_f );

					auto nominator = m_writer.declLocale( cuT( "num" )
						, p_product );
					auto denominator = m_writer.declLocale( cuT( "denom" )
						, p_product * m_writer.paren( 1.0_f - k ) + k );

					m_writer.returnStmt( nominator / denominator );
				}
				, InFloat( &m_writer, cuT( "p_product" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );

			m_geometrySmith = m_writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [this]( Float const & p_NdotV
					, Float const & p_NdotL
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = m_writer.declLocale( cuT( "ggx2" )
						, m_geometrySchlickGGX( p_NdotV, roughness ) );
					auto ggx1 = m_writer.declLocale( cuT( "ggx1" )
						, m_geometrySchlickGGX( p_NdotL, roughness ) );

					m_writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &m_writer, cuT( "p_NdotV" ) )
				, InFloat( &m_writer, cuT( "p_NdotL" ) )
				, InFloat( &m_writer, cuT( "roughness" ) ) );
		}
	
		void DisneyBrdfLightingModel::doDeclareFresnelShlick()
		{
			// Fresnel Function
			m_schlickFresnel = m_writer.implementFunction< Vec3 >( cuT( "FresnelShlick" )
				, [this]( Float const & p_product
					, Vec3 const & p_f0 )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					m_writer.returnStmt( p_f0 + m_writer.paren( vec3( 1.0_f ) - p_f0 ) * pow( 1.0_f - p_product, 5.0 ) );
				}
				, InFloat( &m_writer, cuT( "p_product" ) )
				, InVec3( &m_writer, cuT( "p_f0" ) ) );
			m_schlickFresnel1 = m_writer.implementFunction< Vec3 >( cuT( "FresnelShlick1" )
				, [this]( Float const & p_product )
				{
					auto m = m_writer.declLocale( cuT( "m" )
						, clamp( 1.0f - p_product, 0.0_f, 1.0_f ) );
					auto m2 = m_writer.declLocale( cuT( "m2" )
						, m * m );
					m_writer.returnStmt( m2 *m2 * m );
				}
				, InFloat( &m_writer, cuT( "p_product" ) ) );
		}
	
		Vec3 DisneyBrdfLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & p_direction
			, Vec3 const & albedo
			, Float const & p_metalness
			, Float const & roughness
			, Float const & p_shadowFactor
			, FragmentInput const & fragmentIn )
		{
			return writeFunctionCall< Vec3 >( &m_writer, cuT( "doComputeLight" )
				, light
				, worldEye
				, p_direction
				, albedo
				, p_metalness
				, roughness
				, p_shadowFactor
				, fragmentIn );
		}

		//***********************************************************************************************
	}
}
