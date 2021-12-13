#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include "GuiCommon/System/GradientButton.hpp"

#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#include <wx/propgrid/advprops.h>
#include <iostream>
#pragma warning( pop )

using namespace castor;

namespace GuiCommon
{
	//************************************************************************************************

	ButtonData::ButtonData( ButtonEventMethod method )
		: m_method( method )
	{
	}

	void ButtonData::Call( wxVariant const & var )
	{
		m_method( var );
	}

	//************************************************************************************************

	wxPGWindowList ButtonEventEditor::CreateControls( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxPoint const & p_pos, wxSize const & p_size )const
	{
		// create and return a single button to be used as editor
		// size and pos represent the entire value cell: use that to position the button
		return wxPGWindowList( new GradientButton( p_propgrid, wxID_ANY, _( "View" ), p_pos, p_size ) );
	}

	// since the editor does not need to change the primary control (the button)
	// to reflect changes, UpdateControl is just a no-op
	void ButtonEventEditor::UpdateControl( wxPGProperty * property, wxWindow * ctrl ) const
	{
	}

	bool ButtonEventEditor::OnEvent( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxWindow * p_wnd_primary, wxEvent & p_event )const
	{
		// handle the button event
		if ( p_event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
		{
			// extract the client data from the property
			if ( ButtonData * btn = dynamic_cast< ButtonData * >( p_property->GetClientObject() ) )
			{
				// call the method
				btn->Call( p_property->GetValue() );
				return false;
			}
		}

		return false;
	}
	//************************************************************************************************

	wxFloatProperty * CreateProperty( wxString const & p_name, double const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	wxFloatProperty * CreateProperty( wxString const & p_name, float const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	wxIntProperty * CreateProperty( wxString const & p_name, int const & p_value )
	{
		return new wxIntProperty( p_name, wxPG_LABEL, p_value );
	}

	wxUIntProperty * CreateProperty( wxString const & p_name, uint32_t const & p_value )
	{
		return new wxUIntProperty( p_name, wxPG_LABEL, p_value );
	}

	wxBoolProperty * CreateProperty( wxString const & p_name, bool const & p_value, bool p_checkbox )
	{
		wxBoolProperty * result = new wxBoolProperty( p_name, wxPG_LABEL );
		result->SetAttribute( wxT( "UseCheckbox" ), p_checkbox );
		return result;
	}

	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value )
	{
		return new wxStringProperty( p_name, wxPG_LABEL, p_value );
	}

	wxPGProperty * addAttributes( wxPGProperty * prop )
	{
		prop->SetEditor( wxPGEditor_SpinCtrl );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
		return prop;
	}

	//************************************************************************************************

	wxIMPLEMENT_DYNAMIC_CLASS( gcImageFileProperty, wxFileProperty )

	gcImageFileProperty::gcImageFileProperty( castor::ImageLoader * loader
		, wxString const & label
		, wxString const & name
		, wxString const & value )
		: wxFileProperty{ label, name, value }
		, m_loader{ loader }
	{
		m_wildcard = wxPGGetDefaultImageWildcard();
		doLoadImageFromFile();
	}

	void gcImageFileProperty::OnSetValue()
	{
		wxFileProperty::OnSetValue();

		// Delete old image
		m_image.reset();
		m_bitmap.reset();

		doLoadImageFromFile();
	}

	wxSize gcImageFileProperty::OnMeasureImage( int ) const
	{
		return wxPG_DEFAULT_IMAGE_SIZE;
	}

	void gcImageFileProperty::OnCustomPaint( wxDC & dc
		, wxRect const & rect
		, wxPGPaintData & )
	{
		if ( m_bitmap || m_image )
		{
			// Draw the thumbnail
			// Create the bitmap here because required size is not known in OnSetValue().

			// Delete the cache if required size changed
			if ( m_bitmap && ( m_bitmap->GetWidth() != rect.width || m_bitmap->GetHeight() != rect.height ) )
			{
				m_bitmap.reset();
			}

			if ( !m_bitmap )
			{
				m_image->resample( { uint32_t( rect.width ), uint32_t( rect.height ) } );
				m_bitmap = std::make_unique< wxBitmap >();
				createBitmapFromBuffer( m_image->getPixels()
					, false
					, *m_bitmap );
			}

			dc.DrawBitmap( *m_bitmap, rect.x, rect.y, false );
		}
		else
		{
			// No file - just draw a white box
			dc.SetBrush( *wxWHITE_BRUSH );
			dc.DrawRectangle( rect );
		}
	}

	void gcImageFileProperty::doLoadImageFromFile()
	{
		wxFileName filename = GetFileName();

		// Create the image thumbnail
		if ( filename.FileExists() )
		{
			CU_Require( m_loader );
			auto name = make_String( filename.GetName() );

			try
			{
				auto buffer = m_loader->load( name
					, make_Path( filename.GetFullPath() )
					, {} ).getPixels();

				if ( castor::isCompressed( buffer->getFormat() ) )
				{
					buffer = castor::decompressBuffer( buffer );
				}

				if ( buffer->getFormat() != castor::PixelFormat::eR8G8B8A8_UNORM )
				{
					buffer = castor::PxBufferBase::create( buffer->getDimensions()
						, castor::PixelFormat::eR8G8B8A8_UNORM
						, buffer->getConstPtr()
						, buffer->getFormat() );
				}

				m_image = std::make_unique< Image >( name, *buffer );
			}
			catch ( std::exception & exc )
			{
				std::cerr << exc.what() << std::endl;
			}
		}
	}

	//************************************************************************************************
}
