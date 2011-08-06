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

#include <Castor3D/TextureUnit.hpp>

#ifdef _WIN32
#	ifdef GlRenderSystem_EXPORTS
#		define C3D_Gl_API __declspec(dllexport)
#	else
#		define C3D_Gl_API __declspec(dllimport)
#	endif
#else
#	define C3D_Gl_API
#endif

#undef CASTOR_USE_OPENGL4
#define CASTOR_USE_OPENGL4 0

namespace Castor3D
{
	class RenderWindow;
	class Geometry;

	class GlRenderSystem;
	class GlPlugin;
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
	class GlIntFrameVariable;
	class GlRealFrameVariable;
	template <typename T> class GlOneFrameVariable;
	template <typename T, size_t Count> class GlPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class GlMatrixFrameVariable;
	class CgGlShaderProgram;
	class CgGlShaderObject;
	class CgGlFrameVariableBase;
	class CgGlIntFrameVariable;
	class CgGlRealFrameVariable;
	template <typename T> class CgGlOneFrameVariable;
	template <typename T, size_t Count> class CgGlPointFrameVariable;
	template <typename T, size_t Rows, size_t Columns> class CgGlMatrixFrameVariable;
	class GlPipeline;
	class GlContext;

	class GlBufferBase;
	class GlAttributeBase;
	template <typename T> class GlBuffer;
	template <typename T, size_t Count> class GlAttribute;
	template <typename T, size_t Count> class GlAttributeBuffer;
	class GlUnpackPixelBuffer;

	typedef GlAttribute<real, 3>		GlAttribute3r;
	typedef GlAttribute<real, 2>		GlAttribute2r;
	typedef GlAttribute<int, 3>			GlAttribute3i;
	typedef GlAttribute<int, 2>			GlAttribute2i;
	typedef GlAttributeBuffer<real, 3>	GlAttributeBuffer3r;
	typedef GlAttributeBuffer<real, 2>	GlAttributeBuffer2r;
	typedef GlAttributeBuffer<int, 3>	GlAttributeBuffer3i;
	typedef GlAttributeBuffer<int, 2>	GlAttributeBuffer2i;

	typedef shared_ptr<	GlAttributeBase			>	GlAttributePtr;
	typedef shared_ptr<	GlAttribute3r			>	GlAttribute3rPtr;
	typedef shared_ptr<	GlAttribute2r			>	GlAttribute2rPtr;
	typedef shared_ptr<	GlAttribute3i			>	GlAttribute3iPtr;
	typedef shared_ptr<	GlAttribute2i			>	GlAttribute2iPtr;
	typedef shared_ptr<	GlAttributeBuffer3r		>	GlAttributeBuffer3rPtr;
	typedef shared_ptr<	GlAttributeBuffer2r		>	GlAttributeBuffer2rPtr;
	typedef shared_ptr<	GlAttributeBuffer3i		>	GlAttributeBuffer3iPtr;
	typedef shared_ptr<	GlAttributeBuffer2i		>	GlAttributeBuffer2iPtr;
	typedef shared_ptr<	GlShaderObject			>	GlShaderObjectPtr;
	typedef shared_ptr<	GlShaderProgram			>	GlShaderProgramPtr;
	typedef shared_ptr<	GlFrameVariableBase		>	GlFrameVariablePtr;
	typedef shared_ptr<	CgGlShaderProgram		>	CgGlShaderProgramPtr;
	typedef shared_ptr<	CgGlShaderObject		>	CgGlShaderObjectPtr;
	typedef shared_ptr<	CgGlFrameVariableBase	>	CgGlFrameVariablePtr;

	typedef Container<		GlShaderProgramPtr							>::Vector	GlShaderProgramPtrArray;
	typedef Container<		CgGlShaderProgramPtr						>::Vector	CgGlShaderProgramPtrArray;
	typedef Container<		GlShaderObjectPtr							>::Vector	GlShaderObjectPtrArray;
	typedef Container<		CgGlShaderObjectPtr							>::Vector	CgGlShaderObjectPtrArray;
	typedef KeyedContainer<	String,				GlFrameVariablePtr		>::Map		GlFrameVariablePtrStrMap;
	typedef KeyedContainer<	String,				CgGlFrameVariablePtr	>::Map		CgGlFrameVariablePtrStrMap;

	// OpenGL 2.x
	class GlIndexArray;
	class GlVertexArray;
	class GlIndexBufferObject;
	class GlVertexBufferObject;

	// OpenGL 3.x
	class Gl3VertexBufferObject;
	class GlUniformBufferObject;
	class GlUboVariableBase;
	template <typename T> class GlUboOneVariable;
	template <typename T, size_t Count> class GlUboPointVariable;
	template <typename T, size_t Rows, size_t Columns> class GlUboMatrixVariable;
	template <typename T, size_t Rows> class GlUboSquareMatrixVariable;
	class GlVertexArrayObjects;
	class GlTextureBufferObject;

