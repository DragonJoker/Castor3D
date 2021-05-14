#include "GuiCommon/Properties/Math/PointProperties.hpp"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

using namespace castor;

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4b )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4i )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4ui )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4f )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point2d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point3d )
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( Point4d )

namespace GuiCommon
{
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2bProperty, wxPGProperty, Point2b, Point2b const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3bProperty, wxPGProperty, Point3b, Point3b const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4bProperty, wxPGProperty, Point4b, Point4b const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2iProperty, wxPGProperty, Point2i, Point2i const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3iProperty, wxPGProperty, Point3i, Point3i const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4iProperty, wxPGProperty, Point4i, Point4i const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2uiProperty, wxPGProperty, Point2ui, Point2ui const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3uiProperty, wxPGProperty, Point3ui, Point3ui const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4uiProperty, wxPGProperty, Point4ui, Point4ui const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2fProperty, wxPGProperty, Point2f, Point2f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3fProperty, wxPGProperty, Point3f, Point3f const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4fProperty, wxPGProperty, Point4f, Point4f const &, TextCtrl )

	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point2dProperty, wxPGProperty, Point2d, Point2d const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point3dProperty, wxPGProperty, Point3d, Point3d const &, TextCtrl )
	GC_PG_IMPLEMENT_PROPERTY_CLASS( Point4dProperty, wxPGProperty, Point4d, Point4d const &, TextCtrl )
}
