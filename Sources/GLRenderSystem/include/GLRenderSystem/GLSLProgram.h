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
#ifndef ___GL_GLSLProgram___
#define ___GL_GLSLProgram___

#include "Module_GLSL.h"
#include "Module_GL.h"

namespace Castor3D
{
	class CS3D_GL_API GLShaderProgram : public ShaderProgram
	{
	private:   
		GLuint		m_programObject;
		GLchar *	m_linkerLog;
		std::map <String, GLUniformVariable *> m_mapGLUniformVariables;

	public:
		GLShaderProgram();
		GLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GLShaderProgram();
		virtual void Cleanup();

		//!< Returns the OpenGL Program Object (only needed if you want to control everything yourself) \return The OpenGL Program Object
		GLuint     GetProgramObject() { return m_programObject; }

		virtual void Initialise();

		virtual bool       Link(void);                        //!< Link all Shaders
		virtual void       RetrieveLinkerLog( String & strLog);                //!< Get Linker Messages \return char pointer to linker messages. Memory of this string is available until you link again or destroy this class.

		virtual void       Begin();                           //!< use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		virtual void	   ApplyAllVariables();
		virtual void       End();                             //!< Stop using this shader. OpenGL calls will go through regular pipeline.

		virtual void AddUniformVariable( UniformVariable * p_pUniformVariable);
		std::map <String, GLUniformVariable *> GetGLUniformVariables()const { return m_mapGLUniformVariables; }

		GLint       GetUniformLocation(const GLchar *name);  //!< Retrieve Location (index) of a Uniform Variable

		// Note: unsigned integers require GL_EXT_gpu_shader4 (for example GeForce 8800)
		bool       SetUniform1ui(const GLchar* varname, GLuint v0, GLint index = -1); //!< Specify value of uniform unsigned integer variable. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform2ui(const GLchar* varname, GLuint v0, GLuint v1, GLint index = -1); //!< Specify value of uniform unsigned integer variable. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform3ui(const GLchar* varname, GLuint v0, GLuint v1, GLuint v2, GLint index = -1); //!< Specify value of uniform unsigned integer variable. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform4ui(const GLchar* varname, GLuint v0, GLuint v1, GLuint v2, GLuint v3, GLint index = -1); //!< Specify value of uniform unsigned integer variable. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.

		bool       SetUniform1uiv(const GLchar* varname, GLsizei count, GLuint *value, GLint index = -1); //!< Specify values of uniform array. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform2uiv(const GLchar* varname, GLsizei count, GLuint *value, GLint index = -1); //!< Specify values of uniform array. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform3uiv(const GLchar* varname, GLsizei count, GLuint *value, GLint index = -1); //!< Specify values of uniform array. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.
		bool       SetUniform4uiv(const GLchar* varname, GLsizei count, GLuint *value, GLint index = -1); //!< Specify values of uniform array. \warning Requires GL_EXT_gpu_shader4. \param varname The name of the uniform variable.

		// Receive Uniform variables:
		void       GetUniformfv(const GLchar* varname, GLfloat* values, GLint index = -1); //!< Receive value of uniform variable. \param varname The name of the uniform variable.
		void       GetUniformiv(const GLchar* varname, GLint* values, GLint index = -1); //!< Receive value of uniform variable. \param varname The name of the uniform variable.
		void       GetUniformuiv(const GLchar* varname, GLuint* values, GLint index = -1); //!< Receive value of uniform variable. \warning Requires GL_EXT_gpu_shader4 \param varname The name of the uniform variable.

