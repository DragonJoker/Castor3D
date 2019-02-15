#include "GuiCommon/MatrixProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix2x2f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix3x3f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Matrix4x4f )

namespace GuiCommon
{
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Matrix2fProperty, wxPGProperty, Matrix2x2f, Matrix2x2f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Matrix3fProperty, wxPGProperty, Matrix3x3f, Matrix3x3f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Matrix4fProperty, wxPGProperty, Matrix4x4f, Matrix4x4f const &, TextCtrl )
}
