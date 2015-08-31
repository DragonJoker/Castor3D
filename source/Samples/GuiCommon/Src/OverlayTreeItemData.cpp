#include "OverlayTreeItemData.hpp"

#include <BorderPanelOverlay.hpp>
#include <Engine.hpp>
#include <FunctorEvent.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Overlay.hpp>
#include <PanelOverlay.hpp>
#include <TextOverlay.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	wxOverlayTreeItemData::wxOverlayTreeItemData( Castor3D::OverlaySPtr p_overlay )
		: wxTreeItemPropertyData( ePROPERTY_DATA_TYPE_OVERLAY )
		, m_overlay( p_overlay )
	{
	}

	wxOverlayTreeItemData::~wxOverlayTreeItemData()
	{
	}

	void wxOverlayTreeItemData::OnMaterialChange( Castor::String const & p_name )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				l_overlay->SetMaterial( l_material );
			}
		} ) );
	}

	void wxOverlayTreeItemData::OnPositionChange( Castor::Position const & p_position )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_position, l_overlay]()
		{
			l_overlay->SetPixelPosition( p_position );
		} ) );
	}

	void wxOverlayTreeItemData::OnSizeChange( Castor::Size const & p_size )
	{
		OverlaySPtr l_overlay = GetOverlay();

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_size, l_overlay]()
		{
			l_overlay->SetPixelSize( p_size );
		} ) );
	}

	void wxOverlayTreeItemData::OnBorderMaterialChange( Castor::String const & p_name )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_name, l_overlay]()
		{
			MaterialManager & l_manager = l_overlay->GetEngine()->GetMaterialManager();
			MaterialSPtr l_material = l_manager.find( p_name );

			if ( l_material )
			{
				std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderMaterial( l_material );
			}
		} ) );
	}

	void wxOverlayTreeItemData::OnBorderSizeChange( Castor::Rectangle const & p_size )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_size, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderPixelSize( p_size );
		} ) );
	}

	void wxOverlayTreeItemData::OnBorderInnerUVChange( Castor::Point4d const & p_uv )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_uv, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderInnerUV( p_uv );
		} ) );
	}

	void wxOverlayTreeItemData::OnBorderOuterUVChange( Castor::Point4d const & p_uv )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_uv, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderOuterUV( p_uv );
		} ) );
	}

	void wxOverlayTreeItemData::OnBorderPositionChange( Castor3D::eBORDER_POSITION p_position )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_position, l_overlay]()
		{
			std::static_pointer_cast< BorderPanelOverlay >( l_overlay->GetOverlayCategory() )->SetBorderPosition( p_position );
		} ) );
	}

	void wxOverlayTreeItemData::OnCaptionChange( Castor::String const & p_caption )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_caption, l_overlay]()
		{
			std::static_pointer_cast< TextOverlay >( l_overlay->GetOverlayCategory() )->SetCaption( p_caption );
		} ) );
	}

	void wxOverlayTreeItemData::OnFontChange( Castor::FontSPtr p_font )
	{
		OverlaySPtr l_overlay = GetOverlay();
		CASTOR_ASSERT( l_overlay->GetType() == eOVERLAY_TYPE_TEXT );

		l_overlay->GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [p_font, l_overlay]()
		{
			std::static_pointer_cast< TextOverlay >( l_overlay->GetOverlayCategory() )->SetFont( p_font->GetName() );
		} ) );
	}
}
