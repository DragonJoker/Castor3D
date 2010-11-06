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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Module_Render___
#define ___C3D_Module_Render___

#include "../Module_General.h"

namespace Castor3D
{
	class RenderSystem;

	template <class T, class Y> class Renderer;
	template <class T, class Y> class Renderable;
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
	class TextureBuffer;
	template <typename T> class VertexAttribsBuffer;

	typedef VertexAttribsBuffer<real> VertexAttribsBufferReal;

	typedef Templates::SharedPtr<SubmeshRenderer>				SubmeshRendererPtr;
	typedef Templates::SharedPtr<TextureEnvironmentRenderer>	TextureEnvironmentRendererPtr;
	typedef Templates::SharedPtr<TextureRenderer>				TextureRendererPtr;
	typedef Templates::SharedPtr<PassRenderer>					PassRendererPtr;
	typedef Templates::SharedPtr<LightRenderer>					LightRendererPtr;
	typedef Templates::SharedPtr<WindowRenderer>				WindowRendererPtr;
	typedef Templates::SharedPtr<CameraRenderer>				CameraRendererPtr;
	typedef Templates::SharedPtr<ViewportRenderer>				ViewportRendererPtr;
	typedef Templates::SharedPtr<SceneNodeRenderer>				SceneNodeRendererPtr;
	typedef Templates::SharedPtr<OverlayRenderer>				OverlayRendererPtr;
	typedef Templates::SharedPtr<VertexBuffer>					VertexBufferPtr;
	typedef Templates::SharedPtr<NormalsBuffer>					NormalsBufferPtr;
	typedef Templates::SharedPtr<TextureBuffer>					TextureBufferPtr;
	typedef Templates::SharedPtr<VertexAttribsBufferReal>		VertexAttribsBufferRealPtr;

	typedef C3DVector( SubmeshRendererPtr)				SubmeshRendererPtrArray;
	typedef C3DVector( TextureEnvironmentRendererPtr)	TextureEnvironmentRendererPtrArray;
	typedef C3DVector( TextureRendererPtr)				TextureRendererPtrArray;
	typedef C3DVector( PassRendererPtr)					PassRendererPtrArray;
	typedef C3DVector( LightRendererPtr)				LightRendererPtrArray;
	typedef C3DVector( WindowRendererPtr)				WindowRendererPtrArray;
	typedef C3DVector( CameraRendererPtr)				CameraRendererPtrArray;
	typedef C3DVector( ViewportRendererPtr)				ViewportRendererPtrArray;
	typedef C3DVector( SceneNodeRendererPtr)			SceneNodeRendererPtrArray;
	typedef C3DVector( OverlayRendererPtr)				OverlayRendererPtrArray;
	/*!
	Enumerates the different draw types : Triangles, Lines, Points...
	*/
	typedef enum eDRAW_TYPE
	{
		DTTriangles		= 0,	//!< Triangles : Show textured triangles
		DTLines			= 1,	//!< Lines : Show textured edges
		DTPoints		= 2,	//!< Points : Show textured vertexes
		DTTriangleStrip	= 3,	//!< TriangleStrip : Show textured triangle strips
		DTTriangleFan	= 4,	//!< TriangleFan : Show textured triangle fans
		DTQuads			= 5,	//!< Quads : Show textured quads
		DTQuadStrip		= 6,	//!< QuadStrip : Show textured quad strips
	} eDRAW_TYPE;
}

#endif