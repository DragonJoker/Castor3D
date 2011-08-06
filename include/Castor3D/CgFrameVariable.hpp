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
#ifndef ___C3D_CgFrameVariable___
#define ___C3D_CgFrameVariable___

#include "FrameVariable.hpp"

namespace Castor3D
{
	//! Cg Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class CgFrameVariable
	{
	protected:
		CGprogram m_cgProgram;		//!< Cg program holding this variable

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 *@param p_cgProgram : [in] The program holding this variable
		 *@param p_uiOcc : [in] The number of table occurencies of this variable (if 1, then no table)
		 */
		CgFrameVariable( CGprogram p_cgProgram)
			:	m_cgProgram( p_cgProgram)
		{
		}
		/**
		 * Copy constructor
		 */
		CgFrameVariable( const CgFrameVariable & p_rVariable)
			:	m_cgProgram( p_rVariable.m_cgProgram)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~CgFrameVariable()
		{
		}
		//@}

		/**@name Accessors */
		//@{
		inline CGprogram 	GetProgram		()const { return m_cgProgram; }

		inline void SetProgram	( CGprogram val)	{ m_cgProgram = val; }
		//@}
	};
	//! Cg Single variable typed variable
	/*!
	This is a single variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class CgOneFrameVariable : public CgFrameVariable, public OneFrameVariable<T>
	{
	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgOneFrameVariable( CgShaderProgram * p_pProgram, CgShaderObject * p_pObject, CGprogram p_cgProgram=nullptr, size_t p_uiOcc=1)
			:	CgFrameVariable( p_cgProgram)
			,	OneFrameVariable<T>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		/**
		 * Copy constructor
		 */
		CgOneFrameVariable( const CgOneFrameVariable & p_rVariable)
			:	CgFrameVariable( p_rVariable)
			,	OneFrameVariable<T>( p_rVariable)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~CgOneFrameVariable()
		{
		}
		//@}
	};
	//! Cg Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class CgPointFrameVariable : public CgFrameVariable, public PointFrameVariable<T, Count>
	{
	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgPointFrameVariable( CgShaderProgram * p_pProgram, CgShaderObject * p_pObject, CGprogram p_cgProgram=nullptr, size_t p_uiOcc=1)
			:	CgFrameVariable( p_cgProgram)
			,	PointFrameVariable<T, Count>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		/**
		 * Copy constructor
		 */
		CgPointFrameVariable( const CgPointFrameVariable<T, Count> & p_rVariable)
			:	CgFrameVariable( p_rVariable)
			,	PointFrameVariable<T, Count>( p_rVariable)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~CgPointFrameVariable()
		{
		}
		//@}
	};
	//! Cg Array of MxN dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class CgMatrixFrameVariable : public CgFrameVariable, public MatrixFrameVariable<T, Rows, Columns>
	{
	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgMatrixFrameVariable( CgShaderProgram * p_pProgram, CgShaderObject * p_pObject, CGprogram p_cgProgram=nullptr, size_t p_uiOcc=1)
			:	CgFrameVariable( p_cgProgram)
			,	MatrixFrameVariable<T, Rows, Columns>( p_pProgram, p_pObject, p_uiOcc)
		{
		}
		/**
		 * Copy constructor
		 */
		CgMatrixFrameVariable( const CgMatrixFrameVariable & p_rVariable)
			:	CgFrameVariable( p_rVariable)
			,	MatrixFrameVariable<T, Rows, Columns>( p_rVariable)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~CgMatrixFrameVariable()
		{
		}
		//@}
	};
}

#endif
