#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	uint32_t constexpr ReceiverOffset = 0u;
	uint32_t constexpr RefractionOffset = 1u;
	uint32_t constexpr ReflectionOffset = 2u;
	uint32_t constexpr LightingOffset = 3u;
	uint32_t constexpr EnvMapIndexOffset = 4u;

	Utils::Utils( sdw::ShaderWriter & writer
		, Engine const & engine )
		: m_writer{ writer }
		, m_engine{ engine }
	{
	}

	LightingModelPtr Utils::createLightingModel( castor::String const & name
		, ShadowOptions shadowsOptions
		, bool isOpaqueProgram )
	{
		return m_engine.getLightingModelFactory().create( name
			, m_writer
			, *this
			, std::move( shadowsOptions )
			, isOpaqueProgram );
	}

	void Utils::declareCalcTexCoord()
	{
		if ( m_calcTexCoord )
		{
			return;
		}

		m_calcTexCoord = m_writer.implementFunction< sdw::Vec2 >( "c3d_calcTexCoord"
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

		m_calcVSPosition = m_writer.implementFunction< sdw::Vec3 >( "c3d_calcVSPosition"
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

		m_calcWSPosition = m_writer.implementFunction< sdw::Vec3 >( "c3d_calcWSPosition"
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

		m_applyGamma = m_writer.implementFunction< sdw::Vec3 >( "c3d_applyGamma"
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

		m_removeGamma = m_writer.implementFunction< sdw::Vec3 >( "c3d_removeGamma"
			, [&]( sdw::Float const & gamma
				, sdw::Vec3 const & srgb )
			{
				m_writer.returnStmt( pow( max( srgb, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) ) );
			}
			, sdw::InFloat{ m_writer, "gamma" }
			, sdw::InVec3{ m_writer, "srgb" } );
	}

	void Utils::declareRescaleDepth()
	{
		if ( m_rescaleDepth )
		{
			return;
		}

		m_rescaleDepth = m_writer.implementFunction< sdw::Float >( "c3d_rescaleDepth"
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

		m_getMapNormal = m_writer.implementFunction< sdw::Vec3 >( "c3d_getMapNormal"
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

		declareRescaleDepth();
		m_computeAccumulation = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeAccumulation"
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
						weight = 1.0_f - rescaleDepth( depth
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

		m_fresnelSchlick = m_writer.implementFunction< sdw::Vec3 >( "c3d_fresnelSchlick"
			, [&]( sdw::Float const & product
				, sdw::Vec3 const & f0 )
			{
				auto f90 = clamp( dot( f0, vec3( 50.0_f * 0.33_f ) ), 0.0_f, 1.0_f );
				m_writer.returnStmt( sdw::fma( max( vec3( f90 ), f0 ) - f0
					, vec3( pow( clamp( 1.0_f - product, 0.0_f, 1.0_f ), 5.0_f ) )
					, f0 ) );
			}
			, sdw::InFloat{ m_writer, "product" }
			, sdw::InVec3{ m_writer, "f0" } );
	}

	void Utils::declareInvertVec2Y()
	{
		if ( m_invertVec2Y )
		{
			return;
		}

		m_invertVec2Y = m_writer.implementFunction< sdw::Vec2 >( "c3d_invertVec2Y"
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

		m_invertVec3Y = m_writer.implementFunction< sdw::Vec3 >( "c3d_invertVec3Y"
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

		m_invertVec4Y = m_writer.implementFunction< sdw::Vec4 >( "c3d_invertVec3Y"
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

		m_negateVec2Y = m_writer.implementFunction< sdw::Vec2 >( "c3d_negateVec2Y"
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

		m_negateVec3Y = m_writer.implementFunction< sdw::Vec3 >( "c3d_negateVec3Y"
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

		m_negateVec4Y = m_writer.implementFunction< sdw::Vec4 >( "c3d_negateVec3Y"
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

		m_encodeMaterial = m_writer.implementFunction< sdw::Void >( "c3d_encodeMaterial"
			, [&]( sdw::Int const & receiver
				, sdw::Int const & reflection
				, sdw::Int const & refraction
				, sdw::Int const & lighting
				, sdw::Int const & envMapIndex
				, sdw::Float encoded )
			{
				auto flags = m_writer.declLocale( "flags"
					, 0_u
					+ ( ( m_writer.cast<  sdw::UInt >( receiver ) & 1_u ) << sdw::UInt( ReceiverOffset ) )
					+ ( ( m_writer.cast<  sdw::UInt >( refraction ) & 1_u ) << sdw::UInt( RefractionOffset ) )
					+ ( ( m_writer.cast<  sdw::UInt >( reflection ) & 1_u ) << sdw::UInt( ReflectionOffset ) )
					+ ( ( m_writer.cast<  sdw::UInt >( lighting ) & 1_u ) << sdw::UInt( LightingOffset ) )
					+ ( ( m_writer.cast<  sdw::UInt >( envMapIndex ) << sdw::UInt( EnvMapIndexOffset ) ) ) );
				encoded = m_writer.cast< sdw::Float >( flags );
			}
			, sdw::InInt{ m_writer, "receiver" }
			, sdw::InInt{ m_writer, "reflection" }
			, sdw::InInt{ m_writer, "refraction" }
			, sdw::InInt{ m_writer, "lighting" }
			, sdw::InInt{ m_writer, "envMapIndex" }
			, sdw::OutFloat{ m_writer, "encoded" } );
	}

	void Utils::declareDecodeMaterial()
	{
		if ( m_decodeMaterial )
		{
			return;
		}

		m_decodeMaterial = m_writer.implementFunction< sdw::Void >( "c3d_decodeMaterial"
			, [&]( sdw::Float const & encoded
				, sdw::Int receiver
				, sdw::Int reflection
				, sdw::Int refraction
				, sdw::Int lighting
				, sdw::Int envMapIndex )
			{
				auto flags = m_writer.declLocale( "flags"
					, m_writer.cast<  sdw::UInt >( encoded ) );
				receiver = m_writer.cast< sdw::Int >( ( flags >> sdw::UInt( ReceiverOffset ) ) & 1_u );
				refraction = m_writer.cast< sdw::Int >( ( flags >> sdw::UInt( RefractionOffset ) ) & 1_u );
				reflection = m_writer.cast< sdw::Int >( ( flags >> sdw::UInt( ReflectionOffset ) ) & 1_u );
				lighting = m_writer.cast< sdw::Int >( ( flags >> sdw::UInt( LightingOffset ) ) & 1_u );
				envMapIndex = m_writer.cast< sdw::Int >( ( flags >> sdw::UInt( EnvMapIndexOffset ) ) );
			}
			, sdw::InFloat{ m_writer, "encoded" }
			, sdw::OutInt{ m_writer, "receiver" }
			, sdw::OutInt{ m_writer, "reflection" }
			, sdw::OutInt{ m_writer, "refraction" }
			, sdw::OutInt{ m_writer, "lighting" }
			, sdw::OutInt{ m_writer, "envMapIndex" } );
	}

	void Utils::declareDecodeReceiver()
	{
		if ( m_decodeReceiver )
		{
			return;
		}

		m_decodeReceiver = m_writer.implementFunction< sdw::Void >( "c3d_decodeReceiver"
			, [&]( sdw::Int const & encoded
				, sdw::Int receiver
				, sdw::Int lighting )
			{
				receiver = ( encoded >> sdw::Int( ReceiverOffset ) ) & 1_i;
				lighting = ( encoded >> sdw::Int( LightingOffset ) ) & 1_i;
			}
			, sdw::InInt{ m_writer, "encoded" }
			, sdw::OutInt{ m_writer, "receiver" }
			, sdw::OutInt{ m_writer, "lighting" } );
	}

	void Utils::declareParallaxMappingFunc( PassFlags const & passFlags
		, TextureFlags const & textures )
	{
		if ( checkFlag( textures, TextureFlag::eHeight )
			&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			if ( m_parallaxMapping )
			{
				return;
			}

			m_parallaxMapping = m_writer.implementFunction< sdw::Vec2 >( "c3d_parallaxMapping",
				[&]( sdw::Vec2 const & texCoords
					, sdw::Vec3 const & viewDir
					, sdw::SampledImage2DRgba32 const & heightMap
					, TextureConfigData const & textureConfig )
				{
					IF( m_writer, textureConfig.hgtEnbl == 0.0_f )
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
						, viewDir.xy() / viewDir.z() * textureConfig.hgtFact );
					auto deltaTexCoords = m_writer.declLocale( "deltaTexCoords"
						, p / numLayers );

					auto currentTexCoords = m_writer.declLocale( "currentTexCoords"
						, texCoords );
					auto dx = m_writer.declLocale( "dx"
						, dFdxCoarse( currentTexCoords ) );
					auto dy = m_writer.declLocale( "dy"
						, dFdxCoarse( currentTexCoords ) );
					auto heightIndex = m_writer.declLocale( "heightIndex"
						, m_writer.cast< sdw::UInt >( textureConfig.hgtMask ) );
					auto sampled = m_writer.declLocale( "sampled"
						, heightMap.grad( currentTexCoords, dx, dy ) );
					auto currentDepthMapValue = m_writer.declLocale( "currentDepthMapValue"
						, sampled[heightIndex] );

					WHILE( m_writer, currentLayerDepth < currentDepthMapValue )
					{
						// shift texture coordinates along direction of P
						currentTexCoords -= deltaTexCoords;
						// get depthmap value at current texture coordinates
						sampled = heightMap.grad( currentTexCoords, dx, dy );
						currentDepthMapValue = sampled[heightIndex];
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
					sampled = heightMap.grad( prevTexCoords, dx, dy );
					auto beforeDepth = m_writer.declLocale( "beforeDepth"
						, sampled[heightIndex] - currentLayerDepth + layerDepth );

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

	void Utils::declareParallaxShadowFunc( PassFlags const & passFlags
		, TextureFlags const & textures )
	{
		if ( checkFlag( textures, TextureFlag::eHeight )
			&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) )
		{
			if ( m_parallaxShadow )
			{
				return;
			}

			m_parallaxShadow = m_writer.implementFunction< sdw::Float >( "c3d_parallaxSoftShadowMultiplier"
				, [&]( sdw::Vec3 const & lightDir
					, sdw::Vec2 const initialTexCoord
					, sdw::Float initialHeight
					, sdw::SampledImage2DRgba32 const & heightMap
					, TextureConfigData const & textureConfig )
				{
					IF( m_writer, textureConfig.hgtEnbl == 0.0_f )
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
							, ( lightDir.xy() * textureConfig.hgtFact ) / lightDir.z() / numLayers );

						// current parameters
						auto currentLayerHeight = m_writer.declLocale( "currentLayerHeight"
							, initialHeight - layerHeight );
						auto currentTextureCoords = m_writer.declLocale( "currentTextureCoords"
							, initialTexCoord + texStep );
						auto heightIndex = m_writer.declLocale( "heightIndex"
							, m_writer.cast< sdw::UInt >( textureConfig.hgtMask ) );
						auto sampled = m_writer.declLocale( "sampled"
							, heightMap.sample( currentTextureCoords ) );
						auto heightFromTexture = m_writer.declLocale( "heightFromTexture"
							, sampled[heightIndex] );
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
							sampled = heightMap.sample( currentTextureCoords );
							heightFromTexture = sampled[heightIndex];
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

	void Utils::declareIsSaturated()
	{
		if ( m_isSaturated3D )
		{
			return;
		}

		m_isSaturated3D = m_writer.implementFunction< sdw::Boolean >( "c3d_isSaturated"
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

		m_encodeColor = m_writer.implementFunction<  sdw::UInt >( "c3d_encodeColor"
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

		m_encodeNormal = m_writer.implementFunction<  sdw::UInt >( "c3d_encodeNormal"
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

		m_decodeColor = m_writer.implementFunction< sdw::Vec4 >( "c3d_decodeColor"
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

		m_decodeNormal = m_writer.implementFunction< sdw::Vec3 >( "c3d_decodeNormal"
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

		m_flatten3D = m_writer.implementFunction<  sdw::UInt >( "c3d_flatten3D"
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

		m_unflatten3D = m_writer.implementFunction< sdw::UVec3 >( "c3d_unflatten3D"
			, [&]( sdw::UInt idx
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

	void Utils::declareIsSaturatedImg()
	{
		if ( m_isSaturated3DImg )
		{
			return;
		}

		m_isSaturated3DImg = m_writer.implementFunction< sdw::Boolean >( "c3d_isSaturatedImg"
			, [&]( sdw::IVec3 const & p
				, sdw::Int const & imax )
			{
				m_writer.returnStmt( p.x() == clamp( p.x(), 0_i, imax )
					&& p.y() == clamp( p.y(), 0_i, imax )
					&& p.z() == clamp( p.z(), 0_i, imax ) );
			}
			, sdw::InIVec3{ m_writer, "p" }
			, sdw::InInt{ m_writer, "imax" } );
	}

	void Utils::declareSobelFilterDepth()
	{
		if ( m_sobelFilterDepth )
		{
			return;
		}

		m_sobelFilterDepth = m_writer.implementFunction< sdw::Float >( "c3d_sobelFilterDepth"
			, [&]( sdw::SampledImage2DR32 const & tex
				, sdw::Vec2 const & texSize
				, sdw::Vec2 const & depthRange
				, sdw::Vec2 const & coord
				, sdw::Float const & width )
			{
				auto n = m_writer.declLocale( "n"
					, depthRange.x() );
				auto f = m_writer.declLocale( "f"
					, depthRange.y() );
				auto w = m_writer.declLocale( "w"
					, width / texSize.x() );
				auto h = m_writer.declLocale( "h"
					, width / texSize.y() );

				auto i = m_writer.declLocaleArray< sdw::Float >( "i", 9u );
				i[0] = tex.sample( coord + vec2( -w, -h ) );
				i[1] = tex.sample( coord + vec2( 0.0_f, -h ) );
				i[2] = tex.sample( coord + vec2( w, -h ) );
				i[3] = tex.sample( coord + vec2( -w, 0.0_f ) );
				i[4] = tex.sample( coord );
				i[5] = tex.sample( coord + vec2( w, 0.0_f ) );
				i[6] = tex.sample( coord + vec2( -w, h ) );
				i[7] = tex.sample( coord + vec2( 0.0_f, h ) );
				i[8] = tex.sample( coord + vec2( w, h ) );

				auto sobelEdgeH = m_writer.declLocale( "sobelEdgeH"
					, i[2] + ( 2.0 * i[5] ) + i[8] - ( i[0] + ( 2.0 * i[3] ) + i[6] ) );
				auto sobelEdgeV = m_writer.declLocale( "sobelEdgeV"
					, i[0] + ( 2.0 * i[1] ) + i[2] - ( i[6] + ( 2.0 * i[7] ) + i[8] ) );

				m_writer.returnStmt( sqrt( ( sobelEdgeH * sobelEdgeH ) + ( sobelEdgeV * sobelEdgeV ) ) );
			}
			, sdw::InSampledImage2DR32{ m_writer, "tex" }
			, sdw::InVec2{ m_writer, "texSize" }
			, sdw::InVec2{ m_writer, "depthRange" }
			, sdw::InVec2{ m_writer, "coord" }
			, sdw::InFloat{ m_writer, "width" } );
	}
	
	void Utils::declareSobelFilterNormal()
	{
		if ( m_sobelFilterNormal )
		{
			return;
		}

		m_sobelFilterNormal = m_writer.implementFunction< sdw::Float >( "m_sobelFilterNormal"
			, [&]( sdw::SampledImage2DRgba32 const & tex
				, sdw::Vec2 const & texSize
				, sdw::Vec2 const & coord
				, sdw::Float const & width )
			{
				auto w = m_writer.declLocale( "w"
					, width / texSize.x() );
				auto h = m_writer.declLocale( "h"
					, width / texSize.y() );

				auto A = m_writer.declLocale( "A", tex.sample( coord + vec2( -w, h ) ).xyz() );
				auto B = m_writer.declLocale( "B", tex.sample( coord + vec2( 0.0_f, h ) ).xyz() );
				auto C = m_writer.declLocale( "C", tex.sample( coord + vec2( w, h ) ).xyz() );
				auto D = m_writer.declLocale( "D", tex.sample( coord + vec2( -w, 0.0_f ) ).xyz() );
				auto X = m_writer.declLocale( "X", tex.sample( coord ).xyz() );
				auto E = m_writer.declLocale( "E", tex.sample( coord + vec2( w, 0.0_f ) ).xyz() );
				auto F = m_writer.declLocale( "F", tex.sample( coord + vec2( -w, -h ) ).xyz() );
				auto G = m_writer.declLocale( "G", tex.sample( coord + vec2( 0.0_f, -h ) ).xyz() );
				auto H = m_writer.declLocale( "H", tex.sample( coord + vec2( w, -h ) ).xyz() );

				// compute length of gradient using Sobel/Kroon operator
				auto k0 = m_writer.declLocale( "k0"
					, vec3( sdw::Float{ 17.0f / 23.75f } ) );
				auto k1 = m_writer.declLocale( "k1"
					, vec3( sdw::Float{ 61.0f / 23.75f } ) );
				auto grad_y = m_writer.declLocale( "grad_y"
					, k0 * A + k1 * B + k0 * C - k0 * F - k1 * G - k0 * H );
				auto grad_x = m_writer.declLocale( "grad_x"
					, k0 * C + k1 * E + k0 * H - k0 * A - k1 * D - k0 * F );
				auto g = m_writer.declLocale( "g"
					, length( grad_x ) + length( grad_y ) );

				m_writer.returnStmt( smoothStep( 2.0_f, 3.0_f, g ) );
			}
			, sdw::InSampledImage2DRgba32{ m_writer, "tex" }
			, sdw::InVec2{ m_writer, "texSize" }
			, sdw::InVec2{ m_writer, "coord" }
			, sdw::InFloat{ m_writer, "width" } );
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

	sdw::Float Utils::rescaleDepth( sdw::Float const & depth
		, sdw::Float const & nearPlane
		, sdw::Float const & farPlane )const
	{
		return m_rescaleDepth( depth
			, nearPlane
			, farPlane );
	}

	sdw::Vec4 Utils::computeAccumulation( sdw::Float const & depth
		, sdw::Vec3 const & colour
		, sdw::Float const & alpha
		, sdw::Float const & nearPlane
		, sdw::Float const & farPlane
		, sdw::Float const & accumulationOperator )const
	{
		return m_computeAccumulation( depth
			, colour
			, alpha
			, nearPlane
			, farPlane
			, accumulationOperator );
	}

	sdw::Vec3 Utils::fresnelSchlick( sdw::Float const & product
		, sdw::Vec3 const & f0 )const
	{
		return m_fresnelSchlick( product, f0 );
	}

	void Utils::compute2DMapsContributions( FilteredTextureFlags const & flags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & colour
		, sdw::Float & opacity )
	{
		for ( auto & textureIt : flags )
		{
			if ( checkFlag( textureIt.second.flags, TextureFlag::eColour )
				|| checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
			{
				auto i = textureIt.first;
				auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
				auto config = m_writer.declLocale( "c3d_config" + name
					, textureConfigs.getTextureConfiguration( sdw::UInt( textureIt.second.id ) ) );
				auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
					, texCoords.xy() );
				config.convertUV( m_writer, texCoord );
				auto sampled = m_writer.declLocale< sdw::Vec4 >( "c3d_sampled" + name
					, maps[i].sample( texCoord ) );

				if ( checkFlag( textureIt.second.flags, TextureFlag::eColour ) )
				{
					colour = config.getDiffuse( m_writer, sampled, colour, 1.0_f );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
				{
					opacity = config.getOpacity( m_writer, sampled, opacity );
				}
			}
		}
	}

	void Utils::computeOpacityMapContribution( FilteredTextureFlags const & flags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Float & opacity )
	{
		auto it = checkFlags( flags, TextureFlag::eOpacity );

		if ( it != flags.end() )
		{
			auto i = it->first;
			auto config = m_writer.declLocale( "c3d_opacityMapConfig"
				, textureConfigs.getTextureConfiguration(  sdw::UInt( it->second.id ) ) );
			auto texCoord = m_writer.declLocale( "c3d_texCoordOpacity"
				, texCoords.xy() );
			config.convertUV( m_writer, texCoord );
			auto sampledOpacity = m_writer.declLocale< sdw::Vec4 >( "c3d_sampledOpacity"
				, maps[i].sample( texCoord ) );
			opacity = config.getOpacity( m_writer, sampledOpacity, opacity );
		}
	}

	void Utils::computeGeometryMapsContributions( FilteredTextureFlags const & flags
		, PassFlags const & passFlags
		, TextureConfigurations const & textureConfigs
		, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
		, sdw::Vec3 & texCoords
		, sdw::Float & opacity
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		for ( auto & textureIt : flags )
		{
			if ( isGeometryMap( textureIt.second.flags, passFlags ) )
			{
				auto i = textureIt.first;
				auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
				auto config = m_writer.declLocale( "c3d_configGeom" + name
					, textureConfigs.getTextureConfiguration( m_writer.cast<  sdw::UInt >( textureIt.second.id ) ) );
				doComputeGeometryMapContribution( textureIt.second.flags
					, passFlags
					, name
					, config
					, maps[i]
					, texCoords
					, opacity
					, normal
					, tangent
					, bitangent
					, tangentSpaceViewPosition
					, tangentSpaceFragPosition );
			}
		}
	}

	sdw::Vec4 Utils::computeCommonMapContribution( TextureFlags const & textureFlags
		, PassFlags const & passFlags
		, std::string const & name
		, shader::TextureConfigData const & config
		, sdw::SampledImage2DRgba32 const & map
		, sdw::Float const & gamma
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & emissive
		, sdw::Float & opacity
		, sdw::Float & occlusion
		, sdw::Float & transmittance
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
			, texCoords.xy() );
		config.convertUV( m_writer, texCoord );
		auto result = m_writer.declLocale( "c3d_result" + name
			, map.sample( texCoord ) );

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
		auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
			, texCoords.xy() );
		config.convertUV( m_writer, texCoord );
		auto result = m_writer.declLocale< sdw::Vec4 >( "c3d_result" + name
			, map.sample( texCoord ) );

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

	void Utils::encodeMaterial( sdw::Int const & receiver
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Int const & lighting
		, sdw::Int const & envMapIndex
		, sdw::Float const & encoded )const
	{
		m_encodeMaterial( receiver
			, reflection
			, refraction
			, lighting
			, envMapIndex
			, encoded );
	}

	void Utils::decodeMaterial( sdw::Float const & encoded
		, sdw::Int const & receiver
		, sdw::Int const & reflection
		, sdw::Int const & refraction
		, sdw::Int const & lighting
		, sdw::Int const & envMapIndex )const
	{
		m_decodeMaterial( encoded
			, receiver
			, reflection
			, refraction
			, lighting
			, envMapIndex );
	}

	void Utils::decodeReceiver( sdw::Int & encoded
		, sdw::Int const & receiver
		, sdw::Int const & lighting )const
	{
		m_decodeReceiver( encoded
			, receiver
			, lighting );
	}

	void Utils::applyAlphaFunc( VkCompareOp alphaFunc
		, sdw::Float & alpha
		, sdw::Float const & alphaRef
		, bool opaque )const
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
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_EQUAL:
				IF( m_writer, alpha != alphaRef )
				{
					m_writer.discard();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_LESS_OR_EQUAL:
				IF( m_writer, alpha > alphaRef )
				{
					m_writer.discard();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER:
				IF( m_writer, alpha <= alphaRef )
				{
					m_writer.discard();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_NOT_EQUAL:
				IF( m_writer, alpha == alphaRef )
				{
					m_writer.discard();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
				break;
			case VK_COMPARE_OP_GREATER_OR_EQUAL:
				IF( m_writer, alpha < alphaRef )
				{
					m_writer.discard();
				}
				FI;
				if ( opaque )
				{
					alpha = 1.0_f;
				}
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

	sdw::Boolean Utils::isSaturated( sdw::IVec3 const & p
		, sdw::Int const & imax )const
	{
		return m_isSaturated3DImg( p, imax );
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

	sdw::Float Utils::sobelFilterDepth( sdw::SampledImage2DR32 tex
		, sdw::Vec2 const & texSize
		, sdw::Vec2 const & depthRange
		, sdw::Vec2 const & coord
		, sdw::Float const & width )const
	{
		return m_sobelFilterDepth( tex, texSize, depthRange, coord, width );
	}

	sdw::Float Utils::sobelFilterNormal( sdw::SampledImage2DRgba32 tex
		, sdw::Vec2 const & texSize
		, sdw::Vec2 const & coord
		, sdw::Float const & width )const
	{
		return m_sobelFilterNormal( tex, texSize, coord, width );
	}

	sdw::Mat3 Utils::getTBN( sdw::Vec3 const & normal
		, sdw::Vec3 const & tangent
		, sdw::Vec3 const & bitangent )
	{
		return mat3( normalize( tangent ), normalize( bitangent ), normal );
	}

	bool Utils::isGeometryMap( TextureFlags const & flags
		, PassFlags const & passFlags )
	{
		return checkFlag( flags, TextureFlag::eOpacity )
			|| checkFlag( flags, TextureFlag::eNormal )
			|| ( checkFlag( flags, TextureFlag::eHeight )
				&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
					|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) ) );
	}

	bool Utils::isGeometryOnlyMap( TextureFlags const & flags
		, PassFlags const & passFlags )
	{
		return flags == TextureFlag::eOpacity
			|| flags == TextureFlag::eNormal
			|| flags == TextureFlag::eHeight
			|| flags == ( TextureFlag::eNormal | TextureFlag::eHeight );
	}

	void Utils::doComputeGeometryMapContribution( TextureFlags const & textureFlags
		, PassFlags const & passFlags
		, std::string const & name
		, shader::TextureConfigData const & config
		, sdw::SampledImage2DRgba32 const & map
		, sdw::Vec3 & texCoords
		, sdw::Float & opacity
		, sdw::Vec3 & normal
		, sdw::Vec3 & tangent
		, sdw::Vec3 & bitangent
		, sdw::Vec3 & tangentSpaceViewPosition
		, sdw::Vec3 & tangentSpaceFragPosition )
	{
		auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
			, texCoords.xy() );
		config.convertUV( m_writer, texCoord );

		if ( checkFlag( textureFlags, TextureFlag::eHeight )
			&& ( checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingOne )
				|| checkFlag( passFlags, PassFlag::eParallaxOcclusionMappingRepeat ) )
			&& m_parallaxMapping )
		{
			texCoord = doParallaxMapping( texCoord
				, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
				, map
				, config );
			texCoords.xy() = texCoord;

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

		auto sampled = m_writer.declLocale( "c3d_sampled" + name
			, map.sample( texCoord ) );

		if ( checkFlag( textureFlags, TextureFlag::eNormal ) )
		{
			auto tbn = m_writer.declLocale( "c3d_tbn"
				, shader::Utils::getTBN( normal, tangent, bitangent ) );
			normal = config.getNormal( m_writer, sampled, tbn );
		}

		if ( checkFlag( textureFlags, TextureFlag::eOpacity ) )
		{
			opacity = config.getOpacity( m_writer, sampled, opacity );
		}
	}

	sdw::Vec2 Utils::doParallaxMapping( sdw::Vec2 const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::SampledImage2DRgba32 const & heightMap
		, TextureConfigData const & textureConfig )
	{
		return m_parallaxMapping( texCoords.xy()
			, viewDir
			, heightMap
			, textureConfig );
	}
}
