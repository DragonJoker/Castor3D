#ifndef ___GC_PrecompiledHeader___
#define ___GC_PrecompiledHeader___

#ifdef _WIN32
#   if CASTOR_GTK
#       ifdef __WXGTK__
#           undef __WXMSW__
#       else
#           define __WXMSW__
#       endif
#   else
#       undef __WXGTK__
#       define __WXMSW__
#   endif
#endif

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4311 )
#	pragma warning( disable:4312 )
#	pragma warning( disable:4996 )
#endif

#include <string>

#include <wx/app.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/checklst.h>
#include <wx/colordlg.h>
#include <wx/combobox.h>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dialog.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/frame.h>
#include <wx/gauge.h>
#include <wx/grid.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobox.h>
#include <wx/rawbmp.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/treectrl.h>
#include <wx/aui/framemanager.h>

#include <CastorUtils/CastorUtils.hpp>
#include <CastorUtils/String.hpp>
#include <CastorUtils/Templates.hpp>
#include <CastorUtils/Vector.hpp>
#include <CastorUtils/FileParser.hpp>
#include <CastorUtils/List.hpp>
#include <CastorUtils/Logger.hpp>
#include <CastorUtils/Map.hpp>
#include <CastorUtils/Multimap.hpp>
#include <CastorUtils/Set.hpp>
#include <CastorUtils/PreciseTimer.hpp>
#include <CastorUtils/File.hpp>
#include <CastorUtils/Point.hpp>
#include <CastorUtils/Quaternion.hpp>
#include <CastorUtils/Named.hpp>
#include <CastorUtils/Path.hpp>
#include <CastorUtils/Math.hpp>
#include <CastorUtils/Colour.hpp>
#include <CastorUtils/Angle.hpp>

#include <Castor3D/Prerequisites.hpp>
#include <Castor3D/RenderWindow.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Plugin.hpp>
#include <Castor3D/RenderSystem.hpp>
#include <Castor3D/Ray.hpp>
#include <Castor3D/FrameListener.hpp>
#include <Castor3D/Material.hpp>
#include <Castor3D/Pass.hpp>
#include <Castor3D/TextureUnit.hpp>
#include <Castor3D/Texture.hpp>
#include <Castor3D/Mesh.hpp>
#include <Castor3D/Submesh.hpp>
#include <Castor3D/Geometry.hpp>
#include <Castor3D/Scene.hpp>
#include <Castor3D/SceneNode.hpp>
#include <Castor3D/Light.hpp>
#include <Castor3D/PointLight.hpp>
#include <Castor3D/DirectionalLight.hpp>
#include <Castor3D/SpotLight.hpp>
#include <Castor3D/Animation.hpp>
#include <Castor3D/KeyFrame.hpp>
#include <Castor3D/AnimatedObjectGroup.hpp>
#include <Castor3D/Camera.hpp>
#include <Castor3D/ShaderManager.hpp>
#include <Castor3D/ShaderProgram.hpp>
#include <Castor3D/ShaderObject.hpp>
#include <Castor3D/FrameVariable.hpp>
#include <Castor3D/Overlay.hpp>
#include <Castor3D/SceneFileParser.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#if defined( _MSC_VER )
#	pragma warning( pop )
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

#if wxVERSION_NUMBER >= 2900
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_BICUBIC
#else
#	define	wxIMAGE_QUALITY_HIGHEST wxIMAGE_QUALITY_HIGH
#endif

#define wxCOMBO_NEW	_( "New..." )

#endif
