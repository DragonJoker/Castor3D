/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetalnessComponent_H___
#define ___C3D_MetalnessComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct MetalnessComponent
		: public BaseDataPassComponentT< castor::AtomicGroupChangeTracked< castor::AtomicChangeTracked< float > > >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
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

		class Plugin
			: public PassComponentPlugin
		{
		public:
			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureFlags const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >();
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

		C3D_API explicit MetalnessComponent( Pass & pass
			, float defaultValue = Default );

		C3D_API void accept( PassVisitorBase & vis )override;

		bool isValueSet()const
		{
			return m_value.value().isDirty();
		}

		float getMetalness()const
		{
			return *getData();
		}

		void setMetalness( float v )
		{
			setData( castor::makeChangeTrackedT< std::atomic_bool >( v ) );
		}

		C3D_API static castor::String const TypeName;
		static float constexpr Default = 0.0f;

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
