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

		C3D_API void fillVertexSurface( sdw::type::IOStruct & surface )const;
		C3D_API void fillVertexSurface( sdw::type::BaseStruct & surface )const;

		C3D_API void fillRasterSurface( sdw::type::IOStruct & surface
			, uint32_t & index )const;
		C3D_API void fillRasterSurface( sdw::type::BaseStruct & surface )const;

	private:
		SubmeshComponentRegister const & m_compRegister;
		castor::Vector< SubmeshVertexSurfaceShader * > m_vertexShaders;
		castor::Vector< SubmeshRasterSurfaceShader * > m_rasterShaders;
		PipelineFlags const & m_flags;
	};
}

#endif
