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

		void MetallicBrdfLightingModel::computeCombined( Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			auto c3d_lightsCount = m_writer.getBuiltin< Vec3 >( cuT( "c3d_lightsCount" ) );
			auto begin = m_writer.declLocale( cuT( "begin" )
				, 0_i );
			auto end = m_writer.declLocale( cuT( "end" )
				, m_writer.cast< Int >( c3d_lightsCount.x() ) );

			FOR( m_writer, Int, i, begin, cuT( "i < end" ), cuT( "++i" ) )
			{
				m_writer << m_computeDirectional( getDirectionalLight( i )
					, worldEye
					, albedo
					, metallic
					, roughness
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
					, albedo
					, metallic
					, roughness
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
					, albedo
					, metallic
					, roughness
					, receivesShadows
					, FragmentInput{ fragmentIn }
					, parentOutput );
				m_writer << endi;
			}
			ROF;
		}

		void MetallicBrdfLightingModel::compute( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_writer << m_computeDirectional( DirectionalLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << endi;
		}

		void MetallicBrdfLightingModel::compute( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_writer << m_computeOnePoint( PointLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		void MetallicBrdfLightingModel::compute( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & parentOutput )const
		{
			m_writer << m_computeOneSpot( SpotLight{ light }
				, worldEye
				, albedo
				, metallic
				, roughness
				, receivesShadows
				, FragmentInput{ fragmentIn }
				, parentOutput );
			m_writer << Endi();
		}

		Vec3 MetallicBrdfLightingModel::computeBackLit( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, FragmentInput const & fragmentIn )const
		{
			return m_computeDirectionalBackLit( DirectionalLight{ light }
				, worldEye
				, albedo
				, metallic
				, FragmentInput{ fragmentIn } );
		}

		Vec3 MetallicBrdfLightingModel::computeBackLit( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, FragmentInput const & fragmentIn )const
		{
			return m_computePointBackLit( PointLight{ light }
				, worldEye
				, albedo
				, metallic
				, FragmentInput{ fragmentIn } );
		}

		Vec3 MetallicBrdfLightingModel::computeBackLit( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & albedo
			, Float const & metallic
			, FragmentInput const & fragmentIn )const
		{
			return m_computeSpotBackLit( SpotLight{ light }
				, worldEye
				, albedo
				, metallic
				, FragmentInput{ fragmentIn } );
		}

		void MetallicBrdfLightingModel::doDeclareModel()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeLight();
			doDeclareComputeLightBackLit();
		}

		void MetallicBrdfLightingModel::doDeclareComputeDirectionalLight()
		{
			OutputComponents output{ m_writer };
			m_computeDirectional = m_writer.implementFunction< Void >( cuT( "computeDirectionalLight" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction().xyz() ) );
					auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" )
						, 1.0_f );

					if ( m_shadows != ShadowType::eNone )
					{
						shadowFactor = 1.0_f - min( m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computeDirectionalShadow( light.m_transform()
								, fragmentIn.m_vertex
								, -lightDirection
								, fragmentIn.m_normal ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
						, shadowFactor
						, fragmentIn
						, output );
					parentOutput.m_diffuse += output.m_diffuse;
					parentOutput.m_specular += output.m_specular;
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputePointLight()
		{
			OutputComponents output{ m_writer };
			m_computePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
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
						shadowFactor = 1.0_f - min( m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computePointShadow( fragmentIn.m_vertex
								, light.m_position().xyz()
								, fragmentIn.m_normal
								, light.m_lightBase().m_farPlane()
								, light.m_index() ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
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
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							shadowFactor = 1.0_f - min( m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpotShadow( light.m_transform()
									, fragmentIn.m_vertex
									, -lightToVertex
									, fragmentIn.m_normal
									, light.m_index() ) );
						}

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
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
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOnePointLight()
		{
			OutputComponents output{ m_writer };
			m_computeOnePoint = m_writer.implementFunction< Void >( cuT( "computePointLight" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
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
						shadowFactor = 1.0_f - min( m_writer.cast< Float >( receivesShadows )
							, m_shadowModel->computePointShadow( fragmentIn.m_vertex
								, light.m_position().xyz()
								, fragmentIn.m_normal
								, light.m_lightBase().m_farPlane() ) );
					}

					doComputeLight( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, roughness
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
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeOneSpotLight()
		{
			OutputComponents output{ m_writer };
			m_computeOneSpot = m_writer.implementFunction< Void >( cuT( "computeSpotLight" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Int const & receivesShadows
					, FragmentInput const & fragmentIn
					, OutputComponents & parentOutput )
				{
					OutputComponents output
					{
						m_writer.declLocale( cuT( "lightDiffuse" ), vec3( 0.0_f ) ),
						m_writer.declLocale( cuT( "lightSpecular" ), vec3( 0.0_f ) )
					};
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );
					auto spotFactor = m_writer.declLocale( cuT( "spotFactor" )
						, dot( lightDirection, -light.m_direction() ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						auto shadowFactor = m_writer.declLocale( cuT( "shadowFactor" ), Float( 1 ) );

						if ( m_shadows != ShadowType::eNone )
						{
							shadowFactor = 1.0_f - min( m_writer.cast< Float >( receivesShadows )
								, m_shadowModel->computeSpotShadow( light.m_transform()
									, fragmentIn.m_vertex
									, -lightToVertex
									, fragmentIn.m_normal ) );
						}

						doComputeLight( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, roughness
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
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InInt( &m_writer, cuT( "receivesShadows" ) )
				, FragmentInput{ m_writer }
				, output );
		}

		void MetallicBrdfLightingModel::doDeclareComputeDirectionalLightBackLit()
		{
			OutputComponents output{ m_writer };
			m_computeDirectionalBackLit = m_writer.implementFunction< Vec3 >( cuT( "computeBackLit" )
				, [this]( DirectionalLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( -light.m_direction().xyz() ) );

					m_writer.returnStmt( doComputeLightBackLit( light.m_lightBase()
						, worldEye
						, lightDirection
						, albedo
						, metallic
						, fragmentIn ) );
				}
				, DirectionalLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputePointLightBackLit()
		{
			OutputComponents output{ m_writer };
			m_computePointBackLit = m_writer.implementFunction< Vec3 >( cuT( "computeBackLit" )
				, [this]( PointLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
					, FragmentInput const & fragmentIn )
				{
					PbrMRMaterials materials{ m_writer };
					auto lightToVertex = m_writer.declLocale( cuT( "lightToVertex" )
						, light.m_position().xyz() - fragmentIn.m_vertex );
					auto distance = m_writer.declLocale( cuT( "distance" )
						, length( lightToVertex ) );
					auto lightDirection = m_writer.declLocale( cuT( "lightDirection" )
						, normalize( lightToVertex ) );

					auto backLit = m_writer.declLocale( cuT( "backLit" )
						, doComputeLightBackLit( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, fragmentIn ) );
					auto attenuation = m_writer.declLocale( cuT( "attenuation" )
						, glsl::fma( light.m_attenuation().z()
							, distance * distance
							, glsl::fma( light.m_attenuation().y()
								, distance
								, light.m_attenuation().x() ) ) );
					m_writer.returnStmt( backLit / attenuation );
				}
				, PointLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputeSpotLightBackLit()
		{
			OutputComponents output{ m_writer };
			m_computeSpotBackLit = m_writer.implementFunction< Vec3 >( cuT( "computeBackLit" )
				, [this]( SpotLight const & light
					, Vec3 const & worldEye
					, Vec3 const & albedo
					, Float const & metallic
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
					auto backLit = m_writer.declLocale( cuT( "backLit" )
						, vec3( 0.0_f ) );

					IF( m_writer, spotFactor > light.m_cutOff() )
					{
						backLit = doComputeLightBackLit( light.m_lightBase()
							, worldEye
							, lightDirection
							, albedo
							, metallic
							, fragmentIn );
						auto attenuation = m_writer.declLocale( cuT( "attenuation" )
							, glsl::fma( light.m_attenuation().z()
								, distance * distance
								, glsl::fma( light.m_attenuation().y()
									, distance
									, light.m_attenuation().x() ) ) );
						spotFactor = glsl::fma( m_writer.paren( 1.0_f - spotFactor )
							, 1.0_f / m_writer.paren( 1.0_f - light.m_cutOff() )
							, 1.0_f );
						backLit = spotFactor * backLit / attenuation;
					}
					FI;

					m_writer.returnStmt( backLit );
				}
				, SpotLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, FragmentInput{ m_writer } );
		}

		void MetallicBrdfLightingModel::doDeclareComputeLight()
		{
			OutputComponents output{ m_writer };
			m_computeLight = m_writer.implementFunction< Void >( cuT( "doComputeLight" )
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & direction
					, Vec3 const & albedo
					, Float const & metallic
					, Float const & roughness
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
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
						, glsl::fma( 4.0_f
							, NdotV * NdotL
							, 0.001_f ) );
					auto specReflectance = m_writer.declLocale( cuT( "specReflectance" )
						, nominator / denominator );
					auto kS = m_writer.declLocale( cuT( "kS" )
						, specfresnel );
					auto kD = m_writer.declLocale( cuT( "kD" )
						, vec3( 1.0_f ) - kS );

					kD *= 1.0_f - metallic;

					output.m_diffuse = shadowFactor * m_writer.paren( radiance * NdotL * kD / PI );
					output.m_specular = shadowFactor * m_writer.paren( specReflectance * radiance * NdotL );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "direction" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
				, InFloat( &m_writer, cuT( "roughness" ) )
				, InFloat( &m_writer, cuT( "shadowFactor" ) )
				, FragmentInput{ m_writer }
				, output );
		}
	
		void MetallicBrdfLightingModel::doDeclareComputeLightBackLit()
		{
			OutputComponents output{ m_writer };
			m_computeLightBackLit = m_writer.implementFunction< Vec3 >( cuT( "doComputeLightBackLit" )
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & direction
					, Vec3 const & albedo
					, Float const & metallic
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
					auto HdotV = m_writer.declLocale( cuT( "HdotV" )
						, max( 0.0_f, dot( H, V ) ) );

					auto f0 = m_writer.declLocale( cuT( "f0" )
						, mix( vec3( 0.04_f ), albedo, metallic ) );
					auto specfresnel = m_writer.declLocale( cuT( "specfresnel" )
						, m_schlickFresnel( HdotV, f0 ) );
			
					auto kS = m_writer.declLocale( cuT( "kS" )
						, specfresnel );
					auto kD = m_writer.declLocale( cuT( "kD" )
						, vec3( 1.0_f ) - kS );

					kD *= 1.0_f - metallic;

					m_writer.returnStmt( radiance * NdotL * kD / PI );
				}
				, InLight( &m_writer, cuT( "light" ) )
				, InVec3( &m_writer, cuT( "worldEye" ) )
				, InVec3( &m_writer, cuT( "direction" ) )
				, InVec3( &m_writer, cuT( "albedo" ) )
				, InFloat( &m_writer, cuT( "metallic" ) )
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
						, glsl::fma( NdotH2
							, m_writer.paren( a2 - 1.0 )
							, 1.0_f ) );
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
						, glsl::fma( product
							, m_writer.paren( 1.0_f - k )
							, k ) );

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
					m_writer.returnStmt( glsl::fma( vec3( 1.0_f ) - f0
						, vec3( pow( 1.0_f - product, 5.0_f ) )
						, f0 ) );
				}
				, InFloat( &m_writer, cuT( "product" ) )
				, InVec3( &m_writer, cuT( "f0" ) ) );
		}
	
		void MetallicBrdfLightingModel::doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & direction
			, Vec3 const & albedo
			, Float const & metallic
			, Float const & roughness
			, Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & output )const
		{
			m_writer << m_computeLight( light
				, worldEye
				, direction
				, albedo
				, metallic
				, roughness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, output );
			m_writer << Endi();
		}
	
		Vec3 MetallicBrdfLightingModel::doComputeLightBackLit( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & direction
			, Vec3 const & albedo
			, Float const & metallic
			, FragmentInput const & fragmentIn )const
		{
			return m_computeLightBackLit( light
				, worldEye
				, direction
				, albedo
				, metallic
				, FragmentInput{ fragmentIn } );
		}

		//***********************************************************************************************
	}
}
