#include "PyCastor3DPrerequisites.hpp"

using namespace Castor;
using namespace Castor3D;

BOOST_PYTHON_MODULE( castor )
{
	py::object l_package = py::scope();
	l_package.attr( "__path__" ) = "castor";

	ExportCastorUtils();
	ExportCastor3D();
}

