/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClearcoatNormalMapComponent_H___
#define ___C3D_ClearcoatNormalMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct ClearcoatNormalMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag ClearcoatNormal = TextureFlag( 0x01u );

		struct MaterialShader
			: shader::PassMapMaterialShader
		{
			C3D_API MaterialShader()
				: shader::PassMapMaterialShader{ "clearcoatNormal" }
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
				return makeTextureFlag( getId(), ClearcoatNormal );
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
				return castor::makeUniqueDerived< PassComponent, ClearcoatNormalMapComponent >( pass );
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
				if ( !checkFlag( filter, ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), ClearcoatNormal );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eNormalised;
				result.textureSpace |= TextureSpace::eTangentSpace;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFFFFu : mask ) } );
			}

			castor::String getTextureFlagsName( PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, ClearcoatNormal ) )
					? castor::String{ "ClearcoatNormal" }
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

		C3D_API explicit ClearcoatNormalMapComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const override;
	};
}

#endif
