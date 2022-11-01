/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RoughnessComponent_H___
#define ___C3D_RoughnessComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct RoughnessComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< float > >
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
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
			C3D_API void updateOutputs( sdw::StructInstance const & components
				, sdw::StructInstance const & surface
				, sdw::Vec4 & spcRgh
				, sdw::Vec4 & colMtl
				, sdw::Vec4 & sheen )const override;
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
				, sdw::Vec4 const & crTsIr
				, sdw::Vec4 const & sheen
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

		C3D_API explicit RoughnessComponent( Pass & pass
			, float defaultValue = 1.0f );

		C3D_API float getGlossiness()const;
		C3D_API void setGlossiness( float v );
		C3D_API float getShininess()const;
		C3D_API void setShininess( float v );

		C3D_API void accept( PassVisitorBase & vis )override;

		float getRoughness()const
		{
			return getData();
		}

		void setRoughness( float v )
		{
			setData( v );
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
