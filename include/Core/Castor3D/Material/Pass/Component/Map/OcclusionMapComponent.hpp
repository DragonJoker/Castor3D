/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OcclusionMapComponent_H___
#define ___C3D_OcclusionMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct OcclusionMapComponent
		: public PassMapComponent
	{
		static constexpr TextureFlag Occlusion = TextureFlag( 0x01u );

		struct ComponentsShader
			: shader::PassComponentsShader
		{
			ComponentsShader( PassComponentPlugin const & plugin )
				: shader::PassComponentsShader{ plugin }
			{
			}

			C3D_API void fillComponents( sdw::type::BaseStruct & components
				, shader::Materials const & materials
				, sdw::StructInstance const * surface )const override;
			C3D_API void fillComponentsInits( sdw::type::BaseStruct const & components
				, shader::Materials const & materials
				, shader::Material const * material
				, sdw::StructInstance const * surface
				, sdw::expr::ExprList & inits )const override;
			C3D_API void blendComponents( shader::Materials const & materials
				, sdw::Float const & passMultiplier
				, shader::BlendComponents & res
				, shader::BlendComponents const & src )const override;
			C3D_API void applyComponents( TextureCombine const & combine
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::Vec4 const & sampled
				, shader::BlendComponents & components )const override;

			PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Occlusion );
			}
		};

		class Plugin
			: public PassComponentPlugin
		{
		public:
			void createParsers( castor::AttributeParsers & parsers
				, ChannelFillers & channelFillers )const override;
			bool writeTextureConfig( TextureConfiguration const & configuration
				, castor::String const & tabs
				, castor::StringStream & file )const override;
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

			PassComponentTextureFlag getOcclusionFlags()const override
			{
				return getTextureFlags();
			}

			void filterTextureFlags( ComponentModeFlags filter
				, TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, ComponentModeFlag::eOcclusion ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Occlusion );
			}

			void fillTextureConfiguration( TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FF0000u : mask ) } );
			}

			castor::String getMapFlagsName( PassComponentTextureFlag const & flags )const
			{
				auto [passIndex, textureFlags] = splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Occlusion ) )
					? castor::String{ "Occlusion" }
					: castor::String{};
			}
		};

		static PassComponentPluginUPtr createPlugin()
		{
			return castor::makeUniqueDerived< PassComponentPlugin, Plugin >();
		}

		C3D_API explicit OcclusionMapComponent( Pass & pass );

		C3D_API void fillConfig( TextureConfiguration & configuration
			, PassVisitorBase & vis )const override;

		PassComponentTextureFlag getTextureFlags()const override
		{
			return makeTextureFlag( getId(), Occlusion );
		}

		C3D_API static castor::String const TypeName;

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
