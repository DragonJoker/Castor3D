/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AttenuationComponent_H___
#define ___C3D_AttenuationComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct AttenuationData
	{
		explicit AttenuationData( std::atomic_bool & dirty
			, castor::RgbColour col
			, float dist )
			: colour{ dirty, std::move( col ) }
			, distance{ dirty, dist }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RgbColour > colour;
		castor::AtomicGroupChangeTracked< float > distance;
	};

	struct AttenuationComponent
		: public BaseDataPassComponentT< AttenuationData >
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
				return castor::makeUniqueDerived< PassComponent, AttenuationComponent >( pass );
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

		C3D_API explicit AttenuationComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		castor::RgbColour const & getAttenuationColour()const
		{
			return m_value.colour;
		}

		void setAttenuationColour( castor::RgbColour const & v )
		{
			m_value.colour = v;
		}

		float const & getAttenuationDistance()const
		{
			return m_value.distance;
		}

		void setAttenuationDistance( float v )
		{
			m_value.distance = v;
		}

		C3D_API static castor::String const TypeName;
		C3D_API static float constexpr DefaultComponent{ 1.0f };
		C3D_API static castor::RgbColour constexpr DefaultColour{ DefaultComponent, DefaultComponent, DefaultComponent };
		C3D_API static float constexpr DefaultDistance{ std::numeric_limits< float >::infinity() };

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
