#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorGui
{
	void setMaterialColour( castor3d::Pass & pass, castor::RgbColour const & colour )
	{
		pass.setColour( colour );
	}

	castor::RgbColour const & getMaterialColour( castor3d::Pass const & pass )
	{
		return pass.getColour();
	}

	castor3d::MaterialRPtr createMaterial( castor3d::Engine & engine
		, castor::String const & name
		, castor::RgbColour const & colour )
	{
		auto & cache = engine.getMaterialCache();
		castor3d::MaterialResPtr created;
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

	castor3d::MaterialRPtr createMaterial( castor3d::Engine & engine
		, castor::String const & name
		, castor3d::TextureLayoutSPtr texture )
	{
		auto & cache = engine.getMaterialCache();
		castor3d::MaterialResPtr created;
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
			auto unit = std::make_shared< castor3d::TextureUnit >( engine
				, castor3d::TextureSourceInfo{ nullptr, texture->getCreateInfo() } );
			unit->setConfiguration( castor3d::TextureConfiguration::DiffuseTexture );
			//pass->addTextureUnit( unit );
		}

		castor3d::TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( texture );
		return result.lock().get();
	}
}
