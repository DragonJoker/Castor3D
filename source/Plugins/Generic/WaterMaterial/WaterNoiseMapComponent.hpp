/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterNoiseMapComponent_H___
#define ___C3D_WaterNoiseMapComponent_H___

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/PassMapComponent.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace water
{
	struct WaterNoiseMapComponent
		: public castor3d::PassMapComponent
	{
		static constexpr castor3d::TextureFlag Noise = castor3d::TextureFlag( 0x01u );

		struct MaterialShader
			: castor3d::shader::PassMapMaterialShader
		{
			MaterialShader()
				: castor3d::shader::PassMapMaterialShader{ "waterNoise" }
			{
			}
		};

		struct ComponentsShader
			: castor3d::shader::PassMapComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: castor3d::shader::PassMapComponentsShader{ plugin }
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
			void applyTexture( castor3d::shader::PassShaders const & passShaders
				, castor3d::shader::TextureConfigurations const & textureConfigs
				, castor3d::shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::Material const & material
				, castor3d::shader::BlendComponents & components
				, castor3d::shader::SampleTexture const & sampleTexture )const override;
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::Material const & material
				, castor3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;

			castor3d::PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Noise );
			}
		};

		class Plugin
			: public castor3d::PassMapComponentPlugin
		{
		public:
			explicit Plugin( castor3d::PassComponentRegister const & passComponents )
				: castor3d::PassMapComponentPlugin{ passComponents }
			{
			}

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, WaterNoiseMapComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;
			void createMapComponent( castor3d::Pass & pass
				, std::vector< castor3d::PassComponentUPtr > & result )const override;
			bool hasTexcoordModif( castor3d::PassComponentRegister const & passComponents
				, castor3d::PipelineFlags const * flags )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			castor3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			castor3d::shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return std::make_unique< MaterialShader >();
			}

			void filterTextureFlags( castor3d::ComponentModeFlags filter
				, castor3d::TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, castor3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			castor3d::PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Noise );
			}

			void fillTextureConfiguration( castor3d::TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= castor3d::TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FF0000u : mask ) } );
			}

			castor::String getTextureFlagsName( castor3d::PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = castor3d::splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Noise ) )
					? castor::String{ "WaterNoise" }
					: castor::String{};
			}

		private:
			bool doWriteTextureConfig( castor3d::TextureConfiguration const & configuration
				, uint32_t mask
				, castor::String const & tabs
				, castor::StringStream & file )const override;
		};

		static castor3d::PassComponentPluginUPtr createPlugin( castor3d::PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< castor3d::PassComponentPlugin, Plugin >( passComponent );
		}

		explicit WaterNoiseMapComponent( castor3d::Pass & pass );

		static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		void doFillConfig( castor3d::TextureConfiguration & configuration
			, castor3d::ConfigurationVisitorBase & vis )const override;
	};

	CU_DeclareSmartPtr( water, WaterNoiseMapComponent, );
}

#endif
