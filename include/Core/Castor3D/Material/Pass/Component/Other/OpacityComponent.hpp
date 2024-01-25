/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpacityComponent_H___
#define ___C3D_OpacityComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct OpacityData
	{
		explicit OpacityData( std::atomic_bool & dirty )
			: opacity{ dirty, 1.0f }
			, bwAccumulationOperator{ dirty, castor::makeRangedValue( 1u, 0u, 8u ) }
		{
		}

		castor::AtomicGroupChangeTracked< float > opacity;
		castor::AtomicGroupChangeTracked< castor::RangedValue< uint32_t > > bwAccumulationOperator;
	};

	struct OpacityComponent
		: public BaseDataPassComponentT< OpacityData >
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			using shader::PassComponentsShader::PassComponentsShader;

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
			using PassComponentPlugin::PassComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, OpacityComponent >( pass );
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

		C3D_API explicit OpacityComponent( Pass & pass );

		C3D_API void accept( ConfigurationVisitorBase & vis )override;

		C3D_API void setOpacity( float v );

		bool needsAlphaProcessing()const
		{
			return m_value.opacity < 1.0f;
		}

		float getOpacity()const
		{
			return m_value.opacity;
		}

		uint32_t getBWAccumulationOperator()const
		{
			return m_value.bwAccumulationOperator->value();
		}

		void setBWAccumulationOperator( uint32_t value )
		{
			*m_value.bwAccumulationOperator = value;
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
