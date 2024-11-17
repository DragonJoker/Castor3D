/*
See LICENSE file in root folder
*/
#ifndef ___C3DWR_GlslWaterLightingModel_H___
#define ___C3DWR_GlslWaterLightingModel_H___

#include "GlslWaterProfile.hpp"

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace water::shader
{
	class WaterLightingModel
		: public c3d::LightingModel
	{
	public:
		WaterLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdfHelpers
			, c3d::LightingModelSpec spec
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );
		static castor::StringView getName();
		static c3d::LightingModelPtr create( castor3d::LightingModelID lightingModelId
			, c3d::LightingModelDesc const & desc
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdfHelpers
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

	private:
		void doCombine( c3d::DebugOutputCategory const & debugOutput
			, c3d::ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, c3d::BlendComponents const & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, sdw::Float const & ambientOcclusion
			, c3d::DirectLighting const & directLighting
			, c3d::IndirectLighting const & indirectLighting
			, c3d::ReflectionRefraction const & reflRefr
			, sdw::Vec3 & combineResult )override;
		void processBackground( c3d::DebugOutputCategory const & debugOutput
			, c3d::BlendComponents const & components
			, c3d::ReflectionRefraction reflRefr
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 & backgroundResult );
		void processDirectLighting( c3d::DebugOutputCategory const & debugOutput
			, c3d::BlendComponents const & components
			, c3d::DirectLighting const & directLighting
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 & directLightingResult );
		void processIndirectLighting( c3d::DebugOutputCategory const & debugOutput
			, c3d::BlendComponents const & components
			, c3d::IndirectLighting const & indirectLighting
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 & indirectLightingResult );
	};
}

#endif
