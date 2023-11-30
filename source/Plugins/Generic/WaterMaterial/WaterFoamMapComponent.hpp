/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WaterFoamMapComponent_H___
#define ___C3D_WaterFoamMapComponent_H___

#include "Shaders/GlslWaterProfile.hpp"

#include <Castor3D/Material/Pass/Component/PassMapComponent.hpp>

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

namespace water
{
	struct WaterFoamMapComponent
		: public castor3d::PassMapComponent
	{
		static constexpr castor3d::TextureFlag Foam = castor3d::TextureFlag( 0x01u );

		struct ComponentsShader
			: castor3d::shader::PassComponentsShader
		{
			explicit ComponentsShader( castor3d::PassComponentPlugin const & plugin )
				: castor3d::shader::PassComponentsShader{ plugin }
			{
			}

			void computeTexcoord( castor3d::PipelineFlags const & flags
				, castor3d::shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 & texCoords
				, sdw::Vec2 & texCoord
				, sdw::UInt const & mapId
				, castor3d::shader::BlendComponents & components )const override;
			void computeTexcoord( castor3d::PipelineFlags const & flags
				, castor3d::shader::TextureConfigData const & config
				, sdw::U32Vec3 const & imgCompConfig
				, sdw::CombinedImage2DRgba32 const & map
				, castor3d::shader::DerivTex & texCoords
				, castor3d::shader::DerivTex & texCoord
				, sdw::UInt const & mapId
				, castor3d::shader::BlendComponents & components )const override;
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
			void updateComponent( sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, castor3d::shader::BlendComponents & components
				, bool isFrontCulled )const override;

			castor3d::PassComponentTextureFlag getTextureFlags()const
			{
				return makeTextureFlag( getId(), Foam );
			}
		};

		class Plugin
			: public castor3d::PassMapComponentPlugin
		{
		public:
			explicit Plugin( castor3d::PassComponentRegister const & passComponent )
				: castor3d::PassMapComponentPlugin{ passComponent }
			{
			}

			castor3d::PassComponentUPtr createComponent( castor3d::Pass & pass )const override
			{
				return castor::makeUniqueDerived< PassComponent, WaterFoamMapComponent >( pass );
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

			void filterTextureFlags( castor3d::ComponentModeFlags filter
				, castor3d::TextureCombine & texturesFlags )const override
			{
				if ( !checkFlag( filter, castor3d::ComponentModeFlag::eSpecularLighting )
					&& !checkFlag( filter, castor3d::ComponentModeFlag::eDiffuseLighting )
					&& !checkFlag( filter, castor3d::ComponentModeFlag::eColour ) )
				{
					remFlags( texturesFlags, getTextureFlags() );
				}
			}

			castor3d::PassComponentTextureFlag getTextureFlags()const override
			{
				return makeTextureFlag( getId(), Foam );
			}

			void fillTextureConfiguration( castor3d::TextureConfiguration & result
				, uint32_t mask )const override
			{
				result.textureSpace |= castor3d::TextureSpace::eNormalised;
				addFlagConfiguration( result, { getTextureFlags(), ( mask == 0 ? 0x00FFFFFu : mask ) } );
			}

			castor::String getTextureFlagsName( castor3d::PassComponentTextureFlag const & flags )const override
			{
				auto [passIndex, textureFlags] = castor3d::splitTextureFlag( flags );
				return ( passIndex == getId() && checkFlag( textureFlags, Foam ) )
					? castor::String{ "WaterFoam" }
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

		explicit WaterFoamMapComponent( castor3d::Pass & pass );

		static castor::String const TypeName;

	private:
		castor3d::PassComponentUPtr doClone( castor3d::Pass & pass )const override;
		void doFillConfig( castor3d::TextureConfiguration & configuration
			, castor3d::ConfigurationVisitorBase & vis )const override;
	};

	CU_DeclareSmartPtr( water, WaterFoamMapComponent, );
}

#endif