	typedef GlUboOneVariable<float>					GlUboFloatVariable;
	typedef GlUboOneVariable<int>					GlUboIntVariable;
	typedef GlUboPointVariable<float,	2>			GlUboPoint2fVariable;
	typedef GlUboPointVariable<int,		2>			GlUboPoint2iVariable;
	typedef GlUboPointVariable<float,	3>			GlUboPoint3fVariable;
	typedef GlUboPointVariable<int,		3>			GlUboPoint3iVariable;
	typedef GlUboPointVariable<float,	4>			GlUboPoint4fVariable;
	typedef GlUboPointVariable<int,		4>			GlUboPoint4iVariable;
	typedef GlUboMatrixVariable<real,	2,	2>		GlUboMatrix2x2rVariable;
	typedef GlUboMatrixVariable<real,	3,	3>		GlUboMatrix3x3rVariable;
	typedef GlUboMatrixVariable<real,	4,	4>		GlUboMatrix4x4rVariable;
	typedef GlUboMatrixVariable<float,	2,	2>		GlUboMatrix2x2fVariable;
	typedef GlUboMatrixVariable<float,	3,	3>		GlUboMatrix3x3fVariable;
	typedef GlUboMatrixVariable<float,	4,	4>		GlUboMatrix4x4fVariable;

	typedef shared_ptr <GlUniformBufferObject>		GlUniformBufferObjectPtr;
	typedef shared_ptr <GlUboVariableBase>			GlUboVariablePtr;
	typedef shared_ptr <GlUboFloatVariable>			GlUboFloatVariablePtr;
	typedef shared_ptr <GlUboIntVariable>			GlUboIntVariablePtr;
	typedef shared_ptr <GlUboPoint2fVariable>		GlUboPoint2fVariablePtr;
	typedef shared_ptr <GlUboPoint2iVariable>		GlUboPoint2iVariablePtr;
	typedef shared_ptr <GlUboPoint3fVariable>		GlUboPoint3fVariablePtr;
	typedef shared_ptr <GlUboPoint3iVariable>		GlUboPoint3iVariablePtr;
	typedef shared_ptr <GlUboPoint4fVariable>		GlUboPoint4fVariablePtr;
	typedef shared_ptr <GlUboPoint4iVariable>		GlUboPoint4iVariablePtr;
	typedef shared_ptr <GlUboMatrix2x2rVariable>	GlUboMatrix2x2rVariablePtr;
	typedef shared_ptr <GlUboMatrix3x3rVariable>	GlUboMatrix3x3rVariablePtr;
	typedef shared_ptr <GlUboMatrix4x4rVariable>	GlUboMatrix4x4rVariablePtr;
	typedef shared_ptr <GlUboMatrix2x2fVariable>	GlUboMatrix2x2fVariablePtr;
	typedef shared_ptr <GlUboMatrix3x3fVariable>	GlUboMatrix3x3fVariablePtr;
	typedef shared_ptr <GlUboMatrix4x4fVariable>	GlUboMatrix4x4fVariablePtr;
	typedef shared_ptr <GlVertexArrayObjects>		GlVertexArrayObjectsPtr;
	typedef shared_ptr <GlTextureBufferObject>		GlTextureBufferObjectPtr;

	typedef KeyedContainer<String,	GlUniformBufferObjectPtr>::Map	GlUniformBufferObjectPtrStrMap;
	typedef KeyedContainer<String,	GlUboVariablePtr>::Map			GlUboVariablePtrStrMap;

	// Common
	class GlEnum
	{
	public:
		struct GlPixelFmt
		{
			GLenum Format;
			GLenum Internal;
			GLenum Type;
		};

	private:
		static String		GlslStrings				[];
		static String		GlslErrors				[];
		static GLenum		DrawTypes				[ePRIMITIVE_TYPE_COUNT];
		static GLenum 		TextureDimensions		[eTEXTURE_TYPE_COUNT];
		static GLenum		AlphaFuncs				[TextureUnit::eALPHA_FUNC_COUNT];
		static GLenum		TextureWrapMode			[TextureUnit::eWRAP_MODE_COUNT];
		static GLenum		TextureInterpolation	[TextureUnit::eINTERPOLATION_MODE_COUNT];
		static GLenum 		LightIndexes			[eLIGHT_INDEXES_COUNT];
		static GLenum		SrcBlendFactors			[Pass::eSRC_BLEND_COUNT];
		static GLenum		DstBlendFactors			[Pass::eDST_BLEND_COUNT];
		static GLenum		Usages					[eELEMENT_USAGE_COUNT];
		static GLenum 		TextureArguments		[TextureUnit::eBLEND_SOURCE_COUNT];
		static GLenum 		RgbBlendFuncs			[TextureUnit::eRGB_BLEND_FUNC_COUNT];
		static GLenum 		AlphaBlendFuncs			[TextureUnit::eALPHA_BLEND_FUNC_COUNT];
		static GlPixelFmt	PixelFormats			[ePIXEL_FORMAT_COUNT];
		static GLenum		ShaderTypes				[eSHADER_TYPE_COUNT];

