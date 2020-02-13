/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_POSITION_PROPERTIES_H___
#define ___GUICOMMON_POSITION_PROPERTIES_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/Position.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Position );

namespace GuiCommon
{
	class PositionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( PositionProperty )

	public:
		PositionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Position const & value = castor::Position() );
		virtual ~PositionProperty();

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::Position const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
