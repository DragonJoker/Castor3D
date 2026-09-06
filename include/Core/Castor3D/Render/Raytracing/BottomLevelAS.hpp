/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BottomLevelAS_H___
#define ___C3D_BottomLevelAS_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Raytracing/RaytracingModule.hpp"

namespace c3d
{
	class Geometry;
	class Submesh;
	struct ObjectBufferOffset;

	class BottomLevelAS
	{
	public:
		C3D_API BottomLevelAS( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, Submesh const & submesh
			, ObjectBufferOffset const & objectBuffers
			, Pass const & pass );
		C3D_API ~BottomLevelAS()noexcept;

		C3D_API void build( VkCommandBuffer cb );
		C3D_API void update( VkCommandBuffer cb );

		u64 getASDeviceAddress()const noexcept
		{
			return m_asDeviceAddress;
		}

	private:
		void doBuild( VkCommandBuffer cb, Buffer const & scratchBuffer, bool updateOnly );

	private:
		RenderDevice const & m_device;
		Submesh const & m_submesh;
		VkAccelerationStructureBuildRangeInfoKHR m_buildRangeInfo{};
		VkAccelerationStructureGeometryKHR m_geometryDesc{};
		VkAccelerationStructureBuildSizesInfoKHR m_buildSizesInfo{};
		VkAccelerationStructureBuildSizesInfoKHR m_updateSizesInfo{};
		BufferUPtr m_asBuffer{};
		ashes::AccelerationStructurePtr m_accelerationStructure{};
		BufferUPtr m_buildScratchBuffer{};
		BufferUPtr m_updateScratchBuffer{};
		u64 m_asDeviceAddress{};
	};
}

#endif
