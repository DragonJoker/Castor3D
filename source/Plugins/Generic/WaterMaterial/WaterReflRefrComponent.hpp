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
	struct WaterReflRefrComponent
		: public castor3d::PassComponent
	{
		struct ReflRefrShader
			: public castor3d::shader::PassReflRefrShader
		{
			explicit ReflRefrShader( castor3d::PassComponentPlugin const & plugin )
				: PassReflRefrShader{ plugin }
			{
			}

			void computeReflRefr( castor3d::shader::ReflectionModel & reflections
				, castor3d::shader::BlendComponents & components
				, castor3d::shader::LightSurface const & lightSurface
				, sdw::Vec4 const & position
				, castor3d::shader::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, castor3d::shader::CameraData const & camera
				, castor3d::shader::DirectLighting & lighting
				, castor3d::shader::IndirectLighting & indirect
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
				, castor3d::shader::DebugOutput & debugOutput )const override;
			void computeReflRefr( castor3d::shader::ReflectionModel & reflections
				, castor3d::shader::BlendComponents & components
				, castor3d::shader::LightSurface const & lightSurface
				, castor3d::shader::BackgroundModel & background
				, castor3d::shader::CameraData const & camera
				, castor3d::shader::DirectLighting & lighting
				, castor3d::shader::IndirectLighting & indirect
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
				, castor3d::shader::DebugOutput & debugOutput )const override;
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

			castor3d::shader::PassReflRefrShaderPtr createReflRefrShader()const override
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
