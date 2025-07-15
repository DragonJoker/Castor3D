/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DiffuseTransmissionComponent_H___
#define ___C3D_DiffuseTransmissionComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace c3d
{
	struct DiffuseTransmissionData
	{
		explicit DiffuseTransmissionData( std::atomic_bool & dirty
			, RgbColour col
			, float fac )
			: colour{ dirty, std::move( col ) }
			, factor{ dirty, fac }
		{
		}

		AtomicGroupChangeTracked< RgbColour > colour;
		AtomicGroupChangeTracked< float > factor;
	};

	struct DiffuseTransmissionComponent
		: public BaseDataPassComponentT< DiffuseTransmissionData >
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
				return makeUniqueDerived< PassComponent, DiffuseTransmissionComponent >( pass );
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

			PassComponentFlag getTransmissionFlag()const override
			{
				return getComponentFlags();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit DiffuseTransmissionComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		float const & getTransmissionFactor()const
		{
			return m_value.factor;
		}

		RgbColour const & getTransmissionColour()const
		{
			return m_value.colour;
		}

		void setTransmissionFactor( float v )
		{
			m_value.factor = v;
		}

		void setTransmissionColour( RgbColour const & v )
		{
			m_value.colour = v;
		}

		C3D_API static String const TypeName;
		C3D_API static float constexpr DefaultFactor{ 0.0f };
		C3D_API static float constexpr DefaultComponent{ 1.0f };
		C3D_API static RgbColour constexpr DefaultColour{ DefaultComponent, DefaultComponent, DefaultComponent };

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
