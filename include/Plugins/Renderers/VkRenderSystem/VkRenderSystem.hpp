/*
See LICENSE file in root folder
*/
#ifndef ___VK_RENDER_SYSTEM_H___
#define ___VK_RENDER_SYSTEM_H___

#include "VkRenderSystemPrerequisites.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace VkRender
{
	class RenderSystem
		: public castor3d::RenderSystem
	{
	public:
		RenderSystem( castor3d::Engine & engine
			, castor::String const & appName
			, bool enableValidation );
		~RenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine
			, castor::String const & appName
			, bool enableValidation );
		/**
		*\copydoc		castor3d::RenderSystem::compileShader
		*/
		castor3d::UInt32Array compileShader( castor3d::ShaderModule const & module )override;

	public:
		C3D_Vk_API static castor::String Name;
		C3D_Vk_API static castor::String Type;
	};
}

#endif
