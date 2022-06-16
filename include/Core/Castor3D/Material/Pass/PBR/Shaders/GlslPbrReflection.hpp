/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrReflectionModel_H___
#define ___C3D_GlslPbrReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PbrReflectionModel
		: public ReflectionModel
	{
	public:
		C3D_API PbrReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & envMapBinding
			, uint32_t envMapSet );

	private:
		void doDeclareComputeReflEnvMap()override;
		void doDeclareComputeRefrEnvMap()override;
		void doDeclareComputeReflEnvMaps()override;
		void doDeclareComputeRefrEnvMaps()override;
	};
}

#endif
