/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FFTWaterComponent_H___
#define ___C3D_FFTWaterComponent_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Material/Pass/Component/BaseDataPassComponent.hpp>
#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Shaders/GlslLighting.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>

namespace ocean_fft
{
	struct FFTWaterComponent
		: public castor3d::BaseDataPassComponentT< castor::AtomicGroupChangeTracked< float > >
	{
		struct ComponentsShader
			: castor3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( castor3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, castor3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, castor3d::shader::Materials const & materials
				, castor3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( castor3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, castor3d::shader::BlendComponents & res
				, castor3d::shader::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::Material const & material
				, castor3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		struct MaterialShader
			: castor3d::shader::PassMaterialShader
		{
			MaterialShader();
			void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		class Plugin
			: public castor3d::PassComponentPlugin
		{
		public:
			explicit Plugin( castor3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			void zeroBuffer(castor3d::Pass const & pass
				, castor3d::shader::PassMaterialShader const & materialShader
				, castor3d::PassBuffer & buffer )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< castor3d::PassComponent, FFTWaterComponent >( pass );
			}

			castor3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return castor::make_unique< ComponentsShader >( *this );
			}

			castor3d::shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return castor::make_unique< MaterialShader >();
			}

			void filterComponentFlags( castor3d::ComponentModeFlags filter
				, castor3d::PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, castor3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, castor3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit FFTWaterComponent( castor3d::Pass & pass );

		void accept( castor3d::ConfigurationVisitorBase & vis )override;

		float getWaterDensity()const
		{
			return getData();
		}

		void setWaterDensity( float v )
		{
			setData( v );
		}

		static castor::String const TypeName;
		static float constexpr Default = 1.0f;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		bool doWriteText( castor::String const & tabs
			, castor::Path const & folder
			, castor::String const & subfolder
			, castor::StringStream & file )const override;
		void doFillBuffer( castor3d::PassBuffer & buffer )const override;
	};

	CU_DeclareSmartPtr( ocean_fft, FFTWaterComponent, );
}

#endif
