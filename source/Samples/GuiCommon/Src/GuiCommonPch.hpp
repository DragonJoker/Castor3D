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
#ifndef ___GUICOMMON_PCH_H___
#define ___GUICOMMON_PCH_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	ifdef _WIN32
#		if CASTOR_GTK
#			ifdef __WXGTK__
#				undef __WXMSW__
#			else
#				define __WXMSW__
#			endif
#		else
#			undef __WXGTK__
#			define __WXMSW__
#		endif
#	endif

#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#		pragma warning( disable:4996 )
#	endif

#	include <string>

#	include <wx/app.h>
#	include <wx/button.h>
#	include <wx/bmpbuttn.h>
#	include <wx/checkbox.h>
#	include <wx/checklst.h>
#	include <wx/colordlg.h>
#	include <wx/combobox.h>
#	include <wx/dcbuffer.h>
#	include <wx/dcclient.h>
#	include <wx/dialog.h>
#	include <wx/file.h>
#	include <wx/filename.h>
#	include <wx/filedlg.h>
#	include <wx/frame.h>
#	include <wx/gauge.h>
#	include <wx/grid.h>
#	include <wx/icon.h>
#	include <wx/image.h>
#	include <wx/imaglist.h>
#	include <wx/listctrl.h>
#	include <wx/menu.h>
#	include <wx/msgdlg.h>
#	include <wx/notebook.h>
#	include <wx/panel.h>
#	include <wx/radiobox.h>
#	include <wx/rawbmp.h>
#	include <wx/scrolwin.h>
#	include <wx/sizer.h>
#	include <wx/slider.h>
#	include <wx/statbox.h>
#	include <wx/stattext.h>
#	include <wx/statusbr.h>
#	include <wx/stc/stc.h>
#	include <wx/textctrl.h>
#	include <wx/timer.h>
#	include <wx/treectrl.h>
#	include <wx/aui/framemanager.h>

#	include <CastorUtils.hpp>
#	include <StringUtils.hpp>
#	include <Templates.hpp>
#	include <FileParser.hpp>
#	include <Logger.hpp>
#	include <PreciseTimer.hpp>
#	include <File.hpp>
#	include <Point.hpp>
#	include <Quaternion.hpp>
#	include <Named.hpp>
#	include <Path.hpp>
#	include <Math.hpp>
#	include <Colour.hpp>
#	include <Angle.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <RenderWindow.hpp>
#	include <Engine.hpp>
#	include <Plugin.hpp>
#	include <RenderSystem.hpp>
#	include <Ray.hpp>
#	include <FrameListener.hpp>
#	include <Material.hpp>
#	include <Pass.hpp>
#	include <TextureUnit.hpp>
#	include <Texture.hpp>
#	include <Mesh.hpp>
#	include <Submesh.hpp>
#	include <Geometry.hpp>
#	include <Scene.hpp>
#	include <SceneNode.hpp>
#	include <Light.hpp>
#	include <PointLight.hpp>
#	include <DirectionalLight.hpp>
#	include <SpotLight.hpp>
#	include <Animation.hpp>
#	include <KeyFrame.hpp>
#	include <AnimatedObjectGroup.hpp>
#	include <Camera.hpp>
#	include <ShaderManager.hpp>
#	include <ShaderProgram.hpp>
#	include <ShaderObject.hpp>
#	include <FrameVariable.hpp>
#	include <Overlay.hpp>
#	include <SceneFileParser.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>

#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#else
#	include <Castor3DPrerequisites.hpp>
#endif

#ifndef _UNICODE
#	if CASTOR_UNICODE
#		error "GuiCommon compiling in ASCII mode while Castor3D is in UNICODE mode"
#	endif
#else
#	if !CASTOR_UNICODE
#		error "GuiCommon compiling in UNICODE mode while Castor3D is in ASCII mode"
#	endif
#endif

#endif
