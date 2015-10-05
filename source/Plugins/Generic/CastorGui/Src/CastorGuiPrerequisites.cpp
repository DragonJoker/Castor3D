#include "CastorGuiPrerequisites.hpp"

#include <BorderPanelOverlay.hpp>
#include <Engine.hpp>
#include <InitialiseEvent.hpp>
#include <MaterialManager.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <TextOverlay.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, Colour const & p_colour )
	{
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		MaterialSPtr l_return = l_manager.find( p_name );

		if ( !l_return )
		{
			l_return = std::make_shared< Material >( p_engine, p_name );
			l_return->CreatePass();
			p_engine->PostEvent( MakeInitialiseEvent( *l_return ) );
			l_manager.insert( p_name, l_return );
		}

		l_return->GetPass( 0 )->SetAmbient( p_colour );
		return l_return;
	}

	MaterialSPtr CreateMaterial( Engine * p_engine, String const & p_name, TextureBaseSPtr p_texture )
	{
		MaterialManager & l_manager = p_engine->GetMaterialManager();
		MaterialSPtr l_return = l_manager.find( p_name );

		if ( !l_return )
		{
			l_return = std::make_shared< Material >( p_engine, p_name );
			l_return->CreatePass();
			p_engine->PostEvent( MakeInitialiseEvent( *l_return ) );
			l_manager.insert( p_name, l_return );
		}

		PassSPtr l_pass = l_return->GetPass( 0 );

		if ( l_pass->GetTextureUnitsCount() == 0 )
		{
			l_pass->AddTextureUnit();
		}

		TextureUnitSPtr l_unit = l_pass->GetTextureUnit( 0 );
		l_unit->SetTexture( p_texture );
		return l_return;
	}
}
