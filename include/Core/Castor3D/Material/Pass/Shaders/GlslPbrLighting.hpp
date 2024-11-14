/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrLightingModel_H___
#define ___C3D_GlslPbrLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class PbrLightingModel
		: public LightingModel
	{
	public:
		C3D_API explicit PbrLightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdfHelpers
			, DiffuseBRDFPtr diffuse
			, SpecularBRDFPtr specular
			, SheenBRDFPtr sheen
			, ClearcoatBRDFPtr clearcoat
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );
		C3D_API static castor::StringView getName();
		C3D_API static LightingModelPtr create( LightingModelID lightingModelId
			, DiffuseBrdfDesc const & diffuseBrdf
			, SpecularBrdfDesc const & specularBrdf
			, SheenBrdfDesc const & sheenBrdf
			, ClearcoatBrdfDesc const & clearcoatBrdf
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdfHelpers
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );

	protected:
		C3D_API sdw::Vec3 doGetDiffuseResult( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedDiffuse )override;
		C3D_API sdw::Vec3 doGetSpecularResult( BlendComponents const & components
			, DirectLighting const & lighting
			, IndirectLighting const & indirect
			, sdw::Float const & ambientOcclusion
			, sdw::Vec3 const & reflectedSpecular )override;
	};
}

#endif
