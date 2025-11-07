/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AmbientColourMapComponent_H___
#define ___C3D_AmbientColourMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace c3d
{
	struct AmbientColourMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag Ambient = TextureFlag( 0x01u );
		static constexpr uint32_t ComponentCount = 3u;
		static constexpr uint32_t DefaultMask = 0x00FFFFFF;

		struct MaterialShader
			: shader::PassMapMaterialShader
		{
			C3D_API MaterialShader()
				: shader::PassMapMaterialShader{ cuT( "ambientColour" ) }
			{
			}
		};

		struct ComponentsShader
			: shader::PassMapComponentsShader
		{
			using shader::PassMapComponentsShader::PassMapComponentsShader;

			C3D_API void applyTexture( shader::PassShaders const & passShaders
				, shader::TextureConfigurations const & textureConfigs
				, shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, shader::Material const & material
				, shader::BlendComponents & components
				, shader::SampleTexture const & sampleTexture )const override;

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Ambient );
			}
		};

		class Plugin
			: public PassMapComponentPlugin
		{
		public:
			using PassMapComponentPlugin::PassMapComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return makeUniqueDerived< PassComponent, AmbientColourMapComponent >( pass );
			}

			void createParsers( AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;
			void createMapComponent( Pass & pass
				, Vector< PassComponentUPtr > & result )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return makeRawUnique< ComponentsShader >( *this );
			}

			shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return makeRawUnique< MaterialShader >();
			}

			void filterTextureFlags( ComponentModeFlags filter
				, TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eDiffuseLighting ) )
					remFlags( texturesFlags, getTextureFlags() );
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Ambient );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eColour;
				result.textureSpace |= TextureSpace::eAllowSRGB;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? DefaultMask : mask ) } );
			}

			String getTextureFlagsName( PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Ambient ) )
					? String{ cuT( "AmbientColour" ) }
					: String{};
			}

		private:
			bool doWriteTextureConfig( TextureConfiguration const & configuration
				, uint32_t mask
				, String const & tabs
				, StringStream & file )const override;
		};

		static PassComponentPluginUPtr createPlugin( PassComponentRegister const & passComponent )
		{
			return makeUniqueDerived< PassComponentPlugin, Plugin >( passComponent );
		}

		C3D_API explicit AmbientColourMapComponent( Pass & pass );

		C3D_API PassMapDefaultImageParams createDefaultImage()const;

		C3D_API static String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const override;
	};
}

#endif
