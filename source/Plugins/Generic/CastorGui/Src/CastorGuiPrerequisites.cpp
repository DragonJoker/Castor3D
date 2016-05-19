#include "CastorGuiPrerequisites.hpp"

#include <Engine.hpp>
#include <MaterialManager.hpp>

#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/Pass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, Colour const & p_colour )
	{
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		MaterialSPtr l_return;

		if ( l_manager.Has( p_name ) )
		{
			l_return = l_manager.Find( p_name );
		}

		if ( !l_return )
		{
			l_return = l_manager.Create( p_name, *p_engine );
			l_return->CreatePass();
		}

		l_return->GetPass( 0 )->SetAmbient( p_colour );
		return l_return;
	}

	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, TextureSPtr p_texture )
	{
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		MaterialSPtr l_return;

		if ( l_manager.Has( p_name ) )
		{
			l_return = l_manager.Find( p_name );
		}

		if ( !l_return )
		{
			l_return = l_manager.Create( p_name, *p_engine );
			l_return->CreatePass();
		}

		PassSPtr l_pass = l_return->GetPass( 0 );

		if ( l_pass->GetTextureUnitsCount() == 0 )
		{
			auto l_unit = std::make_shared< TextureUnit >( *p_engine );
			l_unit->SetChannel( eTEXTURE_CHANNEL_DIFFUSE );
			l_pass->AddTextureUnit( l_unit );
		}

		TextureUnitSPtr l_unit = l_pass->GetTextureUnit( 0 );
		l_unit->SetTexture( p_texture );
		return l_return;
	}
}
