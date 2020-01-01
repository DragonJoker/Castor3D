/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlRenderSystem_H___
#define ___C3D_GlRenderSystem_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Castor3D/Render/RenderSystem.hpp>

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d::gl
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
