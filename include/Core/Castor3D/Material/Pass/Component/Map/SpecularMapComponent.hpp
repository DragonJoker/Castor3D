/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SpecularMapComponent_H___
#define ___C3D_SpecularMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace castor3d
{
	struct SpecularMapComponent
		: public PassMapComponent
	{
		static const TextureFlag Specular = TextureFlag( 0x01u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void applyComponents( TextureFlagsArray const & texturesFlags
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, shader::BlendComponents & components )const override;

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Specular );
			}
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			Plugin()
				: PassComponentPlugin{ &Plugin::doUpdateComponent }
			{
			}

			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool writeTextureConfig( TextureConfiguration const & configuration
				, castor::String const & tabs
				, castor::StringStream & file )const override;
			bool isComponentNeeded( TextureFlagsArray const & textures
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

			void filterTextureFlags( ComponentModeFlags filter
				, TextureFlagsArray & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eSpecularLighting ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Specular );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eColour;
				result.textureSpace |= TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFFFFu : mask ) } );
			}

			castor::String getMapFlagsName( PassComponentTextureFlag const & flags )const
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Specular ) )
					? castor::String{ "Specular" }
					: castor::String{};
			}

		private:
			static void doUpdateComponent( PassComponentRegister const & passComponents
				, TextureFlagsArray const & texturesFlags
				, shader::BlendComponents & components );
		};

		static PassComponentPluginUPtr createPlugin()
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >();
		}

		C3D_API explicit SpecularMapComponent( Pass & pass );

		C3D_API void fillConfig( TextureConfiguration & configuration
			, PassVisitorBase & vis )const override;

		PassComponentTextureFlag getTextureFlags()const override
		{
			return makeTextureFlag( getId(), Specular );
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
