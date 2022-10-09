/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassHeaderComponent_H___
#define ___C3D_PassHeaderComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct PassHeaderComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< uint32_t > >
	{
		struct MaterialShader
			: shader::PassMaterialShader
		{
			C3D_API MaterialShader();
			C3D_API void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		C3D_API explicit PassHeaderComponent( Pass & pass );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );
		C3D_API static void zeroBuffer( Pass const & pass
			, shader::PassMaterialShader const & materialShader
			, PassBuffer & buffer );

		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter )
		{
			// Component is never needed in shader.
			return false;
		}

		C3D_API static shader::PassMaterialShaderPtr createMaterialShader()
		{
			return std::make_unique< MaterialShader >();
		}

		C3D_API void accept( PassVisitorBase & vis )override;

		C3D_API PassFlags getPassFlags()const override
		{
			return PassFlag::eNone;
		}

		bool isLightingEnabled()const
		{
			return getData() != 0u;
		}

		uint32_t getLighting()const
		{
			return getData();
		}

		void enableLighting( bool value )
		{
			setData( value ? 1u : 0u );
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
