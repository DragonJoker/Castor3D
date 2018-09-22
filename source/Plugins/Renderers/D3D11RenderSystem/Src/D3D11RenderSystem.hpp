/*
See LICENSE file in root folder
*/
#ifndef ___D3D11_RENDER_SYSTEM_H___
#define ___D3D11_RENDER_SYSTEM_H___

#include "D3D11RenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

#include <Miscellaneous/DynamicLibrary.hpp>

namespace D3D11Render
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
		C3D_D3D11_API static castor::String Name;
		C3D_D3D11_API static castor::String Type;
	};
}

#endif
