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
		: public c3d::BaseDataPassComponentT< c3d::AtomicGroupChangeTracked< float > >
	{
		struct MaterialShader
			: c3d::shader::PassMaterialShader
		{
			MaterialShader();
			void fillMaterialType( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
		};

		struct ComponentsShader
			: c3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( c3d::PassComponentPlugin const & plugin )
				: PassComponentsShader{ plugin }
			{
			}

			void fillComponents( c3d::ComponentModeFlags componentsMask
				, sdw::type::BaseStruct & components
				, c3d::shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			void fillComponentsInits( sdw::type::BaseStruct const & components
				, c3d::shader::Materials const & materials
				, c3d::shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::Vec4 const * clrCot
				, sdw::expr::ExprList & inits )const override;
			void blendComponents( c3d::shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, c3d::shader::BlendComponents & res
				, c3d::shader::BlendComponents const & src )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::shader::Material const & material
				, c3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;
		};

		class Plugin
			: public c3d::PassComponentPlugin
		{
		public:
			explicit Plugin( c3d::PassComponentRegister const & passComponents )
				: PassComponentPlugin{ passComponents }
			{
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			void zeroBuffer(c3d::Pass const & pass
				, c3d::shader::PassMaterialShader const & materialShader
				, c3d::PassBuffer & buffer )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< c3d::PassComponent, FFTWaterComponent >( pass );
			}

			c3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return c3d::makeRawUnique< ComponentsShader >( *this );
			}

			c3d::shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return c3d::makeRawUnique< MaterialShader >();
			}

			void filterComponentFlags( c3d::ComponentModeFlags filter
				, c3d::PassComponentCombine & componentsFlags )const override
			{
				if ( !checkFlag( filter, c3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, c3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( componentsFlags, getComponentFlags() );
				}
			}
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponent )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit FFTWaterComponent( c3d::Pass & pass );

		void accept( c3d::ConfigurationVisitorBase & vis )override;

		float getWaterDensity()const
		{
			return getData();
		}

		void setWaterDensity( float v )
		{
			setData( v );
		}

		static c3d::String const TypeName;
		static float constexpr Default = 1.0f;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		bool doWriteText( c3d::String const & tabs
			, c3d::Path const & folder
			, c3d::String const & subfolder
			, c3d::StringStream & file )const override;
		void doFillBuffer( c3d::PassBuffer & buffer )const override;
	};

	CU_DeclareSmartPtr( ocean_fft, FFTWaterComponent, );
}

#endif
