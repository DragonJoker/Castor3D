/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SIZE_PROPERTIES_H___
#define ___GUICOMMON_SIZE_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Graphics/Size.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Size );

namespace GuiCommon
{
	class SizeProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( SizeProperty )

	public:
		SizeProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Size const & value = castor::Size() );
		virtual ~SizeProperty();

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::Size const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
