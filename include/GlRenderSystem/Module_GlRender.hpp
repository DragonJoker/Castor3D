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
#ifndef ___Gl_ModuleRender___
#define ___Gl_ModuleRender___

//*************************************************************************************************

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <Windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#elif defined( __linux__ )
#	include <GL/glx.h>
#endif
#include <GL/gl.h>

//*************************************************************************************************

#include <CastorUtils/CastorUtils.hpp>

using Castor::real;

//*************************************************************************************************

#include <Castor3D/Prerequisites.hpp>

//*************************************************************************************************

#ifdef _WIN32
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec( dllexport )
#	else
#		define C3D_Gl_API __declspec( dllimport )
#	endif
#else
#	define C3D_Gl_API
#endif

namespace GlRender
{
	class GlRenderSystem;
	class GlPlugin;
	class GlTexture;
	class GlTextureRenderer;
	class GlPassRenderer;
	class GlLightRenderer;
	class GlCameraRenderer;
	class GlWindowRenderer;
	class GlSubmeshRenderer;
	class GlOverlayRenderer;
	class GlShaderObject;
	class GlShaderProgram;
	class GlFrameVariableBase;
	template< typename T > class GlOneFrameVariable;
	template <typename T, uint32_t Count> class GlPointFrameVariable;
	template <typename T, uint32_t Rows, uint32_t Columns> class GlMatrixFrameVariable;
	class GlPipeline;
	class GlContext;
	class GlStaticTexture;
	class GlDynamicTexture;

	template< typename T > class GlBufferBase;
	class GlAttributeBase;
	template< typename T > class GlBuffer;
	template <typename T, uint32_t Count> class GlAttribute;
	template <typename T, uint32_t Count> class GlAttributeBuffer;

	typedef GlAttribute<		real,	4>	GlAttribute4r;
	typedef GlAttribute<		real,	3>	GlAttribute3r;
	typedef GlAttribute<		real,	2>	GlAttribute2r;
	typedef GlAttribute<		int,	4>	GlAttribute4i;
	typedef GlAttribute<		int,	3>	GlAttribute3i;
	typedef GlAttribute<		int,	2>	GlAttribute2i;
	typedef GlAttributeBuffer<	real,	3>	GlAttributeBuffer3r;
	typedef GlAttributeBuffer<	real,	2>	GlAttributeBuffer2r;
	typedef GlAttributeBuffer<	int,	3>	GlAttributeBuffer3i;
	typedef GlAttributeBuffer<	int,	2>	GlAttributeBuffer2i;

	DECLARE_SMART_PTR(	GlContext				);
	DECLARE_SMART_PTR(	GlTexture				);
	DECLARE_SMART_PTR(	GlAttributeBase			);
	DECLARE_SMART_PTR(	GlAttribute3r			);
	DECLARE_SMART_PTR(	GlAttribute2r			);
	DECLARE_SMART_PTR(	GlAttribute3i			);
	DECLARE_SMART_PTR(	GlAttribute2i			);
	DECLARE_SMART_PTR(	GlAttributeBuffer3r		);
	DECLARE_SMART_PTR(	GlAttributeBuffer2r		);
	DECLARE_SMART_PTR(	GlAttributeBuffer3i		);
	DECLARE_SMART_PTR(	GlAttributeBuffer2i		);
	DECLARE_SMART_PTR(	GlShaderObject			);
	DECLARE_SMART_PTR(	GlShaderProgram			);
	DECLARE_SMART_PTR(	GlFrameVariableBase		);
	DECLARE_SMART_PTR(	GlDynamicTexture		);
	DECLARE_SMART_PTR(	GlStaticTexture			);

	DECLARE_VECTOR(	GlShaderProgramSPtr,							GlShaderProgramPtr		);
	DECLARE_VECTOR(	GlShaderObjectSPtr,								GlShaderObjectPtr		);
	DECLARE_VECTOR(	GlAttributeBaseSPtr,							GlAttributePtr			);
	DECLARE_MAP(	Castor::String,		GlFrameVariableBaseSPtr,	GlFrameVariablePtrStr	);

	// OpenGL 2.x
	class GlIndexArray;
	class GlVertexArray;
	class GlIndexBufferObject;
	class GlVertexBufferObject;
	class GlPixelBuffer;
	class GlPackPixelBuffer;
	class GlUnpackPixelBuffer;

	DECLARE_SMART_PTR(	GlIndexArray			);
	DECLARE_SMART_PTR(	GlVertexArray			);
	DECLARE_SMART_PTR(	GlIndexBufferObject		);
	DECLARE_SMART_PTR(	GlVertexBufferObject	);
	DECLARE_SMART_PTR(	GlPixelBuffer			);
	DECLARE_SMART_PTR(	GlPackPixelBuffer		);
	DECLARE_SMART_PTR(	GlUnpackPixelBuffer		);

	// OpenGL 3.x
	class Gl3VertexBufferObject;
	class GlTextureBufferObject;
	class GlFrameBuffer;
	class GlRenderBuffer;
	class GlColourRenderBuffer;
	class GlDepthRenderBuffer;
	class GlTextureAttachment;
	class GlRenderBufferAttachment;

	DECLARE_SMART_PTR(	GlTextureBufferObject		);
	DECLARE_SMART_PTR(	GlFrameBuffer				);
	DECLARE_SMART_PTR(	GlRenderBuffer				);
	DECLARE_SMART_PTR(	GlColourRenderBuffer		);
	DECLARE_SMART_PTR(	GlDepthRenderBuffer			);
	DECLARE_SMART_PTR(	GlTextureAttachment			);
	DECLARE_SMART_PTR(	GlRenderBufferAttachment	);

	DECLARE_VECTOR(	GlTextureBufferObjectSPtr,							GlTextureBufferObjectPtr	);
	DECLARE_VECTOR(	GlDepthRenderBufferSPtr,							GlDepthRenderBufferPtr		);
	DECLARE_VECTOR(	GlTextureAttachmentSPtr,							GlTextureAttachmentPtr		);
	DECLARE_VECTOR(	GlRenderBufferAttachmentSPtr,						GlRenderBufferAttachmentPtr	);
}

#include "OpenGl.hpp"
#endif
