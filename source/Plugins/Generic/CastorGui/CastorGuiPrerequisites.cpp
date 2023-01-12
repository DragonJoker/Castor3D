#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/PassComponentRegister.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorGui
{
	void setMaterialColour( castor3d::Pass & pass, castor::HdrRgbColour const & colour )
	{
		pass.setColour( colour );
	}

	castor::HdrRgbColour const & getMaterialColour( castor3d::Pass const & pass )
	{
		return pass.getColour();
	}

	castor3d::MaterialRPtr createMaterial( castor3d::Engine & engine
		, castor::String const & name
		, castor::HdrRgbColour const & colour )
	{
		auto & cache = engine.getMaterialCache();
		castor3d::MaterialResPtr created;
		auto result = cache.tryAdd( name
			, true
			, created
			, engine
			, engine.getDefaultLightingModel() );

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
			, engine.getDefaultLightingModel() );

		if ( created.lock() == result.lock() )
		{
			result.lock()->createPass();
		}

		auto pass = result.lock()->getPass( 0u );

		if ( pass->getTextureUnitsCount() == 0 )
		{
			auto & texCache = engine.getTextureUnitCache();
			auto & compRegister = engine.getPassComponentsRegister();
			castor3d::TextureConfiguration baseConfig{};
			compRegister.fillTextureConfiguration( compRegister.getColourMapFlags(), baseConfig );
			auto unit = std::make_shared< castor3d::TextureUnit >( engine
				, texCache.getSourceData( castor3d::TextureSourceInfo{ nullptr, texture->getCreateInfo() }
					, castor3d::PassTextureConfig{ { {} }, baseConfig }
					, nullptr ) );
			unit->setConfiguration( baseConfig );
			//pass->addTextureUnit( unit );
		}

		castor3d::TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( texture );
		return result.lock().get();
	}
}
