#include "CastorGuiPrerequisites.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>
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
	void setMaterialColour( Pass & pass, Colour const & colour )
	{
		switch ( pass.getType() )
		{
		case MaterialType::eLegacy:
			static_cast< LegacyPass & >( pass ).setDiffuse( colour );
			break;

		case MaterialType::ePbrMetallicRoughness:
			static_cast< MetallicRoughnessPbrPass & >( pass ).setAlbedo( colour );
			break;

		case MaterialType::ePbrSpecularGlossiness:
			static_cast< SpecularGlossinessPbrPass & >( pass ).setDiffuse( colour );
			break;
		}
	}

	Colour getMaterialColour( Pass const & pass )
	{
		Colour result;

		switch ( pass.getType() )
		{
		case MaterialType::eLegacy:
			result = static_cast< LegacyPass const & >( pass ).getDiffuse();
			break;

		case MaterialType::ePbrMetallicRoughness:
			result = static_cast< MetallicRoughnessPbrPass const & >( pass ).getAlbedo();
			break;

		case MaterialType::ePbrSpecularGlossiness:
			result = static_cast< SpecularGlossinessPbrPass const & >( pass ).getDiffuse();
			break;
		}

		return result;
	}

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
			result = cache.add( p_name, engine.getMaterialsType() );
			result->createPass();
		}

		setMaterialColour( *result->getPass( 0u ), p_colour );
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
			result = cache.add( p_name, engine.getMaterialsType() );
			result->createPass();
		}

		auto pass = result->getPass( 0u );

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
