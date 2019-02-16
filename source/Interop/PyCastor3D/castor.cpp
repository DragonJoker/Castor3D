#include "PyCastor3D/PyCastor3DPrerequisites.hpp"

using namespace castor;
using namespace castor3d;

BOOST_PYTHON_MODULE( castor )
{
	py::object l_package = py::scope();
	l_package.attr( "__path__" ) = "castor";

	ExportCastorUtils();
	ExportCastor3D();
}

