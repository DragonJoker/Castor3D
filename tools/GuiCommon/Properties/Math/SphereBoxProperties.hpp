/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SphereBoxProperties_H___
#define ___GUICOMMON_SphereBoxProperties_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Graphics/BoundingSphere.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, BoundingSphere )

namespace GuiCommon
{
	class BoundingSphereProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( BoundingSphereProperty )

	public:
		BoundingSphereProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::BoundingSphere const & value = castor::BoundingSphere{} );

		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::BoundingSphere const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}

#endif
