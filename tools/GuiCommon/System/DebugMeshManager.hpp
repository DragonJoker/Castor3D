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
		castor::Matrix4x4f world;
		castor::Point4f colour;
	};
	/**
	*\brief
	*	Used to display the cube box of objects.
	*/
	class DebugMeshManager
	{
	public:
		explicit DebugMeshManager( castor3d::RenderTarget const & renderTarget );
		~DebugMeshManager();
		void select( castor3d::Light const & light );
		void select( castor3d::Geometry const & object
			, castor3d::Submesh const & submesh );
		void unselect();

	private:
		void onDisplayObject();
		void onDisplayLight();

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::RenderTarget const & m_renderTarget;
		ashes::PipelineShaderStageCreateInfoArray m_cubeProgram;
		ashes::PipelineShaderStageCreateInfoArray m_meshProgram;
		ashes::VkDescriptorSetLayoutBindingArray m_bindings;
		ashes::WriteDescriptorSetArray m_writes;
		castor::Point4f m_aabbMeshColour{};
		castor::Point4f m_obbMeshColour{};
		castor::Point4f m_obbSelectedSubmeshColour{};
		castor::Point4f m_obbSubmeshColour{};
		castor::Point4f m_obbBoneColour{};
		castor3d::Geometry const * m_object{};
		castor3d::Submesh const * m_submesh{};
		castor3d::Light const * m_light{};
		castor3d::OnSceneUpdateConnection m_sceneConnection;
		castor3d::GpuBufferOffsetT< DebugMeshConfig > m_meshConfigBuffer;
		castor3d::GpuBufferOffsetT< castor::Point4f > m_pointLightVertexBuffer;
		castor::UnorderedMap< uint32_t, castor3d::GpuBufferOffsetT< castor::Point4f > > m_spotLightVertexBuffers;
	};
}

#endif