	public:
		static bool					GlCheckError		( String const & p_strText, bool p_bThrows);
		static bool					CgCheckError		( String const & p_strText, bool p_bThrows);
		static GLenum				GetLockFlags		( size_t p_uiFlags);
		static String				GetGlslErrorString	( int p_index)												{ return GlslStrings[p_index]; }
		static inline GLenum		Get					( ePRIMITIVE_TYPE p_index)									{ return DrawTypes[p_index]; }
		static inline GLenum 		Get					( eTEXTURE_TYPE p_index)									{ return TextureDimensions[p_index]; }
		static inline GLenum		Get					( eLIGHT_INDEXES p_uiIndex)									{ return LightIndexes[p_uiIndex]; }
		static inline GLenum		Get					( TextureUnit::eALPHA_FUNC p_eAlphaFunc)					{ return AlphaFuncs[p_eAlphaFunc]; }
		static inline GLenum		Get					( TextureUnit::eWRAP_MODE p_eWrapMode)						{ return TextureWrapMode[p_eWrapMode]; }
		static inline GLenum		Get					( TextureUnit::eINTERPOLATION_MODE p_eInterpolationMode)	{ return TextureInterpolation[p_eInterpolationMode]; }
		static inline GLenum		Get					( TextureUnit::eBLEND_SOURCE p_eArgument)					{ return TextureArguments[p_eArgument]; }
		static inline GLenum		Get					( TextureUnit::eRGB_BLEND_FUNC p_eMode)						{ return RgbBlendFuncs[p_eMode]; }
		static inline GLenum		Get					( TextureUnit::eALPHA_BLEND_FUNC p_eMode)					{ return AlphaBlendFuncs[p_eMode]; }
		static inline GLenum		Get					( Pass::eSRC_BLEND p_eBlendFactor)							{ return SrcBlendFactors[p_eBlendFactor]; }
		static inline GLenum		Get					( Pass::eDST_BLEND p_eBlendFactor)							{ return DstBlendFactors[p_eBlendFactor]; }
		static inline GlPixelFmt	Get					( Castor::Resources::ePIXEL_FORMAT p_pixelFormat)			{ return PixelFormats[p_pixelFormat]; }
		static inline GLenum		Get					( eELEMENT_USAGE p_eUsage)									{ return Usages[p_eUsage]; }
		static inline GLenum		Get					( eSHADER_TYPE p_eType)										{ return ShaderTypes[p_eType]; }
		static inline GLenum 		GetBufferFlags		( size_t p_uiFlags)											{ return (p_uiFlags & eBUFFER_MODE_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW); }
	};
}

#if CASTOR_USE_DOUBLE
#	define glTranslate			glTranslated
#	define glRotate				glRotated
#	define glMultMatrix			glMultMatrixd
#	define glScale				glScaled
#	define glMultiTexCoord2		glMultiTexCoord2dARB
#	define GL_REAL				GL_DOUBLE
#	define glVertex2r			glVertex2d
#	define glVertex2rv			glVertex2dv
#	define glTexCoord2r			glTexCoord2d
#	define glTexCoord2rv		glTexCoord2dv
#	define glVertex3r			glVertex3d
#	define glVertex3rv			glVertex3dv
#	define glColor3r			glColor3d
#	define glColor3rv			glColor3dv
#else
#	define glTranslate			glTranslatef
#	define glRotate				glRotatef
#	define glMultMatrix			glMultMatrixf
#	define glScale				glScalef
#	define glMultiTexCoord2		glMultiTexCoord2fARB
#	define GL_REAL				GL_FLOAT
#	define glVertex2r			glVertex2f
#	define glVertex2rv			glVertex2fv
#	define glTexCoord2r			glTexCoord2f
#	define glTexCoord2rv		glTexCoord2fv
#	define glVertex3r			glVertex3f
#	define glVertex3rv			glVertex3fv
#	define glColor3r			glColor3f
#	define glColor3rv			glColor3fv

#endif

#   ifndef NDEBUG
#	    define glCheckError( txt) GlEnum::GlCheckError( cuT( txt), false)
#	    define cgCheckError( func, txt) {func;GlEnum::CgCheckError( txt, false);}
#   else
#	    define glCheckError( txt) true
#	    define cgCheckError( func, txt)
#   endif


#define CASTOR_GTK 1
#endif
