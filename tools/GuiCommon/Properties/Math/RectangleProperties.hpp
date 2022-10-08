/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RECTANGLE_PROPERTIES_H___
#define ___GUICOMMON_RECTANGLE_PROPERTIES_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Rectangle )
GC_PG_NS_DECLARE_VARIANT_DATA( castor, HdrRgbColour )
GC_PG_NS_DECLARE_VARIANT_DATA( castor, HdrRgbaColour )

namespace GuiCommon
{
	class RectangleProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( RectangleProperty )

	public:
		RectangleProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Rectangle const & value = castor::Rectangle() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( castor::Rectangle const & value );
	};

	class HdrRgbColourProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( HdrRgbColourProperty )

	public:
		HdrRgbColourProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::HdrRgbColour const & value = castor::HdrRgbColour() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( castor::HdrRgbColour const & value );
	};

	class HdrRgbaColourProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( HdrRgbaColourProperty )

	public:
		HdrRgbaColourProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::HdrRgbaColour const & value = castor::HdrRgbaColour() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( castor::HdrRgbaColour const & value );
	};
}

#endif
