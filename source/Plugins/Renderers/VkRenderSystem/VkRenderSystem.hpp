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
			, AshPluginDescription desc );
		~RenderSystem();

		static castor3d::RenderSystemUPtr create( castor3d::Engine & engine
			, AshPluginDescription desc );

	private:
		castor3d::SpirVShader doCompileShader( castor3d::ShaderModule const & module )const override;

	public:
		static castor::String Name;
		static castor::String Type;
	};
}

#endif
