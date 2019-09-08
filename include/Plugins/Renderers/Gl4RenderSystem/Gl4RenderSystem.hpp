/*
See LICENSE file in root folder
*/
#ifndef ___GL4_RENDER_SYSTEM_H___
#define ___GL4_RENDER_SYSTEM_H___

#include "Gl4RenderSystemPrerequisites.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace Gl4Render
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
		/**
		*\copydoc		castor3d::RenderSystem::compileShader
		*/
		castor3d::UInt32Array compileShader( castor3d::ShaderModule const & module )const override;

	public:
		C3D_Gl4_API static castor::String Name;
		C3D_Gl4_API static castor::String Type;
	};
}

#endif
