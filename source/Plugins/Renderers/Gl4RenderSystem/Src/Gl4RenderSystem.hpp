/*
See LICENSE file in root folder
*/
#ifndef ___GL4_RENDER_SYSTEM_H___
#define ___GL4_RENDER_SYSTEM_H___

#include "Gl4RenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

#include <Miscellaneous/DynamicLibrary.hpp>

namespace Gl4Render
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
		C3D_Gl4_API static castor::String Name;
		C3D_Gl4_API static castor::String Type;
	};
}

#endif
