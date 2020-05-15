#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

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

		Utils::Utils( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void Utils::declareCalcTexCoord()
		{
			if ( m_calcTexCoord )
			{
				return;
			}

			m_calcTexCoord = m_writer.implementFunction< Vec2 >( "calcTexCoord"
				, [&]( Vec2 const & renderPos
					, Vec2 const & renderSize )
				{
					m_writer.returnStmt( renderPos / renderSize );
				}
				, InVec2{ m_writer, "renderPos" }
				, InVec2{ m_writer, "renderSize" } );
		}

		void Utils::declareCalcVSPosition()
		{
			if ( m_calcVSPosition )
			{
				return;
			}

			m_calcVSPosition = m_writer.implementFunction< Vec3 >( "calcVSPosition"
				, [&]( Vec2 const & uv
					, Float const & depth
					, Mat4 const & invProj )
				{
					auto csPosition = m_writer.declLocale( "csPosition"
						, vec3( uv * 2.0_f - 1.0_f, depth ) );
					auto vsPosition = m_writer.declLocale( "vsPosition"
						, invProj * vec4( csPosition, 1.0_f ) );
					m_writer.returnStmt( vsPosition.xyz() / vsPosition.w() );
				}
				, InVec2{ m_writer, "uv" }
				, InFloat{ m_writer, "depth" }
				, InMat4{ m_writer, "invProj" } );
		}

		void Utils::declareCalcWSPosition()
		{
			if ( m_calcWSPosition )
			{
				return;
			}

			m_calcWSPosition = m_writer.implementFunction< Vec3 >( "calcWSPosition"
				, [&]( Vec2 const & uv
					, Float const & depth
					, Mat4 const & invViewProj )
				{
					auto csPosition = m_writer.declLocale( "psPosition"
						, vec3( uv * 2.0_f - 1.0_f, depth ) );
					auto wsPosition = m_writer.declLocale( "wsPosition"
						, invViewProj * vec4( csPosition, 1.0_f ) );
					m_writer.returnStmt( wsPosition.xyz() / wsPosition.w() );
				}
				, InVec2{ m_writer, "uv" }
				, InFloat{ m_writer, "depth" }
				, InMat4{ m_writer, "invViewProj" } );
		}

		void Utils::declareApplyGamma()
		{
			if ( m_applyGamma )
			{
				return;
			}

			m_applyGamma = m_writer.implementFunction< Vec3 >( "applyGamma"
				, [&]( Float const & gamma
					, Vec3 const & hdr )
				{
					m_writer.returnStmt( pow( max( hdr, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( 1.0_f / gamma ) ) );
				}
				, InFloat{ m_writer, "gamma" }
				, InVec3{ m_writer, "hdr" } );
		}

		void Utils::declareRemoveGamma()
		{
			if ( m_removeGamma )
			{
				return;
			}

			m_removeGamma = m_writer.implementFunction< Vec3 >( "removeGamma"
				, [&]( Float const & gamma
					, Vec3 const & srgb )
				{
					m_writer.returnStmt( pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) ) );
				}
				, InFloat{ m_writer, "gamma" }
				, InVec3{ m_writer, "srgb" } );
		}

		void Utils::declareLineariseDepth()
		{
			if ( m_lineariseDepth )
			{
				return;
			}

			m_lineariseDepth = m_writer.implementFunction< Float >( "lineariseDepth"
				, [&]( Float const & depth
					, Float const & nearPlane
					, Float const & farPlane )
				{
					auto z = m_writer.declLocale( "z"
						, depth );
					z *= ( farPlane - nearPlane );
					m_writer.returnStmt( 2.0_f * farPlane * nearPlane / ( farPlane + nearPlane - z ) );
				}
				, InFloat{ m_writer, "depth" }
				, InFloat{ m_writer, "nearPlane" }
				, InFloat{ m_writer, "farPlane" } );
		}

		void Utils::declareGetMapNormal()
		{
			if ( m_getMapNormal )
			{
				return;
			}

			m_getMapNormal = m_writer.implementFunction< Vec3 >( "getMapNormal"
				, [&]( Vec2 const & uv
					, Vec3 const & normal
					, Vec3 const & position )
				{
					auto c3d_mapNormal( m_writer.getVariable< SampledImage2DRgba32 >( "c3d_mapNormal" ) );

					auto mapNormal = m_writer.declLocale( "mapNormal"
						, texture( c3d_mapNormal, uv.xy() ).xyz() );
					mapNormal = sdw::fma( mapNormal
						, vec3( 2.0_f )
						, vec3( -1.0_f ) );
					auto Q1 = m_writer.declLocale( "Q1"
						, dFdx( position ) );
					auto Q2 = m_writer.declLocale( "Q2"
						, dFdy( position ) );
					auto st1 = m_writer.declLocale( "st1"
						, dFdx( uv ) );
					auto st2 = m_writer.declLocale( "st2"
						, dFdy( uv ) );
					auto N = m_writer.declLocale( "N"
						, normalize( normal ) );
					auto T = m_writer.declLocale( "T"
						, normalize( Q1 * st2.t() - Q2 * st1.t() ) );
					auto B = m_writer.declLocale( "B"
						, -normalize( cross( N, T ) ) );
					auto tbn = m_writer.declLocale( "tbn"
						, mat3( T, B, N ) );
					m_writer.returnStmt( normalize( tbn * mapNormal ) );
				}
				, InVec2{ m_writer, "uv" }
				, InVec3{ m_writer, "normal" }
				, InVec3{ m_writer, "position" } );
		}

		void Utils::declareComputeAccumulation()
		{
			if ( m_computeAccumulation )
			{
				return;
			}

			m_computeAccumulation = m_writer.implementFunction< Vec4 >( "computeAccumulation"
				, [&]( Float const & depth
					, Vec3 const & colour
					, Float const & alpha
					, Float const & nearPlane
					, Float const & farPlane
					, Float const & accumulationOperator )
				{
					auto weight = m_writer.declLocale< Float >( "weight"
						, 1.0_f );

					SWITCH( m_writer, m_writer.cast< Int >( accumulationOperator ) )
					{
						CASE( 0 )
						{
							// Naive
							weight = 1.0_f - lineariseDepth( depth
								, nearPlane
								, farPlane );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 1 )
						{
							// (10)
							weight = max( pow( clamp( 1.0_f - depth, 0.0_f, 1.0_f ), 3.0_f ) * 3000.0_f, 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 2 )
						{
							// (9)
							weight = max( min( 0.03_f / ( pow( abs( depth ) / 200.0_f, 4.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 3 )
						{
							// (8)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 10.0_f, 3.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 4 )
						{
							// (7)
							weight = max( min( 10.0_f / ( pow( abs( depth ) / 200.0_f, 6.0_f ) + pow( abs( depth ) / 5.0_f, 2.0_f ) + 0.00001_f ), 3000.0_f ), 0.01_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 5 )
						{
							// (other)
							auto a = m_writer.declLocale( "a"
								, min( alpha, 1.0_f ) * 8.0_f + 0.01_f );
							auto b = m_writer.declLocale( "b"
								, -depth * 0.95_f + 1.0_f );
							weight = clamp( a * a * a * 100000000.0_f * b * b * b, 0.01_f, 300.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 6 )
						{
							// (other)
							auto a = m_writer.declLocale( "a"
								, min( alpha, 1.0_f ) * 8.0_f + 0.01_f );
							auto b = m_writer.declLocale( "b"
								, -depth * 0.95_f + 1.0_f );
							// If your scene has a lot of content very close to the far plane,
							// then include this line (one rsqrt instruction):
							b /= sqrt( 10000.0_f * abs( depth ) );
							weight = clamp( a * a * a * 100000000.0_f * b * b * b, 0.01_f, 300.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
						CASE( 7 )
						{
							// (yet another one)
							weight = max( min( 1.0_f, max( max( colour.r(), colour.g() ), colour.b() ) * alpha ), alpha ) * clamp( 0.03_f / ( 0.00001_f + pow( depth / 200.0_f, 4.0_f ) ), 0.01_f, 3000.0_f );
							m_writer.caseBreakStmt();
						}
						ESAC;
					}
					HCTIWS;

					m_writer.returnStmt( vec4( colour * alpha, alpha ) * weight );
				}
				, InFloat{ m_writer, "depth" }
				, InVec3{ m_writer, "colour" }
				, InFloat{ m_writer, "alpha" }
				, InFloat{ m_writer, "nearPlane" }
				, InFloat{ m_writer, "farPlane" }
				, InFloat{ m_writer, "accumulationOperator" } );
		}

		void Utils::declareFresnelSchlick()
		{
			if ( m_fresnelSchlick )
			{
				return;
			}

			m_fresnelSchlick = m_writer.implementFunction< Vec3 >( "fresnelSchlick"
				, [&]( Float const & product
					, Vec3 const & f0
					, Float const & roughness )
				{
					m_writer.returnStmt( sdw::fma( max( vec3( 1.0_f - roughness ), f0 ) - f0
						, vec3( pow( clamp( 1.0_f - product, 0.0_f, 1.0_f ), 5.0_f ) )
						, f0 ) );
				}
				, InFloat{ m_writer, "product" }
				, InVec3{ m_writer, "f0" }
				, InFloat{ m_writer, "roughness" } );
		}

		void Utils::declareComputeIBL()
		{
			if ( m_computeIBL )
			{
				return;
			}

			m_computeIBL = m_writer.implementFunction< Vec3 >( "computeIBL"
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
					auto V = m_writer.declLocale( "V"
						, normalize( worldEye - position ) );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( normal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( "F"
						, fresnelSchlick( NdotV, f0, roughness ) );
					auto kS = m_writer.declLocale( "kS"
						, F );
					auto kD = m_writer.declLocale( "kD"
						, vec3( 1.0_f ) - kS );
					kD *= 1.0_f - metallic;

					auto irradiance = m_writer.declLocale( "irradiance"
						, texture( irradianceMap, vec3( normal.x(), -normal.y(), normal.z() ) ).rgb() );
					auto diffuseReflection = m_writer.declLocale( "diffuseReflection"
						, irradiance * baseColour );
					auto R = m_writer.declLocale( "R"
						, reflect( -V, normal ) );
					R.y() = -R.y();

					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, textureLod( prefilteredEnvMap, R, roughness * Float( float( MaxIblReflectionLod ) ) ).rgb() );
					auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
						, vec2( NdotV, roughness ) );
					auto envBRDF = m_writer.declLocale( "envBRDF"
						, texture( brdfMap, envBRDFCoord ).rg() );
					auto specularReflection = m_writer.declLocale( "specularReflection"
						, prefilteredColor * sdw::fma( kS
							, vec3( envBRDF.x() )
							, vec3( envBRDF.y() ) ) );

					m_writer.returnStmt( sdw::fma( kD
						, diffuseReflection
						, specularReflection ) );
				}
				, InVec3{ m_writer, "normal" }
				, InVec3{ m_writer, "position" }
				, InVec3{ m_writer, "albedo" }
				, InVec3{ m_writer, "f0" }
				, InFloat{ m_writer, "roughness" }
				, InFloat{ m_writer, "metallic" }
				, InVec3{ m_writer, "worldEye" }
				, InSampledImageCubeRgba32{ m_writer, "irradianceMap" }
				, InSampledImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, InSampledImage2DRgba32{ m_writer, "brdfMap" } );
		}

		void Utils::declareInvertVec2Y()
		{
			if ( m_invertVec2Y )
			{
				return;
			}

			m_invertVec2Y = m_writer.implementFunction< Vec2 >( "invertVec2Y"
				, [&]( Vec2 const & v )
				{
					m_writer.returnStmt( vec2( v.x(), 1.0_f - v.y() ) );
				}
				, InVec2{ m_writer, "v" } );
		}

		void Utils::declareInvertVec3Y()
		{
			if ( m_invertVec3Y )
			{
				return;
			}

			m_invertVec3Y = m_writer.implementFunction< Vec3 >( "invertVec3Y"
				, [&]( Vec3 const & v )
				{
					m_writer.returnStmt( vec3( v.x(), 1.0_f - v.y(), v.z() ) );
				}
				, InVec3{ m_writer, "v" } );
		}

		void Utils::declareInvertVec4Y()
		{
			if ( m_invertVec4Y )
			{
				return;
			}

			m_invertVec4Y = m_writer.implementFunction< Vec4 >( "invertVec3Y"
				, [&]( Vec4 const & v )
				{
					m_writer.returnStmt( vec3( v.x(), 1.0_f - v.y(), v.z(), v.w() ) );
				}
				, InVec4{ m_writer, "v" } );
		}

		void Utils::declareNegateVec2Y()
		{
			if ( m_negateVec2Y )
			{
				return;
			}

			m_negateVec2Y = m_writer.implementFunction< Vec2 >( "negateVec2Y"
				, [&]( Vec2 const & v )
				{
					m_writer.returnStmt( vec2( v.x(), -v.y() ) );
				}
				, InVec2{ m_writer, "v" } );
		}

		void Utils::declareNegateVec3Y()
		{
			if ( m_negateVec3Y )
			{
				return;
			}

			m_negateVec3Y = m_writer.implementFunction< Vec3 >( "negateVec3Y"
				, [&]( Vec3 const & v )
				{
					m_writer.returnStmt( vec3( v.x(), -v.y(), v.z() ) );
				}
				, InVec3{ m_writer, "v" } );
		}

		void Utils::declareNegateVec4Y()
		{
			if ( m_negateVec4Y )
			{
				return;
			}

			m_negateVec4Y = m_writer.implementFunction< Vec4 >( "negateVec3Y"
				, [&]( Vec4 const & v )
				{
					m_writer.returnStmt( vec3( v.x(), -v.y(), v.z(), v.w() ) );
				}
				, InVec4{ m_writer, "v" } );
		}

		void Utils::declareEncodeMaterial()
		{
			if ( m_encodeMaterial )
			{
				return;
			}

			m_encodeMaterial = m_writer.implementFunction< sdw::Void >( "encodeMaterial"
				, [&]( Int const & receiver
					, Int const & reflection
					, Int const & refraction
					, Int const & envMapIndex
					, Float encoded )
				{
					auto flags = m_writer.declLocale( "flags"
						, 0_u
						+ ( ( m_writer.cast< UInt >( receiver ) << UInt( ReceiverOffset ) ) & UInt( ReceiverMask ) )
						+ ( ( m_writer.cast< UInt >( refraction ) << UInt( RefractionOffset ) ) & UInt( RefractionMask ) )
						+ ( ( m_writer.cast< UInt >( reflection ) << UInt( ReflectionOffset ) ) & UInt( ReflectionMask ) )
						+ ( ( m_writer.cast< UInt >( envMapIndex ) << UInt( EnvMapIndexOffset ) ) & UInt( EnvMapIndexMask ) ) );
					encoded = m_writer.cast< Float >( flags );
				}
				, InInt{ m_writer, "receiver" }
				, InInt{ m_writer, "reflection" }
				, InInt{ m_writer, "refraction" }
				, InInt{ m_writer, "envMapIndex" }
				, OutFloat{ m_writer, "encoded" } );
		}

		void Utils::declareDecodeMaterial()
		{
			if ( m_decodeMaterial )
			{
				return;
			}

			m_decodeMaterial = m_writer.implementFunction< sdw::Void >( "decodeMaterial"
				, [&]( Float const & encoded
					, Int receiver
					, Int reflection
					, Int refraction
					, Int envMapIndex )
				{
					auto flags = m_writer.declLocale( "flags"
						, m_writer.cast< UInt >( encoded ) );
					receiver = m_writer.cast< Int >( ( flags & UInt( ReceiverMask ) ) >> UInt( ReceiverOffset ) );
					refraction = m_writer.cast< Int >( ( flags & UInt( RefractionMask ) ) >> UInt( RefractionOffset ) );
					reflection = m_writer.cast< Int >( ( flags & UInt( ReflectionMask ) ) >> UInt( ReflectionOffset ) );
					envMapIndex = m_writer.cast< Int >( ( flags & UInt( EnvMapIndexMask ) ) >> UInt( EnvMapIndexOffset ) );
				}
				, InFloat{ m_writer, "encoded" }
				, OutInt{ m_writer, "receiver" }
				, OutInt{ m_writer, "reflection" }
				, OutInt{ m_writer, "refraction" }
				, OutInt{ m_writer, "envMapIndex" } );
		}

		void Utils::declareDecodeReceiver()
		{
			if ( m_decodeReceiver )
			{
				return;
			}

			m_decodeReceiver = m_writer.implementFunction< sdw::Void >( "decodeReceiver"
				, [&]( Int const & encoded
					, Int receiver )
				{
					receiver = ( encoded & ReceiverMask ) >> ReceiverOffset;
				}
				, InInt{ m_writer, "encoded" }
				, OutInt{ m_writer, "receiver" } );
		}

		void Utils::declareParallaxMappingFunc( PipelineFlags const & flags )
		{
			if ( flags.texturesCount > 0
				&& checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMapping )
				&& checkFlag( flags.textures, TextureFlag::eHeight ) )
			{
				using namespace sdw;

				m_parallaxMapping = m_writer.implementFunction< Vec2 >( "ParallaxMapping",
					[&]( Vec2 const & texCoords
						, Vec3 const & viewDir
						, SampledImage2DRgba32 const & heightMap
						, TextureConfigData const & textureConfig )
					{
						IF( m_writer, textureConfig.heightMask.x() == 0.0_f )
						{
							m_writer.returnStmt( texCoords );
						}
						FI;

						// number of depth layers
						auto minLayers = m_writer.declLocale( "minLayers"
							, 10.0_f );
						auto maxLayers = m_writer.declLocale( "maxLayers"
							, 20.0_f );
						auto numLayers = m_writer.declLocale( "numLayers"
							, mix( maxLayers
								, minLayers
								, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), viewDir ) ) ) );
						// calculate the size of each layer
						auto layerDepth = m_writer.declLocale( "layerDepth"
							, 1.0_f / numLayers );
						// depth of current layer
						auto currentLayerDepth = m_writer.declLocale( "currentLayerDepth"
							, 0.0_f );
						// the amount to shift the texture coordinates per layer (from vector P)
						auto p = m_writer.declLocale( "p"
							, viewDir.xy() * textureConfig.heightFactor );
						auto deltaTexCoords = m_writer.declLocale( "deltaTexCoords"
							, p / numLayers );

						auto currentTexCoords = m_writer.declLocale( "currentTexCoords"
							, texCoords );
						auto currentDepthMapValue = m_writer.declLocale( "currentDepthMapValue"
							, texture( heightMap, currentTexCoords ).r() );

						WHILE( m_writer, currentLayerDepth < currentDepthMapValue )
						{
							// shift texture coordinates along direction of P
							currentTexCoords -= deltaTexCoords;
							// get depthmap value at current texture coordinates
							currentDepthMapValue = textureLod( heightMap, currentTexCoords, 0.0_f ).r();
							// get depth of next layer
							currentLayerDepth += layerDepth;
						}
						ELIHW;

						// get texture coordinates before collision (reverse operations)
						auto prevTexCoords = m_writer.declLocale( "prevTexCoords"
							, currentTexCoords + deltaTexCoords );

						// get depth after and before collision for linear interpolation
						auto afterDepth = m_writer.declLocale( "afterDepth"
							, currentDepthMapValue - currentLayerDepth );
						auto beforeDepth = m_writer.declLocale( "beforeDepth"
							, texture( heightMap, prevTexCoords ).r() - currentLayerDepth + layerDepth );

						// interpolation of texture coordinates
						auto weight = m_writer.declLocale( "weight"
							, vec2( afterDepth / ( afterDepth - beforeDepth ) ) );
						auto finalTexCoords = m_writer.declLocale( "finalTexCoords"
							, sdw::fma( prevTexCoords
								, weight
								, currentTexCoords * ( vec2( 1.0_f ) - weight ) ) );

						m_writer.returnStmt( finalTexCoords );
					}
					, InVec2{ m_writer, "texCoords" }
					, InVec3{ m_writer, "viewDir" }
					, InSampledImage2DRgba32{ m_writer, "heightMap" }
					, InTextureConfigData{ m_writer, "textureConfig" } );
			}
		}

		void Utils::declareParallaxShadowFunc( PipelineFlags const & flags )
		{
			if ( flags.texturesCount > 1
				&& checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMapping )
				&& checkFlag( flags.textures, TextureFlag::eHeight ) )
			{
				using namespace sdw;

				m_parallaxShadow = m_writer.implementFunction< Float >( "ParallaxSoftShadowMultiplier"
					, [&]( Vec3 const & lightDir
						, Vec2 const initialTexCoord
						, Float initialHeight
						, SampledImage2DRgba32 const & heightMap
						, TextureConfigData const & textureConfig )
					{
						IF( m_writer, textureConfig.heightMask[0] == 0.0_f )
						{
							m_writer.returnStmt( 1.0_f );
						}
						FI;

						auto shadowMultiplier = m_writer.declLocale( "shadowMultiplier"
							, 1.0_f );
						auto minLayers = m_writer.declLocale( "minLayers"
							, 10.0_f );
						auto maxLayers = m_writer.declLocale( "maxLayers"
							, 20.0_f );

						// calculate lighting only for surface oriented to the light source
						IF( m_writer, dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) > 0.0_f )
						{
							// calculate initial parameters
							auto numSamplesUnderSurface = m_writer.declLocale( "numSamplesUnderSurface"
								, 0.0_f );
							shadowMultiplier = 0.0_f;
							auto numLayers = m_writer.declLocale( "numLayers"
								, mix( maxLayers
									, minLayers
									, sdw::abs( dot( vec3( 0.0_f, 0.0_f, 1.0_f ), lightDir ) ) ) );
							auto layerHeight = m_writer.declLocale( "layerHeight"
								, initialHeight / numLayers );
							auto texStep = m_writer.declLocale( "deltaTexCoords"
								, ( lightDir.xy() * textureConfig.heightFactor ) / lightDir.z() / numLayers );

							// current parameters
							auto currentLayerHeight = m_writer.declLocale( "currentLayerHeight"
								, initialHeight - layerHeight );
							auto currentTextureCoords = m_writer.declLocale( "currentTextureCoords"
								, initialTexCoord + texStep );
							auto heightFromTexture = m_writer.declLocale( "heightFromTexture"
								, texture( heightMap, currentTextureCoords ).r() );
							auto stepIndex = m_writer.declLocale( "stepIndex"
								, 1_i );

							// while point is below depth 0.0 )
							WHILE( m_writer, currentLayerHeight > 0.0_f )
							{
								// if point is under the surface
								IF( m_writer, heightFromTexture < currentLayerHeight )
								{
									// calculate partial shadowing factor
									numSamplesUnderSurface += 1.0_f;
									auto newShadowMultiplier = m_writer.declLocale( "newShadowMultiplier"
										, ( currentLayerHeight - heightFromTexture )
										* ( 1.0_f - m_writer.cast< Float >( stepIndex ) / numLayers ) );
									shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
								}
								FI;

								// offset to the next layer
								stepIndex += 1_i;
								currentLayerHeight -= layerHeight;
								currentTextureCoords += texStep;
								heightFromTexture = texture( heightMap, currentTextureCoords ).r();
							}
							ELIHW;

							// Shadowing factor should be 1 if there were no points under the surface
							IF( m_writer, numSamplesUnderSurface < 1.0_f )
							{
								shadowMultiplier = 1.0_f;
							}
							ELSE
							{
								shadowMultiplier = 1.0_f - shadowMultiplier;
							}
							FI;
						}
						FI;

						m_writer.returnStmt( shadowMultiplier );
					}
					, InVec3{ m_writer, "lightDir" }
					, InVec2{ m_writer, "initialTexCoord" }
					, InFloat{ m_writer, "initialHeight" }
					, InSampledImage2DRgba32{ m_writer, "heightMap" }
					, InTextureConfigData{ m_writer, "textureConfig" } );
			}
		}

		void Utils::applyAlphaFunc( VkCompareOp alphaFunc
			, sdw::Float & alpha
			, sdw::Float const & alphaRef )
		{
			if ( alphaFunc != VK_COMPARE_OP_ALWAYS )
			{
				using namespace sdw;

				switch ( alphaFunc )
				{
				case VK_COMPARE_OP_NEVER:
					m_writer.discard();
					break;
				case VK_COMPARE_OP_LESS:
					IF( m_writer, alpha >= alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_EQUAL:
					IF( m_writer, alpha != alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_LESS_OR_EQUAL:
					IF( m_writer, alpha > alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_GREATER:
					IF( m_writer, alpha <= alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_NOT_EQUAL:
					IF( m_writer, alpha == alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_GREATER_OR_EQUAL:
					IF( m_writer, alpha < alphaRef )
					{
						m_writer.discard();
					}
					FI;
					alpha = 1.0_f;
					break;
				case VK_COMPARE_OP_ALWAYS:
					break;

				default:
					break;
				}
			}
		}

		sdw::Vec2 Utils::topDownToBottomUp( sdw::Vec2 const & texCoord )const
		{
			return m_invertVec2Y( texCoord );
		}

		sdw::Vec3 Utils::topDownToBottomUp( sdw::Vec3 const & texCoord )const
		{
			return m_invertVec3Y( texCoord );
		}

		sdw::Vec4 Utils::topDownToBottomUp( sdw::Vec4 const & texCoord )const
		{
			return m_invertVec4Y( texCoord );
		}

		sdw::Vec2 Utils::negateTopDownToBottomUp( sdw::Vec2 const & texCoord )const
		{
			return m_negateVec2Y( texCoord );
		}

		sdw::Vec3 Utils::negateTopDownToBottomUp( sdw::Vec3 const & texCoord )const
		{
			return m_negateVec3Y( texCoord );
		}

		sdw::Vec4 Utils::negateTopDownToBottomUp( sdw::Vec4 const & texCoord )const
		{
			return m_negateVec4Y( texCoord );
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
			, Float const & farPlane
			, Float const & accumulationOperator )
		{
			return m_computeAccumulation( depth
				, colour
				, alpha
				, nearPlane
				, farPlane
				, accumulationOperator );
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

		void Utils::computeOpaNmlMapContribution( PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent
			, sdw::Vec3 & normal )
		{
			if ( flags.texturesCount > 0 )
			{
				for ( uint32_t i = 0u; i < flags.texturesCount; ++i )
				{
					auto name = string::stringCast< char >( string::toString( i ) );
					auto config = m_writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureConfig[i / 4u][i % 4u] ) ) );
					auto sampled = m_writer.declLocale< Vec4 >( "sampled" + name
						, texture( maps[i], config.convertUV( m_writer, texCoords.xy() ) ) );

					if ( checkFlag( flags.textures, TextureFlag::eNormal ) )
					{
						normal = config.getNormal( m_writer, sampled, normal, tangent, bitangent );
					}

					if ( checkFlag( flags.textures, TextureFlag::eOpacity ) )
					{
						opacity = config.getOpacity( m_writer, sampled, opacity );
					}
				}
			}
		}

		void Utils::computeOpacityMapContribution( PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Float & opacity )
		{
			if ( flags.texturesCount > 0
				&& checkFlag( flags.textures, TextureFlag::eOpacity ) )
			{
				for ( uint32_t i = 0u; i < flags.texturesCount; ++i )
				{
					auto name = string::stringCast< char >( string::toString( i, std::locale{ "C" } ) );
					auto config = m_writer.declLocale( "config" + name
						, textureConfigs.getTextureConfiguration( m_writer.cast< UInt >( textureConfig[i / 4u][i % 4u] ) ) );
					auto sampled = m_writer.declLocale< Vec4 >( "sampled" + name
						, texture( maps[i], config.convertUV( m_writer, texCoords.xy() ) ) );
						opacity = config.getOpacity( m_writer, sampled, opacity );
				}
			}
		}

		void Utils::computeHeightMapContribution( PipelineFlags const & flags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::UVec4 > const & textureConfig
			, sdw::Vec3 const & tangentSpaceViewPosition
			, sdw::Vec3 const & tangentSpaceFragPosition
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 & texCoords )
		{
			if ( flags.texturesCount > 1
				&& checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMapping )
				&& checkFlag( flags.textures, TextureFlag::eHeight )
				&& flags.heightMapIndex != InvalidIndex )
			{
				auto heightMapConfig = m_writer.declLocale( "heightMapConfig"
					, textureConfigs.getTextureConfiguration( textureConfig[flags.heightMapIndex / 4u][flags.heightMapIndex % 4u] ) );
				texCoords.xy() = m_parallaxMapping( heightMapConfig.convertUV( m_writer, texCoords.xy() )
					, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
					, maps[flags.heightMapIndex]
					, heightMapConfig );
			}
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
