#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
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
						auto k = writer.declLocale( "k"
							, floor( sdw::log( -viewZ ) * clustersLightsData.z() - clustersLightsData.w() ) );

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

						auto nearTile = writer.declLocale( "nearTile"
							, -nearZ * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() ) / writer.cast< sdw::Float >( dimensions().z() ) ) );
						auto farTile = writer.declLocale( "farTile"
							, -nearZ * pow( farZ / nearZ, writer.cast< sdw::Float >( clusterIndex3D.z() + 1_u ) / writer.cast< sdw::Float >( dimensions().z() ) ) );

						writer.returnStmt( vec2( nearTile, farTile ) );
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
#if 0
						lightsMin.z() = max( nearPlane, lightsMin.z() );
						lightsMax.z() = min( max( lightsMin.z() + 0.00001_f, lightsMax.z() ), farPlane );

						auto nearZ = lightsMin.z();
						auto farZ = lightsMax.z();
#else
						auto nearZ = nearPlane;
						auto farZ = farPlane;
#endif
						auto multiply = writer.declLocale( "multiply"
							, writer.cast< sdw::Float >( dimensions().z() ) / sdw::log( farZ / nearZ ) );
						auto add = writer.declLocale( "add"
							, multiply * sdw::log( nearZ ) );
						clustersLightsData = vec4( nearZ, farZ, multiply, add );

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
				, pclustersLightsData , plightsAABBRange );
		}
	}

	//*********************************************************************************************

	ClustersUbo::ClustersUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	ClustersUbo::~ClustersUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ClustersUbo::cpuUpdate( castor::Point3ui gridDim
		, castor::Point2ui clusterSize
		, float viewNear
		, float viewFar
		, uint32_t pointLightsCount
		, uint32_t spotLightsCount )
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
	}

	//*********************************************************************************************
}
