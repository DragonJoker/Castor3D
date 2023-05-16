/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularComponent_H___
#define ___C3D_SpecularComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct SpecularData
	{
		explicit SpecularData( std::atomic_bool & dirty
			, castor::RgbColour defaultValue )
			: colour{ dirty, defaultValue }
			, factor{ dirty, 1.0f }
		{
		}

		castor::AtomicGroupChangeTracked< castor::RgbColour > colour;
		castor::AtomicGroupChangeTracked< float > factor;
	};

	struct SpecularComponent
		: public BaseDataPassComponentT< SpecularData >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void fillComponents( ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
			C3D_API void updateOutputs( sdw::StructInstance const & components
				, sdw::StructInstance const & surface
				, sdw::Vec4 & spcRgh
				, sdw::Vec4 & colMtl )const override;
		};

		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
			C3D_API void updateMaterial( sdw::Vec3 const & albedo
				, sdw::Vec4 const & spcRgh
				, sdw::Vec4 const & colMtl
				, shader::Material & material )const override;
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
				return castor::makeUniqueDerived< PassComponent, SpecularComponent >( pass );
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

		C3D_API explicit SpecularComponent( Pass & pass
			, castor::RgbColour defaultValue = Default );

		C3D_API void accept( PassVisitorBase & vis )override;

		float getFactor()const
		{
			return m_value.factor;
		}

		void setFactor( float v )
		{
			m_value.factor = v;
		}

		castor::RgbColour const & getSpecular()const
		{
			return m_value.colour.value();
		}

		void setSpecular( castor::RgbColour const & v )
		{
			m_value.colour = castor::makeChangeTrackedT< std::atomic_bool >( v );
		}

		void setSpecular( castor::HdrRgbColour const & v
			, float gamma )
		{
			setSpecular( castor::RgbColour{ v, gamma } );
		}

		void setSpecular( castor::Coords3f const & v )
		{
			setSpecular( castor::RgbColour{ v[0u], v[1u], v[2u] } );
		}

		void setSpecular( castor::Point3f const & v )
		{
			setSpecular( castor::RgbColour{ v[0u], v[1u], v[2u] } );
		}

		C3D_API static castor::String const TypeName;
		static float constexpr DefaultFactor = 1.0f;
		static float constexpr DefaultComponent = 1.0f;
		C3D_API static castor::RgbColour const Default;

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
