/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RECTANGLE_PROPERTIES_H___
#define ___GUICOMMON_RECTANGLE_PROPERTIES_H___

#include "GuiCommon/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>

namespace GuiCommon
{
	class RectangleProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( RectangleProperty )

	public:
		RectangleProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Rectangle const & value = castor::Rectangle() );
		virtual ~RectangleProperty();

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		void setValueI( castor::Rectangle const & value );
	};
}

#endif
