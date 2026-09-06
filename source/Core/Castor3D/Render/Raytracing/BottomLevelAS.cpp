#include "Castor3D/Render/Raytracing/BottomLevelAS.hpp"

#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/AccelerationStructure/AccelerationStructure.hpp>

CU_ImplementSmartPtr( c3d, BottomLevelAS )

namespace c3d
{
	//*********************************************************************************************

	namespace blas
	{
		VkAccelerationStructureBuildRangeInfoKHR createBuildRange( Submesh const & submesh
			, ObjectBufferOffset const & objectBuffers )
		{
			VkAccelerationStructureBuildRangeInfoKHR result{};

			auto vertexBuffer = objectBuffers.getBufferChunk( SubmeshData::ePositions );
			result.firstVertex = vertexBuffer.getFirst< Point4f >();
			result.primitiveCount = vertexBuffer.getCount< Point4f >() / 3u;

			if ( auto indexMapping = submesh.getIndexMapping() )
			{
				auto indexBuffer = objectBuffers.getBufferChunk( SubmeshData::eIndex );
				result.primitiveCount = indexBuffer.getCount< u32 >() / indexMapping->getComponentsCount();
				result.primitiveOffset = indexBuffer.getFirst< u32 >();
			}

			return result;
		}

		VkAccelerationStructureGeometryKHR createGeometryDesc( Submesh const & submesh
			, ObjectBufferOffset const & objectBuffers
			, Pass const & pass )
		{
			VkAccelerationStructureGeometryKHR geometryDesc = makeVkStruct< VkAccelerationStructureGeometryKHR >();
			geometryDesc.flags = ( pass.hasAlphaBlending()
				? VkGeometryFlagsKHR{}
				: VK_GEOMETRY_OPAQUE_BIT_KHR );
			geometryDesc.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			geometryDesc.geometry.triangles = makeVkStruct< VkAccelerationStructureGeometryTrianglesDataKHR >();

			auto vertexBuffer = objectBuffers.getBufferChunk( SubmeshData::ePositions );
			geometryDesc.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
			geometryDesc.geometry.triangles.vertexData.deviceAddress = vertexBuffer.getBuffer().buffer->getDeviceAddress();
			geometryDesc.geometry.triangles.vertexStride = sizeof( Point4f );
			geometryDesc.geometry.triangles.maxVertex = submesh.getPointsCount();

			if ( objectBuffers.hasData( SubmeshData::eIndex ) )
			{
				auto indexBuffer = objectBuffers.getBufferChunk( SubmeshData::eIndex );
				geometryDesc.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
				geometryDesc.geometry.triangles.indexData.deviceAddress = indexBuffer.getBuffer().buffer->getDeviceAddress();
			}

			return geometryDesc;
		}

		VkAccelerationStructureBuildSizesInfoKHR fillSizesInfo( RenderDevice const & device
			, Submesh const & submesh
			, VkAccelerationStructureGeometryKHR const & geometryDesc
			, u32 primitiveCount
			, bool update )
		{
			auto accelerationStructureBuildGeometryInfo = makeVkStruct< VkAccelerationStructureBuildGeometryInfoKHR >();
			accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			accelerationStructureBuildGeometryInfo.mode = ( submesh.isDynamic() && update )
				? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
				: VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			accelerationStructureBuildGeometryInfo.flags = submesh.isDynamic()
				? VkBuildAccelerationStructureFlagsKHR{ VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR }
				: VkBuildAccelerationStructureFlagsKHR{ VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR };
			accelerationStructureBuildGeometryInfo.geometryCount = 1u;
			accelerationStructureBuildGeometryInfo.pGeometries = &geometryDesc;
			UInt32Array maxPrimitiveCounts;
			maxPrimitiveCounts.push_back( primitiveCount );
			return device->getAccelerationStructureBuildSizesKHR( VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR
				, accelerationStructureBuildGeometryInfo, maxPrimitiveCounts );
		}
	}

	//*********************************************************************************************

