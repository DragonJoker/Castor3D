/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_CubeBoxProperties_H___
#define ___GUICOMMON_CubeBoxProperties_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, BoundingBox )

namespace GuiCommon
{
	class BoundingBoxProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( BoundingBoxProperty )

	public:
		BoundingBoxProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::BoundingBox const & value = castor::BoundingBox{} );

		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::BoundingBox const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
