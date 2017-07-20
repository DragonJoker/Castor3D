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
			, [&]( Vec2 const & uv
				, Mat4 const & invProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto depth = m_writer.DeclLocale( cuT( "texCoord" )
					, texture( c3d_mapDepth, uv, 0.0_f ).x() );
				auto csPosition = m_writer.DeclLocale( cuT( "psPosition" )
					, vec3( uv * 2.0f - 1.0f, depth * 2.0 - 1.0 ) );
				auto vsPosition = m_writer.DeclLocale( cuT( "vsPosition" )
					, invProj * vec4( csPosition, 1.0 ) );
				vsPosition.xyz() /= vsPosition.w();
				m_writer.Return( vsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InMat4{ &m_writer, cuT( "invProj" ) } );
	}

	void Utils::DeclareCalcVSDepth()
	{
		m_calcVSDepth = m_writer.ImplementFunction< Float >( cuT( "CalcVSDepth" )
			, [&]( Vec2 const & uv
				, Mat4 const & projection )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto depth = m_writer.DeclLocale( cuT( "depth" )
					, texture( c3d_mapDepth, uv ).r() );
				m_writer.Return( projection[3][2] / m_writer.Paren( depth * 2.0_f - 1.0_f - projection[2][2] ) );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InMat4{ &m_writer, cuT( "projection" ) } );
	}

	void Utils::DeclareCalcWSPosition()
	{
		m_calcWSPosition = m_writer.ImplementFunction< Vec3 >( cuT( "CalcWSPosition" )
			, [&]( Vec2 const & uv
				, Mat4 const & invViewProj )
			{
				auto c3d_mapDepth = m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapDepth" ) );
				auto depth = m_writer.DeclLocale( cuT( "depth" )
					, texture( c3d_mapDepth, uv, 0.0_f ).x() );
				auto csPosition = m_writer.DeclLocale( cuT( "psPosition" )
					, vec3( uv * 2.0f - 1.0f, depth * 2.0 - 1.0 ) );
				auto wsPosition = m_writer.DeclLocale( cuT( "wsPosition" )
					, invViewProj * vec4( csPosition, 1.0 ) );
				wsPosition.xyz() /= wsPosition.w();
				m_writer.Return( wsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InMat4{ &m_writer, cuT( "invViewProj" ) } );
	}

	void Utils::DeclareApplyGamma()
	{
		m_applyGamma = m_writer.ImplementFunction< Vec3 >( cuT( "ApplyGamma" )
			, [&]( Float const & gamma
				, Vec3 const & hdr )
			{
				m_writer.Return( pow( hdr, vec3( 1.0_f / gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "gamma" ) }
			, InVec3{ &m_writer, cuT( "hdr" ) } );
	}

	void Utils::DeclareRemoveGamma()
	{
		m_removeGamma = m_writer.ImplementFunction< Vec3 >( cuT( "RemoveGamma" )
			, [&]( Float const & gamma
				, Vec3 const & srgb )
			{
				m_writer.Return( pow( srgb, vec3( gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "gamma" ) }
			, InVec3{ &m_writer, cuT( "srgb" ) } );
	}

	void Utils::DeclareLineariseDepth()
	{
		m_lineariseDepth = m_writer.ImplementFunction< Float >( cuT( "LineariseDepth" )
			, [&]( Float const & depth
				, Mat4 const & invProj )
			{
				auto c3d_fCameraNearPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraNearPlane" ) );
				auto c3d_fCameraFarPlane = m_writer.GetBuiltin< Float >( cuT( "c3d_fCameraFarPlane" ) );
				auto z = m_writer.DeclLocale( cuT( "z" )
					, depth *2.0_f - 1.0_f );
				auto unprojected = m_writer.DeclLocale( cuT( "unprojected" )
					, invProj * vec4( 0.0_f, 0.0_f, z, 1.0_f ) );
				z = unprojected.z() / unprojected.w();
				m_writer.Return( m_writer.Paren( z - c3d_fCameraNearPlane )
					/ m_writer.Paren( c3d_fCameraFarPlane - c3d_fCameraNearPlane ) );

			}
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InMat4{ &m_writer, cuT( "invProj" ) } );
	}

	void Utils::DeclareGetMapNormal()
	{
		m_getMapNormal = m_writer.ImplementFunction< Vec3 >( cuT( "GetMapNormal" )
			, [&]( Vec2 const & uv
				, Vec3 const & normal
				, Vec3 const & position )
			{
				auto c3d_mapNormal( m_writer.GetBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

				auto mapNormal = m_writer.DeclLocale( cuT( "mapNormal" )
					, texture( c3d_mapNormal, uv.xy() ).xyz() );
				mapNormal = mapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 );
				auto Q1 = m_writer.DeclLocale( cuT( "Q1" )
					, dFdx( position ) );
				auto Q2 = m_writer.DeclLocale( cuT( "Q2" )
					, dFdy( position ) );
				auto st1 = m_writer.DeclLocale( cuT( "st1" )
					, dFdx( uv ) );
				auto st2 = m_writer.DeclLocale( cuT( "st2" )
					, dFdy( uv ) );
				auto N = m_writer.DeclLocale( cuT( "N" )
					, normalize( normal ) );
				auto T = m_writer.DeclLocale( cuT( "T" )
					, normalize( Q1 * st2.t() - Q2 * st1.t() ) );
				auto B = m_writer.DeclLocale( cuT( "B" )
					, -normalize( cross( N, T ) ) );
				auto tbn = m_writer.DeclLocale( cuT( "tbn" )
					, mat3( T, B, N ) );
				m_writer.Return( normalize( tbn * mapNormal ) );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) } );
	}

	void Utils::DeclareFresnelSchlick()
	{
		m_fresnelSchlick = m_writer.ImplementFunction< Vec3 >( cuT( "FresnelSchlick" )
			, [&]( Float const & product
				, Vec3 const & f0
				, Float const & roughness )
			{
				m_writer.Return( f0 + m_writer.Paren( max( vec3( 1.0_f - roughness ), f0 ) - f0 ) * pow( 1.0_f - product, 5.0 ) );
			}
			, InFloat{ &m_writer, cuT( "product" ) }
			, InVec3{ &m_writer, cuT( "f0" ) }
			, InFloat{ &m_writer, cuT( "roughness" ) } );
	}

	void Utils::DeclareComputeMetallicIBL()
	{
		m_computeMetallicIBL = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & normal
				, Vec3 const & position
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Vec3 const & worldEye
				, SamplerCube const & irradianceMap
				, SamplerCube const & prefilteredEnvMap
				, Sampler2D const & brdfMap
				, Int const & invertY )
			{
				auto V = m_writer.DeclLocale( cuT( "V" )
					, normalize( worldEye - position ) );
				auto NdotV = m_writer.DeclLocale( cuT( "NdotV" )
					, max( dot( normal, V ), 0.0 ) );
				auto f0 = m_writer.DeclLocale( cuT( "f0" )
					, mix( vec3( 0.04_f ), albedo, metallic ) );
				auto F = m_writer.DeclLocale( cuT( "F" )
					, FresnelSchlick( NdotV, f0, roughness ) );
				auto kS = m_writer.DeclLocale( cuT( "kS" )
					, F );
				auto kD = m_writer.DeclLocale( cuT( "kD" )
					, vec3( 1.0_f ) - kS );
				kD *= 1.0 - metallic;
				auto irradiance = m_writer.DeclLocale( cuT( "irradiance" )
					, texture( irradianceMap, normal ).rgb() );
				auto diffuseReflection = m_writer.DeclLocale( cuT( "diffuseReflection" )
					, irradiance * albedo );

				auto R = m_writer.DeclLocale( cuT( "R" )
					, reflect( -V, normal ) );
				R.y() = m_writer.Ternary( invertY != 0_i, R.y(), -R.y() );
				auto prefilteredColor = m_writer.DeclLocale( cuT( "prefilteredColor" )
					, texture( prefilteredEnvMap, R, roughness * MaxIblReflectionLod ).rgb() );
				auto envBRDF = m_writer.DeclLocale( cuT( "envBRDF" )
					, texture( brdfMap, vec2( NdotV, roughness ) ).rg() );
				auto specularReflection = m_writer.DeclLocale( cuT( "specularReflection" )
					, prefilteredColor * m_writer.Paren( kS * envBRDF.x() + envBRDF.y() ) );

				m_writer.Return( kD * diffuseReflection + specularReflection );
			}
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) }
			, InVec3{ &m_writer, cuT( "albedo" ) }
			, InFloat{ &m_writer, cuT( "metallic" ) }
			, InFloat{ &m_writer, cuT( "roughness" ) }
			, InVec3{ &m_writer, cuT( "worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "irradianceMap" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "prefilteredEnvMap" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "brdfMap" ) }
			, InInt{ &m_writer, cuT( "invertY" ) } );
	}

	void Utils::DeclareComputeSpecularIBL()
	{
		m_computeSpecularIBL = m_writer.ImplementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & normal
				, Vec3 const & position
				, Vec3 const & diffuse
				, Vec3 const & specular
				, Float const & glossiness
				, Vec3 const & worldEye
				, SamplerCube const & irradianceMap
				, SamplerCube const & prefilteredEnvMap
				, Sampler2D const & brdfMap
				, Int const & invertY )
			{
				auto roughness = m_writer.DeclLocale( cuT( "roughness" )
					, 1.0_f - glossiness );
				auto V = m_writer.DeclLocale( cuT( "V" )
					, normalize( worldEye - position ) );
				auto NdotV = m_writer.DeclLocale( cuT( "NdotV" )
					, max( dot( normal, V ), 0.0 ) );
				auto f0 = m_writer.DeclLocale( cuT( "f0" )
					, specular );
				auto F = m_writer.DeclLocale( cuT( "F" )
					, FresnelSchlick( NdotV, f0, roughness ) );
				auto kS = m_writer.DeclLocale( cuT( "kS" )
					, F );
				auto kD = m_writer.DeclLocale( cuT( "kD" )
					, vec3( 1.0_f ) - kS );
				kD *= 1.0 - length( specular );
				auto irradiance = m_writer.DeclLocale( cuT( "irradiance" )
					, texture( irradianceMap, normal ).rgb() );
				auto diffuseReflection = m_writer.DeclLocale( cuT( "diffuseReflection" )
					, irradiance * diffuse );

				auto R = m_writer.DeclLocale( cuT( "R" )
					, reflect( -V, normal ) );
				R.y() = m_writer.Ternary( invertY != 0_i, R.y(), -R.y() );
				auto prefilteredColor = m_writer.DeclLocale( cuT( "prefilteredColor" )
					, texture( prefilteredEnvMap, R, roughness * MaxIblReflectionLod ).rgb() );
				auto envBRDF = m_writer.DeclLocale( cuT( "envBRDF" )
					, texture( brdfMap, vec2( NdotV, roughness ) ).rg() );
				auto specularReflection = m_writer.DeclLocale( cuT( "specularReflection" )
					, prefilteredColor * m_writer.Paren( kS * envBRDF.x() + envBRDF.y() ) );

				m_writer.Return( kD * diffuseReflection + specularReflection );
			}
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) }
			, InVec3{ &m_writer, cuT( "diffuse" ) }
			, InVec3{ &m_writer, cuT( "specular" ) }
			, InFloat{ &m_writer, cuT( "glossiness" ) }
			, InVec3{ &m_writer, cuT( "worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "irradianceMap" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "prefilteredEnvMap" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "brdfMap" ) }
			, InInt{ &m_writer, cuT( "invertY" ) } );
	}

	Vec2 Utils::CalcTexCoord()
	{
		return m_calcTexCoord();
	}

	Vec3 Utils::CalcVSPosition( Vec2 const & uv
		, Mat4 const & invProj )
	{
		return m_calcVSPosition( uv, invProj );
	}

	Float Utils::CalcVSDepth( Vec2 const & uv
		, Mat4 const & projection )
	{
		return m_calcVSDepth( uv, projection );
	}

	Vec3 Utils::CalcWSPosition( Vec2 const & uv
		, Mat4 const & invViewProj )
	{
		return m_calcWSPosition( uv, invViewProj );
	}

	Vec3 Utils::ApplyGamma( Float const & gamma
		, Vec3 const & hdr )
	{
		return m_applyGamma( gamma, hdr );
	}

	Vec3 Utils::RemoveGamma( Float const & gamma
		, Vec3 const & srgb )
	{
		return m_removeGamma( gamma, srgb );
	}

	Vec3 Utils::GetMapNormal( Vec2 const & uv
		, Vec3 const & normal
		, Vec3 const & position )
	{
		return WriteFunctionCall< Vec3 >( &m_writer
			, cuT( "GetMapNormal" )
			, uv
			, normal
			, position );
	}

	Float Utils::LineariseDepth( Float const & depth, Mat4 const & invProj )
	{
		return m_lineariseDepth( depth, invProj );
	}

	Vec3 Utils::FresnelSchlick( Float const & product
		, Vec3 const & f0
		, Float const & roughness )
	{
		return m_fresnelSchlick( product, f0, roughness );
	}

	Vec3 Utils::ComputeMetallicIBL( Vec3 const & normal
		, Vec3 const & position
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Vec3 const & worldEye
		, SamplerCube const & irradianceMap
		, SamplerCube const & prefilteredEnvMap
		, Sampler2D const & brdfMap
		, Int const & invertY )
	{
		return m_computeMetallicIBL( normal
			, position
			, albedo
			, metallic
			, roughness
			, worldEye 
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap
			, invertY );
	}

	Vec3 Utils::ComputeSpecularIBL( Vec3 const & normal
		, Vec3 const & position
		, Vec3 const & diffuse
		, Vec3 const & specular
		, Float const & glossiness
		, Vec3 const & worldEye
		, SamplerCube const & irradianceMap
		, SamplerCube const & prefilteredEnvMap
		, Sampler2D const & brdfMap
		, Int const & invertY )
	{
		return m_computeSpecularIBL( normal
			, position
			, diffuse
			, specular
			, glossiness
			, worldEye 
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap
			, invertY );
	}
}