		/*! This method simply calls glBindAttribLocation for the current ProgramObject
		\warning NVidia implementation is different than the GLSL standard: GLSL attempts to eliminate aliasing
		of vertex attributes but this is integral to NVIDIA’s hardware approach and necessary for maintaining
		compatibility with existing OpenGL applications that NVIDIA customers rely on. NVIDIA’s GLSL 
		implementation therefore does not allow built-in vertex attributes to collide with a generic vertex
		attributes that is assigned to a particular vertex  attribute index with glBindAttribLocation.
		For example, you should not use gl_Normal (a built-in vertex attribute) and also use 
		glBindAttribLocation to bind a generic vertex attribute named "whatever" to vertex attribute index 2
		because gl_Normal aliases to index 2.
		\verbatim
		gl_Vertex                0
		gl_Normal                2
		gl_Color                 3
		gl_SecondaryColor        4
		gl_FogCoord              5
		gl_MultiTexCoord0        8
		gl_MultiTexCoord1        9
		gl_MultiTexCoord2       10
		gl_MultiTexCoord3       11
		gl_MultiTexCoord4       12
		gl_MultiTexCoord5       13
		gl_MultiTexCoord6       14
		gl_MultiTexCoord7       15
		\endverbatim

		\param index Index of the variable
		\param name Name of the attribute.
		*/
		void        BindAttribLocation(GLint index, GLchar* name);

		//GLfloat
		bool        SetVertexAttrib1f(GLuint index, GLfloat v0); 				                         //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component
		bool        SetVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1); 								 //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component
		bool        SetVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);					 //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component
		bool        SetVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);  //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component \param v3 value of the attribute component

		//GLdouble
		bool        SetVertexAttrib1d(GLuint index, GLdouble v0);                                         //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component
		bool        SetVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1);                            //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component
		bool        SetVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2);               //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component
		bool        SetVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);  //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component \param v3 value of the attribute component

		//GLshort
		bool        SetVertexAttrib1s(GLuint index, GLshort v0);                                      //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component
		bool        SetVertexAttrib2s(GLuint index, GLshort v0, GLshort v1);                          //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component
		bool        SetVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2);              //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component
		bool        SetVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3);  //!< Specify value of attribute. \param index The index of the vertex attribute. \param v0 value of the attribute component \param v1 value of the attribute component \param v2 value of the attribute component \param v3 value of the attribute component

		// Normalized Byte (for example for RGBA colors)
		bool        SetVertexAttribNormalizedByte(GLuint index, GLbyte v0, GLbyte v1, GLbyte v2, GLbyte v3); //!< Specify value of attribute. Values will be normalized.

		//GLint (Requires GL_EXT_gpu_shader4)
		bool        SetVertexAttrib1i(GLuint index, GLint v0); //!< Specify value of attribute. Requires GL_EXT_gpu_shader4.
		bool        SetVertexAttrib2i(GLuint index, GLint v0, GLint v1); //!< Specify value of attribute. Requires GL_EXT_gpu_shader4.
		bool        SetVertexAttrib3i(GLuint index, GLint v0, GLint v1, GLint v2); //!< Specify value of attribute. Requires GL_EXT_gpu_shader4.
		bool        SetVertexAttrib4i(GLuint index, GLint v0, GLint v1, GLint v2, GLint v3); //!< Specify value of attribute. Requires GL_EXT_gpu_shader4.

		//GLuint (Requires GL_EXT_gpu_shader4)
		bool        SetVertexAttrib1ui(GLuint index, GLuint v0); //!< Specify value of attribute. \warning Requires GL_EXT_gpu_shader4. \param v0 value of the first component
		bool        SetVertexAttrib2ui(GLuint index, GLuint v0, GLuint v1); //!< Specify value of attribute. \warning Requires GL_EXT_gpu_shader4.
		bool        SetVertexAttrib3ui(GLuint index, GLuint v0, GLuint v1, GLuint v2); //!< Specify value of attribute. \warning Requires GL_EXT_gpu_shader4.
		bool        SetVertexAttrib4ui(GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3); //!< Specify value of attribute. \warning Requires GL_EXT_gpu_shader4.

	private:
		void _deleteShaderObject( GLShaderObject *& p_ppObject);
		GLUniformVariable * _createGLUniformVariable( UniformVariable * p_pUniformVariable);
	};
}

#endif