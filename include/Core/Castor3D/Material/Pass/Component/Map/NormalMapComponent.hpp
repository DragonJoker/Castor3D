/*
See LICENSE file in root folder
*/
#ifndef ___C3D_NormalMapComponent_H___
#define ___C3D_NormalMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct NormalMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag Normal = TextureFlag( 0x01u );

		struct MaterialShader
			: shader::PassMapMaterialShader
		{
			C3D_API MaterialShader()
				: shader::PassMapMaterialShader{ "normal" }
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
				return makeTextureFlag( getId(), Normal );
			}

			C3D_API static void computeMikktNormal( sdw::Float const & nmlGMul
				, sdw::UInt const & nml2Chan
				, sdw::UInt const & mask
				, shader::BlendComponents const & components
				, sdw::Vec4 const & sampled );
		};

		class Plugin
			: public PassMapComponentPlugin
		{
		public:
			using PassMapComponentPlugin::PassMapComponentPlugin;

			PassComponentUPtr createComponent( Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, NormalMapComponent >( pass );
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
				if ( !checkFlag( filter, ComponentModeFlag::eNormals ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getNormalMapFlags()const override
			{
				return getTextureFlags();
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Normal );
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
				return ( passIndex == getId() && checkFlag( textureFlags, Normal ) )
					? castor::String{ "Normal" }
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

		C3D_API explicit NormalMapComponent( Pass & pass );

		C3D_API static castor::String const TypeName;

		void needsConversion()noexcept
		{
			m_needsConversion = true;
		}

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
		void doFillConfig( TextureConfiguration & configuration
			, ConfigurationVisitorBase & vis )const override;

		private:
			bool m_needsConversion{};
	};
}

#endif
