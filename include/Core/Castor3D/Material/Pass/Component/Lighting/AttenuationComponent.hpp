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
		explicit AttenuationData( std::atomic_bool & dirty )
			: colour{ dirty, castor::RgbColour{ 1.0f, 1.0f, 1.0f } }
			, distance{ dirty, 0.0f }
			, thicknessFactor{ dirty, 0.0f }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RgbColour > colour;
		castor::AtomicGroupChangeTracked< float > distance;
		castor::AtomicGroupChangeTracked< float > thicknessFactor;
	};

	struct AttenuationComponent
		: public BaseDataPassComponentT< AttenuationData >
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

		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassComponentPlugin{ passComponent }
			{
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
				return std::make_unique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return std::make_unique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit AttenuationComponent( Pass & pass );

		C3D_API void accept( PassVisitorBase & vis )override;

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

		float const & getThicknessFactor()const
		{
			return m_value.thicknessFactor;
		}

		void setThicknessFactor( float v )
		{
			m_value.thicknessFactor = v;
		}

		C3D_API static castor::String const TypeName;

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
