/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongLightingModel_H___
#define ___C3D_GlslPhongLightingModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	class PhongLightingModel
		: public LightingModel
	{
	public:
		C3D_API PhongLightingModel( LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdfHelpers
			, LightingModelSpec spec
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );
		C3D_API static castor::StringView getName();
		C3D_API static LightingModelPtr create( LightingModelID lightingModelId
			, LightingModelDesc const & desc
			, sdw::ShaderWriter & writer
			, Materials const & materials
			, Utils & utils
			, BRDFHelpers & brdfHelpers
			, Shadow & shadowModel
			, Lights & lights
			, bool enableVolumetric );

	private:
		C3D_API void doCombine( DebugOutputCategory const & debugOutput
			, ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, sdw::Float const & ambientOcclusion
			, DirectLighting const & directLighting
			, IndirectLighting const & indirectLighting
			, ReflectionRefraction const & reflRefr
			, sdw::Vec3 & combineResult )override;
		void processBackground( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, ReflectionRefraction reflRefr
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 & backgroundResult );
		void processDirectLighting( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, DirectLighting const & directLighting
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 & directLightingResult );
		void processIndirectLighting( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, IndirectLighting const & indirectLighting
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 & indirectLightingResult );
	};
}

#endif
