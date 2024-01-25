#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include "GuiCommon/System/GradientButton.hpp"

#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/propgrid/advprops.h>

#include <iostream>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	//************************************************************************************************

	uint32_t toBGRPacked( castor3d::ColourWrapper const & colour )
	{
		uint32_t r = uint32_t( ( *colour.value )->x * 255.0f );
		uint32_t g = uint32_t( ( *colour.value )->y * 255.0f );
		uint32_t b = uint32_t( ( *colour.value )->z * 255.0f );
		return ( r << 0 ) | ( g << 8 ) | ( b << 16 );
	}

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

	wxPGWindowList ButtonEventEditor::CreateControls( wxPropertyGrid * propgrid, wxPGProperty * property, wxPoint const & pos, wxSize const & size )const
	{
		// create and return a single button to be used as editor
		// size and pos represent the entire value cell: use that to position the button
		return wxPGWindowList( new GradientButton( propgrid, wxID_ANY, _( "View" ), pos, size ) );
	}

	// since the editor does not need to change the primary control (the button)
	// to reflect changes, UpdateControl is just a no-op
	void ButtonEventEditor::UpdateControl( wxPGProperty * property, wxWindow * ctrl ) const
	{
	}

	bool ButtonEventEditor::OnEvent( wxPropertyGrid * propgrid, wxPGProperty * property, wxWindow * wnd_primary, wxEvent & event )const
	{
		// handle the button event
		if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
		{
			// extract the client data from the property
			if ( ButtonData * btn = dynamic_cast< ButtonData * >( property->GetClientObject() ) )
			{
				// call the method
				btn->Call( property->GetValue() );
				return false;
			}
		}

		return false;
	}
	//************************************************************************************************

	wxFloatProperty * CreateProperty( wxString const & name, double const & value )
	{
		return new wxFloatProperty( name, wxPG_LABEL, value );
	}

	wxFloatProperty * CreateProperty( wxString const & name, float const & value )
	{
		return new wxFloatProperty( name, wxPG_LABEL, value );
	}

	wxIntProperty * CreateProperty( wxString const & name, int const & value )
	{
		return new wxIntProperty( name, wxPG_LABEL, value );
	}

	wxUIntProperty * CreateProperty( wxString const & name, uint32_t const & value )
	{
		return new wxUIntProperty( name, wxPG_LABEL, value );
	}

	wxBoolProperty * CreateProperty( wxString const & name, bool const & value, bool checkbox )
	{
		wxBoolProperty * result = new wxBoolProperty( name, wxPG_LABEL );
		result->SetAttribute( wxT( "UseCheckbox" ), checkbox );
		return result;
	}

	wxStringProperty * CreateProperty( wxString const & name, wxString const & value )
	{
		return new wxStringProperty( name, wxPG_LABEL, value );
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
				m_bitmap = castor::make_unique< wxBitmap >();
				createBitmapFromBuffer( *m_image->getPixels()
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
					, {} ).getPixels()->clone();

				if ( castor::isCompressed( buffer->getFormat() ) )
				{
					buffer = castor::decompressBuffer( *buffer );
				}

				if ( buffer->getFormat() != castor::PixelFormat::eR8G8B8A8_UNORM )
				{
					buffer = castor::PxBufferBase::create( buffer->getDimensions()
						, castor::PixelFormat::eR8G8B8A8_UNORM
						, buffer->getConstPtr()
						, buffer->getFormat() );
				}

				m_image = castor::make_unique< castor::Image >( name, *buffer );
			}
			catch ( std::exception & exc )
			{
				std::cerr << exc.what() << std::endl;
			}
		}
	}

	//************************************************************************************************
}
