#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::RetU32Vec3 FroxelsData::computeIndex3D( sdw::UInt32 const & pindex )
		{
			if ( !m_computeIndex3DIdx )
			{
				auto & writer = *getWriter();
				m_computeIndex3DIdx = writer.implementFunction< sdw::U32Vec3 >( "c3d_computeFroxelIndex3DIdx"
					, [this, &writer]( sdw::UInt32 const & index )
					{
						auto i = writer.declLocale( "i"
							, index % dimensions().x() );
						auto j = writer.declLocale( "j"
							, index % ( dimensions().x() * dimensions().y() ) / dimensions().x() );
						auto k = writer.declLocale( "k"
							, index / ( dimensions().x() * dimensions().y() ) );

						writer.returnStmt( u32vec3( i, j, k ) );
					}
					, sdw::InUInt32{ writer, "index" } );
			}

			return m_computeIndex3DIdx( pindex );
		}

		sdw::RetUInt32 FroxelsData::getSamplesCountZ( sdw::U32Vec3 const & pfroxelIndex3D, sdw::Vec2 const & pdepthBounds )
		{
			if ( !m_getSamplesCountZ )
			{
				auto & writer = *getWriter();
				m_getSamplesCountZ = writer.implementFunction< sdw::UInt >( "c3d_getSamplesCountZ"
					, [this, &writer]( sdw::U32Vec3 const & froxelIndex3D, sdw::Vec2 const & depthBounds )
					{
						auto maxFroxelBounds = writer.declLocale( "maxFroxelBounds"
							, getDepthBounds( dimensions().z() - 1u ) );
						auto depthRatio = writer.declLocale( "depthRatio"
							, ( depthBounds.y() - depthBounds.x() ) / ( maxFroxelBounds.y() - maxFroxelBounds.x() ) );
						writer.returnStmt( writer.cast< sdw::UInt >( mix( writer.cast< sdw::Float >( sampleCount().z() )
							, writer.cast< sdw::Float >( sampleCount().y() )
							, depthRatio ) ) );
					}
					, sdw::InU32Vec3{ writer, "froxelIndex3D" }
					, sdw::InVec2{ writer, "depthBounds" } );
			}

			return m_getSamplesCountZ( pfroxelIndex3D, pdepthBounds );
		}

		sdw::RetFloat FroxelsData::getScreenSpaceDepth( sdw::Float const & vsDepth )const
		{
			return ( vsDepth * projection()[2][2] + projection()[3][2] ) / vsDepth;
		}

		sdw::RetVec3 FroxelsData::getViewSpacePosition( shader::Utils & utils, sdw::Mat4x4 const & invProjection
			, sdw::Float const & ssX, sdw::Float const & ssY, sdw::Float const & ssZ )
		{
			return utils.screenToView( sdw::vec4( ssX, ssY, ssZ, 1.0_f ), invProjection, sdw::vec2( 1.0_f ) / renderSize() ).xyz();
		}

		sdw::RetUInt32 FroxelsData::computeIndex1D( sdw::U32Vec3 const & froxelIndex3D )const
		{
			return froxelIndex3D.x() + ( dimensions().x() * ( froxelIndex3D.y() + dimensions().y() * froxelIndex3D.z() ) );
		}

		sdw::RetFloat FroxelsData::getSliceDepth( sdw::UInt const & slice )const
		{
			return viewNearFar().x() + ( viewNearFar().y() - viewNearFar().x() ) * depthSegment( slice );
		}

		sdw::Vec2 FroxelsData::getDepthBounds( sdw::UInt const & slice )const
		{
			return sdw::vec2( getSliceDepth( slice ), getSliceDepth( slice + 1u ) );
		}

		sdw::UInt FroxelsData::quantize( sdw::Float const & x )const
		{
			auto & writer = *getWriter();
			return writer.cast< sdw::UInt >( x * 1000.0f );
		}

		sdw::Float FroxelsData::dequantize( sdw::UInt const & x )const
		{
			auto & writer = *getWriter();
			return writer.cast< sdw::Float >( x ) * 0.001f;
		}
	}

	//*********************************************************************************************

	FroxelsUbo::FroxelsUbo( RenderDevice const & device )
		: UboT{ device, MemoryPropertyFlags::eDeviceLocal }
	{
	}

	void FroxelsUbo::cpuUpdate( Point3ui const & gridDim
		, Point2f const & froxelSize
		, Point2f const & renderSize
		, f32 viewNear, f32 viewFar
		, Matrix4x4f const & projection
		, Matrix4x4f const & projectionInverse
		, Point4ui const & sampleCount
		, u32 blurFilterSize
		, ArrayView< f32 const > depthSegments
		, ArrayView< f32 const > kernelWeights )
	{
		auto & configuration = getNCData();
		std::copy( depthSegments.begin(), depthSegments.end(), configuration.depthSegments.begin() );
		std::copy( kernelWeights.begin(), kernelWeights.end(), configuration.kernelWeights.begin() );
		configuration.sampleCount = sampleCount;
		configuration.projection = projection;
		configuration.projectionInverse = projectionInverse;
		configuration.gridDim = gridDim;
		configuration.blurFilterSize = blurFilterSize;
		configuration.froxelSize = froxelSize;
		configuration.viewNearFar = { viewNear, viewFar };
		configuration.renderSize = renderSize;
		configuration.invRenderSize = { std::max( 0.001f, renderSize->x ), std::max( 0.001f, renderSize->y ) };
	}

	//*********************************************************************************************
}
