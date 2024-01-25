/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonLightingModel_H___
#define ___C3D_GlslToonLightingModel_H___

#include "GlslToonProfile.hpp"

#include <Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp>
#include <Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

#ifndef CU_PlatformWindows
#	define C3D_ToonMaterial_API
#else
#	ifdef ToonMaterial_EXPORTS
#		define C3D_ToonMaterial_API __declspec( dllexport )
#	else
#		define C3D_ToonMaterial_API __declspec( dllimport )
#	endif
#endif

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	class ToonLightingModel
	{
	protected:
		void initLightSpecifics( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components );
		sdw::Float getNdotL( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components );
		sdw::Float getNdotH( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components );

	private:
		castor::RawUniquePtr< sdw::Float > m_NdotL;
		castor::RawUniquePtr< sdw::Float > m_NdotH;
	};

	class ToonPhongLightingModel
		: public c3d::PhongLightingModel
		, public ToonLightingModel
	{
	public:
		ToonPhongLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		C3D_ToonMaterial_API static const castor::String getName();
		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitLightSpecifics( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
		sdw::Float doGetNdotL( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
		sdw::Float doGetNdotH( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
	};

	class ToonPbrLightingModel
		: public c3d::PbrLightingModel
		, public ToonLightingModel
	{
	public:
		explicit ToonPbrLightingModel( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

		C3D_ToonMaterial_API static const castor::String getName();
		static c3d::LightingModelUPtr create( castor3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3d::Materials const & materials
			, c3d::Utils & utils
			, c3d::BRDFHelpers & brdf
			, c3d::Shadow & shadowModel
			, c3d::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitLightSpecifics( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
		sdw::Float doGetNdotL( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
		sdw::Float doGetNdotH( c3d::LightSurface const & lightSurface
			, c3d::BlendComponents const & components )override;
	};
 }

#endif
