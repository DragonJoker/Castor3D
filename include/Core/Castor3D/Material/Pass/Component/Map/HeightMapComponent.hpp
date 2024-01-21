/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HeightMapComponent_H___
#define ___C3D_HeightMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct HeightMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag Height = TextureFlag( 0x01u );

		struct MaterialShader
			: shader::PassMapMaterialShader
		{
			C3D_API MaterialShader()
				: shader::PassMapMaterialShader{ "height" }
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

			C3D_API void parallaxMapping( sdw::Vec2 const & texCoords
				, sdw::Vec3 const & viewDir
				, sdw::CombinedImage2DRgba32 const & heightMap
				, shader::TextureConfigData const & textureConfig
				, sdw::UInt const & mask )const;
			C3D_API void parallaxMapping( sdw::Vec3 const & texCoords
				, sdw::Vec3 const & viewDir
				, sdw::CombinedImage2DRgba32 const & heightMap
				, shader::TextureConfigData const & textureConfig
				, sdw::UInt const & mask )const;
			C3D_API void parallaxMapping( shader::DerivTex const & texCoords
				, sdw::Vec3 const & viewDir
				, sdw::CombinedImage2DRgba32 const & heightMap
				, shader::TextureConfigData const & textureConfig
				, sdw::UInt const & mask )const;
			C3D_API void parallaxMapping( sdw::Vec2 texCoords
				, sdw::Vec2 const & dx
				, sdw::Vec2 const & dy
				, sdw::Vec3 const & viewDir
				, sdw::CombinedImage2DRgba32 const & heightMap
				, shader::TextureConfigData const & textureConfig
				, sdw::UInt const & mask )const;
			C3D_API sdw::RetFloat parallaxShadow( sdw::Vec3 const & lightDir
				, sdw::Vec2 const & initialTexCoord
				, sdw::Float const & initialHeight
				, sdw::CombinedImage2DRgba32 const & heightMap
				, shader::TextureConfigData const & textureConfig
				, sdw::UInt const & mask );

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Height );
			}

		private:
			void doComputeTexcoord( PassComponentCombine const & pass
				, shader::TextureConfigData const & config
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec2 & texCoords
				, sdw::UInt const & mask
				, shader::BlendComponents & components )const;
			void doComputeTexcoord( PassComponentCombine const & pass
				, shader::TextureConfigData const & config
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 & texCoords
				, sdw::UInt const & mask
				, shader::BlendComponents & components )const;
			void doComputeTexcoord( PassComponentCombine const & pass
				, shader::TextureConfigData const & config
				, sdw::CombinedImage2DRgba32 const & map
				, shader::DerivTex & texCoords
				, sdw::UInt const & mask
				, shader::BlendComponents & components )const;

		private:
			mutable sdw::Function< sdw::Vec2
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec3
				, sdw::InCombinedImage2DRgba32
				, shader::InTextureConfigData
				, sdw::InUInt > m_parallaxMapping;
			mutable sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InVec2
				, sdw::InFloat
				, sdw::InCombinedImage2DRgba32
				, shader::InTextureConfigData
				, sdw::InUInt > m_parallaxShadow;
		};

		class Plugin
			: public PassMapComponentPlugin
		{
		public:
			using PassMapComponentPlugin::PassMapComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, HeightMapComponent >( pass );
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool isComponentNeeded( TextureCombine const & textures
				, ComponentModeFlags const & filter )const override;
			void createMapComponent( Pass & pass
				, std::vector< PassComponentUPtr > & result )const override;
			bool hasTexcoordModif( PassComponentRegister const & passComponents
				, PipelineFlags const * flags )const override;

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

			PassComponentTextureFlag getHeightMapFlags()const override
			{
				return getTextureFlags();
			}

			void filterTextureFlags( ComponentModeFlags filter
				, TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eGeometry ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Height );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FF0000u : mask ) } );
			}

			castor::String getTextureFlagsName( PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Height ) )
					? castor::String{ "Height" }
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

		C3D_API explicit HeightMapComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const override;
	};
}

#endif