	BottomLevelAS::BottomLevelAS( RenderDevice const & device
		, crg::ResourcesCache & resources
		, String const & name
		, Submesh const & submesh
		, ObjectBufferOffset const & objectBuffers
		, Pass const & pass )
		: m_device{ device }
		, m_submesh{ submesh }
		, m_buildRangeInfo{ blas::createBuildRange( submesh, objectBuffers ) }
		, m_geometryDesc{ blas::createGeometryDesc( submesh, objectBuffers, pass ) }
		, m_buildSizesInfo{ blas::fillSizesInfo( device, submesh, m_geometryDesc, m_buildRangeInfo.primitiveCount, false ) }
		, m_updateSizesInfo{ ( submesh.isDynamic()
			? blas::fillSizesInfo( device, submesh, m_geometryDesc, m_buildRangeInfo.primitiveCount, true )
			: VkAccelerationStructureBuildSizesInfoKHR{} ) }
		, m_asBuffer{ makeBufferBase( m_device
			, resources
			, m_buildSizesInfo.accelerationStructureSize
			, BufferUsageFlags::eAccelerationStructureStorage | BufferUsageFlags::eShaderDeviceAddress
			, MemoryPropertyFlags::eDeviceLocal
			, name + "BLASBuffer" ) }
		, m_accelerationStructure{ m_device->createAccelerationStructure( name + "BLAS"
			, makeVkStruct< VkAccelerationStructureCreateInfoKHR >( VkAccelerationStructureCreateFlagsKHR{}
				, VkBuffer( *m_asBuffer->buffer )
				, 0u
				, m_buildSizesInfo.accelerationStructureSize
				, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR
				, VkDeviceAddress{} ) ) }
		, m_buildScratchBuffer{ makeBufferBase( m_device
			, resources
			, m_buildSizesInfo.buildScratchSize
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eShaderDeviceAddress
			, MemoryPropertyFlags::eDeviceLocal
			, name + "ScratchBuffer" ) }
		, m_updateScratchBuffer{ m_submesh.isDynamic()
			? makeBufferBase( m_device
				, resources
				, m_updateSizesInfo.updateScratchSize
				, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eShaderDeviceAddress
				, MemoryPropertyFlags::eDeviceLocal
				, name + "ScratchBuffer" )
			: nullptr }
		, m_asDeviceAddress{ m_accelerationStructure->getDeviceAddress() }
	{
	}

	BottomLevelAS::~BottomLevelAS()noexcept
	{
		if ( m_updateScratchBuffer )
			m_updateScratchBuffer->destroy();
		m_buildScratchBuffer->destroy();
		m_asBuffer->destroy();
	}

	void BottomLevelAS::build( VkCommandBuffer cb )
	{
		doBuild( cb, *m_buildScratchBuffer, false );
	}

	void BottomLevelAS::update( VkCommandBuffer cb )
	{
		doBuild( cb, *m_updateScratchBuffer, true );
	}

	void BottomLevelAS::doBuild( VkCommandBuffer cb, Buffer const & scratchBuffer, bool updateOnly )
	{
		// Build the AS
		auto buildInfo = makeVkStruct< VkAccelerationStructureBuildGeometryInfoKHR >();
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildInfo.flags = ( m_submesh.isDynamic()
			? VkBuildAccelerationStructureFlagsKHR{ VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR }
		: VkBuildAccelerationStructureFlagsKHR{ VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR } );
		buildInfo.mode = updateOnly
			? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
			: VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildInfo.srcAccelerationStructure = updateOnly
			? *m_accelerationStructure
			: nullptr;
		buildInfo.dstAccelerationStructure = *m_accelerationStructure;
		buildInfo.geometryCount = 1u;
		buildInfo.pGeometries = &m_geometryDesc;
		buildInfo.ppGeometries = nullptr;
		buildInfo.scratchData.deviceAddress = scratchBuffer.buffer->getDeviceAddress();
		auto buildRangeInfosPtr = &m_buildRangeInfo;
		m_device->vkCmdBuildAccelerationStructuresKHR( cb, 1u, &buildInfo, &buildRangeInfosPtr );

		// Directly append a barrier.
		auto bufferMemoryBarrier = makeVkStruct< VkBufferMemoryBarrier >();
		bufferMemoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
		bufferMemoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
		bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferMemoryBarrier.buffer = *m_asBuffer->buffer;
		bufferMemoryBarrier.offset = 0u;
		bufferMemoryBarrier.size = m_asBuffer->getSize();
		m_device->vkCmdPipelineBarrier( cb
			, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR
			, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR
			, VK_DEPENDENCY_BY_REGION_BIT
			, 0u, nullptr
			, 1u, &bufferMemoryBarrier
			, 0u, nullptr );
	}

	//*********************************************************************************************
}
