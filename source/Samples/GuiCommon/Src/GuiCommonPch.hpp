/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#	include <Engine.hpp>
#	include <ShaderManager.hpp>
#	include <Animation/Animation.hpp>
#	include <Animation/KeyFrame.hpp>
#	include <Event/Frame/FrameListener.hpp>
#	include <Material/Material.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/Mesh.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Miscellaneous/Ray.hpp>
#	include <Overlay/Overlay.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Render/RenderWindow.hpp>
#	include <Render/RenderSystem.hpp>
#	include <Scene/Camera.hpp>
#	include <Scene/Geometry.hpp>
#	include <Scene/Scene.hpp>
#	include <Scene/SceneFileParser.hpp>
#	include <Scene/SceneNode.hpp>
#	include <Scene/Animation/AnimatedObjectGroup.hpp>
#	include <Scene/Light/Light.hpp>
#	include <Scene/Light/PointLight.hpp>
#	include <Scene/Light/DirectionalLight.hpp>
#	include <Scene/Light/SpotLight.hpp>
#	include <Shader/ShaderProgram.hpp>
#	include <Shader/FrameVariable.hpp>
#	include <Texture/TextureUnit.hpp>
#	include <Texture/TextureLayout.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>
#else
#	include <Castor3DPrerequisites.hpp>
#endif

#endif
