/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterLightingModel_H___
#define ___C3D_WaterLightingModel_H___

#include "GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

#ifndef CU_PlatformWindows
#	define C3D_WaterMaterial_API
#else
#	ifdef WaterMaterial_EXPORTS
#		define C3D_WaterMaterial_API __declspec( dllexport )
#	else
#		define C3D_WaterMaterial_API __declspec( dllimport )
#	endif
#endif

namespace water::shader
{
	namespace c3d = castor3d::shader;

	class WaterLightingModel
	{
	protected:
		WaterLightingModel( sdw::ShaderWriter & writer );

		void doComputeReflRefr( c3d::ReflectionModel & reflections
			, c3d::BlendComponents & components
			, c3d::LightSurface const & lightSurface
			, c3d::BackgroundModel & background
			, c3d::CameraData const & camera
			, c3d::OutputComponents const & lighting
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, c3d::DebugOutput & debugOutput );

	private:
		std::unique_ptr< sdw::Vec2 > m_normalMapCoords1;
		std::unique_ptr< sdw::Vec2 > m_normalMapCoords2;
		std::unique_ptr< sdw::Vec3 > m_normalMap1;
		std::unique_ptr< sdw::Vec3 > m_normalMap2;
	};

	class WaterPhongLightingModel
		: public c3d::PhongLightingModel
		, public WaterLightingModel
	{
	public:
		WaterPhongLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		C3D_WaterMaterial_API static const castor::String getName();
		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		void computeReflRefr( c3d::ReflectionModel & reflections
			, c3d::BlendComponents & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec4 const & position
			, c3d::BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, c3d::CameraData const & camera
			, c3d::OutputComponents const & lighting
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, c3d::DebugOutput & debugOutput )override;
		void computeReflRefr( c3d::ReflectionModel & reflections
			, c3d::BlendComponents & components
			, c3d::LightSurface const & lightSurface
			, c3d::BackgroundModel & background
			, c3d::CameraData const & camera
			, c3d::OutputComponents const & lighting
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, c3d::DebugOutput & debugOutput )override;
	};

	class WaterPbrLightingModel
		: public c3d::PbrLightingModel
		, public WaterLightingModel
	{
	public:
		explicit WaterPbrLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		C3D_WaterMaterial_API static const castor::String getName();
		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		void computeReflRefr( c3d::ReflectionModel & reflections
			, c3d::BlendComponents & components
			, c3d::LightSurface const & lightSurface
			, sdw::Vec4 const & position
			, c3d::BackgroundModel & background
			, sdw::CombinedImage2DRgba32 const & mippedScene
			, c3d::CameraData const & camera
			, c3d::OutputComponents const & lighting
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, c3d::DebugOutput & debugOutput )override;
		void computeReflRefr( c3d::ReflectionModel & reflections
			, c3d::BlendComponents & components
			, c3d::LightSurface const & lightSurface
			, c3d::BackgroundModel & background
			, c3d::CameraData const & camera
			, c3d::OutputComponents const & lighting
			, sdw::Vec3 const & indirectAmbient
			, sdw::Vec3 const & indirectDiffuse
			, sdw::Vec2 const & sceneUv
			, sdw::UInt const & envMapIndex
			, sdw::Vec3 const & incident
			, sdw::UInt const & hasReflection
			, sdw::UInt const & hasRefraction
			, sdw::Float const & refractionRatio
			, sdw::Vec3 & reflectedDiffuse
			, sdw::Vec3 & reflectedSpecular
			, sdw::Vec3 & refracted
			, sdw::Vec3 & coatReflected
			, sdw::Vec3 & sheenReflected
			, c3d::DebugOutput & debugOutput )override;
	};
}

#endif
