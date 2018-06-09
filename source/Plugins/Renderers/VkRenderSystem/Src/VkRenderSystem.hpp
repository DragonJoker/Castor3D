/*
See LICENSE file in root folder
*/
#ifndef ___VK_RENDER_SYSTEM_H___
#define ___VK_RENDER_SYSTEM_H___

#include "VkRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

#include <Miscellaneous/DynamicLibrary.hpp>

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
		*\copydoc		castor3d::RenderSystem::createGlslWriter
		*/
		glsl::GlslWriter createGlslWriter()override;

	public:
		C3D_Vk_API static castor::String Name;
		C3D_Vk_API static castor::String Type;
	};
}

#endif
