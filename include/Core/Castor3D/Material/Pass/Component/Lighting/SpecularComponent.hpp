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
	struct SpecularComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< castor::AtomicChangeTracked< castor::RgbColour > > >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			ComponentsShader( PassComponentPlugin const & plugin )
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

		static PassComponentPluginUPtr createPlugin()
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >();
		}

		C3D_API explicit SpecularComponent( Pass & pass
			, castor::RgbColour defaultValue = Default );

		C3D_API void accept( PassVisitorBase & vis )override;

		bool isValueSet()const
		{
			return m_value.value().isDirty();
		}

		castor::RgbColour const & getSpecular()const
		{
			return *getData();
		}

		void setSpecular( castor::RgbColour const & v )
		{
			setData( castor::makeChangeTrackedT< std::atomic_bool >( v ) );
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
		static float constexpr DefaultComponent = 0.0f;
		static float constexpr DefaultPhongComponent = 1.0f;
		static float constexpr DefaultPbrComponent = 0.04f;
		C3D_API static castor::RgbColour const Default;
		C3D_API static castor::RgbColour const DefaultPhong;
		C3D_API static castor::RgbColour const DefaultPbr;

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
