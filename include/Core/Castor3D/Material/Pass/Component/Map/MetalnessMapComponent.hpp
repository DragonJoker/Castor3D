/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MetalnessMapComponent_H___
#define ___C3D_MetalnessMapComponent_H___

#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct MetalnessMapComponent
		: public PassMapComponent
	{
		struct ComponentsShader
			: shader::PassComponentsShader
		{
			C3D_API void applyComponents( TextureFlags const & texturesFlags
				, PipelineFlags const * flags
				, shader::TextureConfigData const & config
				, sdw::Vec4 const & sampled
				, shader::BlendComponents const & components )const override;
		};

		C3D_API explicit MetalnessMapComponent( Pass & pass );

		C3D_API static void createParsers( castor::AttributeParsers & parsers
			, ChannelFillers & channelFillers );
		C3D_API static void fillRemapMask( uint32_t maskValue
			, TextureConfiguration & configuration );
		C3D_API static bool writeTextureConfig( TextureConfiguration const & configuration
			, castor::String const & tabs
			, castor::StringStream & file );
		C3D_API static bool isComponentNeeded( TextureFlags const & textures
			, ComponentModeFlags const & filter );
		C3D_API static bool needsMapComponent( TextureConfiguration const & configuration );
		C3D_API static void createMapComponent( Pass & pass
			, std::vector< PassComponentUPtr > & result );

		C3D_API static UpdateComponent getUpdateComponent()
		{
			return updateComponent;
		}

		C3D_API static shader::PassComponentsShaderPtr createComponentsShader()
		{
			return std::make_unique< ComponentsShader >();
		}

		C3D_API void mergeImages( TextureUnitDataSet & result )override;
		C3D_API void fillChannel( TextureConfiguration & configuration
			, uint32_t mask )override;
		C3D_API void fillConfig( TextureConfiguration & config
			, PassVisitorBase & vis )override;

		TextureFlags getTextureFlags()const override
		{
			return TextureFlag::eMetalness;
		}

		C3D_API static castor::String const TypeName;

	private:
		C3D_API static void updateComponent( TextureFlags const & texturesFlags
			, shader::BlendComponents const & components );

	private:
		PassComponentUPtr doClone( Pass & pass )const override;
	};
}

#endif
