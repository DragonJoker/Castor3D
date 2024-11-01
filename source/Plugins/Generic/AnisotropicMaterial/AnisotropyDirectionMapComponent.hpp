/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnisotropyDirectionMapComponent_H___
#define ___C3D_AnisotropyDirectionMapComponent_H___

#include "AnisotropyComponent.hpp"

#include <Castor3D/Material/Pass/Component/PassMapComponent.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace anisotropy
{
	struct AnisotropyDirectionMapComponent
		: public castor3d::PassMapComponent
	{
		static constexpr castor3d::TextureFlag Direction = castor3d::TextureFlag( 0x01u );

		struct MaterialShader
			: castor3d::shader::PassMapMaterialShader
		{
			MaterialShader()
				: castor3d::shader::PassMapMaterialShader{ cuT( "anisotropyDirection" ) }
			{
			}
		};

		struct ComponentsShader
			: castor3d::shader::PassMapComponentsShader
		{
			using castor3d::shader::PassMapComponentsShader::PassMapComponentsShader;

			void applyTexture( castor3d::shader::PassShaders const & passShaders
				, castor3d::shader::TextureConfigurations const & textureConfigs
				, castor3d::shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::Material const & material
				, castor3d::shader::BlendComponents & components
				, castor3d::shader::SampleTexture const & sampleTexture )const override;

			castor3d::PassComponentTextureFlag getTextureFlags()const
			{
				return castor3d::makeTextureFlag( getId(), Direction );
			}
		};

		class Plugin
			: public castor3d::PassMapComponentPlugin
		{
		public:
			using PassMapComponentPlugin::PassMapComponentPlugin;

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, AnisotropyDirectionMapComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, castor3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( castor3d::TextureCombine const & textures
				, castor3d::ComponentModeFlags const & filter )const override;
			void createMapComponent( castor3d::Pass & pass
				, castor::Vector< castor3d::PassComponentUPtr > & result )const override;

			bool isMapComponent()const override
			{
				return true;
			}

			castor3d::shader::PassComponentsShaderPtr createComponentsShader()const override
			{
				return castor::make_unique< ComponentsShader >( *this );
			}

			castor3d::shader::PassMaterialShaderPtr createMaterialShader()const override
			{
				return castor::make_unique< MaterialShader >();
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
				return castor3d::makeTextureFlag( getId(), Direction );
			}

			void fillTextureConfiguration( castor3d::TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= castor3d::TextureSpace::eNormalised;
				result.textureSpace |= castor3d::TextureSpace::eTangentSpace;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFF00u/*RG as of GLTF spec*/ : mask ) } );
			}

			castor::String getTextureFlagsName( castor3d::PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = castor3d::splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Direction ) )
					? castor::String{ cuT( "AnisotropyDirection" ) }
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

		C3D_AnisotropicMaterial_API explicit AnisotropyDirectionMapComponent( castor3d::Pass & pass );

		C3D_AnisotropicMaterial_API static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		void doFillConfig( castor3d::TextureConfiguration & configuration
			, castor3d::ConfigurationVisitorBase & vis )const override;
	};

	CU_DeclareSmartPtr( anisotropy, AnisotropyDirectionMapComponent, C3D_AnisotropicMaterial_API );
}

#endif
