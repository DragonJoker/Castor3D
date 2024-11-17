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
		sdw::RetVec3 computeFresnel( sdw::Float const & NdotV
			, sdw::Float const & roughness
			, sdw::Vec3 const & F0
			, sdw::Float const & specularWeight );
		void computeSpecularBrdfs( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, sdw::Vec3 const & reflectedDiffuse
			, sdw::Vec3 const & reflectedSpecular
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 & metal
			, sdw::Vec3 & dielectric );
		void processBackground( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, ReflectionRefraction reflRefr
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 & backgroundResult );
		void processDirectLighting( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, DirectLighting const & directLighting
			, sdw::Vec3 const & clearcoatFresnel
			, sdw::Vec3 & directLightingResult );
		void processIndirectLighting( DebugOutputCategory const & debugOutput
			, BlendComponents const & components
			, IndirectLighting const & indirectLighting
			, sdw::Vec3 const & metalFresnel
			, sdw::Vec3 const & dielectricFresnel
			, sdw::Vec3 & indirectLightingResult );

		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat > m_computeFresnel;
	};
}

#endif
