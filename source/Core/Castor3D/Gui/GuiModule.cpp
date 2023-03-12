#include "Castor3D/Gui/GuiModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Graphics/Font.hpp>

namespace castor3d
{
	void setMaterialColour( Pass & pass, castor::HdrRgbColour const & colour )
	{
		pass.setColour( colour );
	}

	castor::HdrRgbColour const & getMaterialColour( Pass const & pass )
	{
		return pass.getColour();
	}

	MaterialRPtr createMaterial( Engine & engine
		, castor::String const & name
		, castor::HdrRgbColour const & colour )
	{
		auto & cache = engine.getMaterialCache();
		MaterialResPtr created;
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

	MaterialRPtr createMaterial( Engine & engine
		, castor::String const & name
		, TextureLayoutSPtr texture )
	{
		auto & cache = engine.getMaterialCache();
		MaterialResPtr created;
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
			TextureConfiguration baseConfig{};
			compRegister.fillTextureConfiguration( compRegister.getColourMapFlags(), baseConfig );
			auto unit = std::make_shared< TextureUnit >( engine
				, texCache.getSourceData( TextureSourceInfo{ nullptr, texture->getCreateInfo() }
					, PassTextureConfig{ { {} }, baseConfig }
					, nullptr ) );
			unit->setConfiguration( baseConfig );
			//pass->addTextureUnit( unit );
		}

		TextureUnitSPtr unit = pass->getTextureUnit( 0 );
		unit->setTexture( texture );
		return result.lock().get();
	}
}
