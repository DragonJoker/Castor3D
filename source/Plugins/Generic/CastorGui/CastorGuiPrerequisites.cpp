#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>

#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Material/PhongPass.hpp>
#include <Castor3D/Material/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	void setMaterialColour( Pass & pass, RgbColour const & colour )
	{
		switch ( pass.getType() )
		{
		case MaterialType::ePhong:
			static_cast< PhongPass & >( pass ).setDiffuse( colour );
			break;

		case MaterialType::eMetallicRoughness:
			static_cast< MetallicRoughnessPbrPass & >( pass ).setAlbedo( colour );
			break;

		case MaterialType::eSpecularGlossiness:
			static_cast< SpecularGlossinessPbrPass & >( pass ).setDiffuse( colour );
			break;

		default:
			CU_Failure( "Unsupported Material Type" );
			break;
		}
	}

	RgbColour getMaterialColour( Pass const & pass )
	{
		RgbColour result;

		switch ( pass.getType() )
		{
		case MaterialType::ePhong:
			result = static_cast< PhongPass const & >( pass ).getDiffuse();
			break;

		case MaterialType::eMetallicRoughness:
			result = static_cast< MetallicRoughnessPbrPass const & >( pass ).getAlbedo();
			break;

		case MaterialType::eSpecularGlossiness:
			result = static_cast< SpecularGlossinessPbrPass const & >( pass ).getDiffuse();
			break;

		default:
			CU_Failure( "Unsupported Material Type" );
			break;
		}

		return result;
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
			result = cache.add( p_name, engine.getMaterialsType() );
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
			result = cache.add( name, engine.getMaterialsType() );
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
