#include "Castor3D/Shader/Shaders/GlslVoxelConeTracing.hpp"

#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/Source.hpp>

#include <ShaderAST/Expr/ExprComma.hpp>

namespace castor3d::shader
{
	using namespace sdw;

	//***********************************************************************************************

	VoxelConeTracing::VoxelConeTracing( ShaderWriter & writer
		, Utils const & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	void VoxelConeTracing::declare()
	{
		if ( m_traceCone )
		{
			return;
		}

		m_traceCone = m_writer.implementFunction< Vec4 >( "traceCone"
			, [&]( SampledImage3DRgba32 const & voxels
				, Vec3 const & wsPosition
				, Vec3 const & wsNormal
				, Vec3 const & wsConeDirection
				, Float const & coneAperture
				, VoxelData const & voxelData )
			{
				auto color = m_writer.declLocale( "color"
					, vec3( 0.0_f ) );
				auto alpha = m_writer.declLocale( "alpha"
					, 0.0_f );

				// We need to offset the cone start position to avoid sampling its own voxel (self-occlusion):
				//	Unfortunately, it will result in disconnection between nearby surfaces :(
				auto dist = m_writer.declLocale( "dist"
					, Float{ voxelData.size } ); // offset by cone dir so that first sample of all cones are not the same
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
					IF( m_writer, !m_utils.isSaturated( tc ) || mip >= voxelData.radianceMips )
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
			, InSampledImage3DRgba32{ m_writer, "voxels" }
			, InVec3{ m_writer, "wsPosition" }
			, InVec3{ m_writer, "wsNormal" }
			, InVec3{ m_writer, "wsConeDirection" }
			, InFloat{ m_writer, "coneAperture" }
			, InVoxelData{ m_writer, "voxelData" } );

		auto cones = m_writer.declConstantArray( "cones"
			, std::vector< Vec3 >{ vec3( 0.57735_f, 0.57735, 0.57735 )
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

		m_traceConeRadiance = m_writer.implementFunction< Vec4 >( "traceConeRadiance"
			, [&]( SampledImage3DRgba32 const & voxels
				, Vec3 const & wsPosition
				, Vec3 const & wsNormal
				, VoxelData const & voxelData )
			{
				auto radiance = m_writer.declLocale( "radiance"
					, vec4( 0.0_f ) );

				FOR( m_writer, UInt, cone, 0_u, cone < voxelData.radianceNumCones, ++cone ) // quality is between 1 and 16 cones
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
						, Float{ castor::Angle::fromRadians( castor::PiDiv2< float > / 3 ).tan() }
						, voxelData );
				}
				ROF;

				// final radiance is average of all the cones radiances
				radiance *= voxelData.radianceNumConesInv;
				radiance.a() = clamp( radiance.a(), 0.0_f, 1.0_f );

				m_writer.returnStmt( max( vec4( 0.0_f ), radiance ) );
			}
			, InSampledImage3DRgba32{ m_writer, "voxels" }
			, InVec3{ m_writer, "wsPosition" }
			, InVec3{ m_writer, "wsNormal" }
			, InVoxelData{ m_writer, "voxelData" } );

		m_traceConeReflection = m_writer.implementFunction< Vec4 >( "traceConeReflection"
			, [&]( SampledImage3DRgba32 const & voxels
				, Vec3 const & wsPosition
				, Vec3 const & wsNormal
				, Vec3 const & wsViewVector
				, Float const & roughness
				, VoxelData const & voxelData )
			{
				auto aperture = m_writer.declLocale( "aperture"
					, tan( roughness * Float{ castor::PiDiv2< float > / 10 } ) );
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
			, InSampledImage3DRgba32{ m_writer, "voxels" }
			, InVec3{ m_writer, "wsPosition" }
			, InVec3{ m_writer, "wsNormal" }
			, InVec3{ m_writer, "wsViewVector" }
			, InFloat{ m_writer, "roughness" }
			, InVoxelData{ m_writer, "voxelData" } );
	}

	Vec4 VoxelConeTracing::traceConeRadiance( SampledImage3DRgba32 const & voxels
		, Vec3 const & wsPosition
		, Vec3 const & wsNormal
		, VoxelData const & voxelData )const
	{
		return m_traceConeRadiance( voxels
			, wsPosition
			, wsNormal
			, voxelData );
	}

	Vec4 VoxelConeTracing::traceConeReflection( SampledImage3DRgba32 const & voxels
		, Vec3 const & wsPosition
		, Vec3 const & wsNormal
		, Vec3 const & wsViewVector
		, Float const & roughness
		, VoxelData const & voxelData )const
	{
		return m_traceConeReflection( voxels
			, wsPosition
			, wsNormal
			, wsViewVector
			, roughness
			, voxelData );
	}
}
