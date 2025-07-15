/*
See LICENSE file in root folder
*/
#ifndef ___GC_ColourProperties_H___
#define ___GC_ColourProperties_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/HdrColourComponent.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( c3d, HdrRgbColour )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, HdrRgbaColour )

namespace GuiCommon
{
	class HdrRgbColourProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( HdrRgbColourProperty )

	public:
		explicit HdrRgbColourProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, c3d::HdrRgbColour const & value = c3d::HdrRgbColour() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( c3d::HdrRgbColour const & value );
	};

	class HdrRgbaColourProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( HdrRgbaColourProperty )

	public:
		explicit HdrRgbaColourProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, c3d::HdrRgbaColour const & value = c3d::HdrRgbaColour() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( c3d::HdrRgbaColour const & value );
	};
}

#endif
