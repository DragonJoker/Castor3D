#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
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

		m_calcTexCoord = m_writer.implementFunction< sdw::Vec2 >( "calcTexCoord"
			, [&]( sdw::Vec2 const & renderPos
				, sdw::Vec2 const & renderSize )
			{
				m_writer.returnStmt( renderPos / renderSize );
			}
			, sdw::InVec2{ m_writer, "renderPos" }
			, sdw::InVec2{ m_writer, "renderSize" } );
	}

	void Utils::declareCalcVSPosition()
	{
		if ( m_calcVSPosition )
		{
			return;
		}

		m_calcVSPosition = m_writer.implementFunction< sdw::Vec3 >( "calcVSPosition"
			, [&]( sdw::Vec2 const & uv
				, sdw::Float const & depth
				, sdw::Mat4 const & invProj )
			{
				auto csPosition = m_writer.declLocale( "csPosition"
					, vec3( uv * 2.0_f - 1.0_f, depth ) );
				auto vsPosition = m_writer.declLocale( "vsPosition"
					, invProj * vec4( csPosition, 1.0_f ) );
				m_writer.returnStmt( vsPosition.xyz() / vsPosition.w() );
			}
			, sdw::InVec2{ m_writer, "uv" }
			, sdw::InFloat{ m_writer, "depth" }
			, sdw::InMat4{ m_writer, "invProj" } );
	}

	void Utils::declareCalcWSPosition()
	{
		if ( m_calcWSPosition )
		{
			return;
		}

		m_calcWSPosition = m_writer.implementFunction< sdw::Vec3 >( "calcWSPosition"
			, [&]( sdw::Vec2 const & uv
				, sdw::Float const & depth
				, sdw::Mat4 const & invViewProj )
			{
				auto csPosition = m_writer.declLocale( "psPosition"
					, vec3( uv * 2.0_f - 1.0_f, depth ) );
				auto wsPosition = m_writer.declLocale( "wsPosition"
					, invViewProj * vec4( csPosition, 1.0_f ) );
				m_writer.returnStmt( wsPosition.xyz() / wsPosition.w() );
			}
			, sdw::InVec2{ m_writer, "uv" }
			, sdw::InFloat{ m_writer, "depth" }
			, sdw::InMat4{ m_writer, "invViewProj" } );
	}

	void Utils::declareApplyGamma()
	{
		if ( m_applyGamma )
		{
			return;
		}

		m_applyGamma = m_writer.implementFunction< sdw::Vec3 >( "applyGamma"
			, [&]( sdw::Float const & gamma
				, sdw::Vec3 const & hdr )
			{
				m_writer.returnStmt( pow( max( hdr, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( 1.0_f / gamma ) ) );
			}
			, sdw::InFloat{ m_writer, "gamma" }
			, sdw::InVec3{ m_writer, "hdr" } );
	}

	void Utils::declareRemoveGamma()
	{
		if ( m_removeGamma )
		{
			return;
		}

		m_removeGamma = m_writer.implementFunction< sdw::Vec3 >( "removeGamma"
			, [&]( sdw::Float const & gamma
				, sdw::Vec3 const & srgb )
			{
				m_writer.returnStmt( pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) ) );
			}
			, sdw::InFloat{ m_writer, "gamma" }
			, sdw::InVec3{ m_writer, "srgb" } );
	}

	void Utils::declareLineariseDepth()
	{
		if ( m_lineariseDepth )
		{
			return;
		}

		m_lineariseDepth = m_writer.implementFunction< sdw::Float >( "lineariseDepth"
			, [&]( sdw::Float const & depth
				, sdw::Float const & nearPlane
				, sdw::Float const & farPlane )
			{
				auto z = m_writer.declLocale( "z"
					, depth );
				z *= ( farPlane - nearPlane );
				m_writer.returnStmt( 2.0_f * farPlane * nearPlane / ( farPlane + nearPlane - z ) );
			}
			, sdw::InFloat{ m_writer, "depth" }
			, sdw::InFloat{ m_writer, "nearPlane" }
			, sdw::InFloat{ m_writer, "farPlane" } );
	}

	void Utils::declareGetMapNormal()
	{
		if ( m_getMapNormal )
		{
			return;
		}

		m_getMapNormal = m_writer.implementFunction< sdw::Vec3 >( "getMapNormal"
			, [&]( sdw::Vec2 const & uv
				, sdw::Vec3 const & normal
				, sdw::Vec3 const & position )
			{
				auto c3d_mapNormal( m_writer.getVariable< sdw::SampledImage2DRgba32 >( "c3d_mapNormal" ) );

				auto mapNormal = m_writer.declLocale( "mapNormal"
					, c3d_mapNormal.sample( uv.xy() ).xyz() );
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
			, sdw::InVec2{ m_writer, "uv" }
			, sdw::InVec3{ m_writer, "normal" }
			, sdw::InVec3{ m_writer, "position" } );
	}

	void Utils::declareComputeAccumulation()
	{
		if ( m_computeAccumulation )
		{
			return;
		}

		m_computeAccumulation = m_writer.implementFunction< sdw::Vec4 >( "computeAccumulation"
			, [&]( sdw::Float const & depth
				, sdw::Vec3 const & colour
				, sdw::Float const & alpha
				, sdw::Float const & nearPlane
				, sdw::Float const & farPlane
				, sdw::Float const & accumulationOperator )
			{
				auto weight = m_writer.declLocale< sdw::Float >( "weight"
					, 1.0_f );

				SWITCH( m_writer, m_writer.cast< sdw::Int >( accumulationOperator ) )
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
			, sdw::InFloat{ m_writer, "depth" }
			, sdw::InVec3{ m_writer, "colour" }
			, sdw::InFloat{ m_writer, "alpha" }
			, sdw::InFloat{ m_writer, "nearPlane" }
			, sdw::InFloat{ m_writer, "farPlane" }
			, sdw::InFloat{ m_writer, "accumulationOperator" } );
	}

	void Utils::declareFresnelSchlick()
	{
		if ( m_fresnelSchlick )
		{
			return;
		}

		m_fresnelSchlick = m_writer.implementFunction< sdw::Vec3 >( "fresnelSchlick"
			, [&]( sdw::Float const & product
				, sdw::Vec3 const & f0
				, sdw::Float const & roughness )
			{
				m_writer.returnStmt( sdw::fma( max( vec3( 1.0_f - roughness ), f0 ) - f0
					, vec3( pow( clamp( 1.0_f - product, 0.0_f, 1.0_f ), 5.0_f ) )
					, f0 ) );
			}
			, sdw::InFloat{ m_writer, "product" }
			, sdw::InVec3{ m_writer, "f0" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	void Utils::declareComputeIBL()
	{
		if ( m_computeIBL )
		{
			return;
		}

		m_computeIBL = m_writer.implementFunction< sdw::Vec3 >( "computeIBL"
			, [&]( sdw::Vec3 const & normal
				, sdw::Vec3 const & position
				, sdw::Vec3 const & baseColour
				, sdw::Vec3 const & f0
				, sdw::Float const & roughness
				, sdw::Float const & metallic
				, sdw::Vec3 const & worldEye
				, sdw::SampledImageCubeRgba32 const & irradianceMap
				, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
				, sdw::SampledImage2DRgba32 const & brdfMap )
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
					, irradianceMap.sample( vec3( normal.x(), -normal.y(), normal.z() ) ).rgb() );
				auto diffuseReflection = m_writer.declLocale( "diffuseReflection"
					, irradiance * baseColour );
				auto R = m_writer.declLocale( "R"
					, reflect( -V, normal ) );
				R.y() = -R.y();

				auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
					, prefilteredEnvMap.lod( R, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb() );
				auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
					, vec2( NdotV, roughness ) );
				auto envBRDF = m_writer.declLocale( "envBRDF"
					, brdfMap.sample( envBRDFCoord ).rg() );
				auto specularReflection = m_writer.declLocale( "specularReflection"
					, prefilteredColor * sdw::fma( kS
						, vec3( envBRDF.x() )
						, vec3( envBRDF.y() ) ) );

				m_writer.returnStmt( sdw::fma( kD
					, diffuseReflection
					, specularReflection ) );
			}
			, sdw::InVec3{ m_writer, "normal" }
			, sdw::InVec3{ m_writer, "position" }
			, sdw::InVec3{ m_writer, "albedo" }
			, sdw::InVec3{ m_writer, "f0" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InFloat{ m_writer, "metallic" }
			, sdw::InVec3{ m_writer, "worldEye" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "irradianceMap" }
			, sdw::InSampledImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
			, sdw::InSampledImage2DRgba32{ m_writer, "brdfMap" } );
	}

	void Utils::declareInvertVec2Y()
	{
		if ( m_invertVec2Y )
		{
			return;
		}

		m_invertVec2Y = m_writer.implementFunction< sdw::Vec2 >( "invertVec2Y"
			, [&]( sdw::Vec2 const & v )
			{
				m_writer.returnStmt( vec2( v.x(), 1.0_f - v.y() ) );
			}
			, sdw::InVec2{ m_writer, "v" } );
	}

	void Utils::declareInvertVec3Y()
	{
		if ( m_invertVec3Y )
		{
			return;
		}

		m_invertVec3Y = m_writer.implementFunction< sdw::Vec3 >( "invertVec3Y"
			, [&]( sdw::Vec3 const & v )
			{
				m_writer.returnStmt( vec3( v.x(), 1.0_f - v.y(), v.z() ) );
			}
			, sdw::InVec3{ m_writer, "v" } );
	}

	void Utils::declareInvertVec4Y()
	{
		if ( m_invertVec4Y )
		{
			return;
		}

		m_invertVec4Y = m_writer.implementFunction< sdw::Vec4 >( "invertVec3Y"
			, [&]( sdw::Vec4 const & v )
			{
				m_writer.returnStmt( vec3( v.x(), 1.0_f - v.y(), v.z(), v.w() ) );
			}
			, sdw::InVec4{ m_writer, "v" } );
	}

	void Utils::declareNegateVec2Y()
	{
		if ( m_negateVec2Y )
		{
			return;
		}

		m_negateVec2Y = m_writer.implementFunction< sdw::Vec2 >( "negateVec2Y"
			, [&]( sdw::Vec2 const & v )
			{
				m_writer.returnStmt( vec2( v.x(), -v.y() ) );
			}
			, sdw::InVec2{ m_writer, "v" } );
	}

	void Utils::declareNegateVec3Y()
	{
		if ( m_negateVec3Y )
		{
			return;
		}

		m_negateVec3Y = m_writer.implementFunction< sdw::Vec3 >( "negateVec3Y"
			, [&]( sdw::Vec3 const & v )
			{
				m_writer.returnStmt( vec3( v.x(), -v.y(), v.z() ) );
			}
			, sdw::InVec3{ m_writer, "v" } );
	}

	void Utils::declareNegateVec4Y()
	{
		if ( m_negateVec4Y )
		{
			return;
		}

		m_negateVec4Y = m_writer.implementFunction< sdw::Vec4 >( "negateVec3Y"
			, [&]( sdw::Vec4 const & v )
			{
				m_writer.returnStmt( vec3( v.x(), -v.y(), v.z(), v.w() ) );
			}
			, sdw::InVec4{ m_writer, "v" } );
	}

	void Utils::declareEncodeMaterial()
	{
		if ( m_encodeMaterial )
		{
			return;
		}

		m_encodeMaterial = m_writer.implementFunction< sdw::Void >( "encodeMaterial"
			, [&]( sdw::Int const & receiver
				, sdw::Int const & reflection
				, sdw::Int const & refraction
				, sdw::Int const & envMapIndex
				, sdw::Float encoded )
			{
				auto flags = m_writer.declLocale( "flags"
					, 0_u
					+ ( ( m_writer.cast<  sdw::UInt >( receiver ) <<  sdw::UInt( ReceiverOffset ) ) &  sdw::UInt( ReceiverMask ) )
					+ ( ( m_writer.cast<  sdw::UInt >( refraction ) <<  sdw::UInt( RefractionOffset ) ) &  sdw::UInt( RefractionMask ) )
					+ ( ( m_writer.cast<  sdw::UInt >( reflection ) <<  sdw::UInt( ReflectionOffset ) ) &  sdw::UInt( ReflectionMask ) )
					+ ( ( m_writer.cast<  sdw::UInt >( envMapIndex ) <<  sdw::UInt( EnvMapIndexOffset ) ) &  sdw::UInt( EnvMapIndexMask ) ) );
				encoded = m_writer.cast< sdw::Float >( flags );
			}
			, sdw::InInt{ m_writer, "receiver" }
			, sdw::InInt{ m_writer, "reflection" }
			, sdw::InInt{ m_writer, "refraction" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::OutFloat{ m_writer, "encoded" } );
	}

	void Utils::declareDecodeMaterial()
	{
		if ( m_decodeMaterial )
		{
			return;
		}

		m_decodeMaterial = m_writer.implementFunction< sdw::Void >( "decodeMaterial"
			, [&]( sdw::Float const & encoded
				, sdw::Int receiver
				, sdw::Int reflection
				, sdw::Int refraction
				, sdw::Int envMapIndex )
			{
				auto flags = m_writer.declLocale( "flags"
					, m_writer.cast<  sdw::UInt >( encoded ) );
				receiver = m_writer.cast< sdw::Int >( ( flags &  sdw::UInt( ReceiverMask ) ) >>  sdw::UInt( ReceiverOffset ) );
				refraction = m_writer.cast< sdw::Int >( ( flags &  sdw::UInt( RefractionMask ) ) >>  sdw::UInt( RefractionOffset ) );
				reflection = m_writer.cast< sdw::Int >( ( flags &  sdw::UInt( ReflectionMask ) ) >>  sdw::UInt( ReflectionOffset ) );
				envMapIndex = m_writer.cast< sdw::Int >( ( flags &  sdw::UInt( EnvMapIndexMask ) ) >>  sdw::UInt( EnvMapIndexOffset ) );
			}
			, sdw::InFloat{ m_writer, "encoded" }
			, sdw::OutInt{ m_writer, "receiver" }
			, sdw::OutInt{ m_writer, "reflection" }
			, sdw::OutInt{ m_writer, "refraction" }
			, sdw::OutInt{ m_writer, "envMapIndex" } );
	}

	void Utils::declareDecodeReceiver()
	{
		if ( m_decodeReceiver )
		{
			return;
		}

		m_decodeReceiver = m_writer.implementFunction< sdw::Void >( "decodeReceiver"
			, [&]( sdw::Int const & encoded
				, sdw::Int receiver )
			{
				receiver = ( encoded & ReceiverMask ) >> ReceiverOffset;
			}
			, sdw::InInt{ m_writer, "encoded" }
			, sdw::OutInt{ m_writer, "receiver" } );
	}

	void Utils::declareParallaxMappingFunc( PipelineFlags const & flags )
	{
		if ( checkFlags( flags.textures, TextureFlag::eHeight ) != flags.textures.end()
			&& ( checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingOne )
			|| checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			if ( m_parallaxMapping )
			{
				return;
			}

			m_parallaxMapping = m_writer.implementFunction< sdw::Vec2 >( "ParallaxMapping",
				[&]( sdw::Vec2 const & texCoords
					, sdw::Vec3 const & viewDir
					, sdw::SampledImage2DRgba32 const & heightMap
					, TextureConfigData const & textureConfig )
				{
					IF( m_writer, textureConfig.heightMask.x() == 0.0_f )
					{
						m_writer.returnStmt( texCoords );
					}
					FI;

					// number of depth layers
					auto minLayers = m_writer.declLocale( "minLayers"
						, 32.0_f );
					auto maxLayers = m_writer.declLocale( "maxLayers"
						, 64.0_f );
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
						, viewDir.xy() / viewDir.z() * textureConfig.heightFactor );
					auto deltaTexCoords = m_writer.declLocale( "deltaTexCoords"
						, p / numLayers );

					auto currentTexCoords = m_writer.declLocale( "currentTexCoords"
						, texCoords );
					auto dx = m_writer.declLocale( "dx"
						, dFdxCoarse( currentTexCoords ) );
					auto dy = m_writer.declLocale( "dy"
						, dFdxCoarse( currentTexCoords ) );
					auto currentDepthMapValue = m_writer.declLocale( "currentDepthMapValue"
						, heightMap.grad( currentTexCoords, dx, dy ).r() );

					WHILE( m_writer, currentLayerDepth < currentDepthMapValue )
					{
						// shift texture coordinates along direction of P
						currentTexCoords -= deltaTexCoords;
						// get depthmap value at current texture coordinates
						currentDepthMapValue = heightMap.grad( currentTexCoords, dx, dy ).r();
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
						, heightMap.grad( prevTexCoords, dx, dy ).r() - currentLayerDepth + layerDepth );

					// interpolation of texture coordinates
					auto weight = m_writer.declLocale( "weight"
						, vec2( afterDepth / ( afterDepth - beforeDepth ) ) );
					auto finalTexCoords = m_writer.declLocale( "finalTexCoords"
						, sdw::fma( prevTexCoords
							, weight
							, currentTexCoords * ( vec2( 1.0_f ) - weight ) ) );

					m_writer.returnStmt( finalTexCoords );
				}
				, sdw::InVec2{ m_writer, "texCoords" }
				, sdw::InVec3{ m_writer, "viewDir" }
				, sdw::InSampledImage2DRgba32{ m_writer, "heightMap" }
				, InTextureConfigData{ m_writer, "textureConfig" } );
		}
	}

	void Utils::declareIsSaturated()
	{
		if ( m_isSaturated3D )
		{
			return;
		}

		m_isSaturated3D = m_writer.implementFunction< sdw::Boolean >( "isSaturated"
			, [&]( sdw::Vec3 const & p )
			{
				m_writer.returnStmt( p.x() == clamp( p.x(), 0.0_f, 1.0_f )
					&& p.y() == clamp( p.y(), 0.0_f, 1.0_f )
					&& p.z() == clamp( p.z(), 0.0_f, 1.0_f ) );
			}
			, sdw::InVec3{ m_writer, "p" } );
	}

	void Utils::declareEncodeColor()
	{
		if ( m_encodeColor )
		{
			return;
		}

		m_encodeColor = m_writer.implementFunction<  sdw::UInt >( "encodeColor"
			, [&]( sdw::Vec4 const & color )
			{
				auto hdrRange = m_writer.declConstant( "hdrRange", 10.0_f );

				// normalize color to LDR
				auto hdr = m_writer.declLocale( "hdr"
					, length( color.rgb() ) );
				color.rgb() /= hdr;

				// encode LDR color and HDR range
				auto iColor = m_writer.declLocale( "iColor"
					, uvec3( color.rgb() * 255.0_f ) );
				auto iHDR = m_writer.declLocale( "iHDR"
					, m_writer.cast<  sdw::UInt >( clamp( hdr / hdrRange, 0.0_f, 1.0_f ) * 127.0_f ) );
				auto colorMask = m_writer.declLocale( "colorMask"
					, ( iHDR << 24_u ) | ( iColor.r() << 16_u ) | ( iColor.g() << 8_u ) | iColor.b() );

				// encode alpha into highest bit
				auto iAlpha = m_writer.declLocale( "iAlpha"
					, m_writer.ternary( color.a() > 0.0_f, 1_u, 0_u ) );
				colorMask |= ( iAlpha << 31_u );

				m_writer.returnStmt( colorMask );
			}
			, sdw::InVec4{ m_writer, "color" } );
	}

	void Utils::declareEncodeNormal()
	{
		if ( m_encodeNormal )
		{
			return;
		}

		m_encodeNormal = m_writer.implementFunction<  sdw::UInt >( "encodeNormal"
			, [&]( sdw::Vec3 const & normal )
			{
				auto iNormal = m_writer.declLocale( "iNormal"
					, ivec3( normal * 255.0_f ) );
				auto uNormalSigns = m_writer.declLocale< sdw::UVec3 >( "uNormalSigns"
					, uvec3( m_writer.cast<  sdw::UInt >( ( iNormal.x() >> 5 ) & 0x04000000 )
						, m_writer.cast<  sdw::UInt >( ( iNormal.y() >> 14 ) & 0x00020000 )
						, m_writer.cast<  sdw::UInt >( ( iNormal.z() >> 23 ) & 0x00000100 ) ) );
				auto uNormal = m_writer.declLocale( "uNormal"
					, uvec3( abs( iNormal ) ) );
				auto normalMask = m_writer.declLocale( "normalMask"
					, ( uNormalSigns.x() | ( uNormal.x() << 18_u )
						| uNormalSigns.y() | ( uNormal.y() << 9_u )
						| uNormalSigns.z() | uNormal.z() ) );
				m_writer.returnStmt( normalMask );
			}
			, sdw::InVec3{ m_writer, "normal" } );
	}

	void Utils::declareDecodeColor()
	{
		if ( m_decodeColor )
		{
			return;
		}

		m_decodeColor = m_writer.implementFunction< sdw::Vec4 >( "decodeColor"
			, [&](  sdw::UInt const & colorMask )
			{
				auto hdrRange = m_writer.declConstant( "hdrRange", 10.0_f );

				auto color = m_writer.declLocale< sdw::Vec4 >( "color" );
				auto hdr = m_writer.declLocale( "hdr"
					, m_writer.cast< sdw::Float >( ( colorMask >> 24u ) & 0x0000007f ) );
				color.r() = m_writer.cast< sdw::Float >( ( colorMask >> 16u ) & 0x000000ff );
				color.g() = m_writer.cast< sdw::Float >( ( colorMask >> 8u ) & 0x000000ff );
				color.b() = m_writer.cast< sdw::Float >( colorMask & 0x000000ff );

				hdr /= 127.0f;
				color.rgb() /= vec3( 255.0_f );

				color.rgb() *= hdr * hdrRange;

				color.a() = m_writer.cast< sdw::Float >( ( colorMask >> 31u ) & 0x00000001_u );

				m_writer.returnStmt( color );
			}
			, sdw::InUInt{ m_writer, "colorMask" } );
	}

	void Utils::declareDecodeNormal()
	{
		if ( m_decodeNormal )
		{
			return;
		}

		m_decodeNormal = m_writer.implementFunction< sdw::Vec3 >( "decodeNormal"
			, [&](  sdw::UInt const & normalMask )
			{
				auto iNormal = m_writer.declLocale( "iNormal"
					, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 18 ) & 0x000000ff )
						, m_writer.cast< sdw::Int >( ( normalMask >> 9 ) & 0x000000ff )
						, m_writer.cast< sdw::Int >( normalMask & 0x000000ff ) ) );
				auto iNormalSigns = m_writer.declLocale( "iNormalSigns"
					, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 25 ) & 0x00000002 )
						, m_writer.cast< sdw::Int >( ( normalMask >> 16 ) & 0x00000002 )
						, m_writer.cast< sdw::Int >( ( normalMask >> 7 ) & 0x00000002 ) ) );
				iNormalSigns = 1_i - iNormalSigns;
				auto normal = m_writer.declLocale( "normal"
					, vec3( iNormal ) / 255.0f );
				normal *= vec3( iNormalSigns );
				m_writer.returnStmt( normal );
			}
			, sdw::InUInt{ m_writer, "normalMask" } );
	}

	void Utils::declareFlatten()
	{
		if ( m_flatten3D )
		{
			return;
		}

		m_flatten3D = m_writer.implementFunction<  sdw::UInt >( "flatten3D"
			, [&]( sdw::UVec3 const & coord
				, sdw::UVec3 const & dim )
			{
				m_writer.returnStmt( ( coord.z() * dim.x() * dim.y() ) + ( coord.y() * dim.x() ) + coord.x() );
			}
			, sdw::InUVec3{ m_writer, "coord" }
			, sdw::InUVec3{ m_writer, "dim" } );
	}

	void Utils::declareUnflatten()
	{
		if ( m_unflatten3D )
		{
			return;
		}

		m_unflatten3D = m_writer.implementFunction< sdw::UVec3 >( "unflatten3D"
			, [&](  sdw::UInt idx
				, sdw::UVec3 const & dim )
			{
				auto z = m_writer.declLocale( "z"
					, idx / ( dim.x() * dim.y() ) );
				idx -= ( z * dim.x() * dim.y() );
				auto y = m_writer.declLocale( "y"
					, idx / dim.x() );
				auto x = m_writer.declLocale( "x"
					, idx % dim.x() );
				m_writer.returnStmt( uvec3( x, y, z ) );
			}
			, sdw::InUInt{ m_writer, "idx" }
			, sdw::InUVec3{ m_writer, "dim" } );
	}

	void Utils::declareTraceCone()
	{
		if ( m_traceCone )
		{
			return;
		}

		declareIsSaturated();

		auto cones = m_writer.declConstantArray( "cones"
			, std::vector< sdw::Vec3 >{ vec3( 0.57735_f, 0.57735, 0.57735 )
				, vec3( 0.57735_f, -0.57735, -0.57735 )
				, vec3( -0.57735_f, 0.57735, -0.57735 )
				, vec3( -0.57735_f, -0.57735, 0.57735 )
				, vec3( -0.903007_f, -0.182696, -0.388844 )
				, vec3( -0.903007_f, 0.182696, 0.388844 )
				, vec3( 0.903007_f, -0.182696, 0.388844 )
				, vec3( 0.903007_f, 0.182696, -0.388844 )
				, vec3( -0.388844_f, -0.903007, -0.182696 )
				, vec3( 0.388844_f, -0.903007, 0.182696 )
				, vec3( 0.388844_f, 0.903007, -0.182696 )
				, vec3( -0.388844_f, 0.903007, 0.182696 )
				, vec3( -0.182696_f, -0.388844, -0.903007 )
				, vec3( 0.182696_f, 0.388844, -0.903007 )
				, vec3( -0.182696_f, 0.388844, 0.903007 )
				, vec3( 0.182696_f, -0.388844, 0.903007 ) } );
		
		m_traceCone = m_writer.implementFunction< sdw::Vec4 >( "traceCone"
			, [&]( sdw::SampledImage3DRgba32 const & voxels
				, sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsNormal
				, sdw::Vec3 const & wsConeDirection
				, sdw::Float const & coneAperture
				, VoxelData const & voxelData )
			{
				auto color = m_writer.declLocale( "color"
					, vec3( 0.0_f ) );
				auto alpha = m_writer.declLocale( "alpha"
					, 0.0_f );

				// We need to offset the cone start position to avoid sampling its own voxel (self-occlusion):
				//	Unfortunately, it will result in disconnection between nearby surfaces :(
				auto dist = m_writer.declLocale( "dist"
					, sdw::Float{ voxelData.size } ); // offset by cone dir so that first sample of all cones are not the same
				auto startPos = m_writer.declLocale( "startPos"
					, wsPosition + wsNormal * vec3( voxelData.size * 2.0f * sqrt( 2.0f ) ) ); // sqrt2 is diagonal voxel half-extent

				// We will break off the loop if the sampling distance is too far for performance reasons:
				WHILE( m_writer, dist < voxelData.radianceMaxDistance && alpha < 1.0_f )
				{
					auto diameter = m_writer.declLocale( "diameter"
						, max( voxelData.size, 2 * coneAperture * dist ) );
					auto mip = m_writer.declLocale( "mip"
						, log2( diameter * voxelData.sizeInv ) );

					// Because we do the ray-marching in world space, we need to remap into 3d texture space before sampling:
					//	todo: optimization could be doing ray-marching in texture space
					auto tc = m_writer.declLocale( "tc"
						, startPos + wsConeDirection * vec3( dist ) );
					tc = tc * vec3( voxelData.sizeInv );
					tc *= voxelData.resolutionInv;
					tc = tc * vec3( 0.5_f, -0.5f, 0.5f ) + 0.5f;

					// break if the ray exits the voxel grid, or we sample from the last mip:
					IF( m_writer, !isSaturated( tc ) || mip >= voxelData.radianceMips )
					{
						m_writer.loopBreakStmt();
					}
					FI;

					auto sam = m_writer.declLocale( "sam"
						, voxels.lod( tc, mip ) );

					// this is the correct blending to avoid black-staircase artifact (ray stepped front-to back, so blend front to back):
					auto a = m_writer.declLocale( "a"
						, 1.0_f - alpha );
					color += a * sam.rgb();
					alpha += a * sam.a();

					// step along ray:
					dist += diameter * voxelData.rayStepSize;
				}
				ELIHW;

				m_writer.returnStmt( vec4( color, alpha ) );
			}
			, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
			, sdw::InVec3{ m_writer, "wsPosition" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InVec3{ m_writer, "wsConeDirection" }
			, sdw::InFloat{ m_writer, "coneAperture" }
			, InVoxelData{ m_writer, "voxelData" } );

		m_traceConeRadiance = m_writer.implementFunction< sdw::Vec4 >( "traceConeRadiance"
			, [&]( sdw::SampledImage3DRgba32 const & voxels
				, sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsNormal
				, shader::VoxelData const & voxelData )
			{
				auto radiance = m_writer.declLocale( "radiance"
					, vec4( 0.0_f ) );

				FOR( m_writer, sdw::UInt, cone, 0_u, cone < voxelData.radianceNumCones, ++cone ) // quality is between 1 and 16 cones
				{
					// approximate a hemisphere from random points inside a sphere:
					//  (and modulate cone with surface normal, no banding this way)
					auto coneDirection = m_writer.declLocale( "coneDirection"
						, normalize( cones[cone] + wsNormal ) );
					// if point on sphere is facing below normal (so it's located on bottom hemisphere), put it on the opposite hemisphere instead:
					coneDirection *= m_writer.ternary( dot( coneDirection, wsNormal ) < 0.0_f, -1.0_f, 1.0_f );

					radiance += m_traceCone( voxels
						, wsPosition
						, wsNormal
						, coneDirection
						, sdw::Float{ castor::Angle::fromRadians( castor::PiDiv2< float > / 3 ).tan() }
						, voxelData );
				}
				ROF;

				// final radiance is average of all the cones radiances
				radiance *= voxelData.radianceNumConesInv;
				radiance.a() = clamp( radiance.a(), 0.0_f, 1.0_f );

				m_writer.returnStmt( max( vec4( 0.0_f ), radiance ) );
			}
			, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
			, sdw::InVec3{ m_writer, "wsPosition" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, InVoxelData{ m_writer, "voxelData" } );

		m_traceConeReflection = m_writer.implementFunction< sdw::Vec4 >( "traceConeReflection"
			, [&]( sdw::SampledImage3DRgba32 const & voxels
				, sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsNormal
				, sdw::Vec3 const & wsViewVector
				, sdw::Float const & roughness
				, shader::VoxelData const & voxelData )
			{
				auto aperture = m_writer.declLocale( "aperture"
					, tan( roughness * sdw::Float{ castor::PiDiv2< float > / 10 } ) );
				auto coneDirection = m_writer.declLocale( "coneDirection"
					, reflect( -wsViewVector, wsNormal ) );

				auto reflection = m_writer.declLocale( "reflection"
					, m_traceCone( voxels
						, wsPosition
						, wsNormal
						, coneDirection
						, aperture
						, voxelData ) );

				m_writer.returnStmt( vec4( max( vec3( 0.0_f ), reflection.rgb() )
					, clamp( reflection.a(), 0.0_f, 1.0_f ) ) );
			}
			, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
			, sdw::InVec3{ m_writer, "wsPosition" }
			, sdw::InVec3{ m_writer, "wsNormal" }
			, sdw::InVec3{ m_writer, "wsViewVector" }
			, sdw::InFloat{ m_writer, "roughness" }
			, InVoxelData{ m_writer, "voxelData" } );
	}

	void Utils::declareVoxelizeFunctions()
	{
		declareIsSaturated();
		declareEncodeColor();
		declareEncodeNormal();
		declareDecodeColor();
		declareDecodeNormal();
		declareFlatten();
		declareUnflatten();
	}

	void Utils::declareParallaxShadowFunc( PipelineFlags const & flags )
	{
		if ( checkFlags( flags.textures, TextureFlag::eHeight ) != flags.textures.end()
			&& ( checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( flags.passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			if ( m_parallaxShadow )
			{
				return;
			}

			m_parallaxShadow = m_writer.implementFunction< sdw::Float >( "ParallaxSoftShadowMultiplier"
				, [&]( sdw::Vec3 const & lightDir
					, sdw::Vec2 const initialTexCoord
					, sdw::Float initialHeight
					, sdw::SampledImage2DRgba32 const & heightMap
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
							, heightMap.sample( currentTextureCoords ).r() );
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
									* ( 1.0_f - m_writer.cast< sdw::Float >( stepIndex ) / numLayers ) );
								shadowMultiplier = max( shadowMultiplier, newShadowMultiplier );
							}
							FI;

							// offset to the next layer
							stepIndex += 1_i;
							currentLayerHeight -= layerHeight;
							currentTextureCoords += texStep;
							heightFromTexture = heightMap.sample( currentTextureCoords ).r();
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
				, sdw::InVec3{ m_writer, "lightDir" }
				, sdw::InVec2{ m_writer, "initialTexCoord" }
				, sdw::InFloat{ m_writer, "initialHeight" }
				, sdw::InSampledImage2DRgba32{ m_writer, "heightMap" }
				, InTextureConfigData{ m_writer, "textureConfig" } );
		}
	}

	void Utils::applyAlphaFunc( VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & alphaRef )const
	{
		if ( alphaFunc != VK_COMPARE_OP_ALWAYS )
		{
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

	sdw::Boolean Utils::isSaturated( sdw::Vec3 const & p )const
	{
		return m_isSaturated3D( p );
	}

	sdw::UInt Utils::encodeColor( sdw::Vec4 const & color )const
	{
		return m_encodeColor( color );
	}

	sdw::UInt Utils::encodeNormal( sdw::Vec3 const & normal )const
	{
		return m_encodeNormal( normal );
	}

	sdw::Vec4 Utils::decodeColor( sdw::UInt const & colorMask )const
	{
		return m_decodeColor( colorMask );
	}

	sdw::Vec3 Utils::decodeNormal( sdw::UInt const & normalMask )const
	{
		return m_decodeNormal( normalMask );
	}

	sdw::UInt Utils::flatten( sdw::UVec3 const & p
		, sdw::UVec3 const & dim )const
	{
		return m_flatten3D( p, dim );
	}

	sdw::UVec3 Utils::unflatten( sdw::UInt const & p
		, sdw::UVec3 const & dim )const
	{
		return m_unflatten3D( p, dim );
	}

	sdw::Vec4 Utils::traceConeRadiance( sdw::SampledImage3DRgba32 const & voxels
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsNormal
		, VoxelData const & voxelData )const
	{
		return m_traceConeRadiance( voxels
			, wsPosition
			, wsNormal
			, voxelData );
	}

	sdw::Vec4 Utils::traceConeReflection( sdw::SampledImage3DRgba32 const & voxels
		, sdw::Vec3 const & wsPosition
		, sdw::Vec3 const & wsNormal
		, sdw::Vec3 const & wsViewVector
		, sdw::Float const & roughness
		, VoxelData const & voxelData )const
	{
		return m_traceConeReflection( voxels
			, wsPosition
			, wsNormal
			, wsViewVector
			, roughness
			, voxelData );
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

	sdw::Vec2 Utils::calcTexCoord( sdw::Vec2 const & renderPos
			, sdw::Vec2 const & renderSize )const
	{
		return m_calcTexCoord( renderPos, renderSize );
	}

	sdw::Vec3 Utils::calcVSPosition( sdw::Vec2 const & uv
		, sdw::Float const & depth
		, sdw::Mat4 const & invProj )const
	{
		return m_calcVSPosition( uv
			, depth
			, invProj );
	}

	sdw::Vec3 Utils::calcWSPosition( sdw::Vec2 const & uv
		, sdw::Float const & depth
		, sdw::Mat4 const & invViewProj )const
	{
		return m_calcWSPosition( uv
			, depth
			, invViewProj );
	}

	sdw::Vec3 Utils::applyGamma( sdw::Float const & gamma
		, sdw::Vec3 const & hdr )const
	{
		return m_applyGamma( gamma, hdr );
	}

	sdw::Vec3 Utils::removeGamma( sdw::Float const & gamma
		, sdw::Vec3 const & srgb )const
	{
		return m_removeGamma( gamma, srgb );
	}

	sdw::Vec3 Utils::getMapNormal( sdw::Vec2 const & uv
		, sdw::Vec3 const & normal
		, sdw::Vec3 const & position )const
	{
		return m_getMapNormal( uv, normal, position );
	}

	sdw::Float Utils::lineariseDepth( sdw::Float const & depth
		, sdw::Float const & nearPlane
		, sdw::Float const & farPlane )const
	{
		return m_lineariseDepth( depth
			, nearPlane
			, farPlane );
	}

	sdw::Vec4 Utils::computeAccumulation( sdw::Float const & depth
		, sdw::Vec3 const & colour
		, sdw::Float const & alpha
		, sdw::Float const & nearPlane
		, sdw::Float const & farPlane
		, sdw::Float const & accumulationOperator )
	{
		return m_computeAccumulation( depth
			, colour
			, alpha
			, nearPlane
			, farPlane
			, accumulationOperator );
	}

	sdw::Vec3 Utils::fresnelSchlick( sdw::Float const & product
		, sdw::Vec3 const & f0
		, sdw::Float const & roughness )const
	{
		return m_fresnelSchlick( product
			, f0
			, roughness );
	}

	sdw::Vec3 Utils::computeMetallicIBL( sdw::Vec3 const & normal
		, sdw::Vec3 const & position
		, sdw::Vec3 const & albedo
		, sdw::Float const & metallic
		, sdw::Float const & roughness
		, sdw::Vec3 const & worldEye
		, sdw::SampledImageCubeRgba32 const & irradianceMap
		, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
		, sdw::SampledImage2DRgba32 const & brdfMap )const
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

	sdw::Vec3 Utils::computeSpecularIBL( sdw::Vec3 const & normal
		, sdw::Vec3 const & position
		, sdw::Vec3 const & diffuse
		, sdw::Vec3 const & specular
		, sdw::Float const & glossiness
		, sdw::Vec3 const & worldEye
		, sdw::SampledImageCubeRgba32 const & irradianceMap
		, sdw::SampledImageCubeRgba32 const & prefilteredEnvMap
		, sdw::SampledImage2DRgba32 const & brdfMap )const
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

	sdw::Mat3 Utils::getTBN( sdw::Vec3 const & normal
		, sdw::Vec3 const & tangent
		, sdw::Vec3 const & bitangent )
	{
		return mat3( normalize( tangent ), normalize( bitangent ), normal );
	}

	void Utils::computeColourMapContribution( TextureFlagsArray const & flags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & colour )
	{
		auto it = checkFlags( flags, TextureFlag::eDiffuse );

		if ( it != flags.end() )
		{
			auto i = uint32_t( std::distance( flags.begin(), it ) );
			auto colourMapConfig = m_writer.declLocale( "colourMapConfig"
				, textureConfigs.getTextureConfiguration(  sdw::UInt( it->id + 1u ) ) );
			auto sampledColour = m_writer.declLocale< sdw::Vec4 >( "sampledColour"
				, maps[i].sample( colourMapConfig.convertUV( m_writer, texCoords.xy() ) ) );
			colour = colourMapConfig.getDiffuse( m_writer, sampledColour, colour, 1.0_f );
		}
	}

	void Utils::computeOpacityMapContribution( TextureFlagsArray const & flags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Float & opacity )
	{
		auto it = checkFlags( flags, TextureFlag::eOpacity );

		if ( it != flags.end() )
		{
			auto i = uint32_t( std::distance( flags.begin(), it ) );
			auto opacityMapConfig = m_writer.declLocale( "opacityMapConfig"
				, textureConfigs.getTextureConfiguration(  sdw::UInt( it->id + 1u ) ) );
			auto sampledOpacity = m_writer.declLocale< sdw::Vec4 >( "sampledOpacity"
				, maps[i].sample( opacityMapConfig.convertUV( m_writer, texCoords.xy() ) ) );
			opacity = opacityMapConfig.getOpacity( m_writer, sampledOpacity, opacity );
		}
	}

	void Utils::computeNormalMapContribution( TextureFlagsArray const & flags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 & texCoords )
	{
		auto it = checkFlags( flags, TextureFlag::eNormal );

		if ( it != flags.end() )
		{
			auto i = uint32_t( std::distance( flags.begin(), it ) );
			auto normalMapConfig = m_writer.declLocale( "normalMapConfig"
				, textureConfigs.getTextureConfiguration(  sdw::UInt( it->id ) ) );
			auto sampledNormal = m_writer.declLocale< sdw::Vec4 >( "sampledNormal"
				, maps[i].sample( normalMapConfig.convertUV( m_writer, texCoords.xy() ) ) );
			auto tbn = m_writer.declLocale( "tbn"
				, shader::Utils::getTBN( normal, tangent, bitangent ) );
			normal = normalMapConfig.getNormal( m_writer, sampledNormal, tbn );
		}
	}

	void Utils::computeHeightMapContribution( TextureFlagsArray const & flags
		, PassFlags const & passFlags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Vec3 const & tangentSpaceViewPosition
		, sdw::Vec3 const & tangentSpaceFragPosition
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 & texCoords )
	{
		auto it = checkFlags( flags, TextureFlag::eHeight );

		if ( it != flags.end()
			&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			auto i = uint32_t( std::distance( flags.begin(), it ) );
			auto heightMapConfig = m_writer.declLocale( "heightMapConfig"
				, textureConfigs.getTextureConfiguration(  sdw::UInt( it->id ) ) );
			texCoords.xy() = parallaxMapping( texCoords.xy()
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, maps[i]
				, heightMapConfig );

			IF( m_writer, texCoords.x() > 1.0_f
				|| texCoords.y() > 1.0_f
				|| texCoords.x() < 0.0_f
				|| texCoords.y() < 0.0_f )
			{
				m_writer.discard();
			}
			FI;
		}
	}

	sdw::Vec4 Utils::computeGeometryMapContribution( TextureFlags const & textureFlags
		, PassFlags const & passFlags
		, std::string const & name
		, shader::TextureConfigData const & config
		, sdw::SampledImage2DRgba32 const & map
		, sdw::Vec3 const & texCoords
		, sdw::Float & opacity
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto result = m_writer.declLocale< sdw::Vec4 >( "result" + name
			, map.sample( config.convertUV( m_writer, texCoords.xy() ) ) );

		if ( checkFlag( textureFlags, TextureFlag::eHeight )
			&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			texCoords.xy() = parallaxMapping( texCoords.xy()
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, map
				, config );

			if ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne ) )
			{
				IF( m_writer, texCoords.x() > 1.0_f
					|| texCoords.y() > 1.0_f
					|| texCoords.x() < 0.0_f
					|| texCoords.y() < 0.0_f )
				{
					m_writer.discard();
				}
				FI;
			}
		}

		if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
		{
			opacity = config.getOpacity( m_writer, result, opacity );
		}

		return result;
	}

	void Utils::computeGeometryMapsContributions( TextureFlagsArray const & flags
		, PassFlags const & passFlags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Float & opacity
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		for ( uint32_t i = 0u; i < flags.size(); ++i )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( i, std::locale{ "C" } ) );
			auto config = m_writer.declLocale( "config" + name
				, textureConfigs.getTextureConfiguration( m_writer.cast<  sdw::UInt >( flags[i].id ) ) );
			computeGeometryMapContribution( flags[i].flags
				, passFlags
				, name
				, config
				, maps[i]
				, texCoords
				, opacity
				, tangentSpaceViewPosition
				, tangentSpaceFragPosition );
		}
	}

	sdw::Vec4 Utils::computeCommonMapContribution( TextureFlags const & textureFlags
		, PassFlags const & passFlags
		, std::string const & name
		, shader::TextureConfigData const & config
		, sdw::SampledImage2DRgba32 const & map
		, sdw::Float const & gamma
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto result = computeGeometryMapContribution( textureFlags
			, passFlags
			, name
			, config
			, map
			, texCoords
			, opacity
			, tangentSpaceViewPosition
			, tangentSpaceFragPosition );

		if ( checkFlag( textureFlags, TextureFlag::eNormal ) )
		{
			auto tbn = m_writer.declLocale( "tbn"
				, shader::Utils::getTBN( normal, tangent, bitangent ) );
			normal = config.getNormal( m_writer, result, tbn );
		}

		if ( checkFlag( textureFlags, TextureFlag::eEmissive ) )
		{
			emissive = config.getEmissive( m_writer, result, emissive, gamma );
		}

		if ( checkFlag( textureFlags, TextureFlag::eOcclusion ) )
		{
			occlusion = config.getOcclusion( m_writer, result, occlusion );
		}

		if ( checkFlag( textureFlags, TextureFlag::eTransmittance ) )
		{
			transmittance = config.getTransmittance( m_writer, result, transmittance );
		}

		return result;
	}

	sdw::Vec4 Utils::computeCommonMapVoxelContribution( TextureFlags const & textureFlags
		, PassFlags const & passFlags
		, std::string const & name
		, shader::TextureConfigData const & config
		, sdw::SampledImage2DRgba32 const & map
		, sdw::Float const & gamma
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion )
	{
		auto result = m_writer.declLocale< sdw::Vec4 >( "result" + name
			, map.sample( config.convertUV( m_writer, texCoords.xy() ) ) );

		if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
		{
			opacity = config.getOpacity( m_writer, result, opacity );
		}

		if ( checkFlag( textureFlags, TextureFlag::eEmissive ) )
		{
			emissive = config.getEmissive( m_writer, result, emissive, gamma );
		}

		if ( checkFlag( textureFlags, TextureFlag::eOcclusion ) )
		{
			occlusion = config.getOcclusion( m_writer, result, occlusion );
		}

		return result;
	}

	void Utils::computeCommonMapsContributions( TextureFlagsArray const & flags
		, PassFlags const & passFlags
		, sdw::Float const & gamma
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::UVec4 > const & textureConfig
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		for ( uint32_t i = 0u; i < flags.size(); ++i )
		{
			auto name = castor::string::stringCast< char >( castor::string::toString( i, std::locale{ "C" } ) );
			auto config = m_writer.declLocale( "config" + name
				, textureConfigs.getTextureConfiguration( m_writer.cast<  sdw::UInt >( flags[i].id ) ) );
			computeCommonMapContribution( flags[i].flags
				, passFlags
				, name
				, config
				, maps[i]
				, gamma
				, texCoords
				, normal
				, tangent
				, bitangent
				, emissive
				, opacity
				, occlusion
				, transmittance
				, tangentSpaceViewPosition
				, tangentSpaceFragPosition );
		}
	}

	sdw::Vec2 Utils::parallaxMapping( sdw::Vec2 const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::SampledImage2DRgba32 const & heightMap
		, TextureConfigData const & textureConfig )
	{
		return m_parallaxMapping( textureConfig.convertUV( m_writer, texCoords.xy() )
			, viewDir
			, heightMap
			, textureConfig );

	}

	void Utils::encodeMaterial( sdw::Int const & receiver
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Int const & envMapIndex
		, sdw::Float const & encoded )const
	{
		m_encodeMaterial( receiver
			, reflection
			, refraction
			, envMapIndex
			, encoded );
	}

	void Utils::decodeMaterial( sdw::Float const & encoded
		, sdw::Int const & receiver
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Int const & envMapIndex )const
	{
		m_decodeMaterial( encoded
			, receiver
			, reflection
			, refraction
			, envMapIndex );
	}

	void Utils::decodeReceiver( sdw::Int & encoded
		, sdw::Int const & receiver )const
	{
		m_decodeReceiver( encoded
			, receiver );
	}
}
