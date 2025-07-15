/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_QUATERNION_PROPERTIES_H___
#define ___GUICOMMON_QUATERNION_PROPERTIES_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Math/Quaternion.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Quaternion )

namespace GuiCommon
{
	class QuaternionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( QuaternionProperty )
	public:

		QuaternionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, c3d::Quaternion const & value = c3d::Quaternion() );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( c3d::Quaternion const & value )
		{
			m_value = WXVARIANT( value );
		}

	private:
		c3d::Point3f m_axis;
		c3d::Angle m_angle;
	};
}

#endif
