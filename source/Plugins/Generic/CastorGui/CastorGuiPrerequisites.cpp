#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
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

	MaterialRPtr createMaterial( Engine & engine
		, String const & name
		, RgbColour const & colour )
	{
		auto & cache = engine.getMaterialCache();
		MaterialResPtr created;
		auto result = cache.tryAdd( name
			, true
			, created
			, engine
			, engine.getPassesType() );

		if ( created.lock() == result.lock() )
		{
			result.lock()->createPass();
		}

		setMaterialColour( *result.lock()->getPass( 0u ), colour );
		return result.lock().get();
	}

	MaterialRPtr createMaterial( Engine & engine
		, String const & name
		, TextureLayoutSPtr texture )
	{
		auto & cache = engine.getMaterialCache();
		MaterialResPtr created;
		auto result = cache.tryAdd( name
			, true
			, created
			, engine
			, engine.getPassesType() );

		if ( created.lock() == result.lock() )
		{
			result.lock()->createPass();
		}

		auto pass = result.lock()->getPass( 0u );

		if ( pass->getTextureUnitsCount() == 0 )
		{
			auto unit = std::make_shared< TextureUnit >( engine );
			unit->setConfiguration( TextureConfiguration::DiffuseTexture );
			pass->addTextureUnit( unit );
		}

		TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( texture );
		return result.lock().get();
	}
}
