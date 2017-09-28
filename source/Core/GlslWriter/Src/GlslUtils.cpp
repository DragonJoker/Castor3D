#include "GlslUtils.hpp"

#include "GlslIntrinsics.hpp"

using namespace castor;

namespace glsl
{
	uint32_t const Utils::MaxIblReflectionLod = 4;

	Utils::Utils( GlslWriter & p_writer )
		: m_writer{ p_writer }
	{
	}

	void Utils::declareCalcTexCoord()
	{
		m_calcTexCoord = m_writer.implementFunction< Vec2 >( cuT( "calcTexCoord" )
			, [&]()
			{
				auto gl_FragCoord = m_writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );
				auto c3d_renderSize = m_writer.getBuiltin< Vec2 >( cuT( "c3d_renderSize" ) );
				m_writer.returnStmt( gl_FragCoord.xy() / c3d_renderSize );
			} );
	}

	void Utils::declareCalcVSPosition()
	{
		m_calcVSPosition = m_writer.implementFunction< Vec3 >( cuT( "calcVSPosition" )
			, [&]( Vec2 const & uv
				, Float const & depth
				, Mat4 const & invProj )
			{
				auto csPosition = m_writer.declLocale( cuT( "psPosition" )
					, vec3( uv * 2.0f - 1.0f, depth * 2.0 - 1.0 ) );
				auto vsPosition = m_writer.declLocale( cuT( "vsPosition" )
					, invProj * vec4( csPosition, 1.0 ) );
				vsPosition.xyz() /= vsPosition.w();
				m_writer.returnStmt( vsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InMat4{ &m_writer, cuT( "invProj" ) } );
	}

	void Utils::declareCalcWSPosition()
	{
		m_calcWSPosition = m_writer.implementFunction< Vec3 >( cuT( "calcWSPosition" )
			, [&]( Vec2 const & uv
				, Float const & depth
				, Mat4 const & invViewProj )
			{
				auto csPosition = m_writer.declLocale( cuT( "psPosition" )
					, vec3( uv * 2.0f - 1.0f, depth * 2.0 - 1.0 ) );
				auto wsPosition = m_writer.declLocale( cuT( "wsPosition" )
					, invViewProj * vec4( csPosition, 1.0 ) );
				wsPosition.xyz() /= wsPosition.w();
				m_writer.returnStmt( wsPosition.xyz() );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InMat4{ &m_writer, cuT( "invViewProj" ) } );
	}

	void Utils::declareApplyGamma()
	{
		m_applyGamma = m_writer.implementFunction< Vec3 >( cuT( "applyGamma" )
			, [&]( Float const & gamma
				, Vec3 const & hdr )
			{
				m_writer.returnStmt( pow( hdr, vec3( 1.0_f / gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "gamma" ) }
			, InVec3{ &m_writer, cuT( "hdr" ) } );
	}

	void Utils::declareRemoveGamma()
	{
		m_removeGamma = m_writer.implementFunction< Vec3 >( cuT( "removeGamma" )
			, [&]( Float const & gamma
				, Vec3 const & srgb )
			{
				m_writer.returnStmt( pow( srgb, vec3( gamma ) ) );
			}
			, InFloat{ &m_writer, cuT( "gamma" ) }
			, InVec3{ &m_writer, cuT( "srgb" ) } );
	}

	void Utils::declareLineariseDepth()
	{
		m_lineariseDepth = m_writer.implementFunction< Float >( cuT( "lineariseDepth" )
			, [&]( Float const & depth
				, Float const & nearPlane
				, Float const & farPlane )
			{
				auto z = m_writer.declLocale( cuT( "z" )
					, depth * 2.0_f - 1.0_f );
				z *= m_writer.paren( farPlane - nearPlane );
				m_writer.returnStmt( 2.0 * nearPlane / m_writer.paren( farPlane + nearPlane - z ) );
			}
			, InFloat{ &m_writer, cuT( "depth" ) }
			, InFloat{ &m_writer, cuT( "nearPlane" ) }
			, InFloat{ &m_writer, cuT( "farPlane" ) } );
	}

	void Utils::declareGetMapNormal()
	{
		m_getMapNormal = m_writer.implementFunction< Vec3 >( cuT( "getMapNormal" )
			, [&]( Vec2 const & uv
				, Vec3 const & normal
				, Vec3 const & position )
			{
				auto c3d_mapNormal( m_writer.getBuiltin< Sampler2D >( cuT( "c3d_mapNormal" ) ) );

				auto mapNormal = m_writer.declLocale( cuT( "mapNormal" )
					, texture( c3d_mapNormal, uv.xy() ).xyz() );
				mapNormal = mapNormal * 2.0_f - vec3( 1.0_f, 1.0, 1.0 );
				auto Q1 = m_writer.declLocale( cuT( "Q1" )
					, dFdx( position ) );
				auto Q2 = m_writer.declLocale( cuT( "Q2" )
					, dFdy( position ) );
				auto st1 = m_writer.declLocale( cuT( "st1" )
					, dFdx( uv ) );
				auto st2 = m_writer.declLocale( cuT( "st2" )
					, dFdy( uv ) );
				auto N = m_writer.declLocale( cuT( "N" )
					, normalize( normal ) );
				auto T = m_writer.declLocale( cuT( "T" )
					, normalize( Q1 * st2.t() - Q2 * st1.t() ) );
				auto B = m_writer.declLocale( cuT( "B" )
					, -normalize( cross( N, T ) ) );
				auto tbn = m_writer.declLocale( cuT( "tbn" )
					, mat3( T, B, N ) );
				m_writer.returnStmt( normalize( tbn * mapNormal ) );
			}
			, InVec2{ &m_writer, cuT( "uv" ) }
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) } );
	}

	void Utils::declareFresnelSchlick()
	{
		m_fresnelSchlick = m_writer.implementFunction< Vec3 >( cuT( "fresnelSchlick" )
			, [&]( Float const & product
				, Vec3 const & f0
				, Float const & roughness )
			{
				m_writer.returnStmt( f0 + m_writer.paren( max( vec3( 1.0_f - roughness ), f0 ) - f0 ) * pow( 1.0_f - product, 5.0 ) );
			}
			, InFloat{ &m_writer, cuT( "product" ) }
			, InVec3{ &m_writer, cuT( "f0" ) }
			, InFloat{ &m_writer, cuT( "roughness" ) } );
	}

	void Utils::declareComputeMetallicIBL()
	{
		m_computeMetallicIBL = m_writer.implementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & normal
				, Vec3 const & position
				, Vec3 const & albedo
				, Float const & metallic
				, Float const & roughness
				, Vec3 const & worldEye
				, SamplerCube const & irradianceMap
				, SamplerCube const & prefilteredEnvMap
				, Sampler2D const & brdfMap )
			{
				auto V = m_writer.declLocale( cuT( "V" )
					, normalize( worldEye - position ) );
				auto NdotV = m_writer.declLocale( cuT( "NdotV" )
					, max( dot( normal, V ), 0.0 ) );
				auto f0 = m_writer.declLocale( cuT( "f0" )
					, mix( vec3( 0.04_f ), albedo, metallic ) );
				auto F = m_writer.declLocale( cuT( "F" )
					, fresnelSchlick( NdotV, f0, roughness ) );
				auto kS = m_writer.declLocale( cuT( "kS" )
					, F );
				auto kD = m_writer.declLocale( cuT( "kD" )
					, vec3( 1.0_f ) - kS );
				kD *= 1.0 - metallic;
				auto irradiance = m_writer.declLocale( cuT( "irradiance" )
					, texture( irradianceMap, normal ).rgb() );
				auto diffuseReflection = m_writer.declLocale( cuT( "diffuseReflection" )
					, irradiance * albedo );

				auto R = m_writer.declLocale( cuT( "R" )
					, reflect( -V, normal ) );
				R.y() = -R.y();
				auto prefilteredColor = m_writer.declLocale( cuT( "prefilteredColor" )
					, texture( prefilteredEnvMap, R, roughness * MaxIblReflectionLod ).rgb() );
				auto envBRDF = m_writer.declLocale( cuT( "envBRDF" )
					, texture( brdfMap, vec2( NdotV, roughness ) ).rg() );
				auto specularReflection = m_writer.declLocale( cuT( "specularReflection" )
					, prefilteredColor * m_writer.paren( kS * envBRDF.x() + envBRDF.y() ) );

				m_writer.returnStmt( kD * diffuseReflection + specularReflection );
			}
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) }
			, InVec3{ &m_writer, cuT( "albedo" ) }
			, InFloat{ &m_writer, cuT( "metallic" ) }
			, InFloat{ &m_writer, cuT( "roughness" ) }
			, InVec3{ &m_writer, cuT( "worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "irradianceMap" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "prefilteredEnvMap" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "brdfMap" ) } );
	}

	void Utils::declareComputeSpecularIBL()
	{
		m_computeSpecularIBL = m_writer.implementFunction< Vec3 >( cuT( "ComputeIBL" )
			, [&]( Vec3 const & normal
				, Vec3 const & position
				, Vec3 const & diffuse
				, Vec3 const & specular
				, Float const & glossiness
				, Vec3 const & worldEye
				, SamplerCube const & irradianceMap
				, SamplerCube const & prefilteredEnvMap
				, Sampler2D const & brdfMap )
			{
				auto roughness = m_writer.declLocale( cuT( "roughness" )
					, 1.0_f - glossiness );
				auto V = m_writer.declLocale( cuT( "V" )
					, normalize( worldEye - position ) );
				auto NdotV = m_writer.declLocale( cuT( "NdotV" )
					, max( dot( normal, V ), 0.0 ) );
				auto f0 = m_writer.declLocale( cuT( "f0" )
					, specular );
				auto F = m_writer.declLocale( cuT( "F" )
					, fresnelSchlick( NdotV, f0, roughness ) );
				auto kS = m_writer.declLocale( cuT( "kS" )
					, F );
				auto kD = m_writer.declLocale( cuT( "kD" )
					, vec3( 1.0_f ) - kS );
				kD *= 1.0 - length( specular );
				auto irradiance = m_writer.declLocale( cuT( "irradiance" )
					, texture( irradianceMap, normal ).rgb() );
				auto diffuseReflection = m_writer.declLocale( cuT( "diffuseReflection" )
					, irradiance * diffuse );

				auto R = m_writer.declLocale( cuT( "R" )
					, reflect( -V, normal ) );
				R.y() = -R.y();
				auto prefilteredColor = m_writer.declLocale( cuT( "prefilteredColor" )
					, texture( prefilteredEnvMap, R, roughness * MaxIblReflectionLod ).rgb() );
				auto envBRDF = m_writer.declLocale( cuT( "envBRDF" )
					, texture( brdfMap, vec2( NdotV, roughness ) ).rg() );
				auto specularReflection = m_writer.declLocale( cuT( "specularReflection" )
					, prefilteredColor * m_writer.paren( kS * envBRDF.x() + envBRDF.y() ) );

				m_writer.returnStmt( kD * diffuseReflection + specularReflection );
			}
			, InVec3{ &m_writer, cuT( "normal" ) }
			, InVec3{ &m_writer, cuT( "position" ) }
			, InVec3{ &m_writer, cuT( "diffuse" ) }
			, InVec3{ &m_writer, cuT( "specular" ) }
			, InFloat{ &m_writer, cuT( "glossiness" ) }
			, InVec3{ &m_writer, cuT( "worldEye" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "irradianceMap" ) }
			, InParam< SamplerCube >{ &m_writer, cuT( "prefilteredEnvMap" ) }
			, InParam< Sampler2D >{ &m_writer, cuT( "brdfMap" ) } );
	}

	Vec2 Utils::calcTexCoord()
	{
		return m_calcTexCoord();
	}

	Vec3 Utils::calcVSPosition( Vec2 const & uv
		, Float const & depth
		, Mat4 const & invProj )
	{
		return m_calcVSPosition( uv
			, depth
			, invProj );
	}

	Vec3 Utils::calcWSPosition( Vec2 const & uv
		, Float const & depth
		, Mat4 const & invViewProj )
	{
		return m_calcWSPosition( uv
			, depth
			, invViewProj );
	}

	Vec3 Utils::applyGamma( Float const & gamma
		, Vec3 const & hdr )
	{
		return m_applyGamma( gamma, hdr );
	}

	Vec3 Utils::removeGamma( Float const & gamma
		, Vec3 const & srgb )
	{
		return m_removeGamma( gamma, srgb );
	}

	Vec3 Utils::getMapNormal( Vec2 const & uv
		, Vec3 const & normal
		, Vec3 const & position )
	{
		return writeFunctionCall< Vec3 >( &m_writer
			, cuT( "getMapNormal" )
			, uv
			, normal
			, position );
	}

	Float Utils::lineariseDepth( Float const & depth
		, Float const & nearPlane
		, Float const & farPlane )
	{
		return m_lineariseDepth( depth
			, nearPlane
			, farPlane );
	}

	Vec3 Utils::fresnelSchlick( Float const & product
		, Vec3 const & f0
		, Float const & roughness )
	{
		return m_fresnelSchlick( product
			, f0
			, roughness );
	}

	Vec3 Utils::computeMetallicIBL( Vec3 const & normal
		, Vec3 const & position
		, Vec3 const & albedo
		, Float const & metallic
		, Float const & roughness
		, Vec3 const & worldEye
		, SamplerCube const & irradianceMap
		, SamplerCube const & prefilteredEnvMap
		, Sampler2D const & brdfMap )
	{
		return m_computeMetallicIBL( normal
			, position
			, albedo
			, metallic
			, roughness
			, worldEye 
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap );
	}

	Vec3 Utils::computeSpecularIBL( Vec3 const & normal
		, Vec3 const & position
		, Vec3 const & diffuse
		, Vec3 const & specular
		, Float const & glossiness
		, Vec3 const & worldEye
		, SamplerCube const & irradianceMap
		, SamplerCube const & prefilteredEnvMap
		, Sampler2D const & brdfMap )
	{
		return m_computeSpecularIBL( normal
			, position
			, diffuse
			, specular
			, glossiness
			, worldEye 
			, irradianceMap
			, prefilteredEnvMap
			, brdfMap );
	}
}
