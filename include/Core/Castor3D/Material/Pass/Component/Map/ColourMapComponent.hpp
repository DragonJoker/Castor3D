/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourMapComponent_H___
#define ___C3D_ColourMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct ColourMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag Colour = TextureFlag( 0x01u );

		struct MaterialShader
			: shader::PassMapMaterialShader
		{
			C3D_API MaterialShader()
				: shader::PassMapMaterialShader{ "colour" }
			{
			}
		};

		struct ComponentsShader
			: shader::PassMapComponentsShader
		{
			explicit ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassMapComponentsShader{ plugin }
			{
			}

			C3D_API void applyTexture( shader::PassShaders const & passShaders
				, shader::TextureConfigurations const & textureConfigs
				, shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, shader::Material const & material
				, shader::BlendComponents & components
				, shader::SampleTexture const & sampleTexture )const override;

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Colour );
			}
		};

		class Plugin
			: public PassMapComponentPlugin
		{
		public:
			explicit Plugin( PassComponentRegister const & passComponent )
				: PassMapComponentPlugin{ passComponent }
			{
			}

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, ColourMapComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;
			void createMapComponent( Pass & pass
				, std::vector< PassComponentUPtr > & result )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return std::make_unique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return std::make_unique< MaterialShader >();
			}

			void filterTextureFlags( ComponentModeFlags filter
				, TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eColour ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getColourMapFlags()const override
			{
				return getTextureFlags();
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Colour );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eColour;
				result.textureSpace |= TextureSpace::eAllowSRGB;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFFFFu : mask ) } );
			}

			castor::String getTextureFlagsName( PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Colour ) )
					? castor::String{ "Colour" }
					: castor::String{};
			}

		private:
			bool doWriteTextureConfig( TextureConfiguration const & configuration
				, uint32_t mask
				, castor::String const & tabs
				, castor::StringStream & file )const override;
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit ColourMapComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const override;
	};
}

#endif
