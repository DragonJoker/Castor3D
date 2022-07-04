#include "AtmosphereScattering/AtmosphereWorleyPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace worley
	{
		enum Bindings : uint32_t
		{
			eOutput,
			eCount,
		};

		static castor3d::ShaderPtr getProgram( uint32_t dimension )
		{
			sdw::ComputeWriter writer;

			auto outputTexture = writer.declStorageImg< sdw::WImage3DRgba32 >("outputTexture"
				, uint32_t( Bindings::eOutput )
				, 0u );

			// =====================================================================================
			// Code from Sebastien Hillarie 3d noise generator https://github.com/sebh/TileableVolumeNoise
			auto frequenceMul = writer.declConstantArray( "frequenceMul"
				, std::vector< sdw::Float >{ 2.0_f, 8.0_f, 14.0_f, 20.0_f, 26.0_f, 32.0_f } );

			//Code from https://github.com/NadirRoGue
			//Special thanks https://github.com/NadirRoGue

			auto hash = writer.implementFunction< sdw::Float >( "hash"
				, [&]( sdw::Int const & n )
				{
					writer.returnStmt( fract( sin( writer.cast< sdw::Float >( n ) + 1.951_f ) * 43758.5453123_f ) );
				}
				, sdw::InInt{ writer, "n" } );

			auto noise = writer.implementFunction< sdw::Float >( "noise"
				, [&]( sdw::Vec3 const & x )
				{
					auto p = writer.declLocale( "p"
						, floor( x ) );
					auto f = writer.declLocale( "f"
						, fract( x ) );

					f = f * f * ( vec3( 3.0_f ) - vec3( 2.0_f ) * f );
					auto n = writer.declLocale( "n"
						, p.x() + p.y() * 57.0_f + 113.0_f * p.z() );
					writer.returnStmt( mix(
						mix(
							mix( hash( writer.cast< sdw::Int >( n + 0.0_f ) ), hash( writer.cast< sdw::Int >( n + 1.0_f ) ), f.x() ),
							mix( hash( writer.cast< sdw::Int >( n + 57.0_f ) ), hash( writer.cast< sdw::Int >( n + 58.0_f ) ), f.x() ),
							f.y() ),
						mix(
							mix( hash( writer.cast< sdw::Int >( n + 113.0_f ) ), hash( writer.cast< sdw::Int >( n + 114.0_f ) ), f.x() ),
							mix( hash( writer.cast< sdw::Int >( n + 170.0_f ) ), hash( writer.cast< sdw::Int >( n + 171.0_f ) ), f.x() ),
							f.y() ),
						f.z() ) );
				}
				, sdw::InVec3{ writer, "x" } );

			auto cells = writer.implementFunction< sdw::Float >( "cells"
				, [&]( sdw::Vec3 const & p
					, sdw::Float const & cellCount )
				{
					auto pCell = writer.declLocale( "pCell"
						, p * cellCount );
					auto d = writer.declLocale( "d"
						, 1.0e10_f );

					FOR( writer, sdw::Int, xo, -1_i, xo <= 1_i, xo++ )
					{
						FOR( writer, sdw::Int, yo, -1_i, yo <= 1_i, yo++ )
						{
							FOR( writer, sdw::Int, zo, -1_i, zo <= 1_i, zo++ )
							{
								auto tp = writer.declLocale( "tp"
									, floor( pCell ) + sdw::vec3( writer.cast< sdw::Float >( xo ), writer.cast< sdw::Float >( yo ), writer.cast< sdw::Float >( zo ) ) );

								tp = pCell - tp - noise( mod( tp, vec3( cellCount / 1.0_f ) ) );

								d = min( d, dot( tp, tp ) );
							}
							ROF;
						}
						ROF;
					}
					ROF;

					d = min( d, 1.0_f );
					d = max( d, 0.0_f );

					writer.returnStmt( d );
				}
				, sdw::InVec3{ writer, "p" }
				, sdw::InFloat{ writer, "cellCount" } );


			// From GLM (gtc/noise.hpp & detail/_noise.hpp)
			auto mod289 = writer.implementFunction< sdw::Vec4 >( "mod289"
				, [&]( sdw::Vec4 const & x )
				{
					writer.returnStmt( x - floor( x * vec4( 1.0_f ) / vec4( 289.0_f ) ) * vec4( 289.0_f ) );
				}
				, sdw::InVec4{ writer, "x" } );

			auto permute = writer.implementFunction< sdw::Vec4 >( "permute"
				, [&]( sdw::Vec4 const & x )
				{
					writer.returnStmt( mod289( ( ( x * 34.0_f ) + 1.0_f ) * x ) );
				}
				, sdw::InVec4{ writer, "x" } );

			auto taylorInvSqrt = writer.implementFunction< sdw::Vec4 >( "taylorInvSqrt"
				, [&]( sdw::Vec4 const & r )
				{
						writer.returnStmt( vec4( 1.79284291400159_f ) - vec4( 0.85373472095314_f ) * r );
				}
				, sdw::InVec4{ writer, "r" } );

			auto fade = writer.implementFunction< sdw::Vec4 >( "fade"
				, [&]( sdw::Vec4 const & t )
				{
					writer.returnStmt( ( t * t * t ) * ( t * ( t * vec4( 6.0_f ) - vec4( 15.0_f ) ) + vec4( 10.0_f ) ) );
				}
				, sdw::InVec4{ writer, "t" } );

			auto glmPerlin4D = writer.implementFunction< sdw::Float >( "glmPerlin4D"
				, [&]( sdw::Vec4 const & position
					, sdw::Vec4 const & rep )
				{
					auto Pi0 = writer.declLocale( "Pi0", mod( floor( position ), rep ) );	// Integer part for indexing
					auto Pi1 = writer.declLocale( "Pi1", mod( Pi0 + vec4( 1.0_f ), rep ) );		// Integer part + 1
					//Pi0 = mod(Pi0, vec4(289));
					//Pi1 = mod(Pi1, vec4(289));
					auto Pf0 = writer.declLocale( "Pf0", fract( position ) );	// Fractional part for interpolation
					auto Pf1 = writer.declLocale( "Pf1", Pf0 - vec4( 1.0_f ) );		// Fractional part - 1.0
					auto ix = writer.declLocale( "ix", vec4( Pi0.x(), Pi1.x(), Pi0.x(), Pi1.x() ) );
					auto iy = writer.declLocale( "iy", vec4( Pi0.y(), Pi0.y(), Pi1.y(), Pi1.y() ) );
					auto iz0 = writer.declLocale( "iz0", vec4( Pi0.z() ) );
					auto iz1 = writer.declLocale( "iz1", vec4( Pi1.z() ) );
					auto iw0 = writer.declLocale( "iw0", vec4( Pi0.w() ) );
					auto iw1 = writer.declLocale( "iw1", vec4( Pi1.w() ) );

					auto ixy = writer.declLocale( "ixy", permute( permute( ix ) + iy ) );
					auto ixy0 = writer.declLocale( "ixy0", permute( ixy + iz0 ) );
					auto ixy1 = writer.declLocale( "ixy1", permute( ixy + iz1 ) );
					auto ixy00 = writer.declLocale( "ixy00", permute( ixy0 + iw0 ) );
					auto ixy01 = writer.declLocale( "ixy01", permute( ixy0 + iw1 ) );
					auto ixy10 = writer.declLocale( "ixy10", permute( ixy1 + iw0 ) );
					auto ixy11 = writer.declLocale( "ixy11", permute( ixy1 + iw1 ) );

					auto gx00 = writer.declLocale( "gx00", ixy00 / vec4( 7.0_f ) );
					auto gy00 = writer.declLocale( "gy00", floor( gx00 ) / vec4( 7.0_f ) );
					auto gz00 = writer.declLocale( "gz00", floor( gy00 ) / vec4( 6.0_f ) );
					gx00 = fract( gx00 ) - vec4( 0.5_f );
					gy00 = fract( gy00 ) - vec4( 0.5_f );
					gz00 = fract( gz00 ) - vec4( 0.5_f );
					auto gw00 = writer.declLocale( "gw00", vec4( 0.75_f ) - abs( gx00 ) - abs( gy00 ) - abs( gz00 ) );
					auto sw00 = writer.declLocale( "sw00", step( gw00, vec4( 0.0_f ) ) );
					gx00 -= sw00 * ( step( vec4( 0.0_f ), gx00 ) - vec4( 0.5_f ) );
					gy00 -= sw00 * ( step( vec4( 0.0_f ), gy00 ) - vec4( 0.5_f ) );

					auto gx01 = writer.declLocale( "gx01", ixy01 / vec4( 7.0_f ) );
					auto gy01 = writer.declLocale( "gy01", floor( gx01 ) / vec4( 7.0_f ) );
					auto gz01 = writer.declLocale( "gz01", floor( gy01 ) / vec4( 6.0_f ) );
					gx01 = fract( gx01 ) - vec4( 0.5_f );
					gy01 = fract( gy01 ) - vec4( 0.5_f );
					gz01 = fract( gz01 ) - vec4( 0.5_f );
					auto gw01 = writer.declLocale( "gw01", vec4( 0.75_f ) - abs( gx01 ) - abs( gy01 ) - abs( gz01 ) );
					auto sw01 = writer.declLocale( "sw01", step( gw01, vec4( 0.0_f ) ) );
					gx01 -= sw01 * ( step( vec4( 0.0_f ), gx01 ) - vec4( 0.5_f ) );
					gy01 -= sw01 * ( step( vec4( 0.0_f ), gy01 ) - vec4( 0.5_f ) );

					auto gx10 = writer.declLocale( "gx10", ixy10 / vec4( 7.0_f ) );
					auto gy10 = writer.declLocale( "gy10", floor( gx10 ) / vec4( 7.0_f ) );
					auto gz10 = writer.declLocale( "gz10", floor( gy10 ) / vec4( 6.0_f ) );
					gx10 = fract( gx10 ) - vec4( 0.5_f );
					gy10 = fract( gy10 ) - vec4( 0.5_f );
					gz10 = fract( gz10 ) - vec4( 0.5_f );
					auto gw10 = writer.declLocale( "gw10", vec4( 0.75_f ) - abs( gx10 ) - abs( gy10 ) - abs( gz10 ) );
					auto sw10 = writer.declLocale( "sw10", step( gw10, vec4( 0.0_f ) ) );
					gx10 -= sw10 * ( step( vec4( 0.0_f ), gx10 ) - vec4( 0.5_f ) );
					gy10 -= sw10 * ( step( vec4( 0.0_f ), gy10 ) - vec4( 0.5_f ) );

					auto gx11 = writer.declLocale( "gx11", ixy11 / vec4( 7.0_f ) );
					auto gy11 = writer.declLocale( "gy11", floor( gx11 ) / vec4( 7.0_f ) );
					auto gz11 = writer.declLocale( "gz11", floor( gy11 ) / vec4( 6.0_f ) );
					gx11 = fract( gx11 ) - vec4( 0.5_f );
					gy11 = fract( gy11 ) - vec4( 0.5_f );
					gz11 = fract( gz11 ) - vec4( 0.5_f );
					auto gw11 = writer.declLocale( "gw11", vec4( 0.75_f ) - abs( gx11 ) - abs( gy11 ) - abs( gz11 ) );
					auto sw11 = writer.declLocale( "sw11", step( gw11, vec4( 0.0_f ) ) );
					gx11 -= sw11 * ( step( vec4( 0.0_f ), gx11 ) - vec4( 0.5_f ) );
					gy11 -= sw11 * ( step( vec4( 0.0_f ), gy11 ) - vec4( 0.5_f ) );

					auto g0000 = writer.declLocale( "g0000", vec4( gx00.x(), gy00.x(), gz00.x(), gw00.x() ) );
					auto g1000 = writer.declLocale( "g1000", vec4( gx00.y(), gy00.y(), gz00.y(), gw00.y() ) );
					auto g0100 = writer.declLocale( "g0100", vec4( gx00.z(), gy00.z(), gz00.z(), gw00.z() ) );
					auto g1100 = writer.declLocale( "g1100", vec4( gx00.w(), gy00.w(), gz00.w(), gw00.w() ) );
					auto g0010 = writer.declLocale( "g0010", vec4( gx10.x(), gy10.x(), gz10.x(), gw10.x() ) );
					auto g1010 = writer.declLocale( "g1010", vec4( gx10.y(), gy10.y(), gz10.y(), gw10.y() ) );
					auto g0110 = writer.declLocale( "g0110", vec4( gx10.z(), gy10.z(), gz10.z(), gw10.z() ) );
					auto g1110 = writer.declLocale( "g1110", vec4( gx10.w(), gy10.w(), gz10.w(), gw10.w() ) );
					auto g0001 = writer.declLocale( "g0001", vec4( gx01.x(), gy01.x(), gz01.x(), gw01.x() ) );
					auto g1001 = writer.declLocale( "g1001", vec4( gx01.y(), gy01.y(), gz01.y(), gw01.y() ) );
					auto g0101 = writer.declLocale( "g0101", vec4( gx01.z(), gy01.z(), gz01.z(), gw01.z() ) );
					auto g1101 = writer.declLocale( "g1101", vec4( gx01.w(), gy01.w(), gz01.w(), gw01.w() ) );
					auto g0011 = writer.declLocale( "g0011", vec4( gx11.x(), gy11.x(), gz11.x(), gw11.x() ) );
					auto g1011 = writer.declLocale( "g1011", vec4( gx11.y(), gy11.y(), gz11.y(), gw11.y() ) );
					auto g0111 = writer.declLocale( "g0111", vec4( gx11.z(), gy11.z(), gz11.z(), gw11.z() ) );
					auto g1111 = writer.declLocale( "g1111", vec4( gx11.w(), gy11.w(), gz11.w(), gw11.w() ) );

					auto norm00 = writer.declLocale( "norm00"
						, taylorInvSqrt( vec4( dot( g0000, g0000 ), dot( g0100, g0100 ), dot( g1000, g1000 ), dot( g1100, g1100 ) ) ) );
					g0000 *= norm00.x();
					g0100 *= norm00.y();
					g1000 *= norm00.z();
					g1100 *= norm00.w();

					auto norm01 = writer.declLocale( "norm01"
						, taylorInvSqrt( vec4( dot( g0001, g0001 ), dot( g0101, g0101 ), dot( g1001, g1001 ), dot( g1101, g1101 ) ) ) );
					g0001 *= norm01.x();
					g0101 *= norm01.y();
					g1001 *= norm01.z();
					g1101 *= norm01.w();

					auto norm10 = writer.declLocale( "norm10"
						, taylorInvSqrt( vec4( dot( g0010, g0010 ), dot( g0110, g0110 ), dot( g1010, g1010 ), dot( g1110, g1110 ) ) ) );
					g0010 *= norm10.x();
					g0110 *= norm10.y();
					g1010 *= norm10.z();
					g1110 *= norm10.w();

					auto norm11 = writer.declLocale( "norm11"
						, taylorInvSqrt( vec4( dot( g0011, g0011 ), dot( g0111, g0111 ), dot( g1011, g1011 ), dot( g1111, g1111 ) ) ) );
					g0011 *= norm11.x();
					g0111 *= norm11.y();
					g1011 *= norm11.z();
					g1111 *= norm11.w();

					auto n0000 = writer.declLocale( "n0000", dot( g0000, Pf0 ) );
					auto n1000 = writer.declLocale( "n1000", dot( g1000, vec4( Pf1.x(), Pf0.y(), Pf0.z(), Pf0.w() ) ) );
					auto n0100 = writer.declLocale( "n0100", dot( g0100, vec4( Pf0.x(), Pf1.y(), Pf0.z(), Pf0.w() ) ) );
					auto n1100 = writer.declLocale( "n1100", dot( g1100, vec4( Pf1.x(), Pf1.y(), Pf0.z(), Pf0.w() ) ) );
					auto n0010 = writer.declLocale( "n0010", dot( g0010, vec4( Pf0.x(), Pf0.y(), Pf1.z(), Pf0.w() ) ) );
					auto n1010 = writer.declLocale( "n1010", dot( g1010, vec4( Pf1.x(), Pf0.y(), Pf1.z(), Pf0.w() ) ) );
					auto n0110 = writer.declLocale( "n0110", dot( g0110, vec4( Pf0.x(), Pf1.y(), Pf1.z(), Pf0.w() ) ) );
					auto n1110 = writer.declLocale( "n1110", dot( g1110, vec4( Pf1.x(), Pf1.y(), Pf1.z(), Pf0.w() ) ) );
					auto n0001 = writer.declLocale( "n0001", dot( g0001, vec4( Pf0.x(), Pf0.y(), Pf0.z(), Pf1.w() ) ) );
					auto n1001 = writer.declLocale( "n1001", dot( g1001, vec4( Pf1.x(), Pf0.y(), Pf0.z(), Pf1.w() ) ) );
					auto n0101 = writer.declLocale( "n0101", dot( g0101, vec4( Pf0.x(), Pf1.y(), Pf0.z(), Pf1.w() ) ) );
					auto n1101 = writer.declLocale( "n1101", dot( g1101, vec4( Pf1.x(), Pf1.y(), Pf0.z(), Pf1.w() ) ) );
					auto n0011 = writer.declLocale( "n0011", dot( g0011, vec4( Pf0.x(), Pf0.y(), Pf1.z(), Pf1.w() ) ) );
					auto n1011 = writer.declLocale( "n1011", dot( g1011, vec4( Pf1.x(), Pf0.y(), Pf1.z(), Pf1.w() ) ) );
					auto n0111 = writer.declLocale( "n0111", dot( g0111, vec4( Pf0.x(), Pf1.y(), Pf1.z(), Pf1.w() ) ) );
					auto n1111 = writer.declLocale( "n1111", dot( g1111, Pf1 ) );

					auto fade_xyzw = writer.declLocale( "fade_xyzw", fade( Pf0 ) );
					auto n_0w = writer.declLocale( "n_0w", mix( vec4( n0000, n1000, n0100, n1100 ), vec4( n0001, n1001, n0101, n1101 ), vec4( fade_xyzw.w() ) ) );
					auto n_1w = writer.declLocale( "n_1w", mix( vec4( n0010, n1010, n0110, n1110 ), vec4( n0011, n1011, n0111, n1111 ), vec4( fade_xyzw.w() ) ) );
					auto n_zw = writer.declLocale( "n_zw", mix( n_0w, n_1w, vec4( fade_xyzw.z() ) ) );
					auto n_yzw = writer.declLocale( "n_yzw", mix( vec2( n_zw.x(), n_zw.y() ), vec2( n_zw.z(), n_zw.w() ), vec2( fade_xyzw.y() ) ) );
					auto n_xyzw = writer.declLocale( "n_xyzw", mix( n_yzw.x(), n_yzw.y(), fade_xyzw.x() ) );
					writer.returnStmt( 2.2_f * n_xyzw );
				}
				, sdw::InVec4{ writer, "position" }
				, sdw::InVec4{ writer, "rep" } );

			// ======================================================================

			auto worleyNoise3D = writer.implementFunction< sdw::Float >( "worleyNoise3D"
				, [&]( sdw::Vec3 const & p
					, sdw::Float const & cellCount )
				{
					writer.returnStmt( cells( p, cellCount ) );
				}
				, sdw::InVec3{ writer, "p" }
				, sdw::InFloat{ writer, "cellCount" } );

			auto perlinNoise3D = writer.implementFunction< sdw::Float >( "perlinNoise3D"
				, [&]( sdw::Vec3 const & pIn
					, sdw::Float frequency
					, sdw::UInt const & octaveCount )
				{
					auto octaveFrenquencyFactor = writer.declLocale( "octaveFrenquencyFactor"
						, 2.0_f );			// noise frequency factor between octave, forced to 2

					// Compute the sum for each octave
					auto sum = writer.declLocale( "sum"
						, 0.0_f );
					auto weightSum = writer.declLocale( "weightSum"
						, 0.0_f );
					auto weight = writer.declLocale( "weight"
						, 0.5_f );

					FOR( writer, sdw::UInt, oct, 0_u, oct < octaveCount, ++oct )
					{
						// Perlin vec3 is bugged in GLM on the Z axis :(, black stripes are visible
						// So instead we use 4d Perlin and only use xyz...
						//glm::vec3 p(x * freq, y * freq, z * freq);
						//float val = glm::perlin(p, glm::vec3(freq)) *0.5 + 0.5;

						auto p = writer.declLocale( "p"
							, vec4( pIn.x(), pIn.y(), pIn.z(), 0.0_f ) * vec4( frequency ) );
						auto val = writer.declLocale( "val"
							, glmPerlin4D( p, vec4( frequency ) ) );

						sum += val * weight;
						weightSum += weight;

						weight *= weight;
						frequency *= octaveFrenquencyFactor;
					}
					ROF;

					auto noise = writer.declLocale( "pCell"
						, ( sum / weightSum ) * 0.5_f + 0.5_f );
					noise = min( noise, 1.0_f );
					noise = max( noise, 0.0_f );
					writer.returnStmt( noise );
				}
				, sdw::InVec3{ writer, "pIn" }
				, sdw::InFloat{ writer, "frequency" }
				, sdw::InUInt{ writer, "octaveCount" } );

			auto stackable3DNoise = writer.implementFunction< sdw::Vec4 >( "stackable3DNoise"
				, [&]( sdw::IVec3 const & pixel )
				{
					auto coord = writer.declLocale( "coord"
						, vec3( writer.cast< sdw::Float >( pixel.x() ) / float( dimension )
							, writer.cast< sdw::Float >( pixel.y() ) / float( dimension )
							, writer.cast< sdw::Float >( pixel.z() ) / float( dimension ) ) );

					// 3 octaves
					auto cellCount = writer.declLocale( "cellCount"
						, 2.0_f );
					auto worleyNoise0 = writer.declLocale( "worleyNoise0"
						, ( 1.0_f - worleyNoise3D( coord, cellCount * 1.0_f ) ) );
					auto worleyNoise1 = writer.declLocale( "worleyNoise1"
						, ( 1.0_f - worleyNoise3D( coord, cellCount * 2.0_f ) ) );
					auto worleyNoise2 = writer.declLocale( "worleyNoise2"
						, ( 1.0_f - worleyNoise3D( coord, cellCount * 4.0_f ) ) );
					auto worleyNoise3 = writer.declLocale( "worleyNoise3"
						, ( 1.0_f - worleyNoise3D( coord, cellCount * 8.0_f ) ) );
					auto worleyFBM0 = writer.declLocale( "worleyFBM0"
						, worleyNoise0 * 0.625_f + worleyNoise1 * 0.25_f + worleyNoise2 * 0.125_f );
					auto worleyFBM1 = writer.declLocale( "worleyFBM1"
						, worleyNoise1 * 0.625_f + worleyNoise2 * 0.25_f + worleyNoise3 * 0.125_f );
					auto worleyFBM2 = writer.declLocale( "worleyFBM2"
						, worleyNoise2 * 0.75_f + worleyNoise3 * 0.25_f );
					// cellCount=4 -> worleyNoise4 is just noise due to sampling frequency=texel freque. So only take into account 2 frequencies for FBM

					writer.returnStmt( vec4( worleyFBM0, worleyFBM1, worleyFBM2, 1.0_f ) );
				}
				, sdw::InIVec3{ writer, "pixel" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 4u, 4u, 4u }
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, ivec3( in.globalInvocationID.xyz() ) );

					outputTexture.store( pixel, stackable3DNoise( pixel ) );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereWorleyPass::AtmosphereWorleyPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & resultView
		, bool & enabled )
		: m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "WorleyPass", worley::getProgram( getExtent( resultView ).width ) }
		, m_stages{ makeShaderState( device, m_computeShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & computePass = graph.createPass( "WorleyPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::ComputePass >( framePass
					, context
					, graph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( renderSize.width / 4u )
						.groupCountY( renderSize.height / 4u )
						.groupCountZ( renderSize.depth / 4u )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		computePass.addDependencies( previousPasses );
		computePass.addOutputStorageView( resultView
			, worley::eOutput );
		auto & mipsPass = graph.createPass( "WorleyMipsGenPass"
			, [&device, &enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::GenerateMipmaps >( framePass
					, context
					, graph
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [&enabled](){ return enabled; } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		mipsPass.addDependency( computePass );
		mipsPass.addTransferInOutView( resultView );
		m_lastPass = &mipsPass;
	}

	void AtmosphereWorleyPass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
