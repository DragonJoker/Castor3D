/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CS_PRECOMPILED_HEADER_H___
#define ___CS_PRECOMPILED_HEADER_H___

#include <config.hpp>

#if CASTOR_USE_PCH
#	ifdef _WIN32
#		undef __WXGTK__
#	   define __WXMSW__
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
#	include <wx/cmdline.h>
#	include <wx/colordlg.h>
#	include <wx/combobox.h>
#	include <wx/dcclient.h>
#	include <wx/dialog.h>
#	include <wx/display.h>
#	include <wx/file.h>
#	include <wx/filedlg.h>
#	include <wx/frame.h>
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
#	include <wx/scrolwin.h>
#	include <wx/sizer.h>
#	include <wx/slider.h>
#	include <wx/statbox.h>
#	include <wx/stattext.h>
#	include <wx/statusbr.h>
#	include <wx/stdpaths.h>
#	include <wx/textctrl.h>
#	include <wx/timer.h>
#	include <wx/toolbar.h>
#	include <wx/treectrl.h>

#	include <CastorUtils.hpp>
#	include <Data/File.hpp>
#	include <Data/Path.hpp>
#	include <Design/Named.hpp>
#	include <Design/Templates.hpp>
#	include <Graphics/Colour.hpp>
#	include <Log/Logger.hpp>
#	include <Math/Angle.hpp>
#	include <Math/Math.hpp>
#	include <Math/Point.hpp>
#	include <Math/Quaternion.hpp>
#	include <Miscellaneous/StringUtils.hpp>
#	include <Miscellaneous/PreciseTimer.hpp>

#	include <Castor3DPrerequisites.hpp>
#	include <Engine.hpp>
#	include <ShaderCache.hpp>
#	include <Animation/Animation.hpp>
#	include <Animation/KeyFrame.hpp>
#	include <Scene/Animation/AnimatedObjectGroup.hpp>
#	include <Event/Frame/FrameListener.hpp>
#	include <Material/Material.hpp>
#	include <Material/Pass.hpp>
#	include <Mesh/Mesh.hpp>
#	include <Mesh/Subdivider.hpp>
#	include <Mesh/Submesh.hpp>
#	include <Mesh/Vertex.hpp>
#	include <Miscellaneous/Ray.hpp>
#	include <Overlay/Overlay.hpp>
#	include <Plugin/Plugin.hpp>
#	include <Render/RenderSystem.hpp>
#	include <Render/RenderTarget.hpp>
#	include <Render/RenderWindow.hpp>
#	include <Scene/Camera.hpp>
#	include <Scene/Geometry.hpp>
#	include <Scene/Scene.hpp>
#	include <Scene/SceneNode.hpp>
#	include <Scene/SceneFileParser.hpp>
#	include <Scene/Light/DirectionalLight.hpp>
#	include <Scene/Light/Light.hpp>
#	include <Scene/Light/PointLight.hpp>
#	include <Scene/Light/SpotLight.hpp>
#	include <Shader/FrameVariable.hpp>
#	include <Shader/ShaderProgram.hpp>
#	include <Texture/TextureUnit.hpp>

#	include <iostream>
#	include <string>
#	include <vector>
#	include <cmath>

#	include <ImagesLoader.hpp>
#	include <RendererSelector.hpp>
#	include <ShaderDialog.hpp>
#	include <SplashScreen.hpp>

#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#endif

#endif
