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
#ifndef ___Cg_FrameVariable___
#define ___Cg_FrameVariable___

#include <Castor3D/FrameVariable.hpp>
#include "CgShaderProgram.hpp"
#include "CgShaderObject.hpp"

namespace CgShader
{
	template< typename Type										> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, uint32_t Count						> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, uint32_t p_uiOcc );
	template< typename Type, uint32_t Rows, uint32_t Columns	> inline void ApplyVariable( CGparameter p_cgParameter, Type const * p_pValue, uint32_t p_uiOcc );

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
		/**
		 * Constructor
		 *\param[in]	p_cgProgram	The program holding this variable
		 *\param[in]	p_uiOcc	The number of table occurencies of this variable (if 1, then no table)
		 */
		CgFrameVariable( CGprogram p_cgProgram=NULL );
		/**
		 * Copy constructor
		 */
		CgFrameVariable( CgFrameVariable const & p_rVariable );
		/**
		 * Move constructor
		 */
		CgFrameVariable( CgFrameVariable && p_rVariable );
		/**
		 * Destructor
		 */
		virtual ~CgFrameVariable();

		inline CGprogram 	GetProgram		()const { return m_cgProgram; }
		inline void SetProgram	( CGprogram val)	{ m_cgProgram = val; }
	};
	//! Cg Single variable typed variable
	/*!
	This is a single variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template< typename T >
	class CgOneFrameVariable : public CgFrameVariable, public Castor3D::OneFrameVariable< T >
	{
	public:
		/**
		 * Constructor
		 */
		CgOneFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc=1 );
		/**
		 * Copy constructor
		 */
		CgOneFrameVariable( CgOneFrameVariable< T > const & p_rVariable );
		/**
		 * Move constructor
		 */
		CgOneFrameVariable( CgOneFrameVariable< T > && p_rVariable );
		/**
		 * Destructor
		 */
		virtual ~CgOneFrameVariable();

		virtual void SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage );
	};
	//! Cg Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template< typename T, uint32_t Count >
	class CgPointFrameVariable : public CgFrameVariable, public Castor3D::PointFrameVariable< T, Count >
	{
	public:
		/**
		 * Constructor
		 */
		CgPointFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc=1 );
		/**
		 * Copy constructor
		 */
		CgPointFrameVariable( CgPointFrameVariable< T, Count > const & p_rVariable );
		/**
		 * Move constructor
		 */
		CgPointFrameVariable( CgPointFrameVariable< T, Count > && p_rVariable );
		/**
		 * Destructor
		 */
		virtual ~CgPointFrameVariable();

		virtual void SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage );
	};
	//! Cg Array of MxN dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template< typename T, uint32_t Rows, uint32_t Columns >
	class CgMatrixFrameVariable : public CgFrameVariable, public Castor3D::MatrixFrameVariable< T, Rows, Columns >
	{
	public:
		/**
		 * Constructor
		 */
		CgMatrixFrameVariable( CgShaderProgram * p_pProgram, uint32_t p_uiOcc=1 );
		/**
		 * Copy constructor
		 */
		CgMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > const & p_rVariable );
		/**
		 * Move constructor
		 */
		CgMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > && p_rVariable );
		/**
		 * Destructor
		 */
		virtual ~CgMatrixFrameVariable();

		virtual void SetStage( Castor3D::ShaderObjectBaseSPtr p_pStage );
	};

#include "CgFrameVariable.inl"
}

#endif
