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
	namespace c3ds = c3d::shader;

	struct WaterReflRefrComponent
		: public c3d::PassComponent
	{
		struct ReflRefrShader
			: public c3ds::PassReflRefrShader
		{
			using c3ds::PassReflRefrShader::PassReflRefrShader;

			void computeWithTransmission( c3ds::ReflectionModel & reflections
				, c3ds::BlendComponents & components
				, c3ds::LightSurface const & lightSurface
				, c3ds::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, c3ds::CameraData const & camera
				, c3ds::RenderData const & render
				, c3ds::DirectLighting & lighting
				, c3ds::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3ds::ReflectionRefraction & output
				, c3ds::DebugOutputCategory const & debugOutput )const override;
			void computeWithoutTransmission( c3ds::ReflectionModel & reflections
				, c3ds::BlendComponents & components
				, c3ds::LightSurface const & lightSurface
				, c3ds::BackgroundModel & background
				, c3ds::CameraData const & camera
				, c3ds::RenderData const & render
				, c3ds::DirectLighting & lighting
				, c3ds::IndirectLighting & indirect
				, sdw::Vec2 const & sceneUv
				, sdw::UInt const & envMapIndex
				, sdw::Vec3 const & incident
				, c3ds::ReflectionRefraction & output
				, c3ds::DebugOutputCategory const & debugOutput )const override;
		};

		class Plugin
			: public c3d::PassComponentPlugin
		{
		public:
			explicit Plugin( c3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< c3d::PassComponent, WaterReflRefrComponent >( pass );
			}

			c3ds::PassReflRefrShaderPtr createReflRefrShader()const override
			{
				return c3d::makeRawUnique< ReflRefrShader >( *this );
			}

			bool isReflRefrComponent()const override
			{
				return true;
			}

			void filterComponentFlags( c3d::ComponentModeFlags filter
				, c3d::PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, c3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, c3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponent )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit WaterReflRefrComponent( c3d::Pass & pass );

		static c3d::String const TypeName;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
	};

	CU_DeclareSmartPtr( water, WaterReflRefrComponent, );
}

#endif
