/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongReflectionModel_H___
#define ___C3D_GlslPhongReflectionModel_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d::shader
{
	class PhongReflectionModel
		: public ReflectionModel
	{
	public:
		C3D_API PhongReflectionModel( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & envMapBinding
			, uint32_t envMapSet );

	private:
		void doAdjustAmbient( sdw::Vec3 & ambient )const override;
		void doAdjustAlbedo( sdw::Vec3 & albedo )const override;
		void doDeclareComputeReflEnvMap()override;
		void doDeclareComputeRefrEnvMap()override;
		void doDeclareComputeReflEnvMaps()override;
		void doDeclareComputeRefrEnvMaps()override;
	};
}

#endif
