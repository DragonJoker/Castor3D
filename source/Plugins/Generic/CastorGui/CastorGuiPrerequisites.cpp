#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	void setMaterialColour( Pass & pass, RgbColour const & colour )
	{
		pass.setColour( colour );
	}

	RgbColour const & getMaterialColour( Pass const & pass )
	{
		return pass.getColour();
	}

	MaterialSPtr CreateMaterial( Engine & engine, String const & p_name, RgbColour const & p_colour )
	{
		auto & cache = engine.getMaterialCache();
		MaterialSPtr result;

		if ( cache.has( p_name ) )
		{
			result = cache.find( p_name );
		}

		if ( !result )
		{
			result = cache.add( p_name, engine.getPassesType() );
			result->createPass();
		}

		setMaterialColour( *result->getPass( 0u ), p_colour );
		return result;
	}

	MaterialSPtr CreateMaterial( Engine & engine
		, String const & name
		, TextureLayoutSPtr texture )
	{
		auto & cache = engine.getMaterialCache();
		MaterialSPtr result;

		if ( cache.has( name ) )
		{
			result = cache.find( name );
		}

		if ( !result )
		{
			result = cache.add( name, engine.getPassesType() );
			result->createPass();
		}

		auto pass = result->getPass( 0u );

		if ( pass->getTextureUnitsCount() == 0 )
		{
			auto unit = std::make_shared< TextureUnit >( engine );
			unit->setConfiguration( TextureConfiguration::DiffuseTexture );
			pass->addTextureUnit( unit );
		}

		TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( texture );
		return result;
	}
}
