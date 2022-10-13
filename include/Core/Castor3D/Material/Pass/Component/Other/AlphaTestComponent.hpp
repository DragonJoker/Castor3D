/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AlphaTestComponent_H___
#define ___C3D_AlphaTestComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct AlphaTestData
	{
		explicit AlphaTestData( std::atomic_bool & dirty )
			: alphaRefValue{ dirty, 0.0f }
			, alphaFunc{ dirty, VK_COMPARE_OP_ALWAYS }
			, blendAlphaFunc{ dirty, VK_COMPARE_OP_ALWAYS }
		{
		}

		castor::AtomicGroupChangeTracked< float > alphaRefValue;
		castor::AtomicGroupChangeTracked< VkCompareOp > alphaFunc;
		castor::AtomicGroupChangeTracked< VkCompareOp > blendAlphaFunc;
	};

	struct AlphaTestComponent
		: public BaseDataPassComponentT< AlphaTestData >
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
			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureFlagsArray const & textures
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

		C3D_API explicit AlphaTestComponent( Pass & pass );

		C3D_API void accept( PassVisitorBase & vis )override;

		C3D_API PassFlags getPassFlags()const override
		{
			return ( hasAlphaTest() || hasBlendAlphaTest() )
				? PassFlag::eAlphaTest
				: PassFlag::eNone;
		}

		bool hasAlphaTest()const
		{
			return getAlphaFunc() != VK_COMPARE_OP_ALWAYS;
		}

		bool hasBlendAlphaTest()const
		{
			return getBlendAlphaFunc() != VK_COMPARE_OP_ALWAYS;
		}

		VkCompareOp getAlphaFunc()const
		{
			return m_value.alphaFunc;
		}

		VkCompareOp getBlendAlphaFunc()const
		{
			return m_value.blendAlphaFunc;
		}

		float getAlphaRefValue()const
		{
			return m_value.alphaRefValue;
		}

		void setAlphaFunc( VkCompareOp value )
		{
			m_value.alphaFunc = value;
		}

		void setBlendAlphaFunc( VkCompareOp value )
		{
			m_value.blendAlphaFunc = value;
		}

		void setAlphaRefValue( float value )
		{
			m_value.alphaRefValue = value;
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
