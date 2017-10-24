/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_CubeBoxProperties_H___
#define ___GUICOMMON_CubeBoxProperties_H___

#include "AdditionalProperties.hpp"

#include <Graphics/CubeBox.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, CubeBox );

namespace GuiCommon
{
	class CubeBoxProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( CubeBoxProperty )

	public:
		CubeBoxProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::CubeBox const & value = castor::CubeBox{} );
		virtual ~CubeBoxProperty();

		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::CubeBox const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
