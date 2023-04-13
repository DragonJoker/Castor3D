/*
See LICENSE file in root folder
*/
#ifndef ___GC_ColourProperties_H___
#define ___GC_ColourProperties_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, HdrRgbColour )
GC_PG_NS_DECLARE_VARIANT_DATA( castor, HdrRgbaColour )

namespace GuiCommon
{
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
