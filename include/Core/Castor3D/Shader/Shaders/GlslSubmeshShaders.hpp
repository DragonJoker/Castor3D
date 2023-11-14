/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSubmeshShaders_H___
#define ___C3D_GlslSubmeshShaders_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp"

namespace castor3d::shader
{
	class SubmeshShaders
	{
	public:
		C3D_API SubmeshShaders( SubmeshComponentRegister const & compRegister
			, PipelineFlags const & flags );

		C3D_API void fillSurface( sdw::type::IOStruct & surface )const;
		C3D_API void fillSurface( sdw::type::BaseStruct & surface )const;

	private:
		SubmeshComponentRegister const & m_compRegister;
		std::vector< SubmeshSurfaceShader * > m_shaders;
		PipelineFlags const & m_flags;
	};
}

#endif
