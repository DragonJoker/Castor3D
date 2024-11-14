/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterReflRefrComponent_H___
#define ___C3D_WaterReflRefrComponent_H___

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/PassComponent.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslLighting.hpp>

namespace water
{
	namespace c3d = castor3d::shader;

	struct WaterReflRefrComponent
		: public castor3d::PassComponent
	{
		struct ReflRefrShader
			: public c3d::PassReflRefrShader
		{
			using c3d::PassReflRefrShader::PassReflRefrShader;

			void computeWithTransmission( c3d::ReflectionModel & reflections
				, c3d::BlendComponents & components
				, c3d::LightSurface const & lightSurface
				, c3d::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, c3d::CameraData const & camera
				, c3d::DirectLighting & lighting
				, c3d::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3d::ReflectionRefraction & output
				, c3d::DebugOutputCategory const & debugOutput )const override;
			void computeWithoutTransmission( c3d::ReflectionModel & reflections
				, c3d::BlendComponents & components
				, c3d::LightSurface const & lightSurface
				, c3d::BackgroundModel & background
				, c3d::CameraData const & camera
				, c3d::DirectLighting & lighting
				, c3d::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3d::ReflectionRefraction & output
				, c3d::DebugOutputCategory const & debugOutput )const override;
		};

		class Plugin
			: public castor3d::PassComponentPlugin
		{
		public:
			explicit Plugin( castor3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< castor3d::PassComponent, WaterReflRefrComponent >( pass );
			}

			c3d::PassReflRefrShaderPtr createReflRefrShader()const override
			{
				return castor::make_unique< ReflRefrShader >( *this );
			}

			bool isReflRefrComponent()const override
			{
				return true;
			}

			void filterComponentFlags( castor3d::ComponentModeFlags filter
				, castor3d::PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, castor3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, castor3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit WaterReflRefrComponent( castor3d::Pass & pass );

		static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
	};

	CU_DeclareSmartPtr( water, WaterReflRefrComponent, );
}

#endif
