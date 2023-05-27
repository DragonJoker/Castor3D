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
		std::unique_ptr< sdw::UInt32Array > m_pointLightIndices;
		std::unique_ptr< sdw::U32Vec2Array > m_pointLightClusters;
		std::unique_ptr< sdw::UInt32Array > m_spotLightIndices;
		std::unique_ptr< sdw::U32Vec2Array > m_spotLightClusters;
	};

#define C3D_ClustersAABB( writer, binding, set ) \
	auto clustersAABBBuffer = writer.declStorageBuffer( "c3d_clustersAABBBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3D_clustersAABB = clustersAABBBuffer.declMemberArray< shader::AABB >( "cb" ); \
	clustersAABBBuffer.end()



#define C3D_PointLightClusterGridEx( writer, binding, set, enabled ) \
	auto pointLightClusterGridBuffer = writer.declStorageBuffer( "c3d_pointLightClusterGridBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightClusterGrid = pointLightClusterGridBuffer.declMemberArray< sdw::UVec2 >( "pg", enabled ); \
	pointLightClusterGridBuffer.end()

#define C3D_PointLightClusterGrid( writer, binding, set ) \
	C3D_PointLightClusterGridEx( writer, binding, set, true )

#define C3D_SpotLightClusterGridEx( writer, binding, set, enabled ) \
	auto spotLightClusterGridBuffer = writer.declStorageBuffer( "c3d_spotLightClusterGridBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightClusterGrid = spotLightClusterGridBuffer.declMemberArray< sdw::UVec2 >( "sg", enabled ); \
	spotLightClusterGridBuffer.end()

#define C3D_SpotLightClusterGrid( writer, binding, set ) \
	C3D_SpotLightClusterGridEx( writer, binding, set, true )



#define C3D_PointLightClusterIndexEx( writer, binding, set, enabled ) \
	auto pointLightClusterIndexBuffer = writer.declStorageBuffer( "c3d_pointLightClusterIndexBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightClusterListCount = pointLightClusterIndexBuffer.declMember< sdw::UInt >( "pc" , enabled ); \
	auto c3d_pointLightClusterIndex = pointLightClusterIndexBuffer.declMemberArray< sdw::UInt >( "pg", enabled ); \
	pointLightClusterIndexBuffer.end()

#define C3D_PointLightClusterIndex( writer, binding, set ) \
	C3D_PointLightClusterIndexEx( writer, binding, set, true )

#define C3D_SpotLightClusterIndexEx( writer, binding, set, enabled ) \
	auto spotLightClusterIndexBuffer = writer.declStorageBuffer( "c3d_spotLightClusterIndexBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightClusterListCount = spotLightClusterIndexBuffer.declMember< sdw::UInt >( "sc" , enabled ); \
	auto c3d_spotLightClusterIndex = spotLightClusterIndexBuffer.declMemberArray< sdw::UInt >( "sg", enabled ); \
	spotLightClusterIndexBuffer.end()

#define C3D_SpotLightClusterIndex( writer, binding, set ) \
	C3D_SpotLightClusterIndexEx( writer, binding, set, true )



#define C3D_LightsAABB( writer, binding, set ) \
	auto lightsAABBBuffer = writer.declStorageBuffer( "c3d_lightsAABBBuffer" \
		, uint32_t( binding ) \
		, set ); \
	auto c3d_lightsAABB = lightsAABBBuffer.declMemberArray< shader::AABB >( "cb" ); \
	lightsAABBBuffer.end()

#define C3D_PointLightBVHEx( writer, binding, set, enabled ) \
	auto pointLightBVHBuffer = writer.declStorageBuffer( "c3d_pointLightBVHBuffer" \
		, uint32_t( binding ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightBVH = pointLightBVHBuffer.declMemberArray< shader::AABB >( "pb", enabled ); \
	pointLightBVHBuffer.end()

#define C3D_PointLightBVH( writer, binding, set ) \
	C3D_PointLightBVHEx( writer, binding, set, true )

#define C3D_SpotLightBVHEx( writer, binding, set, enabled ) \
	auto spotLightBVHBuffer = writer.declStorageBuffer( "c3d_spotLightBVHBuffer" \
		, uint32_t( binding ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightBVH = spotLightBVHBuffer.declMemberArray< shader::AABB >( "sb", enabled ); \
	spotLightBVHBuffer.end()

#define C3D_SpotLightBVH( writer, binding, set ) \
	C3D_SpotLightBVHEx( writer, binding, set, true )



#define C3D_PointLightIndicesEx( writer, binding, set, enabled ) \
	auto pointLightIndicesBuffer = writer.declStorageBuffer( "c3d_pointLightIndicesBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightIndices = pointLightIndicesBuffer.declMemberArray< sdw::UInt >( "pi" , enabled ); \
	pointLightIndicesBuffer.end()

#define C3D_PointLightIndices( writer, binding, set ) \
	C3D_PointLightIndicesEx( writer, binding, set, true )

#define C3D_SpotLightIndicesEx( writer, binding, set, enabled ) \
	auto spotLightIndicesBuffer = writer.declStorageBuffer( "c3d_spotLightIndicesBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightIndices = spotLightIndicesBuffer.declMemberArray< sdw::UInt >( "si" , enabled ); \
	spotLightIndicesBuffer.end()

#define C3D_SpotLightIndices( writer, binding, set ) \
	C3D_SpotLightIndicesEx( writer, binding, set, true )



#define C3D_PointLightMortonCodesEx( writer, binding, set, enabled ) \
	auto pointLightMortonCodesBuffer = writer.declStorageBuffer( "c3d_pointLightMortonCodesBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_pointLightMortonCodes = pointLightMortonCodesBuffer.declMemberArray< sdw::UInt >( "pm", enabled ); \
	pointLightMortonCodesBuffer.end()

#define C3D_PointLightMortonCodes( writer, binding, set ) \
	C3D_PointLightMortonCodesEx( writer, binding, set, true )

#define C3D_SpotLightMortonCodesEx( writer, binding, set, enabled ) \
	auto spotLightMortonCodesBuffer = writer.declStorageBuffer( "c3d_spotLightMortonCodesBuffer" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, set \
		, sdw::type::MemoryLayout::eStd430 \
		, enabled ); \
	auto c3d_spotLightMortonCodes= spotLightMortonCodesBuffer.declMemberArray< sdw::UInt >( "sm", enabled ); \
	spotLightMortonCodesBuffer.end()

#define C3D_SpotLightMortonCodes( writer, binding, set ) \
	C3D_SpotLightMortonCodesEx( writer, binding, set, true )
}

#endif
