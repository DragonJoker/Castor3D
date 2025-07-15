/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterFoamMapComponent_H___
#define ___C3D_WaterFoamMapComponent_H___

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/PassMapComponent.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace water
{
	struct WaterFoamMapComponent
		: public c3d::PassMapComponent
	{
		static constexpr c3d::TextureFlag Foam = c3d::TextureFlag( 0x01u );

		struct MaterialShader
			: c3d::shader::PassMapMaterialShader
		{
			MaterialShader()
				: c3d::shader::PassMapMaterialShader{ cuT( "waterFoam" ) }
			{
			}
		};

		struct ComponentsShader
			: c3d::shader::PassMapComponentsShader
		{
			explicit ComponentsShader( c3d::PassComponentPlugin const & plugin )
				: c3d::shader::PassMapComponentsShader{ plugin }
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
			void applyTexture( c3d::shader::PassShaders const & passShaders
				, c3d::shader::TextureConfigurations const & textureConfigs
				, c3d::shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::shader::Material const & material
				, c3d::shader::BlendComponents & components
				, c3d::shader::SampleTexture const & sampleTexture )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::shader::Material const & material
				, c3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;

			c3d::PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Foam );
			}
		};

		class Plugin
			: public c3d::PassMapComponentPlugin
		{
		public:
			explicit Plugin( c3d::PassComponentRegister const & passComponent )
				: c3d::PassMapComponentPlugin{ passComponent }
			{
			}

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< PassComponent, WaterFoamMapComponent >( pass );
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;
			void createMapComponent( c3d::Pass & pass
				, c3d::Vector< c3d::PassComponentUPtr > & result )const override;
			bool hasTexcoordModif( c3d::PassComponentRegister const & passComponents
				, c3d::PipelineFlags const * flags )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			c3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return c3d::makeRawUnique< ComponentsShader >( *this );
			}

			c3d::shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return c3d::makeRawUnique< MaterialShader >();
			}

			void filterTextureFlags( c3d::ComponentModeFlags filter
				, c3d::TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, c3d::ComponentModeFlag::eSpecularLighting )
					&& !checkFlag( filter, c3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, c3d::ComponentModeFlag::eColour ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			c3d::PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Foam );
			}

			void fillTextureConfiguration( c3d::TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= c3d::TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFFFu : mask ) } );
			}

			c3d::String getTextureFlagsName( c3d::PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = c3d::splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Foam ) )
					? c3d::String{ cuT( "WaterFoam" ) }
					: c3d::String{};
			}

		private:
			bool doWriteTextureConfig( c3d::TextureConfiguration const & configuration
				, uint32_t mask
				, c3d::String const & tabs
				, c3d::StringStream & file )const override;
		};

		static c3d::PassComponentPluginUPtr createPlugin( c3d::PassComponentRegister const & passComponent )
		{
			return c3d::makeUniqueDerived< c3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit WaterFoamMapComponent( c3d::Pass & pass );

		static c3d::String const TypeName;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		void doFillConfig( c3d::TextureConfiguration & configuration
			, c3d::ConfigurationVisitorBase & vis )const override;
	};

	CU_DeclareSmartPtr( water, WaterFoamMapComponent, );
}

#endif
