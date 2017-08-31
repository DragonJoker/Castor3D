/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_QUATERNION_PROPERTIES_H___
#define ___GUICOMMON_QUATERNION_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Math/Quaternion.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Quaternion );

namespace GuiCommon
{
	class QuaternionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( QuaternionProperty )
	public:

		QuaternionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Quaternion const & value = castor::Quaternion() );
		virtual ~QuaternionProperty();

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::Quaternion const & value )
		{
			m_value = WXVARIANT( value );
		}

	private:
		castor::Point3r m_axis;
		castor::Angle m_angle;
	};
}

#endif
