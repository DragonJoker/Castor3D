#include "CastorGuiPrerequisites.hpp"

#include <BorderPanelOverlay.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <TextOverlay.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	PanelOverlaySPtr CreatePanel( Engine * p_engine, String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		return l_overlay->GetPanelOverlay();
	}

	PanelOverlaySPtr CreatePanel( Engine * p_engine, String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		return l_overlay->GetPanelOverlay();
	}

	BorderPanelOverlaySPtr CreateBorderPanel( Engine * p_engine, String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, Point4d const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		l_overlay->SetPosition( p_position );
		l_overlay->SetSize( p_size );
		BorderPanelOverlaySPtr l_return = l_overlay->GetBorderPanelOverlay();
		l_return->SetBorderMaterial( p_bordersMaterial );
		l_return->SetBorderSize( p_bordersSize );
		return l_return;
	}

	BorderPanelOverlaySPtr CreateBorderPanel( Engine * p_engine, String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, Rectangle const & p_bordersSize, MaterialSPtr p_bordersMaterial, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_name, p_parent, nullptr );
		l_overlay->SetPixelPosition( p_position );
		l_overlay->SetPixelSize( p_size );
		l_overlay->SetMaterial( p_material );
		BorderPanelOverlaySPtr l_return = l_overlay->GetBorderPanelOverlay();
		l_return->SetBorderMaterial( p_bordersMaterial );
		l_return->SetBorderPixelSize( p_bordersSize );
		return l_return;
	}

	TextOverlaySPtr CreateText( Engine * p_engine, String const & p_name, Point2d const & p_position, Point2d const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPosition( p_position );
		l_return->SetSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}

	TextOverlaySPtr CreateText( Engine * p_engine, String const & p_name, Position const & p_position, Size const & p_size, MaterialSPtr p_material, FontSPtr p_font, OverlaySPtr p_parent )
	{
		OverlaySPtr l_overlay = p_engine->CreateOverlay( eOVERLAY_TYPE_TEXT, p_name, p_parent, nullptr );
		l_overlay->SetMaterial( p_material );
		TextOverlaySPtr l_return = l_overlay->GetTextOverlay();
		l_return->SetPixelPosition( p_position );
		l_return->SetPixelSize( p_size );
		l_return->SetFont( p_font->GetName() );
		return l_return;
	}
}
