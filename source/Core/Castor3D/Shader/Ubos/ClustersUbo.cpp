#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d::shader, ClustersData )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		sdw::RetU32Vec3 ClustersData::computeClusterIndex3D( sdw::UInt32 const pindex )
		{
			if ( !m_computeClusterIndex3DIdx )
			{
				auto & writer = *getWriter();
				m_computeClusterIndex3DIdx = writer.implementFunction< sdw::U32Vec3 >( "c3d_computeClusterIndex3DIdx"
					, [&]( sdw::UInt32 const index )
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

			return m_computeClusterIndex3DIdx( pindex );
		}

		sdw::RetU32Vec3 ClustersData::computeClusterIndex3D( sdw::Vec2 const pscreenPos
			, sdw::Float pviewZ
			, sdw::Vec4 const pclustersLightsData )
		{
			if ( !m_computeClusterIndex3DPos )
			{
				auto & writer = *getWriter();
				m_computeClusterIndex3DPos = writer.implementFunction< sdw::U32Vec3 >( "c3d_computeClusterIndex3DPos"
					, [&]( sdw::Vec2 const screenPos
						, sdw::Float viewZ
						, sdw::Vec4 const clustersLightsData )
					{
						auto i = writer.declLocale( "i"
							, screenPos.x() / writer.cast< sdw::Float >( clusterSize().x() ) );
						auto j = writer.declLocale( "j"
							, screenPos.y() / writer.cast< sdw::Float >( clusterSize().y() ) );

						sdw::UInt const ExponentialBase = 0_u;
						sdw::UInt const ExponentialBiased = 1_u;
						sdw::UInt const Linear = 2_u;
						sdw::UInt const ExponentialLinearHybrid = 3_u;

						auto k = writer.declLocale( "k", 0.0_f );

						IF( writer, splitScheme() == ExponentialBase )
						{
							k = floor( sdw::log( -viewZ ) * clustersLightsData.z() - clustersLightsData.w() );
						}
						ELSEIF( splitScheme() == ExponentialBiased )
						{
							auto nearZ = writer.declLocale( "nearZ"
								, clustersLightsData.x() );
							auto farZ = writer.declLocale( "farZ"
								, clustersLightsData.y() );
							k = max( 0.0_f, floor( sdw::log( -viewZ / nearZ ) * clustersLightsData.w() - writer.cast< sdw::Float >( dimensions().z() ) * bias() ) );
						}
						ELSEIF( splitScheme() == Linear )
						{
							k = floor( writer.cast< sdw::Float >( dimensions().z() ) * ( -viewZ - clustersLightsData.z() ) / ( clustersLightsData.w() - clustersLightsData.z() ) );
						}
						ELSE
						{
							auto nearZ = writer.declLocale( "nearZ"
								, clustersLightsData.x() );
							auto farZ = writer.declLocale( "farZ"
								, clustersLightsData.y() );
							auto limZ = writer.declLocale( "limZ"
								, max( bias(), nearZ ) );
							auto depthBias = writer.declLocale( "depthBias"
								, sdw::log( limZ / nearZ ) / sdw::log( farZ / limZ ) );
							k = max( 0.0_f, floor( sdw::log( -viewZ / nearZ ) * clustersLightsData.z() - writer.cast< sdw::Float >( dimensions().z() ) * depthBias ) );
						}
						FI;

						writer.returnStmt( u32vec3( i, j, k ) );
					}
					, sdw::InVec2{ writer, "screenPos" }
					, sdw::InFloat{ writer, "viewZ" }
					, sdw::InVec4{ writer, "clustersLightsData" } );
			}

			return m_computeClusterIndex3DPos( pscreenPos
				, pviewZ
				, pclustersLightsData );
		}

		sdw::RetUInt32 ClustersData::computeClusterIndex1D( sdw::U32Vec3 const pclusterIndex3D )
		{
			if ( !m_computeClusterIndex1D )
			{
				auto & writer = *getWriter();
				m_computeClusterIndex1D = writer.implementFunction< sdw::UInt32 >( "c3d_computeClusterIndex1D"
					, [&]( sdw::U32Vec3 const clusterIndex3D )
					{
						writer.returnStmt( clusterIndex3D.x() + ( dimensions().x() * ( clusterIndex3D.y() + dimensions().y() * clusterIndex3D.z() ) ) );
					}
					, sdw::InU32Vec3{ writer, "clusterIndex3D" } );
			}

			return m_computeClusterIndex1D( pclusterIndex3D );
		}

		sdw::RetVec2 ClustersData::getClusterDepthBounds( sdw::U32Vec3 const pclusterIndex3D
			, sdw::Vec4 const pclustersLightsData
			, sdw::Vec4 const plightsAABBRange )
		{
			if ( !m_getClusterDepthBounds )
			{
				auto & writer = *getWriter();
				m_getClusterDepthBounds = writer.implementFunction< sdw::Vec2 >( "c3d_getClusterDepthBounds"
					, [&]( sdw::U32Vec3 const clusterIndex3D
						, sdw::Vec4 const clustersLightsData
						, sdw::Vec4 const lightsAABBRange )
					{
						auto nearZ = writer.declLocale( "nearZ"
							, clustersLightsData.x() );
						auto farZ = writer.declLocale( "farZ"
							, clustersLightsData.y() );

						sdw::UInt const ExponentialBase = 0_u;
						sdw::UInt const ExponentialBiased = 1_u;
						sdw::UInt const Linear = 2_u;
						sdw::UInt const ExponentialLinearHybrid = 3_u;

						IF( writer, splitScheme() == ExponentialBase )
						{
							auto nearTile = writer.declLocale( "nearTile"
								, -nearZ * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() ) / writer.cast< sdw::Float >( dimensions().z() ) ) );
							auto farTile = writer.declLocale( "farTile"
								, -nearZ * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) / writer.cast< sdw::Float >( dimensions().z() ) ) );
							writer.returnStmt( vec2( nearTile, farTile ) );
						}
						ELSEIF( splitScheme() == ExponentialBiased )
						{
							auto nearTile = writer.declLocale( "nearTile"
								, -nearZ * pow( farZ / nearZ, ( writer.cast< sdw::Float >( clusterIndex3D.z() ) + writer.cast< sdw::Float >( dimensions().z() ) * bias() ) / clustersLightsData.z() ) );
							auto farTile = writer.declLocale( "farTile"
								, -nearZ * pow( farZ / nearZ, ( writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) + writer.cast< sdw::Float >( dimensions().z() ) * bias() ) / clustersLightsData.z() ) );
							writer.returnStmt( vec2( nearTile, farTile ) );
						}
						ELSEIF( splitScheme() == Linear )
						{
							auto nearTile = writer.declLocale( "nearTile"
								, -nearZ - writer.cast< sdw::Float >( clusterIndex3D.z() ) * ( farZ - nearZ ) / writer.cast< sdw::Float >( dimensions().z() ) );
							auto farTile = writer.declLocale( "farTile"
								, -nearZ - writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) * ( farZ - nearZ ) / writer.cast< sdw::Float >( dimensions().z() ) );
							writer.returnStmt( vec2( nearTile, farTile ) );
						}
						ELSE
						{
							auto limZ = writer.declLocale( "limZ"
								, max( bias(), nearZ ) );
							auto depthBias = writer.declLocale( "depthBias"
								, sdw::log( limZ / nearZ ) / sdw::log( farZ / limZ ) );
							auto nearTile = writer.declLocale( "nearTile"
								, writer.ternary( clusterIndex3D.z() == 0_u
									, -nearZ
									, -clustersLightsData.w() * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() ) / ( writer.cast< sdw::Float >( dimensions().z() ) * ( 1.0_f + depthBias ) ) ) ) );
							auto farTile = writer.declLocale( "farTile"
								, writer.ternary( clusterIndex3D.z() == 0_u
									, -nearZ
									, -clustersLightsData.w() * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) / ( writer.cast< sdw::Float >( dimensions().z() ) * ( 1.0_f + depthBias ) ) ) ) );
							writer.returnStmt( vec2( nearTile, farTile ) );
						}
						FI;
					}
					, sdw::InU32Vec3{ writer, "clusterIndex3D" }
					, sdw::InVec4{ writer, "clustersLightsData" }
					, sdw::InVec4{ writer, "lightsAABBRange" } );
			}

			return m_getClusterDepthBounds( pclusterIndex3D, pclustersLightsData, plightsAABBRange );
		}

		sdw::RetVoid ClustersData::computeGlobalLightsData( sdw::Vec4 const plightsMin
			, sdw::Vec4 const plightsMax
			, sdw::Float const pnearPlane
			, sdw::Float const pfarPlane
			, sdw::Vec4 & pclustersLightsData
			, sdw::Vec4 & plightsAABBRange )
		{
			if ( !m_computeGlobalLightsData )
			{
				auto & writer = *getWriter();
				m_computeGlobalLightsData = writer.implementFunction< sdw::Void >( "c3d_computeGlobalLightsData"
					, [&]( sdw::Vec4 lightsMin
						, sdw::Vec4 lightsMax
						, sdw::Float const nearPlane
						, sdw::Float const farPlane
						, sdw::Vec4 clustersLightsData
						, sdw::Vec4 lightsAABBRange )
					{
						if ( m_config && m_config->limitClustersToLightsAABB )
						{
							// Right handed means Z will be negative, hence minZ and maxZ are inverted.
							auto nearZ = writer.declLocale( "nearZ"
								, -lightsMax.z() );
							auto farZ = writer.declLocale( "farZ"
								, -lightsMin.z() );
							nearZ = max( nearPlane, nearZ );
							farZ = min( max( nearZ + 0.00001_f, farZ ), farPlane );
							lightsMax.z() = -nearZ;
							lightsMin.z() = -farZ;
						}
						else
						{
							auto nearZ = writer.declLocale( "nearZ"
								, nearPlane );
							auto farZ = writer.declLocale( "farZ"
								, farPlane );
						}

						auto nearZ = writer.getVariable < sdw::Float >( "nearZ" );
						auto farZ = writer.getVariable < sdw::Float >( "farZ" );

						sdw::UInt const ExponentialBase = 0_u;
						sdw::UInt const ExponentialBiased = 1_u;
						sdw::UInt const Linear = 2_u;
						sdw::UInt const ExponentialLinearHybrid = 3_u;

						IF( writer, splitScheme() == ExponentialBase )
						{
							auto multiply = writer.declLocale( "multiply"
								, writer.cast< sdw::Float >( dimensions().z() ) / sdw::log( farZ / nearZ ) );
							auto add = writer.declLocale( "add"
								, multiply * sdw::log( nearZ ) );
							clustersLightsData = vec4( nearZ, farZ, multiply, add );
						}
						ELSEIF( splitScheme() == ExponentialBiased )
						{
							auto d = writer.declLocale( "d"
								, writer.cast< sdw::Float >( dimensions().z() ) * ( 1.0_f + bias() ) );
							auto e = writer.declLocale( "e"
								, d / sdw::log( farZ / nearZ ) );
							clustersLightsData = vec4( nearZ, farZ, d, e );
						}
						ELSEIF( splitScheme() == Linear )
						{
							clustersLightsData = vec4( nearZ, farZ, nearZ, farZ );
						}
						ELSE
						{
							auto limZ = writer.declLocale( "limZ"
								, max( bias(), nearZ ) );
							auto depthBias = writer.declLocale( "depthBias"
								, sdw::log( limZ / nearZ ) / sdw::log( farZ / limZ ) );
							auto nTimesOnePlusB = writer.declLocale( "nTimesOnePlusB"
								, ( writer.cast< sdw::Float >( dimensions().z() ) * ( 1.0f + depthBias ) ) );
							auto d = writer.declLocale( "d"
								, nTimesOnePlusB / sdw::log( farZ / nearZ ) );
							auto e = writer.declLocale( "e"
								, nearZ * pow( farZ / nearZ, depthBias / ( 1.0f + depthBias ) ) );
							clustersLightsData = vec4( nearZ, farZ, d, e );
						}
						FI;

						lightsAABBRange = vec4( vec3( 1.0_f ) / ( lightsMax - lightsMin ).xyz(), 1.0_f );
					}
					, sdw::PVec4{ writer, "lightsMin" }
					, sdw::PVec4{ writer, "lightsMax" }
					, sdw::InFloat{ writer, "nearPlane" }
					, sdw::InFloat{ writer, "farPlane" }
					, sdw::OutVec4{ writer, "clustersLightsData" }
					, sdw::OutVec4{ writer, "lightsAABBRange" } );
			}

			return m_computeGlobalLightsData( plightsMin, plightsMax
				, pnearPlane, pfarPlane
				, pclustersLightsData, plightsAABBRange );
		}
	}

	//*********************************************************************************************

	ClustersUbo::ClustersUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	ClustersUbo::~ClustersUbo()noexcept
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ClustersUbo::cpuUpdate( castor::Point3ui gridDim
		, castor::Point2ui clusterSize
		, float viewNear
		, float viewFar
		, uint32_t pointLightsCount
		, uint32_t spotLightsCount
		, ClusterSplitScheme splitScheme
		, float bias
		, bool enableWaveIntrinsics )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.gridDim = gridDim;
		configuration.clusterSize = clusterSize;
		configuration.viewNearFar = { viewNear, viewFar };
		configuration.pointLightsCount = pointLightsCount;
		configuration.spotLightsCount = spotLightsCount;
		configuration.pointLightLevelsCount = FrustumClusters::getNumLevels( pointLightsCount );
		configuration.spotLightLevelsCount = FrustumClusters::getNumLevels( spotLightsCount );
		configuration.splitScheme = uint32_t( splitScheme );
		configuration.bias = bias;
		configuration.enableWaveIntrinsics = enableWaveIntrinsics ? 1u : 0u;
	}

	//*********************************************************************************************
}
