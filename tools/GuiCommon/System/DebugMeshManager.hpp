/*
See LICENSE file in root folder
*/
#ifndef ___GC_DebugMeshManager_HPP___
#define ___GC_DebugMeshManager_HPP___
#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>

#include <Castor3D/Buffer/GpuBufferOffset.hpp>
#include <Castor3D/Buffer/ObjectBufferOffset.hpp>

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace GuiCommon
{
	struct DebugMeshConfig
	{
		c3d::Matrix4x4f world;
		c3d::Point4f colour;
	};
	/**
	*\brief
	*	Used to display the cube box of objects.
	*/
	class DebugMeshManager
	{
	public:
		explicit DebugMeshManager( c3d::RenderTarget const & renderTarget );
		~DebugMeshManager();
		void select( c3d::LightInstance const & light );
		void select( c3d::Geometry const & object
			, c3d::Submesh const & submesh );
		void unselect();

	private:
		void onDisplayObject();
		void onDisplayLight();

	private:
		c3d::RenderDevice const & m_device;
		c3d::RenderTarget const & m_renderTarget;
		ashes::PipelineShaderStageCreateInfoArray m_cubeProgram;
		ashes::PipelineShaderStageCreateInfoArray m_meshProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_bindings;
		ashes::WriteDescriptorSetArray m_writes;
		c3d::Point4f m_aabbMeshColour{};
		c3d::Point4f m_obbMeshColour{};
		c3d::Point4f m_obbSelectedSubmeshColour{};
		c3d::Point4f m_obbSubmeshColour{};
		c3d::Point4f m_obbBoneColour{};
		c3d::Geometry const * m_object{};
		c3d::Submesh const * m_submesh{};
		c3d::LightInstance const * m_light{};
		c3d::OnSceneUpdateConnection m_sceneConnection;
		c3d::GpuBufferOffsetT< DebugMeshConfig > m_meshConfigBuffer;
		c3d::GpuBufferOffsetT< c3d::Point4f > m_pointLightVertexBuffer;
		c3d::HashMap< uint32_t, c3d::GpuBufferOffsetT< c3d::Point4f > > m_spotLightVertexBuffers;
	};
}

#endif
