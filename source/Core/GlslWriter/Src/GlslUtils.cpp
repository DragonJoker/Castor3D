#include "GlslUtils.hpp"

#include "GlslIntrinsics.hpp"

using namespace Castor;

namespace GLSL
{
	Utils::Utils( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Utils::DeclareCalcTexCoord()
	{
		m_calcTexCoord = m_writer.ImplementFunction< Vec2 >( cuT( "CalcTexCoord" )
			, [&]()
			{
				auto gl_FragCoord = m_writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );
				auto c3d_renderSize = m_writer.GetBuiltin< Vec2 >( cuT( "c3d_renderSize" ) );
				m_writer.Return( gl_FragCoord.xy() / c3d_renderSize );
			} );
	}

	void Utils::DeclareCalcVSPosition()
	{
		m_calcVSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcVSPosition" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_invProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.DeclLocale( cuT( "l_texCoord" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.DeclLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_vsPosition = m_writer.DeclLocale( cuT( "l_vsPosition" )
					, p_invProj * vec4( l_csPosition, 1.0 ) );
				l_vsPosition.xyz() /= l_vsPosition.w();
				m_writer.Return( l_vsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invProj" ) } );
	}

	void Utils::DeclareCalcVSDepth()
	{
		m_calcVSDepth = m_writer.ImplementFunction< Float >( cuT( "CalcVSDepth" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_projection )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.DeclLocale( cuT( "l_depth" )
					, texture( c3d_mapDepth, p_uv ).r() );
				m_writer.Return( p_projection[3][2] / m_writer.Paren( l_depth * 2.0_f - 1.0_f - p_projection[2][2] ) );
			}
			, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_projection" ) } );
	}

