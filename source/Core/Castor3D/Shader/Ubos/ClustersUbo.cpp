#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d::shader, ClustersData )

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		C3D_API sdw::RetU32Vec3 ClustersData::computeClusterIndex3D( sdw::UInt32 const pindex )
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

		C3D_API sdw::RetU32Vec3 ClustersData::computeClusterIndex3D( sdw::Vec2 const pscreenPos
			, sdw::Float pviewZ )
		{
			if ( !m_computeClusterIndex3DPos )
			{
				auto & writer = *getWriter();
				m_computeClusterIndex3DPos = writer.implementFunction< sdw::U32Vec3 >( "c3d_computeClusterIndex3DPos"
					, [&]( sdw::Vec2 const screenPos
						, sdw::Float viewZ )
					{
						auto i = writer.declLocale( "i"
							, screenPos.x() / writer.cast< sdw::Float >( clusterSize().x() ) );
						auto j = writer.declLocale( "j"
							, screenPos.y() / writer.cast< sdw::Float >( clusterSize().y() ) );
						// It is assumed that view space z is negative (right-handed coordinate system)
						// so the view-space z coordinate needs to be negated to make it positive.
						auto k = writer.declLocale( "k"
							, sdw::log( -viewZ / viewNear() ) * logGridDimY() );

						writer.returnStmt( u32vec3( i, j, k ) );
					}
					, sdw::InVec2{ writer, "screenPos" }
					, sdw::InFloat{ writer, "viewZ" } );
			}

			return m_computeClusterIndex3DPos( pscreenPos, pviewZ );
		}

		C3D_API sdw::RetUInt32 ClustersData::computeClusterIndex1D( sdw::U32Vec3 const pclusterIndex3D )
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
		, float viewNear
		, uint32_t clusterSize
		, float nearK
		, uint32_t pointLightsCount
		, uint32_t spotLightsCount )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo.getData();
		configuration.gridDim = gridDim;
		configuration.clusterSize = { clusterSize, clusterSize };
		configuration.viewNear = viewNear;
		configuration.nearK = nearK;
		configuration.logGridDimY = 1.0f / std::log( nearK );
		configuration.pointLightsCount = pointLightsCount;
		configuration.spotLightsCount = spotLightsCount;
		configuration.pointLightLevelsCount = FrustumClusters::getNumLevels( pointLightsCount );
		configuration.spotLightLevelsCount = FrustumClusters::getNumLevels( spotLightsCount );
	}

	//*********************************************************************************************
}
