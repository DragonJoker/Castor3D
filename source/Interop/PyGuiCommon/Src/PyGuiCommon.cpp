#include "PyGuiCommonPrerequisites.hpp"

using namespace castor;
using namespace castor3d;
using namespace GuiCommon;

BOOST_PYTHON_MODULE( castor_wx )
{
	py::object l_castor( py::handle<>( py::borrowed( PyImport_AddModule( "castor" ) ) ) );
	py::object l_wx( py::handle<>( py::borrowed( PyImport_AddModule( "wx" ) ) ) );

	py::def( "makeWindowHandle", cpy::WindowHandleCreatorFromWindow() );
	py::def( "makeWindowHandle", cpy::WindowHandleCreatorFromFrame() );
}