	void Utils::DeclareCalcWSPosition()
	{
		m_calcWSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcWSPosition" )
			, [&]( Vec2 const & p_uv
				, Mat4 const & p_invViewProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto l_depth = m_writer.DeclLocale( cuT( "l_depth" )
					, texture( c3d_mapDepth, p_uv, 0.0_f ).x() );
				auto l_csPosition = m_writer.DeclLocale( cuT( "l_psPosition" )
					, vec3( p_uv * 2.0f - 1.0f, l_depth * 2.0 - 1.0 ) );
				auto l_wsPosition = m_writer.DeclLocale( cuT( "l_wsPosition" )
					, p_invViewProj * vec4( l_csPosition, 1.0 ) );
				l_wsPosition.xyz() /= l_wsPosition.w();
				m_writer.Return( l_wsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InMat4{ &m_writer, cuT( "p_invViewProj" ) } );
	}

	void Utils::DeclareApplyGamma()
	{
		m_applyGamma = m_writer.ImplementFunction< Vec3 >( cuT( "ApplyGamma" )
			, [&]( Float const & p_gamma
				, Vec3 const & p_HDR )
			{
				m_writer.Return( pow( p_HDR, vec3( 1.0_f / p_gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "p_gamma" ) }
			, InVec3{ &m_writer, cuT( "p_HDR" ) } );
	}

	void Utils::DeclareRemoveGamma()
	{
		m_removeGamma = m_writer.ImplementFunction< Vec3 >( cuT( "RemoveGamma" )
			, [&]( Float const & p_gamma
				, Vec3 const & p_sRGB )
			{
				m_writer.Return( pow( p_sRGB, vec3( p_gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "p_gamma" ) }
			, InVec3{ &m_writer, cuT( "p_sRGB" ) } );
	}

	void Utils::DeclareLineariseDepth()
	{
		m_lineariseDepth = m_writer.ImplementFunction< Float >( cuT( "LineariseDepth" )
			, [&]( Float const & p_depth
				, Mat4 const & p_invProj )
			{
				auto c3d_fCameraNearPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraNearPlane" ) );
				auto c3d_fCameraFarPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraFarPlane" ) );
				auto l_z = m_writer.DeclLocale( cuT( "l_z" )
					, p_depth *2.0_f - 1.0_f );
				auto l_unprojected = m_writer.DeclLocale( cuT( "l_unprojected" )
					, p_invProj * vec4( 0.0_f, 0.0_f, l_z, 1.0_f ) );
				l_z = l_unprojected.z() / l_unprojected.w();
				m_writer.Return( m_writer.Paren( l_z - c3d_fCameraNearPlane )
					/ m_writer.Paren( c3d_fCameraFarPlane - c3d_fCameraNearPlane ) );

			}
			, InFloat{ &m_writer, cuT( "p_depth" ) }
			, InMat4{ &m_writer, cuT( "p_invProj" ) } );
	}

	void Utils::DeclareGetMapNormal()
	{
		m_getMapNormal = m_writer.ImplementFunction< Vec3 >( cuT( "GetMapNormal" )
			, [&]( Vec2 const & p_uv
				, Vec3 const & p_normal
				, Vec3 const & p_position )
			{
				auto c3d_mapNormal( m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

				auto l_mapNormal = m_writer.DeclLocale( cuT( "l_mapNormal" )
					, texture( c3d_mapNormal, p_uv.xy() ).xyz() );
				l_mapNormal = l_mapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 );
				auto l_Q1 = m_writer.DeclLocale( cuT( "l_Q1" )
					, dFdx( p_position ) );
				auto l_Q2 = m_writer.DeclLocale( cuT( "l_Q2" )
					, dFdy( p_position ) );
				auto l_st1 = m_writer.DeclLocale( cuT( "l_st1" )
					, dFdx( p_uv ) );
				auto l_st2 = m_writer.DeclLocale( cuT( "l_st2" )
					, dFdy( p_uv ) );
				auto l_N = m_writer.DeclLocale( cuT( "l_N" )
					, normalize( p_normal ) );
				auto l_T = m_writer.DeclLocale( cuT( "l_T" )
					, normalize( l_Q1 * l_st2.t() - l_Q2 * l_st1.t() ) );
				auto l_B = m_writer.DeclLocale( cuT( "l_B" )
					, -normalize( cross( l_N, l_T ) ) );
				auto l_tbn = m_writer.DeclLocale( cuT( "l_tbn" )
					, mat3( l_T, l_B, l_N ) );
				m_writer.Return( normalize( l_tbn * l_mapNormal ) );
			}
			, InVec2{ &m_writer, cuT( "p_uv" ) }
			, InVec3{ &m_writer, cuT( "p_normal" ) }
			, InVec3{ &m_writer, cuT( "p_position" ) } );
	}

	void Utils::DeclareFresnelSchlick()
	{
		m_fresnelSchlick = m_writer.ImplementFunction< Vec3 >( cuT( "FresnelSchlick" )
			, [&]( Float const & p_product
				, Vec3 const & p_f0
				, Float const & p_roughness )
			{
				m_writer.Return( p_f0 + m_writer.Paren( max( vec3( 1.0_f - p_roughness ), p_f0 ) - p_f0 ) * pow( 1.0_f - p_product, 5.0 ) );
			}
			, InFloat{ &m_writer, cuT( "p_product" ) }
			, InVec3{ &m_writer, cuT( "p_f0" ) }
			, InFloat{ &m_writer, cuT( "p_roughness" ) } );
	}

	void Utils::DeclareComputeMetallicIBL()
	{
		m_computeMetallicIBL = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & p_normal
				, Vec3 const & p_position
				, Vec3 const & p_albedo
				, Float const & p_metalness
				, Float const & p_roughness
				, Vec3 const & p_worldEye
				, SamplerCube const & p_irradiance
				, SamplerCube const & p_prefiltered
				, Sampler2D const & p_brdf
				, Int const & p_invertY )
			{
				auto l_V = m_writer.DeclLocale( cuT( "l_V" )
					, normalize( p_worldEye - p_position ) );
				auto l_NdotV = m_writer.DeclLocale( cuT( "l_NdotV" )
					, max( dot( p_normal, l_V ), 0.0 ) );
				auto l_f0 = m_writer.DeclLocale( cuT( "l_f0" )
					, mix( vec3( 0.04_f ), p_albedo, p_metalness ) );
				auto l_F = m_writer.DeclLocale( cuT( "l_F" )
					, FresnelSchlick( l_NdotV, l_f0, p_roughness ) );
				auto l_kS = m_writer.DeclLocale( cuT( "l_kS" )
					, l_F );
				auto l_kD = m_writer.DeclLocale( cuT( "l_kD" )
					, vec3( 1.0_f ) - l_kS );
				l_kD *= 1.0 - p_metalness;
				auto l_irradiance = m_writer.DeclLocale( cuT( "l_irradiance" )
					, texture( p_irradiance, p_normal ).rgb() );
				auto l_diffuse = m_writer.DeclLocale( cuT( "l_diffuse" )
					, l_irradiance * p_albedo );

				auto l_R = m_writer.DeclLocale( cuT( "l_R" )
					, reflect( -l_V, p_normal ) );
				l_R.y() = m_writer.Ternary( p_invertY != 0_i, l_R.y(), -l_R.y() );
				auto l_prefilteredColor = m_writer.DeclLocale( cuT( "l_prefilteredColor" )
					, texture( p_prefiltered, l_R, p_roughness * MaxIblReflectionLod ).rgb() );
				auto l_envBRDF = m_writer.DeclLocale( cuT( "l_envBRDF" )
					, texture( p_brdf, vec2( l_NdotV, p_roughness ) ).rg() );
				auto l_specular = m_writer.DeclLocale( cuT( "l_specular" )
					, l_prefilteredColor * m_writer.Paren( l_kS * l_envBRDF.x() + l_envBRDF.y() ) );

				m_writer.Return( l_kD * l_diffuse + l_specular );
			}
			, InVec3{ &m_writer, cuT( "p_normal" ) }
			, InVec3{ &m_writer, cuT( "p_position" ) }
			, InVec3{ &m_writer, cuT( "p_albedo" ) }
			, InFloat{ &m_writer, cuT( "p_metalness" ) }
			, InFloat{ &m_writer, cuT( "p_roughness" ) }
			, InVec3{ &m_writer, cuT( "p_worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "p_irradiance" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "p_prefiltered" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "p_brdf" ) }
			, InInt{ &m_writer, cuT( "p_invertY" ) } );
	}

	void Utils::DeclareComputeSpecularIBL()
	{
		m_computeSpecularIBL = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & p_normal
				, Vec3 const & p_position
				, Vec3 const & p_diffuse
				, Vec3 const & p_specular
				, Float const & p_glossiness
				, Vec3 const & p_worldEye
				, SamplerCube const & p_irradiance
				, SamplerCube const & p_prefiltered
				, Sampler2D const & p_brdf
				, Int const & p_invertY )
			{
				auto l_roughness = m_writer.DeclLocale( cuT( "l_roughness" )
					, 1.0_f - p_glossiness );
				auto l_V = m_writer.DeclLocale( cuT( "l_V" )
					, normalize( p_worldEye - p_position ) );
				auto l_NdotV = m_writer.DeclLocale( cuT( "l_NdotV" )
					, max( dot( p_normal, l_V ), 0.0 ) );
				auto l_f0 = m_writer.DeclLocale( cuT( "l_f0" )
					, p_specular );
				auto l_F = m_writer.DeclLocale( cuT( "l_F" )
					, FresnelSchlick( l_NdotV, l_f0, l_roughness ) );
				auto l_kS = m_writer.DeclLocale( cuT( "l_kS" )
					, l_F );
				auto l_kD = m_writer.DeclLocale( cuT( "l_kD" )
					, vec3( 1.0_f ) - l_kS );
				l_kD *= 1.0 - length( p_specular );
				auto l_irradiance = m_writer.DeclLocale( cuT( "l_irradiance" )
					, texture( p_irradiance, p_normal ).rgb() );
				auto l_diffuse = m_writer.DeclLocale( cuT( "l_diffuse" )
					, l_irradiance * p_diffuse );

				auto l_R = m_writer.DeclLocale( cuT( "l_R" )
					, reflect( -l_V, p_normal ) );
				l_R.y() = m_writer.Ternary( p_invertY != 0_i, l_R.y(), -l_R.y() );
				auto l_prefilteredColor = m_writer.DeclLocale( cuT( "l_prefilteredColor" )
					, texture( p_prefiltered, l_R, l_roughness * MaxIblReflectionLod ).rgb() );
				auto l_envBRDF = m_writer.DeclLocale( cuT( "l_envBRDF" )
					, texture( p_brdf, vec2( l_NdotV, l_roughness ) ).rg() );
				auto l_specular = m_writer.DeclLocale( cuT( "l_specular" )
					, l_prefilteredColor * m_writer.Paren( l_kS * l_envBRDF.x() + l_envBRDF.y() ) );

				m_writer.Return( l_kD * l_diffuse + l_specular );
			}
			, InVec3{ &m_writer, cuT( "p_normal" ) }
			, InVec3{ &m_writer, cuT( "p_position" ) }
			, InVec3{ &m_writer, cuT( "p_diffuse" ) }
			, InVec3{ &m_writer, cuT( "p_specular" ) }
			, InFloat{ &m_writer, cuT( "p_glossiness" ) }
			, InVec3{ &m_writer, cuT( "p_worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "p_irradiance" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "p_prefiltered" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "p_brdf" ) }
			, InInt{ &m_writer, cuT( "p_invertY" ) } );
	}

	Vec2 Utils::CalcTexCoord()
	{
		return m_calcTexCoord();
	}

	Vec3 Utils::CalcVSPosition( Vec2 const & p_uv
		, Mat4 const & p_invProj )
	{
		return m_calcVSPosition( p_uv, p_invProj );
	}

	Float Utils::CalcVSDepth( Vec2 const & p_uv
		, Mat4 const & p_projection )
	{
		return m_calcVSDepth( p_uv, p_projection );
	}

	Vec3 Utils::CalcWSPosition( Vec2 const & p_uv
		, Mat4 const & p_invViewProj )
	{
		return m_calcWSPosition( p_uv, p_invViewProj );
	}

	Vec3 Utils::ApplyGamma( Float const & p_gamma
		, Vec3 const & p_HDR )
	{
		return m_applyGamma( p_gamma, p_HDR );
	}

	Vec3 Utils::RemoveGamma( Float const & p_gamma
		, Vec3 const & p_sRGB )
	{
		return m_removeGamma( p_gamma, p_sRGB );
	}

	Vec3 Utils::GetMapNormal( Vec2 const & p_uv
		, Vec3 const & p_normal
		, Vec3 const & p_position )
	{
		return WriteFunctionCall< Vec3 >( &m_writer
			, cuT( "GetMapNormal" )
			, p_uv
			, p_normal
			, p_position );
	}

	Float Utils::LineariseDepth( Float const & p_depth, Mat4 const & p_invProj )
	{
		return m_lineariseDepth( p_depth, p_invProj );
	}

	Vec3 Utils::FresnelSchlick( Float const & p_product
		, Vec3 const & p_f0
		, Float const & p_roughness )
	{
		return m_fresnelSchlick( p_product, p_f0, p_roughness );
	}

	Vec3 Utils::ComputeMetallicIBL( Vec3 const & p_normal
		, Vec3 const & p_position
		, Vec3 const & p_albedo
		, Float const & p_metalness
		, Float const & p_roughness
		, Vec3 const & p_worldEye
		, SamplerCube const & p_irradiance
		, SamplerCube const & p_prefiltered
		, Sampler2D const & p_brdf
		, Int const & p_invertY )
	{
		return m_computeMetallicIBL( p_normal
			, p_position
			, p_albedo
			, p_metalness
			, p_roughness
			, p_worldEye 
			, p_irradiance
			, p_prefiltered
			, p_brdf
			, p_invertY );
	}

	Vec3 Utils::ComputeSpecularIBL( Vec3 const & p_normal
		, Vec3 const & p_position
		, Vec3 const & p_diffuse
		, Vec3 const & p_specular
		, Float const & p_glossiness
		, Vec3 const & p_worldEye
		, SamplerCube const & p_irradiance
		, SamplerCube const & p_prefiltered
		, Sampler2D const & p_brdf
		, Int const & p_invertY )
	{
		return m_computeSpecularIBL( p_normal
			, p_position
			, p_diffuse
			, p_specular
			, p_glossiness
			, p_worldEye 
			, p_irradiance
			, p_prefiltered
			, p_brdf
			, p_invertY );
	}
}
