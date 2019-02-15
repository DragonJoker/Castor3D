/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PCH_H___
#define ___GUICOMMON_PCH_H___

#include <config.hpp>
#include <Castor3DPrerequisites.hpp>

#if CU_UsePCH
#	ifdef CU_PlatformWindows
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
#	include <Miscellaneous/StringUtils.hpp>
#	include <Design/Templates.hpp>
#	include <FileParser/FileParser.hpp>
#	include <Log/Logger.hpp>
#	include <Miscellaneous/PreciseTimer.hpp>
#	include <Data/File.hpp>
#	include <Math/Point.hpp>
#	include <Math/Quaternion.hpp>
#	include <Design/Named.hpp>
#	include <Data/Path.hpp>
#	include <Math/Math.hpp>
#	include <Graphics/Colour.hpp>
#	include <Math/Angle.hpp>

#	include <Engine.hpp>
#	include <Cache/ShaderCache.hpp>
#	include <Animation/Animation.hpp>
#	include <Animation/AnimationKeyFrame.hpp>
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
