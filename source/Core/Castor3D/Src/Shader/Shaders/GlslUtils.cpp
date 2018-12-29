#include "GlslUtils.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		uint32_t constexpr ReceiverMask = 0x00000080u;
		uint32_t constexpr RefractionMask = 0x00000040u;
		uint32_t constexpr ReflectionMask = 0x00000020u;
		uint32_t constexpr EnvMapIndexMask = 0x0000001Fu;

		uint32_t constexpr ReceiverOffset = 7u;
		uint32_t constexpr RefractionOffset = 6u;
		uint32_t constexpr ReflectionOffset = 5u;
		uint32_t constexpr EnvMapIndexOffset = 0u;

		uint32_t const Utils::MaxIblReflectionLod = 4;

		Utils::Utils( sdw::ShaderWriter & writer
			, bool isTopDown
			, bool isZeroToOneDepth )
			: m_writer{ writer }
			, m_isTopDown{ isTopDown }
			, m_isZeroToOneDepth{ isZeroToOneDepth }
		{
		}

		void Utils::declareCalcTexCoord()
		{
			m_calcTexCoord = m_writer.implementFunction< Vec2 >( cuT( "calcTexCoord" )
				, [&]( Vec2 const & renderPos
					, Vec2 const & renderSize )
				{
					m_writer.returnStmt( renderPos / renderSize );
				}
				, InVec2{ m_writer, cuT( "renderPos" ) }
				, InVec2{ m_writer, cuT( "renderSize" ) } );
		}

		void Utils::declareCalcVSPosition()
		{
			m_calcVSPosition = m_writer.implementFunction< Vec3 >( cuT( "calcVSPosition" )
				, [&]( Vec2 const & uv
					, Float const & depth
					, Mat4 const & invProj )
				{
					auto csPosition = m_writer.declLocale( cuT( "csPosition" )
						, vec3( uv * 2.0_f - 1.0_f
							, ( m_isZeroToOneDepth
								? depth
								: depth * 2.0_f - 1.0_f ) ) );
					auto vsPosition = m_writer.declLocale( cuT( "vsPosition" )
						, invProj * vec4( csPosition, 1.0 ) );
					vsPosition.xyz() /= vsPosition.w();
					m_writer.returnStmt( vsPosition.xyz() );
				}
				, InVec2{ m_writer, cuT( "uv" ) }
				, InFloat{ m_writer, cuT( "depth" ) }
				, InMat4{ m_writer, cuT( "invProj" ) } );
		}

		void Utils::declareCalcWSPosition()
		{
			m_calcWSPosition = m_writer.implementFunction< Vec3 >( cuT( "calcWSPosition" )
				, [&]( Vec2 const & uv
					, Float const & depth
					, Mat4 const & invViewProj )
				{
					auto csPosition = m_writer.declLocale( cuT( "psPosition" )
						, vec3( uv * 2.0_f - 1.0_f
							, ( m_isZeroToOneDepth
								? depth
								: depth * 2.0_f - 1.0_f ) ) );
					auto wsPosition = m_writer.declLocale( cuT( "wsPosition" )
						, invViewProj * vec4( csPosition, 1.0_f ) );
					wsPosition.xyz() /= wsPosition.w();
					m_writer.returnStmt( wsPosition.xyz() );
				}
				, InVec2{ m_writer, cuT( "uv" ) }
				, InFloat{ m_writer, cuT( "depth" ) }
				, InMat4{ m_writer, cuT( "invViewProj" ) } );
		}

		void Utils::declareApplyGamma()
		{
			m_applyGamma = m_writer.implementFunction< Vec3 >( cuT( "applyGamma" )
				, [&]( Float const & gamma
					, Vec3 const & hdr )
				{
					m_writer.returnStmt( pow( hdr, vec3( 1.0_f / gamma ) ) );
				}
				, InFloat{ m_writer, cuT( "gamma" ) }
				, InVec3{ m_writer, cuT( "hdr" ) } );
		}

		void Utils::declareRemoveGamma()
		{
			m_removeGamma = m_writer.implementFunction< Vec3 >( cuT( "removeGamma" )
				, [&]( Float const & gamma
					, Vec3 const & srgb )
				{
					m_writer.returnStmt( pow( srgb, vec3( gamma ) ) );
				}
				, InFloat{ m_writer, cuT( "gamma" ) }
				, InVec3{ m_writer, cuT( "srgb" ) } );
		}

		void Utils::declareLineariseDepth()
		{
			m_lineariseDepth = m_writer.implementFunction< Float >( cuT( "lineariseDepth" )
				, [&]( Float const & depth
					, Float const & nearPlane
					, Float const & farPlane )
				{
					auto z = m_writer.declLocale( cuT( "z" )
						, ( m_isZeroToOneDepth
							? depth
							: depth * 2.0_f - 1.0_f ) );
					z *= m_writer.paren( farPlane - nearPlane );
					m_writer.returnStmt( 2.0 * farPlane * nearPlane / m_writer.paren( farPlane + nearPlane - z ) );
				}
				, InFloat{ m_writer, cuT( "depth" ) }
				, InFloat{ m_writer, cuT( "nearPlane" ) }
				, InFloat{ m_writer, cuT( "farPlane" ) } );
		}

		void Utils::declareGetMapNormal()
		{
			m_getMapNormal = m_writer.implementFunction< Vec3 >( cuT( "getMapNormal" )
				, [&]( Vec2 const & uv
					, Vec3 const & normal
					, Vec3 const & position )
				{
					auto c3d_mapNormal( m_writer.getVariable< SampledImage2DRgba32 >( cuT( "c3d_mapNormal" ) ) );

					auto mapNormal = m_writer.declLocale( cuT( "mapNormal" )
						, texture( c3d_mapNormal, uv.xy() ).xyz() );
					mapNormal = sdw::fma( mapNormal
						, vec3( 2.0_f )
						, vec3( -1.0_f ) );
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
				, InVec2{ m_writer, cuT( "uv" ) }
				, InVec3{ m_writer, cuT( "normal" ) }
				, InVec3{ m_writer, cuT( "position" ) } );
		}

		void Utils::declareComputeAccumulation()
		{
			m_computeAccumulation = m_writer.implementFunction< Vec4 >( cuT( "computeAccumulation" )
				, [&]( Float const & depth
					, Vec3 const & colour
					, Float const & alpha
					, Float const & nearPlane
					, Float const & farPlane )
				{
					//// Naive
					//auto z = utils.lineariseDepth( z
					//	, nearPlane
					//	, farPlane );
					//auto weight = m_writer.declLocale( cuT( "weight" ), 1.0_f - z );

					// (10)
					auto z = lineariseDepth( depth
						, nearPlane
						, farPlane );
					auto weight = m_writer.declLocale( cuT( "weight" )
						, max( pow( clamp( 1.0_f - depth, 0.0_f, 1.0_f ), 3.0_f ) * 3e3_f, 1e-2_f ) );

					//// (9)
					//auto weight = m_writer.declLocale( cuT( "weight" )
					//	, max( min( 0.03_f / writer.paren( pow( glsl::abs( z ) / 200.0_f, 4.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

					//// (8)
					//auto weight = m_writer.declLocale( cuT( "weight" )
					//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( z ) / 200.0_f, 6.0_f ) + pow( glsl::abs( z ) / 10.0_f, 3.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

					//// (7)
					//auto weight = m_writer.declLocale( cuT( "weight" )
					//	, max( min( 10.0_f / writer.paren( pow( glsl::abs( z ) / 200.0_f, 6.0_f ) + pow( glsl::abs( z ) / 5.0_f, 2.0_f ) + 1e-5 ), 3e3 ), 1e-2 ) );

					//// (other)
					//auto a = m_writer.declLocale( cuT( "a" )
					//	, min( alpha, 1.0 ) * 8.0 + 0.01 );
					//auto b = m_writer.declLocale( cuT( "b" )
					//	, -z * 0.95 + 1.0 );
					///* If your scene has a lot of content very close to the far plane,
					//then include this line (one rsqrt instruction):
					//b /= sqrt(1e4 * abs(csZ)); */
					//auto weight = m_writer.declLocale( cuT( "weight" )
					//	, clamp( a * a * a * 1e8 * b * b * b, 1e-2, 3e2 ) );

					m_writer.returnStmt( vec4( colour * alpha, alpha ) * weight );
				}
				, InFloat{ m_writer, cuT( "depth" ) }
				, InVec3{ m_writer, cuT( "colour" ) }
				, InFloat{ m_writer, cuT( "alpha" ) }
				, InFloat{ m_writer, cuT( "nearPlane" ) }
				, InFloat{ m_writer, cuT( "farPlane" ) } );
		}

		void Utils::declareFresnelSchlick()
		{
			m_fresnelSchlick = m_writer.implementFunction< Vec3 >( cuT( "fresnelSchlick" )
				, [&]( Float const & product
					, Vec3 const & f0
					, Float const & roughness )
				{
					m_writer.returnStmt( sdw::fma( max( vec3( 1.0_f - roughness ), f0 ) - f0
						, vec3( pow( 1.0_f - product, 5.0_f ) )
						, f0 ) );
				}
				, InFloat{ m_writer, cuT( "product" ) }
				, InVec3{ m_writer, cuT( "f0" ) }
				, InFloat{ m_writer, cuT( "roughness" ) } );
		}

		void Utils::declareComputeIBL()
		{
			m_computeIBL = m_writer.implementFunction< Vec3 >( cuT( "computeIBL" )
				, [&]( Vec3 const & normal
					, Vec3 const & position
					, Vec3 const & baseColour
					, Vec3 const & f0
					, Float const & roughness
					, Float const & metallic
					, Vec3 const & worldEye
					, SampledImageCubeRgba32 const & irradianceMap
					, SampledImageCubeRgba32 const & prefilteredEnvMap
					, SampledImage2DRgba32 const & brdfMap )
				{
					auto V = m_writer.declLocale( cuT( "V" )
						, normalize( worldEye - position ) );
					auto NdotV = m_writer.declLocale( cuT( "NdotV" )
						, max( dot( normal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( cuT( "F" )
						, fresnelSchlick( NdotV, f0, roughness ) );
					auto kS = m_writer.declLocale( cuT( "kS" )
						, F );
					auto kD = m_writer.declLocale( cuT( "kD" )
						, vec3( 1.0_f ) - kS );
					kD *= 1.0 - metallic;
					auto irradiance = m_writer.declLocale( cuT( "irradiance" )
						, texture( irradianceMap, vec3( normal.x(), -normal.y(), normal.z() ) ).rgb() );
					auto diffuseReflection = m_writer.declLocale( cuT( "diffuseReflection" )
						, irradiance * baseColour );

					auto R = m_writer.declLocale( cuT( "R" )
						, reflect( -V, normal ) );
					R.y() = -R.y();
					auto prefilteredColor = m_writer.declLocale( cuT( "prefilteredColor" )
						, textureLod( prefilteredEnvMap, R, roughness * Float( float( MaxIblReflectionLod ) ) ).rgb() );
					auto envBRDFCoord = m_writer.declLocale( cuT( "envBRDFCoord" )
						, vec2( NdotV, roughness ) );
					auto envBRDF = m_writer.declLocale( cuT( "envBRDF" )
						, texture( brdfMap, envBRDFCoord ).rg() );
					auto specularReflection = m_writer.declLocale( cuT( "specularReflection" )
						, prefilteredColor * sdw::fma( kS
							, vec3( envBRDF.x() )
							, vec3( envBRDF.y() ) ) );

					m_writer.returnStmt( sdw::fma( kD
						, diffuseReflection
						, specularReflection ) );
				}
				, InVec3{ m_writer, cuT( "normal" ) }
				, InVec3{ m_writer, cuT( "position" ) }
				, InVec3{ m_writer, cuT( "albedo" ) }
				, InVec3{ m_writer, cuT( "f0" ) }
				, InFloat{ m_writer, cuT( "roughness" ) }
				, InFloat{ m_writer, cuT( "metallic" ) }
				, InVec3{ m_writer, cuT( "worldEye" ) }
				, InSampledImageCubeRgba32{ m_writer, cuT( "irradianceMap" ) }
				, InSampledImageCubeRgba32{ m_writer, cuT( "prefilteredEnvMap" ) }
				, InSampledImage2DRgba32{ m_writer, cuT( "brdfMap" ) } );
		}

		void Utils::declareInvertVec2Y()
		{
			m_invertVec2Y = m_writer.implementFunction< Vec2 >( "invertVec2Y"
				, [&]( Vec2 const & v )
				{
					m_writer.returnStmt( vec2( v.x(), 1.0_f - v.y() ) );
				}
				, InVec2{ m_writer, "v" } );
		}

		void Utils::declareInvertVec3Y()
		{
			m_invertVec3Y = m_writer.implementFunction< Vec3 >( "invertVec3Y"
				, [&]( Vec3 const & v )
				{
					m_writer.returnStmt( vec3( v.x(), 1.0_f - v.y(), v.z() ) );
				}
				, InVec3{ m_writer, "v" } );
		}

		void Utils::declareEncodeMaterial()
		{
			m_encodeMaterial = m_writer.implementFunction< sdw::Void >( cuT( "encodeMaterial" )
				, [&]( Int const & receiver
					, Int const & reflection
					, Int const & refraction
					, Int const & envMapIndex
					, Float encoded )
				{
					auto flags = m_writer.declLocale( cuT( "flags" )
						, 0_u
						+ m_writer.paren( m_writer.paren( m_writer.cast< UInt >( receiver ) << UInt( ReceiverOffset ) )		& UInt( ReceiverMask ) )
						+ m_writer.paren( m_writer.paren( m_writer.cast< UInt >( refraction ) << UInt( RefractionOffset ) )	& UInt( RefractionMask ) )
						+ m_writer.paren( m_writer.paren( m_writer.cast< UInt >( reflection ) << UInt( ReflectionOffset ) )	& UInt( ReflectionMask ) )
						+ m_writer.paren( m_writer.paren( m_writer.cast< UInt >( envMapIndex ) << UInt( EnvMapIndexOffset ) )	& UInt( EnvMapIndexMask ) ) );
					encoded = m_writer.cast< Float >( flags );
				}
				, InInt{ m_writer, cuT( "receiver" ) }
				, InInt{ m_writer, cuT( "reflection" ) }
				, InInt{ m_writer, cuT( "refraction" ) }
				, InInt{ m_writer, cuT( "envMapIndex" ) }
				, OutFloat{ m_writer, cuT( "encoded" ) } );
		}

		void Utils::declareDecodeMaterial()
		{
			m_decodeMaterial = m_writer.implementFunction< sdw::Void >( cuT( "decodeMaterial" )
				, [&]( Float const & encoded
					, Int receiver
					, Int reflection
					, Int refraction
					, Int envMapIndex )
				{
					auto flags = m_writer.declLocale( cuT( "flags" )
						, m_writer.cast< UInt >( encoded ) );
					receiver = m_writer.cast< Int >( m_writer.paren( flags & UInt( ReceiverMask ) ) >> UInt( ReceiverOffset ) );
					refraction = m_writer.cast< Int >( m_writer.paren( flags & UInt( RefractionMask ) ) >> UInt( RefractionOffset ) );
					reflection = m_writer.cast< Int >( m_writer.paren( flags & UInt( ReflectionMask ) ) >> UInt( ReflectionOffset ) );
					envMapIndex = m_writer.cast< Int >( m_writer.paren( flags & UInt( EnvMapIndexMask ) ) >> UInt( EnvMapIndexOffset ) );
				}
				, InFloat{ m_writer, cuT( "encoded" ) }
				, OutInt{ m_writer, cuT( "receiver" ) }
				, OutInt{ m_writer, cuT( "reflection" ) }
				, OutInt{ m_writer, cuT( "refraction" ) }
				, OutInt{ m_writer, cuT( "envMapIndex" ) } );
		}

		void Utils::declareDecodeReceiver()
		{
			m_decodeReceiver = m_writer.implementFunction< sdw::Void >( cuT( "decodeReceiver" )
				, [&]( Int const & encoded
					, Int receiver )
				{
					receiver = m_writer.paren( encoded & ReceiverMask ) >> ReceiverOffset;
				}
				, InInt{ m_writer, cuT( "encoded" ) }
				, OutInt{ m_writer, cuT( "receiver" ) } );
		}

		sdw::Vec2 Utils::bottomUpToTopDown( sdw::Vec2 const & texCoord )const
		{
			if ( !m_isTopDown )
			{
				return m_invertVec2Y( texCoord );
			}

			return texCoord;
		}

		sdw::Vec2 Utils::topDownToBottomUp( sdw::Vec2 const & texCoord )const
		{
			if ( m_isTopDown )
			{
				return m_invertVec2Y( texCoord );
			}

			return texCoord;
		}

		sdw::Vec3 Utils::bottomUpToTopDown( sdw::Vec3 const & texCoord )const
		{
			if ( !m_isTopDown )
			{
				return m_invertVec3Y( texCoord );
			}

			return texCoord;
		}

		sdw::Vec3 Utils::topDownToBottomUp( sdw::Vec3 const & texCoord )const
		{
			if ( m_isTopDown )
			{
				return m_invertVec3Y( texCoord );
			}

			return texCoord;
		}

		Vec2 Utils::calcTexCoord( Vec2 const & renderPos
				, Vec2 const & renderSize )const
		{
			return m_calcTexCoord( renderPos, renderSize );
		}

		Vec3 Utils::calcVSPosition( Vec2 const & uv
			, Float const & depth
			, Mat4 const & invProj )const
		{
			return m_calcVSPosition( uv
				, depth
				, invProj );
		}

		Vec3 Utils::calcWSPosition( Vec2 const & uv
			, Float const & depth
			, Mat4 const & invViewProj )const
		{
			return m_calcWSPosition( uv
				, depth
				, invViewProj );
		}

		Vec3 Utils::applyGamma( Float const & gamma
			, Vec3 const & hdr )const
		{
			return m_applyGamma( gamma, hdr );
		}

		Vec3 Utils::removeGamma( Float const & gamma
			, Vec3 const & srgb )const
		{
			return m_removeGamma( gamma, srgb );
		}

		Vec3 Utils::getMapNormal( Vec2 const & uv
			, Vec3 const & normal
			, Vec3 const & position )const
		{
			return m_getMapNormal( uv, normal, position );
		}

		Float Utils::lineariseDepth( Float const & depth
			, Float const & nearPlane
			, Float const & farPlane )const
		{
			return m_lineariseDepth( depth
				, nearPlane
				, farPlane );
		}

		Vec4 Utils::computeAccumulation( Float const & depth
			, Vec3 const & colour
			, Float const & alpha
			, Float const & nearPlane
			, Float const & farPlane )
		{
			return m_computeAccumulation( depth
				, colour
				, alpha
				, nearPlane
				, farPlane );
		}

		Vec3 Utils::fresnelSchlick( Float const & product
			, Vec3 const & f0
			, Float const & roughness )const
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
			, SampledImageCubeRgba32 const & irradianceMap
			, SampledImageCubeRgba32 const & prefilteredEnvMap
			, SampledImage2DRgba32 const & brdfMap )const
		{
			return m_computeIBL( normal
				, position
				, albedo
				, mix( vec3( 0.04_f ), albedo, vec3( metallic ) )
				, roughness
				, metallic
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
			, SampledImageCubeRgba32 const & irradianceMap
			, SampledImageCubeRgba32 const & prefilteredEnvMap
			, SampledImage2DRgba32 const & brdfMap )const
		{
			return m_computeIBL( normal
				, position
				, diffuse
				, specular
				, 1.0_f - glossiness
				, length( specular )
				, worldEye
				, irradianceMap
				, prefilteredEnvMap
				, brdfMap );
		}

		void Utils::encodeMaterial( Int const & receiver
			, Int const & reflection
			, Int const & refraction
			, Int const & envMapIndex
			, Float const & encoded )const
		{
			m_encodeMaterial( receiver
				, reflection
				, refraction
				, envMapIndex
				, encoded );
		}

		void Utils::decodeMaterial( Float const & encoded
			, Int const & receiver
			, Int const & reflection
			, Int const & refraction
			, Int const & envMapIndex )const
		{
			m_decodeMaterial( encoded
				, receiver
				, reflection
				, refraction
				, envMapIndex );
		}

		void Utils::decodeReceiver( Int & encoded
			, Int const & receiver )const
		{
			m_decodeReceiver( encoded
				, receiver );
		}
	}
}
