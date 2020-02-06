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
	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, Colour const & p_colour )
	{
		auto & l_cache = p_engine->GetMaterialCache();
		MaterialSPtr l_return;

		if ( l_cache.Has( p_name ) )
		{
			l_return = l_cache.Find( p_name );
		}

		if ( !l_return )
		{
			l_return = l_cache.Add( p_name, MaterialType::eLegacy );
			l_return->CreatePass();
		}

		l_return->GetTypedPass< MaterialType::eLegacy >( 0u )->SetAmbient( p_colour );
		return l_return;
	}

	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, TextureLayoutSPtr p_texture )
	{
		auto & l_cache = p_engine->GetMaterialCache();
		MaterialSPtr l_return;

		if ( l_cache.Has( p_name ) )
		{
			l_return = l_cache.Find( p_name );
		}

		if ( !l_return )
		{
			l_return = l_cache.Add( p_name, MaterialType::eLegacy);
			l_return->CreatePass();
		}

		REQUIRE( l_return->GetType() == MaterialType::eLegacy );
		auto l_pass = l_return->GetTypedPass< MaterialType::eLegacy >( 0u );

		if ( l_pass->GetTextureUnitsCount() == 0 )
		{
			auto l_unit = std::make_shared< TextureUnit >( *p_engine );
			l_unit->SetChannel( TextureChannel::eDiffuse );
			l_pass->AddTextureUnit( l_unit );
		}

		TextureUnitSPtr l_unit = l_pass->GetTextureUnit( 0 );
		l_unit->SetTexture( p_texture );
		return l_return;
	}
}
