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

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	MaterialSPtr CreateMaterial( Engine & p_engine, String const & p_name, Colour const & p_colour )
	{
		auto & cache = p_engine.GetMaterialCache();
		MaterialSPtr result;

		if ( cache.Has( p_name ) )
		{
			result = cache.Find( p_name );
		}

		if ( !result )
		{
			result = cache.Add( p_name, MaterialType::eLegacy );
			result->CreatePass();
		}

		result->GetTypedPass< MaterialType::eLegacy >( 0u )->SetDiffuse( p_colour );
		return result;
	}

	MaterialSPtr CreateMaterial( Engine & p_engine, String const & p_name, TextureLayoutSPtr p_texture )
	{
		auto & cache = p_engine.GetMaterialCache();
		MaterialSPtr result;

		if ( cache.Has( p_name ) )
		{
			result = cache.Find( p_name );
		}

		if ( !result )
		{
			result = cache.Add( p_name, MaterialType::eLegacy );
			result->CreatePass();
		}

		REQUIRE( result->GetType() == MaterialType::eLegacy );
		auto pass = result->GetTypedPass< MaterialType::eLegacy >( 0u );

		if ( pass->GetTextureUnitsCount() == 0 )
		{
			auto unit = std::make_shared< TextureUnit >( p_engine );
			unit->SetChannel( TextureChannel::eDiffuse );
			pass->AddTextureUnit( unit );
		}

		TextureUnitSPtr unit = pass->GetTextureUnit( 0 );
		unit->SetTexture( p_texture );
		return result;
	}
}
