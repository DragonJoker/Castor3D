#include "CastorGuiPrerequisites.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/LegacyPass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	MaterialSPtr CreateMaterial( Engine & engine, String const & p_name, Colour const & p_colour )
	{
		auto & cache = engine.getMaterialCache();
		MaterialSPtr result;

		if ( cache.has( p_name ) )
		{
			result = cache.find( p_name );
		}

		if ( !result )
		{
			result = cache.add( p_name, MaterialType::eLegacy );
			result->createPass();
		}

		result->getTypedPass< MaterialType::eLegacy >( 0u )->setDiffuse( p_colour );
		return result;
	}

	MaterialSPtr CreateMaterial( Engine & engine, String const & p_name, TextureLayoutSPtr p_texture )
	{
		auto & cache = engine.getMaterialCache();
		MaterialSPtr result;

		if ( cache.has( p_name ) )
		{
			result = cache.find( p_name );
		}

		if ( !result )
		{
			result = cache.add( p_name, MaterialType::eLegacy );
			result->createPass();
		}

		REQUIRE( result->getType() == MaterialType::eLegacy );
		auto pass = result->getTypedPass< MaterialType::eLegacy >( 0u );

		if ( pass->getTextureUnitsCount() == 0 )
		{
			auto unit = std::make_shared< TextureUnit >( engine );
			unit->setChannel( TextureChannel::eDiffuse );
			pass->addTextureUnit( unit );
		}

		TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( p_texture );
		return result;
	}
}
