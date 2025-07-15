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
	namespace c3ds = c3d::shader;

	class ToonLightingModel
	{
	protected:
		void initLightSpecifics( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components );
		c3ds::DerivFloat getNdotL( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components );
		c3ds::DerivFloat getNdotH( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components );

	private:
		c3d::RawUniquePtr< c3ds::DerivFloat > m_NdotL;
		c3d::RawUniquePtr< c3ds::DerivFloat > m_NdotH;
	};

	class ToonPhongLightingModel
		: public c3ds::PhongLightingModel
		, public ToonLightingModel
	{
	public:
		ToonPhongLightingModel( c3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::LightingModelSpec spec
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );

		C3D_ToonMaterial_API static const c3d::String getName();
		static c3ds::LightingModelPtr create( c3d::LightingModelID lightingModelId
			, c3ds::LightingModelDesc const & desc
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitLightSpecifics( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
		c3ds::DerivFloat doGetNdotL( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
		c3ds::DerivFloat doGetNdotH( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
	};

	class ToonPbrLightingModel
		: public c3ds::PbrLightingModel
		, public ToonLightingModel
	{
	public:
		explicit ToonPbrLightingModel( c3d::LightingModelID lightingModelId
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::LightingModelSpec spec
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );

		C3D_ToonMaterial_API static const c3d::String getName();
		static c3ds::LightingModelPtr create( c3d::LightingModelID lightingModelId
			, c3ds::LightingModelDesc const & desc
			, sdw::ShaderWriter & writer
			, c3ds::Materials const & materials
			, c3ds::Utils & utils
			, c3ds::BRDFHelpers & brdfHelpers
			, c3ds::Shadow & shadowModel
			, c3ds::Lights & lights
			, bool enableVolumetric );

	protected:
		void doInitLightSpecifics( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
		c3ds::DerivFloat doGetNdotL( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
		c3ds::DerivFloat doGetNdotH( c3ds::LightSurface const & lightSurface
			, c3ds::BlendComponents const & components )override;
	};
 }

#endif
