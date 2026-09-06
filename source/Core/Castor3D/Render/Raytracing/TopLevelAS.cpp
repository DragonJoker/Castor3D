#include "Castor3D/Render/Raytracing/TopLevelAS.hpp"

#include "Castor3D/Render/Raytracing/BottomLevelAS.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

#include <ashespp/AccelerationStructure/AccelerationStructure.hpp>

CU_ImplementSmartPtr( c3d, TopLevelAS )

namespace c3d
{
	//*********************************************************************************************

	namespace tlas
	{
		VkTransformMatrixKHR convert( Matrix4x4f transform )
		{
			VkTransformMatrixKHR result;
			transform.transpose();
			memcpy( &result.matrix, transform.constPtr(), sizeof( float ) * 12u );
			return result;
		}

		VkGeometryInstanceFlagsKHR convert( BLASInstanceFlags const & flags )
		{
			VkGeometryInstanceFlagsKHR result{};
			if ( checkFlag( flags, BLASInstanceFlag::eForceOpaque ) )
				result |= VkGeometryInstanceFlagBitsKHR::VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
			if ( checkFlag( flags, BLASInstanceFlag::eForceNonOpaque ) )
				result |= VkGeometryInstanceFlagBitsKHR::VK_GEOMETRY_INSTANCE_FORCE_NO_OPAQUE_BIT_KHR;
			return result;
		}

		BufferUPtrT< VkAccelerationStructureInstanceKHR > createInstancesBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BLASInstanceArray const & blases )
		{
			auto result = makeBuffer< VkAccelerationStructureInstanceKHR >( device
					, resources
					, blases.size()
					, BufferUsageFlags::eAccelerationStructureBuildInputReadOnly
							| BufferUsageFlags::eShaderDeviceAddress
					, MemoryPropertyFlags::eHostVisible
					, name + cuT( "/TLAS/InstanceBuffer" ) );

			if ( auto buffer = result->lock() )
			{
				for ( BLASInstance const & instance : blases )
				{
					*buffer = VkAccelerationStructureInstanceKHR{ .transform = tlas::convert( instance.transform )
						, .instanceCustomIndex = instance.instanceID
						, .mask = instance.instanceMask
						, .instanceShaderBindingTableRecordOffset = instance.instanceShaderBindingTableRecordOffset
						, .flags = convert( instance.flags )
						, .accelerationStructureReference = instance.blas->getASDeviceAddress() };
					++buffer;
				}

				result->flush();
				result->unlock();
			}

			return result;
		}

		VkAccelerationStructureGeometryKHR createInstancesDesc( BufferBase const & instanceBuffer )
		{
			VkAccelerationStructureGeometryKHR geometryDesc = makeVkStruct< VkAccelerationStructureGeometryKHR >();
			geometryDesc.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
			geometryDesc.flags = VkGeometryFlagsKHR{};
			geometryDesc.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
			geometryDesc.geometry.instances.arrayOfPointers = VK_FALSE;
			geometryDesc.geometry.instances.data.deviceAddress = instanceBuffer.buffer->getDeviceAddress();

			return geometryDesc;
		}

		VkAccelerationStructureBuildSizesInfoKHR fillBuildSizesInfo( RenderDevice const & device
			, VkAccelerationStructureGeometryKHR const & geometryDesc )
		{
			auto accelerationStructureBuildGeometryInfo = makeVkStruct< VkAccelerationStructureBuildGeometryInfoKHR >();
			accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			accelerationStructureBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
			accelerationStructureBuildGeometryInfo.geometryCount = 1u;
			accelerationStructureBuildGeometryInfo.pGeometries = &geometryDesc;
			UInt32Array maxPrimitiveCounts;
			maxPrimitiveCounts.push_back( 1u );
			return device->getAccelerationStructureBuildSizesKHR( VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR
				, accelerationStructureBuildGeometryInfo, maxPrimitiveCounts );
		}

		VkAccelerationStructureBuildRangeInfoKHR createBuildRange( BLASInstanceArray blases )
		{
			VkAccelerationStructureBuildRangeInfoKHR result{};

			result.firstVertex = 0u;
			result.primitiveCount = u32( blases.size() );
			result.primitiveOffset = 0u;
			result.transformOffset = 0u;
			return result;
		}
	}

	//*********************************************************************************************

	TopLevelAS::TopLevelAS( RenderDevice const & device
		, crg::ResourcesCache & resources
		, String const & name
		, BLASInstanceArray blases )
		: m_device{ device }
		, m_blases{ c3d::move( blases ) }
		, m_instancesBuffer{ tlas::createInstancesBuffer( device, resources, name, m_blases ) }
		, m_instancesDesc{ tlas::createInstancesDesc( *m_instancesBuffer ) }
		, m_buildSizesInfo{ tlas::fillBuildSizesInfo( device, m_instancesDesc ) }
		, m_buildRangeInfo{ tlas::createBuildRange( m_blases ) }
		, m_asBuffer{ makeBufferBase( m_device
			, resources
			, m_buildSizesInfo.accelerationStructureSize
			, BufferUsageFlags::eAccelerationStructureStorage | BufferUsageFlags::eShaderDeviceAddress
			, MemoryPropertyFlags::eDeviceLocal
			, name + "/TLAS/ASBuffer" ) }
		, m_accelerationStructure{ m_device->createAccelerationStructure( name + "/TLAS"
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
			, name + "/TLAS/BuildScratchBuffer" ) }
		, m_updateScratchBuffer{ makeBufferBase( m_device
			, resources
			, m_buildSizesInfo.updateScratchSize
			, BufferUsageFlags::eStorageBuffer | BufferUsageFlags::eShaderDeviceAddress
			, MemoryPropertyFlags::eDeviceLocal
			, name + "/TLAS/UpdateScratchBuffer" ) }
		, m_asDeviceAddress{ m_accelerationStructure->getDeviceAddress() }
	{
	}

	TopLevelAS::~TopLevelAS()noexcept
	{
		m_updateScratchBuffer->destroy();
		m_buildScratchBuffer->destroy();
		m_asBuffer->destroy();
	}

	void TopLevelAS::build( VkCommandBuffer cb )
	{
		doBuild( cb, *m_buildScratchBuffer, false );
	}

	void TopLevelAS::update( VkCommandBuffer cb )
	{
		doBuild( cb, *m_updateScratchBuffer, true );
	}

	void TopLevelAS::doBuild( VkCommandBuffer cb, Buffer const & scratchBuffer, bool updateOnly )
	{
		// Build the AS
		auto buildInfo = makeVkStruct< VkAccelerationStructureBuildGeometryInfoKHR >();
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildInfo.flags = VkBuildAccelerationStructureFlagsKHR{ VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR | VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR };
		buildInfo.mode = updateOnly
			? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR
			: VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildInfo.srcAccelerationStructure = updateOnly
			? *m_accelerationStructure
			: nullptr;
		buildInfo.dstAccelerationStructure = *m_accelerationStructure;
		buildInfo.geometryCount = 1u;
		buildInfo.pGeometries = &m_instancesDesc;
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
