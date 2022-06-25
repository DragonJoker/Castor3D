#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	Utils::Utils( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	LightingModelPtr Utils::createLightingModel( Engine const & engine
		, castor::String const & name
		, ShadowOptions shadowsOptions
		, SssProfiles const * sssProfiles
		, bool enableVolumetric )
	{
		return engine.getLightingModelFactory().create( name
			, m_writer
			, *this
			, std::move( shadowsOptions )
			, sssProfiles
			, enableVolumetric );
	}

	void Utils::declareSwap1F()
	{
		if ( m_swap1F )
		{
			return;
		}

		m_swap1F = m_writer.implementFunction< sdw::Void >( "c3d_swap1F"
			, [&]( sdw::Float A
				, sdw::Float B )
			{
				auto tmp = m_writer.declLocale( "tmp", A );
				A = B;
				B = tmp;
			}
			, sdw::InOutFloat{ m_writer, "A" }
			, sdw::InOutFloat{ m_writer, "B" } );
	}

	void Utils::declareDistanceSquared2F()
	{
		if ( m_distanceSquared2F )
		{
			return;
		}

		m_distanceSquared2F = m_writer.implementFunction< sdw::Float >( "c3d_distanceSquared2F"
			, [&]( sdw::Vec2 A
				, sdw::Vec2 const & B )
			{
				A -= B;
				m_writer.returnStmt( dot( A, A ) );
			}
			, sdw::InVec2{ m_writer, "A" }
			, sdw::InVec2{ m_writer, "B" } );
	}

	void Utils::declareDistanceSquared3F()
	{
		if ( m_distanceSquared3F )
		{
			return;
		}

		m_distanceSquared3F = m_writer.implementFunction< sdw::Float >( "c3d_distanceSquared3F"
			, [&]( sdw::Vec3 A
				, sdw::Vec3 const & B )
			{
				A -= B;
				m_writer.returnStmt( dot( A, A ) );
			}
			, sdw::InVec3{ m_writer, "A" }
			, sdw::InVec3{ m_writer, "B" } );
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
				auto c3d_mapNormal( m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapNormal" ) );

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

		m_invertVec4Y = m_writer.implementFunction< sdw::Vec4 >( "c3d_invertVec4Y"
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

		m_negateVec4Y = m_writer.implementFunction< sdw::Vec4 >( "c3d_negateVec4Y"
			, [&]( sdw::Vec4 const & v )
			{
				m_writer.returnStmt( vec3( v.x(), -v.y(), v.z(), v.w() ) );
			}
			, sdw::InVec4{ m_writer, "v" } );
	}

	void Utils::declareParallaxMappingFunc()
	{
		if ( m_parallaxMapping )
		{
			return;
		}

		m_parallaxMapping = m_writer.implementFunction< sdw::Vec2 >( "c3d_parallaxMapping",
			[&]( sdw::Vec2 const & texCoords
				, sdw::Vec3 const & viewDir
				, sdw::CombinedImage2DRgba32 const & heightMap
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
			, sdw::InCombinedImage2DRgba32{ m_writer, "heightMap" }
			, InTextureConfigData{ m_writer, "textureConfig" } );
	}

	void Utils::declareParallaxShadowFunc()
	{
		if ( m_parallaxShadow )
		{
			return;
		}

		m_parallaxShadow = m_writer.implementFunction< sdw::Float >( "c3d_parallaxSoftShadowMultiplier"
			, [&]( sdw::Vec3 const & lightDir
				, sdw::Vec2 const initialTexCoord
				, sdw::Float initialHeight
				, sdw::CombinedImage2DRgba32 const & heightMap
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
			, sdw::InCombinedImage2DRgba32{ m_writer, "heightMap" }
			, InTextureConfigData{ m_writer, "textureConfig" } );
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
					, m_writer.cast< sdw::Float >( ( colorMask >> 24u ) & 0x0000007f_u ) );
				color.r() = m_writer.cast< sdw::Float >( ( colorMask >> 16u ) & 0x000000ff_u );
				color.g() = m_writer.cast< sdw::Float >( ( colorMask >> 8u ) & 0x000000ff_u );
				color.b() = m_writer.cast< sdw::Float >( colorMask & 0x000000ff_u );

				hdr /= 127.0_f;
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
					, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 18_u ) & 0x000000ff_u )
						, m_writer.cast< sdw::Int >( ( normalMask >> 9_u ) & 0x000000ff_u )
						, m_writer.cast< sdw::Int >( normalMask & 0x000000ff_u ) ) );
				auto iNormalSigns = m_writer.declLocale( "iNormalSigns"
					, ivec3( m_writer.cast< sdw::Int >( ( normalMask >> 25_u ) & 0x00000002_u )
						, m_writer.cast< sdw::Int >( ( normalMask >> 16_u ) & 0x00000002_u )
						, m_writer.cast< sdw::Int >( ( normalMask >> 7_u ) & 0x00000002_u ) ) );
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

	void Utils::declareClipToScreen()
	{
		if ( m_clipToScreen )
		{
			return;
		}

		auto epsilon = m_writer.declConstant( "epsilon", 0.00001_f );

		m_clipToScreen = m_writer.implementFunction< sdw::Vec4 >( "c3d_clipToScreen"
			, [&]( sdw::Vec4 const & in )
			{
				auto out = m_writer.declLocale( "out", in );

				IF( m_writer, abs( out.w() ) < epsilon )
				{
					out.w() = epsilon;
				}
				FI;

				out.xyz() /= out.w();
				out.xyz() = fma( out.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) );
				m_writer.returnStmt( out );
			}
			, sdw::InVec4{ m_writer, "in" } );
	}

	void Utils::swap( sdw::Float const A, sdw::Float const & B )
	{
		declareSwap1F();
		m_swap1F( A, B );
	}

	sdw::Float Utils::saturate( sdw::Float const v )
	{
		return clamp( v, 0.0_f, 1.0_f );
	}

	sdw::Vec3 Utils::saturate( sdw::Vec3 const v )
	{
		return clamp( v, vec3( 0.0_f ), vec3( 1.0_f ) );
	}

	sdw::Float Utils::distanceSquared( sdw::Vec2 const A, sdw::Vec2 const & B )
	{
		declareDistanceSquared2F();
		return m_distanceSquared2F( A, B );
	}

	sdw::Float Utils::distanceSquared( sdw::Vec3 const A, sdw::Vec3 const & B )
	{
		declareDistanceSquared3F();
		return m_distanceSquared3F( A, B );
	}

	sdw::Vec2 Utils::topDownToBottomUp( sdw::Vec2 const & texCoord )
	{
		declareInvertVec2Y();
		return m_invertVec2Y( texCoord );
	}

	sdw::Vec3 Utils::topDownToBottomUp( sdw::Vec3 const & texCoord )
	{
		declareInvertVec3Y();
		return m_invertVec3Y( texCoord );
	}

	sdw::Vec4 Utils::topDownToBottomUp( sdw::Vec4 const & texCoord )
	{
		declareInvertVec4Y();
		return m_invertVec4Y( texCoord );
	}

	sdw::Vec2 Utils::negateTopDownToBottomUp( sdw::Vec2 const & texCoord )
	{
		declareNegateVec2Y();
		return m_negateVec2Y( texCoord );
	}

	sdw::Vec3 Utils::negateTopDownToBottomUp( sdw::Vec3 const & texCoord )
	{
		declareNegateVec3Y();
		return m_negateVec3Y( texCoord );
	}

	sdw::Vec4 Utils::negateTopDownToBottomUp( sdw::Vec4 const & texCoord )
	{
		declareNegateVec4Y();
		return m_negateVec4Y( texCoord );
	}

	sdw::Vec2 Utils::calcTexCoord( sdw::Vec2 const & renderPos
			, sdw::Vec2 const & renderSize )
	{
		declareCalcTexCoord();
		return m_calcTexCoord( renderPos, renderSize );
	}

	sdw::Vec3 Utils::calcVSPosition( sdw::Vec2 const & uv
		, sdw::Float const & depth
		, sdw::Mat4 const & invProj )
	{
		declareCalcVSPosition();
		return m_calcVSPosition( uv
			, depth
			, invProj );
	}

	sdw::Vec3 Utils::calcWSPosition( sdw::Vec2 const & uv
		, sdw::Float const & depth
		, sdw::Mat4 const & invViewProj )
	{
		declareCalcWSPosition();
		return m_calcWSPosition( uv
			, depth
			, invViewProj );
	}

	sdw::Vec3 Utils::applyGamma( sdw::Float const & gamma
		, sdw::Vec3 const & hdr )
	{
		declareApplyGamma();
		return m_applyGamma( gamma, hdr );
	}

	sdw::Vec3 Utils::removeGamma( sdw::Float const & gamma
		, sdw::Vec3 const & srgb )
	{
		declareRemoveGamma();
		return m_removeGamma( gamma, srgb );
	}

	sdw::Vec3 Utils::getMapNormal( sdw::Vec2 const & uv
		, sdw::Vec3 const & normal
		, sdw::Vec3 const & position )
	{
		declareGetMapNormal();
		return m_getMapNormal( uv, normal, position );
	}

	sdw::Float Utils::rescaleDepth( sdw::Float const & depth
		, sdw::Float const & nearPlane
		, sdw::Float const & farPlane )
	{
		declareRescaleDepth();
		return m_rescaleDepth( depth
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
		declareComputeAccumulation();
		return m_computeAccumulation( depth
			, colour
			, alpha
			, nearPlane
			, farPlane
			, accumulationOperator );
	}

	sdw::Vec3 Utils::fresnelSchlick( sdw::Float const & product
		, sdw::Vec3 const & f0 )
	{
		declareFresnelSchlick();
		return m_fresnelSchlick( product, f0 );
	}

	void Utils::compute2DMapsContributions( FilteredTextureFlags const & flags
		, TextureConfigurations const & textureConfigs
		, TextureAnimations const & textureAnims
		, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
		, sdw::Vec3 const & texCoords
		, sdw::Vec3 & colour
		, sdw::Float & opacity )
	{
		if ( !textureConfigs.isEnabled() )
		{
			return;
		}

		for ( auto & textureIt : flags )
		{
			if ( checkFlag( textureIt.second.flags, TextureFlag::eColour )
				|| checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
			{
				auto i = textureIt.first;
				auto name = castor::string::stringCast< char >( castor::string::toString( i ) );
				CU_Require( textureIt.second.id > 0u );
				auto config = m_writer.declLocale( "c3d_config" + name
					, textureConfigs.getTextureConfiguration( sdw::UInt( textureIt.second.id ) ) );
				auto anim = m_writer.declLocale( "c3d_anim" + name
					, textureAnims.getTextureAnimation( sdw::UInt( textureIt.second.id ) ) );
				auto texCoord = m_writer.declLocale( "c3d_texCoord" + name
					, texCoords.xy() );
				config.transformUV( anim, texCoord );
				auto sampled = m_writer.declLocale< sdw::Vec4 >( "c3d_sampled" + name
					, maps[i].sample( texCoord ) );

				if ( checkFlag( textureIt.second.flags, TextureFlag::eColour ) )
				{
					colour = config.getColour( sampled, colour );
				}

				if ( checkFlag( textureIt.second.flags, TextureFlag::eOpacity ) )
				{
					opacity = config.getOpacity( sampled, opacity );
				}
			}
		}
	}

	sdw::Vec2 Utils::parallaxMapping( sdw::Vec2 const & texCoords
		, sdw::Vec3 const & viewDir
		, sdw::CombinedImage2DRgba32 const & heightMap
		, TextureConfigData const & textureConfig )
	{
		declareParallaxMappingFunc();
		return m_parallaxMapping( texCoords.xy()
			, viewDir
			, heightMap
			, textureConfig );
	}

	sdw::Vec4 Utils::clipToScreen( sdw::Vec4 const & in )
	{
		declareClipToScreen();
		return m_clipToScreen( in );
	}

	sdw::Boolean Utils::isSaturated( sdw::Vec3 const & p )
	{
		declareIsSaturated();
		return m_isSaturated3D( p );
	}

	sdw::Boolean Utils::isSaturated( sdw::IVec3 const & p
		, sdw::Int const & imax )
	{
		declareIsSaturatedImg();
		return m_isSaturated3DImg( p, imax );
	}

	sdw::UInt Utils::encodeColor( sdw::Vec4 const & color )
	{
		declareEncodeColor();
		return m_encodeColor( color );
	}

	sdw::UInt Utils::encodeNormal( sdw::Vec3 const & normal )
	{
		declareEncodeNormal();
		return m_encodeNormal( normal );
	}

	sdw::Vec4 Utils::decodeColor( sdw::UInt const & colorMask )
	{
		declareDecodeColor();
		return m_decodeColor( colorMask );
	}

	sdw::Vec3 Utils::decodeNormal( sdw::UInt const & normalMask )
	{
		declareDecodeNormal();
		return m_decodeNormal( normalMask );
	}

	sdw::UInt Utils::flatten( sdw::UVec3 const & p
		, sdw::UVec3 const & dim )
	{
		declareFlatten();
		return m_flatten3D( p, dim );
	}

	sdw::UVec3 Utils::unflatten( sdw::UInt const & p
		, sdw::UVec3 const & dim )
	{
		declareUnflatten();
		return m_unflatten3D( p, dim );
	}

	sdw::Vec4 Utils::sampleMap( PassFlags const & passFlags
		, sdw::CombinedImage2DRgba32 const & map
		, sdw::Vec2 const & texCoords )
	{
		return ( checkFlag( passFlags, PassFlag::eUntile )
			? sampleUntiled( map, texCoords )
			: map.sample( texCoords ) );
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

	sdw::Vec4 Utils::sampleUntiled( sdw::CombinedImage2DRgba32 const & pmap
		, sdw::Vec2 const & ptexCoords )
	{
		if ( !m_sampleUntiled )
		{
			m_hash4 = m_writer.implementFunction< sdw::Vec4 >( "hash4"
				, [&]( sdw::Vec2 const & p )
				{
					m_writer.returnStmt( fract( sin( vec4( 1.0_f + dot( p, vec2( 37.0_f, 17.0_f ) ),
						2.0_f + dot( p, vec2( 11.0_f, 47.0_f ) ),
						3.0_f + dot( p, vec2( 41.0_f, 29.0_f ) ),
						4.0_f + dot( p, vec2( 23.0_f, 31.0_f ) ) ) ) * 103.0_f ) );
				}
				, sdw::InVec2{ m_writer, "p" } );

			m_sampleUntiled = m_writer.implementFunction< sdw::Vec4 >( "sampleUntiled"
				, [&]( sdw::CombinedImage2DRgba32 const & map
					, sdw::Vec2 const & texCoords )
				{
					auto iuv = m_writer.declLocale( "iuv", floor( texCoords ) );
					auto fuv = m_writer.declLocale( "fuv", fract( texCoords ) );

					// generate per-tile transform
					auto ofa = m_writer.declLocale( "ofa", m_hash4( iuv + vec2( 0.0_f, 0.0_f ) ) );
					auto ofb = m_writer.declLocale( "ofb", m_hash4( iuv + vec2( 1.0_f, 0.0_f ) ) );
					auto ofc = m_writer.declLocale( "ofc", m_hash4( iuv + vec2( 0.0_f, 1.0_f ) ) );
					auto ofd = m_writer.declLocale( "ofd", m_hash4( iuv + vec2( 1.0_f, 1.0_f ) ) );

					auto ddx = m_writer.declLocale( "ddx", dFdx( texCoords ) );
					auto ddy = m_writer.declLocale( "ddy", dFdy( texCoords ) );

					// transform per-tile uvs
					ofa.zw() = sign( ofa.zw() - 0.5_f );
					ofb.zw() = sign( ofb.zw() - 0.5_f );
					ofc.zw() = sign( ofc.zw() - 0.5_f );
					ofd.zw() = sign( ofd.zw() - 0.5_f );

					// uv's, and derivarives (for correct mipmapping)
					auto uva = m_writer.declLocale( "uva", texCoords * ofa.zw() + ofa.xy() );
					auto ddxa = m_writer.declLocale( "ddxa", ddx * ofa.zw() );
					auto ddya = m_writer.declLocale( "ddya", ddy * ofa.zw() );
					auto uvb = m_writer.declLocale( "uvb", texCoords * ofb.zw() + ofb.xy() );
					auto ddxb = m_writer.declLocale( "ddxb", ddx * ofb.zw() );
					auto ddyb = m_writer.declLocale( "ddyb", ddy * ofb.zw() );
					auto uvc = m_writer.declLocale( "uvc", texCoords * ofc.zw() + ofc.xy() );
					auto ddxc = m_writer.declLocale( "ddxc", ddx * ofc.zw() );
					auto ddyc = m_writer.declLocale( "ddyc", ddy * ofc.zw() );
					auto uvd = m_writer.declLocale( "uvd", texCoords * ofd.zw() + ofd.xy() );
					auto ddxd = m_writer.declLocale( "ddxd", ddx * ofd.zw() );
					auto ddyd = m_writer.declLocale( "ddyd", ddy * ofd.zw() );

					// fetch and blend
					auto b = m_writer.declLocale( "b", smoothStep( vec2( 0.25_f ), vec2( 0.75_f ), fuv ) );

					m_writer.returnStmt( mix( mix( map.grad( uva, ddxa, ddya ), map.grad( uvb, ddxb, ddyb ), vec4( b.x() ) )
						, mix( map.grad( uvc, ddxc, ddyc ), map.grad( uvd, ddxd, ddyd ), vec4( b.x() ) )
						, vec4( b.y() ) ) );
				}
				, sdw::InCombinedImage2DRgba32{ m_writer, "map" }
				, sdw::InVec2{ m_writer, "texCoords" } );
		}

		return m_sampleUntiled( pmap, ptexCoords );
	}
}
