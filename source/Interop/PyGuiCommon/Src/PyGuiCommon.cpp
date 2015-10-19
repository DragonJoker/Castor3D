/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#include "PyGuiCommonPrerequisites.hpp"

using namespace Castor;
using namespace Castor3D;
using namespace GuiCommon;

BOOST_PYTHON_MODULE( castor_wx )
{
	py::object l_castor( py::handle<>( py::borrowed( PyImport_AddModule( "castor" ) ) ) );
	py::object l_wx( py::handle<>( py::borrowed( PyImport_AddModule( "wx" ) ) ) );
	
	py::def( "makeWindowHandle", cpy::WindowHandleCreatorFromWindow() );
	py::def( "makeWindowHandle", cpy::WindowHandleCreatorFromFrame() );
}
