/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslClusteredLights_H___
#define ___C3D_GlslClusteredLights_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	class ClusteredLights
	{
	public:
		C3D_API explicit ClusteredLights( sdw::ShaderWriter & writer
			, uint32_t & binding
			, uint32_t set
			, bool enabled = true );

		C3D_API void computeCombinedDifSpec( Lights & lights
			, LightingModel & lightingModel
			, BlendComponents const & components
			, BackgroundModel & background
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, sdw::Vec2 const screenPosition
			, sdw::Float const viewDepth
			, DebugOutput & debugOutput
			, OutputComponents & output );

		bool isEnabled()const noexcept
		{
			return m_enabled;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		bool m_enabled;
		ClustersDataUPtr m_clusterData;
		std::unique_ptr< sdw::UInt32Array > m_pointLightGridIndex;
		std::unique_ptr< sdw::U32Vec2Array > m_pointLightGridCluster;
		std::unique_ptr< sdw::UInt32Array > m_spotLightGridIndex;
		std::unique_ptr< sdw::U32Vec2Array > m_spotLightGridCluster;
	};

#define C3D_ClustersAABB( writer, binding, set ) \
	auto clustersAABBBuffer = writer.declStorageBuffer( "c3D_clustersAABBBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3D_clustersAABB = clustersAABBBuffer.declMemberArray< shader::AABB >( "b" ); \
	clustersAABBBuffer.end()

#define C3D_PointLightGridIndicesEx( writer, binding, set, enabled ) \
	auto pointLightGridIndexBuffer = writer.declStorageBuffer( "c3d_pointLightGridIndexBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightGridListCount = pointLightGridIndexBuffer.declMember< sdw::UInt >( "pc" , enabled ); \
	auto c3d_pointLightGridIndices = pointLightGridIndexBuffer.declMemberArray< sdw::UInt >( "pi" , enabled ); \
	pointLightGridIndexBuffer.end()

#define C3D_PointLightGridClustersEx( writer, binding, set, enabled ) \
	auto pointLightGridClustersBuffer = writer.declStorageBuffer( "c3d_pointLightGridClustersBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightGridClusters = pointLightGridClustersBuffer.declMemberArray< sdw::UVec2 >( "pg", enabled ); \
	pointLightGridClustersBuffer.end()

#define C3D_SpotLightGridIndicesEx( writer, binding, set, enabled ) \
	auto spotLightGridIndexBuffer = writer.declStorageBuffer( "c3d_spotLightGridIndexBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightGridListCount = spotLightGridIndexBuffer.declMember< sdw::UInt >( "sc" , enabled ); \
	auto c3d_spotLightGridIndices = spotLightGridIndexBuffer.declMemberArray< sdw::UInt >( "si" , enabled ); \
	spotLightGridIndexBuffer.end()

#define C3D_SpotLightGridClustersEx( writer, binding, set, enabled ) \
	auto spotLightGridClustersBuffer = writer.declStorageBuffer( "c3d_spotLightGridClustersBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightGridClusters = spotLightGridClustersBuffer.declMemberArray< sdw::UVec2 >( "sg", enabled ); \
	spotLightGridClustersBuffer.end()

#define C3D_PointLightGridIndices( writer, binding, set ) \
	C3D_PointLightGridIndicesEx( writer, binding, set, true )

#define C3D_PointLightGridClusters( writer, binding, set ) \
	C3D_PointLightGridClustersEx( writer, binding, set, true )

#define C3D_SpotLightGridIndices( writer, binding, set ) \
	C3D_SpotLightGridIndicesEx( writer, binding, set, true )

#define C3D_SpotLightGridClusters( writer, binding, set ) \
	C3D_SpotLightGridClustersEx( writer, binding, set, true )
}

#endif
