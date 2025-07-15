/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpacityComponent_H___
#define ___C3D_OpacityComponent_H___

#include "Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace c3d
{
	struct OpacityData
	{
		explicit OpacityData( std::atomic_bool & dirty
			, float opa
			, RangedValue< uint32_t > acc )
			: opacity{ dirty, opa }
			, bwAccumulationOperator{ dirty, std::move( acc ) }
		{
		}

		AtomicGroupChangeTracked< float > opacity;
		AtomicGroupChangeTracked< RangedValue< uint32_t > > bwAccumulationOperator;
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
				return makeUniqueDerived< PassComponent, OpacityComponent >( pass );
			}

			void createParsers( AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			void zeroBuffer( Pass const & pass
				, shader::PassMaterialShader const & materialShader
				, PassBuffer & buffer )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return makeRawUnique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return makeRawUnique< MaterialShader >();
			}
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
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

		C3D_API static String const TypeName;
		C3D_API static float constexpr DefaultOpacity{ 1.0f };
		C3D_API static uint32_t constexpr DefaultBwAccumulationOperator{ 1u };
		C3D_API static uint32_t constexpr MinBwAccumulationOperator{ 0u };
		C3D_API static uint32_t constexpr MaxBwAccumulationOperator{ 8u };

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		bool doWriteText( String const & tabs
			, Path const & folder
			, String const & subfolder
			, StringStream & file )const override;
		void doFillBuffer( PassBuffer & buffer )const override;
	};
}

#endif
