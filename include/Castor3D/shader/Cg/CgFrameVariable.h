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
#ifndef ___C3D_CgFrameVariable___
#define ___C3D_CgFrameVariable___

#include "../FrameVariable.h"

namespace Castor3D
{
	//! Cg Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class CgFrameVariable : public FrameVariable
	{
	protected:
		CGparameter m_cgParameter;
		CGprogram m_cgProgram;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 *@param p_cgProgram : [in] The program holding this variable
		 *@param p_uiOcc : [in] The number of table occurencies of this variable (if 1, then no table)
		 */
		CgFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
			:	FrameVariable( p_uiOcc)
			,	m_cgParameter( NULL)
			,	m_cgProgram( p_cgProgram)
		{
		}
		/**
		 * Copy constructor
		 */
		CgFrameVariable( const CgFrameVariable & p_rVariable)
			:	FrameVariable( p_rVariable)
			,	m_cgParameter( p_rVariable.m_cgParameter)
			,	m_cgProgram( p_rVariable.m_cgProgram)
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
		inline CGparameter 	GetParameter	()const { return m_cgParameter; }
		inline CGprogram 	GetProgram		()const { return m_cgProgram; }
		inline void SetProgram		( CGprogram val)								{ m_cgProgram = val; }
		inline void SetParameter	( CGparameter val)								{ m_cgParameter = val; }
		inline void SetStrValue		( const String & p_strValue, size_t p_uiIndex) 	{ FrameVariable::SetStrValue( p_strValue, p_uiIndex); }
		inline void SetChanged		( bool p_bVal=true)								{ FrameVariable::SetChanged( p_bVal); }
		//@}
	};
	//! Cg Variable type shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution.
	It has a variable type, so it can be derived in int, bool or real (essentially)
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class TCgFrameVariable : public CgFrameVariable
	{
	protected:
		typedef Policy<T> policy;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		TCgFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc);
		/**
		 * Copy constructor
		 */
		TCgFrameVariable( const TCgFrameVariable & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~TCgFrameVariable();
		//@}

		/**@name Accessors */
		//@{
		virtual String GetSubType() { return typeid( T).name(); }
		inline void SetStrValue		( const String & p_strValue, size_t p_uiIndex) 	{ CgFrameVariable::SetStrValue( p_strValue, p_uiIndex); }
		inline void SetChanged		( bool p_bVal=true)								{ CgFrameVariable::SetChanged( p_bVal); }
		//@}
	};
	//! Cg Single variable typed variable
	/*!
	This is a single variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class CgOneFrameVariable : public TCgFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_tValue;	//!< The single value of the variable

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgOneFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		CgOneFrameVariable( const CgOneFrameVariable & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~CgOneFrameVariable();
		//@}

		/**@name Accessors */
		//@{
		inline T &			operator []( size_t p_uiIndex)		{ return m_tValue[p_uiIndex]; }
		inline const T &	operator []( size_t p_uiIndex)const	{ return m_tValue[p_uiIndex]; }
		inline T &			GetValue( size_t p_uiIndex=0)		{ return m_tValue[p_uiIndex]; }
		inline const T &	GetValue( size_t p_uiIndex=0)const	{ return m_tValue[p_uiIndex]; }
		virtual FrameVariable::eTYPE GetType() { return FrameVariable::eOne; }
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0);
		inline void SetValue( const T & p_tValue, size_t p_uiIndex=0) { policy::assign( m_tValue[p_uiIndex], p_tValue);TCgFrameVariable<T>::SetChanged(); }
		//@}
	};
	//! Cg Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class CgPointFrameVariable : public TCgFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_pValues;
		Point<T, Count> * m_ptValue;	//!< The value of the variable

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgPointFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		CgPointFrameVariable( const CgPointFrameVariable<T, Count> & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~CgPointFrameVariable();
		//@}

		/**@name Accessors */
		//@{
		inline Point<T, Count> &		operator []( size_t p_uiIndex)		{ return m_ptValue[p_uiIndex]; }
		inline const Point<T, Count> &	operator []( size_t p_uiIndex)const	{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> &		GetValue( size_t p_uiIndex=0)		{ return m_ptValue[p_uiIndex]; }
		inline const Point<T, Count> &	GetValue( size_t p_uiIndex=0)const	{ return m_ptValue[p_uiIndex]; }
		virtual FrameVariable::eTYPE GetType();
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0);
		inline void SetValue( const Point<T, Count> & p_ptValue, size_t p_uiIndex=0) { m_ptValue[p_uiIndex] = p_ptValue;TCgFrameVariable<T>::SetChanged(); }
		//@}
	};
	//! Cg Array of MxN dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class CgMatrixFrameVariable : public TCgFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_pValues;
		Matrix <T, Rows, Columns> * m_mValue;	//!< The value of the variable

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		CgMatrixFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		CgMatrixFrameVariable( const CgMatrixFrameVariable & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~CgMatrixFrameVariable();
		//@}

		/**@name Accessors */
		//@{
		inline Matrix <T, Rows, Rows> &			operator []( size_t p_uiIndex)		{ return m_mValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	operator []( size_t p_uiIndex)const	{ return m_mValue[p_uiIndex]; }
		inline Matrix <T, Rows, Rows> &			GetValue( size_t p_uiIndex=0)		{ return m_mValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	GetValue( size_t p_uiIndex=0)const	{ return m_mValue[p_uiIndex]; }
		virtual FrameVariable::eTYPE GetType();
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0);
		inline void SetValue( const Matrix <T, Rows, Columns> & p_mValue, size_t p_uiIndex=0) { m_mValue[p_uiIndex] = p_mValue;TCgFrameVariable<T>::SetChanged(); }
		//@}
	};

#include "CgFrameVariable.inl"
}

#endif
