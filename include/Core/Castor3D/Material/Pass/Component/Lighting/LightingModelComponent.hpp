/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingModelComponent_H___
#define ___C3D_LightingModelComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
{
	struct LightingModelComponent
		: public BaseDataPassComponentT< AtomicGroupChangeTracked< uint32_t > >
	{
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
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return makeUniqueDerived< PassComponent, LightingModelComponent >( pass );
			}

			void createParsers( AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return makeRawUnique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit LightingModelComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API String getLightingModelName()const;
		C3D_API String getDiffuseBrdfName()const;
		C3D_API String getSpecularBrdfName()const;
		C3D_API String getSheenBrdfName()const;
		C3D_API String getClearcoatBrdfName()const;
		C3D_API String getScatteringModelName()const;

		LightingModelID getLightingModelId()const
		{
			return LightingModelID( getData() );
		}

		void setLightingModelId( LightingModelID v )
		{
			setData( uint32_t( v ) );
			m_zeroBasedValue = v - 1u;
		}

		C3D_API static String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( String const & tabs
			, Path const & folder
			, String const & subfolder
			, StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;

	private:
		uint32_t m_zeroBasedValue{};
	};
}

#endif
