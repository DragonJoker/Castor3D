/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SheenComponent_H___
#define ___C3D_SheenComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct SheenData
	{
		explicit SheenData( std::atomic_bool & dirty
			, castor::HdrRgbColour col
			, float rgh )
			: colour{ dirty, std::move( col ) }
			, roughness{ dirty, rgh }
		{
		}

		castor::AtomicGroupChangeTracked< castor::HdrRgbColour > colour;
		castor::AtomicGroupChangeTracked< float > roughness;
	};

	struct SheenComponent
		: public BaseDataPassComponentT< SheenData >
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
				return castor::makeUniqueDerived< PassComponent, SheenComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return castor::make_unique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return castor::make_unique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit SheenComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		castor::HdrRgbColour const & getSheenColour()const
		{
			return m_value.colour;
		}

		float const & getRoughnessFactor()const
		{
			return m_value.roughness;
		}

		void setSheenColour( castor::HdrRgbColour const & v )
		{
			m_value.colour = v;
		}

		void setRoughnessFactor( float v )
		{
			m_value.roughness = v;
		}

		C3D_API static castor::String const TypeName;
		C3D_API static float constexpr DefaultComponent{ 0.0f };
		C3D_API static castor::HdrRgbColour constexpr DefaultFactor{ DefaultComponent, DefaultComponent, DefaultComponent };
		C3D_API static float constexpr DefaultRoughness{ 0.0f };

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;
	};
}

#endif
