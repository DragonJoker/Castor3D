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
			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents const & res
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

		C3D_API explicit RoughnessComponent( Pass & pass
			, float defaultValue = 1.0f );

		C3D_API float getGlossiness()const;
		C3D_API void setGlossiness( float v );
		C3D_API float getShininess()const;
		C3D_API void setShininess( float v );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );
		C3D_API static void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer );
		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter );

		C3D_API static shader::PassComponentsShaderPtr createComponentsShader()
		{
			return std::make_unique< ComponentsShader >();
		}

		C3D_API static shader::PassMaterialShaderPtr createMaterialShader()
		{
			return std::make_unique< MaterialShader >();
		}

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
