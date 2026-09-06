/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TopLevelAS_H___
#define ___C3D_TopLevelAS_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Raytracing/RaytracingModule.hpp"

#include "Castor3D/Render/Buffer.hpp"

namespace c3d
{
	struct BLASInstance
	{
		BLASInstance( Matrix4x4f ptransform = Matrix4x4f::getIdentity()
			, BottomLevelAS const * pblas = {}
			, u32 pinstanceID = {}
			, u32 pinstanceMask = {}
			, u32 pinstanceShaderBindingTableRecordOffset = {}
			, BLASInstanceFlags pflags = BLASInstanceFlag::eNone )
			: transform{ ptransform }
			, blas{ pblas }
			, instanceID{ pinstanceID }
			, instanceMask{ pinstanceMask }
			, instanceShaderBindingTableRecordOffset{ pinstanceShaderBindingTableRecordOffset }
			, flags{ pflags }
		{
		}

		Matrix4x4f transform;
		BottomLevelAS const * blas;
		u32 instanceID;
		u32 instanceMask;
		u32 instanceShaderBindingTableRecordOffset;
		BLASInstanceFlags flags;
	};

	class TopLevelAS
	{
	public:
		C3D_API TopLevelAS( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, BLASInstanceArray blases );
		C3D_API ~TopLevelAS()noexcept;

		C3D_API void build( VkCommandBuffer cb );
		C3D_API void update( VkCommandBuffer cb );

		template< typename BindingT >
		ashes::WriteDescriptorSet getDescriptorWrite( BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return ashes::WriteDescriptorSet{ uint32_t( dstBinding ), dstArrayElement
				, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
				, { VkDescriptorBufferInfo{ m_asBuffer->getBuffer(), 0u, m_asBuffer->getSize() } } };
		}

		template< typename BindingT >
		void addDescriptorWriteT( ashes::WriteDescriptorSetArray & writes
			, BindingT dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
		}

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & writes
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			writes.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}

		template< typename BindingT >
		VkDescriptorSetLayoutBinding getLayoutBinding( BindingT index
			, VkShaderStageFlags stages )const
		{
			return VkDescriptorSetLayoutBinding{ uint32_t( index )
				, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
				, 1u
				, stages
				, nullptr };
		}

		template< typename BindingT >
		void addLayoutBindingT( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, BindingT index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
		}

		void addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags stages )const
		{
			bindings.push_back( getLayoutBinding( index, stages ) );
			++index;
		}

	private:
		void doBuild( VkCommandBuffer cb, Buffer const & scratchBuffer, bool updateOnly );

	private:
		RenderDevice const & m_device;
		BLASInstanceArray m_blases;
		BufferUPtrT< VkAccelerationStructureInstanceKHR > m_instancesBuffer{};
		VkAccelerationStructureGeometryKHR m_instancesDesc{};
		VkAccelerationStructureBuildSizesInfoKHR m_buildSizesInfo{};
		VkAccelerationStructureBuildRangeInfoKHR m_buildRangeInfo{};
		BufferUPtr m_asBuffer{};
		ashes::AccelerationStructurePtr m_accelerationStructure{};
		BufferUPtr m_buildScratchBuffer{};
		BufferUPtr m_updateScratchBuffer{};
		u64 m_asDeviceAddress{};
	};
}

#endif
