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
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

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
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent }
			{
			}

			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override
			{
				return checkFlag( filter, ComponentModeFlag::eNormals )
					|| checkFlag( filter, ComponentModeFlag::eDiffuseLighting )
					|| checkFlag( filter, ComponentModeFlag::eSpecularLighting )
					|| checkFlag( filter, ComponentModeFlag::eOcclusion );
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit NormalComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
