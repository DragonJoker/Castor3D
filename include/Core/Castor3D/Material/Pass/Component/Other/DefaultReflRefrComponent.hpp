/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DefaultReflRefrComponent_H___
#define ___C3D_DefaultReflRefrComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct DefaultReflRefrComponent
		: public PassComponent
	{
		struct ReflRefrShader
			: shader::PassReflRefrShader
		{
			explicit ReflRefrShader( PassComponentPlugin const & plugin )
				: shader::PassReflRefrShader{ plugin }
			{
			}

			C3D_API void computeReflRefr( shader::ReflectionModel & reflections
				, shader::BlendComponents & components
				, shader::LightSurface const & lightSurface
				, sdw::Vec4 const & position
				, shader::BackgroundModel & background
				, sdw::CombinedImage2DRgba32 const & mippedScene
				, shader::CameraData const & camera
				, shader::OutputComponents & lighting
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
				, shader::DebugOutput & debugOutput )const override;
			C3D_API void computeReflRefr( shader::ReflectionModel & reflections
				, shader::BlendComponents & components
				, shader::LightSurface const & lightSurface
				, shader::BackgroundModel & background
				, shader::CameraData const & camera
				, shader::OutputComponents & lighting
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
				, shader::DebugOutput & debugOutput )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent }
			{
			}

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, DefaultReflRefrComponent >( pass );
			}

			bool isReflRefrComponent()const override
			{
				return true;
			}

			shader::PassReflRefrShaderPtr createReflRefrShader()const override
			{
				return std::make_unique< ReflRefrShader >( *this );
			}

			void filterComponentFlags( ComponentModeFlags filter
				, PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit DefaultReflRefrComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
