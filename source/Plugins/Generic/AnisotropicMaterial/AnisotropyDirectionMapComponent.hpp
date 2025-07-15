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
		: public c3d::PassMapComponent
	{
		static constexpr c3d::TextureFlag Direction = c3d::TextureFlag( 0x01u );

		struct MaterialShader
			: c3d::shader::PassMapMaterialShader
		{
			MaterialShader()
				: c3d::shader::PassMapMaterialShader{ cuT( "anisotropyDirection" ) }
			{
			}
		};

		struct ComponentsShader
			: c3d::shader::PassMapComponentsShader
		{
			using c3d::shader::PassMapComponentsShader::PassMapComponentsShader;

			void applyTexture( c3d::shader::PassShaders const & passShaders
				, c3d::shader::TextureConfigurations const & textureConfigs
				, c3d::shader::TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, c3d::shader::Material const & material
				, c3d::shader::BlendComponents & components
				, c3d::shader::SampleTexture const & sampleTexture )const override;

			c3d::PassComponentTextureFlag getTextureFlags()const
			{
				return c3d::makeTextureFlag( getId(), Direction );
			}
		};

		class Plugin
			: public c3d::PassMapComponentPlugin
		{
		public:
			using PassMapComponentPlugin::PassMapComponentPlugin;

			c3d::PassComponentUPtr createComponent( c3d::Pass & pass )const override
			{
				return c3d::makeUniqueDerived< PassComponent, AnisotropyDirectionMapComponent >( pass );
			}

			void createParsers( c3d::AttributeParsers & parsers
				, c3d::ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( c3d::TextureCombine const & textures
				, c3d::ComponentModeFlags const & filter )const override;
			void createMapComponent( c3d::Pass & pass
				, c3d::Vector< c3d::PassComponentUPtr > & result )const override;

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
				if ( !checkFlag( filter, c3d::ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			c3d::PassComponentTextureFlag getTextureFlags()const override
			{
				return c3d::makeTextureFlag( getId(), Direction );
			}

			void fillTextureConfiguration( c3d::TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= c3d::TextureSpace::eNormalised;
				result.textureSpace |= c3d::TextureSpace::eTangentSpace;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFF00u/*RG as of GLTF spec*/ : mask ) } );
			}

			c3d::String getTextureFlagsName( c3d::PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = c3d::splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Direction ) )
					? c3d::String{ cuT( "AnisotropyDirection" ) }
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

		C3D_AnisotropicMaterial_API explicit AnisotropyDirectionMapComponent( c3d::Pass & pass );

		C3D_AnisotropicMaterial_API static c3d::String const TypeName;

	private:
		c3d::PassComponentUPtr doClone( c3d::Pass & pass )const override;
		void doFillConfig( c3d::TextureConfiguration & configuration
			, c3d::ConfigurationVisitorBase & vis )const override;
	};

	CU_DeclareSmartPtr( anisotropy, AnisotropyDirectionMapComponent, C3D_AnisotropicMaterial_API );
}

#endif
