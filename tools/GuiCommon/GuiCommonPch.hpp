/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PCH_H___
#define ___GUICOMMON_PCH_H___

#include <CastorUtils/config.hpp>
#include <Castor3D/Castor3DPrerequisites.hpp>

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

#	include <CastorUtils/CastorUtils.hpp>
#	include <CastorUtils/Miscellaneous/StringUtils.hpp>
#	include <CastorUtils/Design/Templates.hpp>
#	include <CastorUtils/FileParser/FileParser.hpp>
#	include <CastorUtils/Log/Logger.hpp>
#	include <CastorUtils/Miscellaneous/PreciseTimer.hpp>
#	include <CastorUtils/Data/File.hpp>
#	include <CastorUtils/Math/Point.hpp>
#	include <CastorUtils/Math/Quaternion.hpp>
#	include <CastorUtils/Design/Named.hpp>
#	include <CastorUtils/Data/Path.hpp>
#	include <CastorUtils/Math/Math.hpp>
#	include <CastorUtils/Graphics/Colour.hpp>
#	include <CastorUtils/Math/Angle.hpp>

#	include <Castor3D/Engine.hpp>
#	include <Castor3D/Cache/ShaderCache.hpp>
#	include <Castor3D/Animation/Animation.hpp>
#	include <Castor3D/Animation/AnimationKeyFrame.hpp>
#	include <Castor3D/Event/Frame/FrameListener.hpp>
#	include <Castor3D/Material/Material.hpp>
#	include <Castor3D/Material/Pass/Pass.hpp>
#	include <Castor3D/Material/Texture/TextureUnit.hpp>
#	include <Castor3D/Material/Texture/TextureLayout.hpp>
#	include <Castor3D/Model/Mesh/Mesh.hpp>
#	include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#	include <Castor3D/Overlay/Overlay.hpp>
#	include <Castor3D/Plugin/Plugin.hpp>
#	include <Castor3D/Render/Ray.hpp>
#	include <Castor3D/Render/RenderWindow.hpp>
#	include <Castor3D/Render/RenderSystem.hpp>
#	include <Castor3D/Scene/Camera.hpp>
#	include <Castor3D/Scene/Geometry.hpp>
#	include <Castor3D/Scene/Scene.hpp>
#	include <Castor3D/Scene/SceneFileParser.hpp>
#	include <Castor3D/Scene/SceneNode.hpp>
#	include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#	include <Castor3D/Scene/Light/Light.hpp>
#	include <Castor3D/Scene/Light/PointLight.hpp>
#	include <Castor3D/Scene/Light/DirectionalLight.hpp>
#	include <Castor3D/Scene/Light/SpotLight.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>
#else
#	include <Castor3D/Castor3DPrerequisites.hpp>
#endif

#endif
