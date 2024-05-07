/*
See LICENSE file in root folder
*/
#ifndef ___GC_CubeBoxManager_HPP___
#define ___GC_CubeBoxManager_HPP___
#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>

#include <Castor3D/Buffer/GpuBufferOffset.hpp>

#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace GuiCommon
{
	struct CubeBoxConfig
	{
		castor::Matrix4x4f world;
		castor::Point4f colour;
	};
	/**
	*\brief
	*	Used to display the cube box of objects.
	*/
	class CubeBoxManager
	{
	public:
		explicit CubeBoxManager( castor3d::RenderTarget const & renderTarget );
		~CubeBoxManager();
		void displayObject( castor3d::Geometry const & object
			, castor3d::Submesh const & submesh );
		void hideObject( castor3d::Geometry const & object );

	private:
		void onSceneUpdate( castor3d::Scene const & scene );

	private:
		castor3d::RenderTarget const & m_renderTarget;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		ashes::VkDescriptorSetLayoutBindingArray m_bindings;
		ashes::WriteDescriptorSetArray m_writes;
		castor::Point4f m_aabbMeshColour{};
		castor::Point4f m_obbMeshColour{};
		castor::Point4f m_obbSelectedSubmeshColour{};
		castor::Point4f m_obbSubmeshColour{};
		castor::Point4f m_obbBoneColour{};
		castor3d::Geometry const * m_object{};
		castor3d::Submesh const * m_submesh{};
		castor3d::OnSceneUpdateConnection m_sceneConnection;
		castor3d::GpuBufferOffsetT< CubeBoxConfig > m_cubeBoxBuffer;
	};
}

#endif
