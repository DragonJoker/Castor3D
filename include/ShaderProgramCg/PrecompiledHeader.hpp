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
#ifndef ___Cg_PrecompiledHeader___
#	define ___Cg_PrecompiledHeader___

#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )

//*************************************************************************************************

#	include <fstream>
#	include <set>
#	include <string>

//*************************************************************************************************

#	include <Cg/cg.h>
#	if defined( Cg_Direct3D11 )
#		if defined( _MSC_VER )
#			pragma warning( push )
#			pragma warning( disable:4311 )
#			pragma warning( disable:4312 )
#		endif
#		include <Dx11RenderSystem/Dx11.hpp>
#		include <Cg/cgD3D11.h>
#		if defined( _MSC_VER )
#			pragma warning( pop )
#		endif
#	endif
#	if defined( Cg_Direct3D10 )
#		if defined( _MSC_VER )
#			pragma warning( push )
#			pragma warning( disable:4311 )
#			pragma warning( disable:4312 )
#		endif
#		include <Cg/cgD3D10.h>
#		if defined( _MSC_VER )
#			pragma warning( pop )
#		endif
#	endif
#	if defined( Cg_Direct3D9 )
#		if defined( _MSC_VER )
#			pragma warning( push )
#			pragma warning( disable:4311 )
#			pragma warning( disable:4312 )
#		endif
#		include <Cg/cgD3D9.h>
#		if defined( _MSC_VER )
#			pragma warning( pop )
#		endif
#	endif
#	if defined( Cg_OpenGL )
#		if defined( _WIN32 )
#			if defined( _MSC_VER )
#				pragma warning( push )
#				pragma warning( disable:4311 )
#				pragma warning( disable:4312 )
#			endif
#			include <Windows.h>
#			if defined( _MSC_VER )
#				pragma warning( pop )
#			endif
#		endif
#		include <GL/gl.h>
#		include <Cg/cgGL.h>
#	endif

//*************************************************************************************************

#	include <CastorUtils/PreciseTimer.hpp>

//*************************************************************************************************

#	include <Castor3D/Prerequisites.hpp>
#	include <Castor3D/ShaderProgram.hpp>
#	include <Castor3D/ShaderManager.hpp>
#	include <Castor3D/ShaderObject.hpp>
#	include <Castor3D/FrameVariable.hpp>
#	include <Castor3D/TextureUnit.hpp>
#	include <Castor3D/Pass.hpp>
#	include <Castor3D/Material.hpp>
#	include <Castor3D/Context.hpp>
#	include <Castor3D/Pipeline.hpp>
#	include <Castor3D/Version.hpp>
#	include <Castor3D/Buffer.hpp>

//*************************************************************************************************

using Castor3D::real;

#	pragma warning( pop )

#endif
