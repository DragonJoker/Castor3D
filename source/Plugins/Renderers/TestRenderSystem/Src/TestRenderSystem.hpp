/*
See LICENSE file in root folder
*/
#ifndef ___Test_RenderSystem_H___
#define ___Test_RenderSystem_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Render/RenderSystem.hpp>

#include <Miscellaneous/DynamicLibrary.hpp>

namespace TestRender
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
		C3D_Test_API static castor::String Name;
		C3D_Test_API static castor::String Type;
	};
}

#endif
