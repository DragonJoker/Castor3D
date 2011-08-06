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
#ifndef ___Gl_FrameVariable___
#define ___Gl_FrameVariable___

#include "Module_GlRender.hpp"

#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

namespace Castor3D
{
	template <typename Type> struct OneVariableApplyer;
	template <typename Type, size_t Count> struct PointVariableApplyer;
	template <typename Type, size_t Rows, size_t Columns> struct MatrixVariableApplyer;

	class GlFrameVariableBase
	{
	protected:
		GLint m_iGlIndex;
		GLuint * m_uiParentProgram;
		bool m_bPresentInProgram;

	public:
		GlFrameVariableBase( GLuint * p_uiProgram);
		virtual ~GlFrameVariableBase();
		virtual void Apply()=0;

	protected:
		void GetVariableLocation( const GLchar * p_pVarName);
		template <typename Type> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
		template <typename Type, int Count> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
		template <typename Type, int Rows, int Columns> void _apply( String const & p_strName, Type const * p_pValue, size_t p_uiOcc);
	};

	template <typename T>
	class GlOneFrameVariable : public OneFrameVariable<T>, public GlFrameVariableBase, public MemoryTraced< GlOneFrameVariable<T> >
	{
	public:
		GlOneFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc);
		GlOneFrameVariable( OneFrameVariable<T> * p_pVariable);
		virtual ~GlOneFrameVariable();

		virtual void Bind(){}
		virtual void Apply();
	};

	template <typename T, size_t Count>
	class GlPointFrameVariable : public PointFrameVariable<T, Count>, public GlFrameVariableBase, public MemoryTraced< GlPointFrameVariable<T, Count> >
	{
	public:
		GlPointFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc);
		GlPointFrameVariable( PointFrameVariable<T, Count> * p_pVariable);
		virtual ~GlPointFrameVariable();

		virtual void Bind(){}
		virtual void Apply();
	};

	template <typename T, size_t Rows, size_t Columns>
	class GlMatrixFrameVariable : public MatrixFrameVariable<T, Rows, Columns>, public GlFrameVariableBase, public MemoryTraced< GlMatrixFrameVariable<T, Rows, Columns> >
	{
	public:
		GlMatrixFrameVariable( GlShaderProgram * p_pProgram, GlShaderObject * p_pObject, size_t p_uiOcc);
		GlMatrixFrameVariable( MatrixFrameVariable<T, Rows, Columns> * p_pVariable);
		virtual ~GlMatrixFrameVariable();

		virtual void Bind(){}
		virtual void Apply();
	};
#include "GlFrameVariable.inl"
}


#endif
