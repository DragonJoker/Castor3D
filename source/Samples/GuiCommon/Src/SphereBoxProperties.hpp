/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SphereBoxProperties_H___
#define ___GUICOMMON_SphereBoxProperties_H___

#include "AdditionalProperties.hpp"

#include <Graphics/SphereBox.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, SphereBox );

namespace GuiCommon
{
	class SphereBoxProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( SphereBoxProperty )

	public:
		SphereBoxProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::SphereBox const & value = castor::SphereBox{} );
		virtual ~SphereBoxProperty();

		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::SphereBox const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
