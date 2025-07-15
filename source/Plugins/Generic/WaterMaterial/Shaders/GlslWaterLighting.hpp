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
		: public c3ds::LightingModel
	{
	public:
		WaterLightingModel( c3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::LightingModelSpec spec
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );
		static c3d::StringView getName();
		static c3ds::LightingModelPtr create( c3d::LightingModelID lightingModelId
			, c3ds::LightingModelDesc const & desc
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );

	private:
		void doCombine( c3ds::DebugOutputCategory const & debugOutput
			, c3ds::ReflectionModel & reflections
			, sdw::CombinedImage2DRgba32 const & brdf
			, c3ds::BlendComponents const & components
			, c3ds::LightSurface const & lightSurface
			, sdw::Vec3 const & incident
			, sdw::Float const & ambientOcclusion
			, c3ds::DirectLighting const & directLighting
			, c3ds::IndirectLighting const & indirectLighting
			, c3ds::ReflectionRefraction const & reflRefr
			, sdw::Vec3 & combineResult )override;
		void processBackground( c3ds::DebugOutputCategory const & debugOutput
			, c3ds::BlendComponents const & components
			, c3ds::ReflectionRefraction reflRefr
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 & backgroundResult );
		void processDirectLighting( c3ds::DebugOutputCategory const & debugOutput
			, c3ds::BlendComponents const & components
			, c3ds::DirectLighting const & directLighting
			, sdw::Float const & fresnelFactor
			, sdw::Vec3 & directLightingResult );
		void processIndirectLighting( c3ds::DebugOutputCategory const & debugOutput
			, c3ds::BlendComponents const & components
			, c3ds::IndirectLighting const & indirectLighting
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 & indirectLightingResult );
	};
}

#endif
