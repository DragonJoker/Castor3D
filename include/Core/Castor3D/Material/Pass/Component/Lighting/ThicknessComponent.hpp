/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ThicknessComponent_H___
#define ___C3D_ThicknessComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace c3d
{
	struct ThicknessComponent
		: public BaseDataPassComponentT< AtomicGroupChangeTracked< float > >
	{
		struct MaterialShader
			: shader::PassMaterialShader
		{
			MaterialShader();
			void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			using shader::PassComponentsShader::PassComponentsShader;

			void fillComponents( ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return makeUniqueDerived< PassComponent, ThicknessComponent >( pass );
			}

			void createParsers( AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return makeRawUnique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return makeRawUnique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit ThicknessComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		float const & getThicknessFactor()const
		{
			return getData();
		}

		void setThicknessFactor( float v )
		{
			setData( v );
		}

		C3D_API static String const TypeName;
		C3D_API static float constexpr Default = 0.0f;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( String const & tabs
			, Path const & folder
			, String const & subfolder
			, StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;
	};
}

#endif
