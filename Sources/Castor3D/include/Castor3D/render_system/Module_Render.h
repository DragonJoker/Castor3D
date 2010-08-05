/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Module_Render___
#define ___C3D_Module_Render___

#include "../Module_General.h"

namespace Castor3D
{
	class RenderSystem;

	template <class T> class Renderer;
	class SubmeshRenderer;
	class TextureEnvironmentRenderer;
	class TextureRenderer;
	class PassRenderer;
	class LightRenderer;
	class WindowRenderer;
	class CameraRenderer;
	class ViewportRenderer;
	class SceneNodeRenderer;
	class OverlayRenderer;

	class VertexBuffer;
	class NormalsBuffer;
	class VertexAttribsBuffer;
	class TextureBuffer;

	typedef std::vector < SubmeshRenderer * >				SubmeshRendererPtrArray;
	typedef std::vector < TextureEnvironmentRenderer * >	TextureEnvironmentRendererPtrArray;
	typedef std::vector < TextureRenderer * >				TextureRendererPtrArray;
	typedef std::vector < PassRenderer * >					PassRendererPtrArray;
	typedef std::vector < LightRenderer * >					LightRendererPtrArray;
	typedef std::vector < WindowRenderer * >				WindowRendererPtrArray;
	typedef std::vector < CameraRenderer * >				CameraRendererPtrArray;
	typedef std::vector < ViewportRenderer * >				ViewportRendererPtrArray;
	typedef std::vector < SceneNodeRenderer * >				SceneNodeRendererPtrArray;
	typedef std::vector < OverlayRenderer * >				OverlayRendererPtrArray;
}

#endif