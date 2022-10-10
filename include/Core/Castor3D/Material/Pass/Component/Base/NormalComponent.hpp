/*
See LICENSE file in root folder
*/
#ifndef ___C3D_NormalComponent_H___
#define ___C3D_NormalComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct NormalComponent
		: public PassComponent
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents const & res
				, shader::BlendComponents const & src )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			bool isComponentNeeded( TextureFlags const & textures
				, ComponentModeFlags const & filter )const override
			{
				return checkFlag( filter, ComponentModeFlag::eGeometry )
					|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
					|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
					|| checkFlag( filter, ComponentModeFlag::eOcclusion );
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >();
			}
		};

		C3D_API static PassComponentPluginUPtr createPlugin()
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >();
		}

		C3D_API explicit NormalComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
