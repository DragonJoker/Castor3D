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
		/**
		 * Constructor
		 */
		CgFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
			:	FrameVariable( p_uiOcc),
				m_cgParameter( NULL),
				m_cgProgram( p_cgProgram)
		{
		}
		/**
		 * Copy constructor
		 */
		CgFrameVariable( const CgFrameVariable & p_rVariable)
			:	FrameVariable( p_rVariable),
				m_cgParameter( p_rVariable.m_cgParameter),
				m_cgProgram( p_rVariable.m_cgProgram)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~CgFrameVariable()=0
		{
		}

	public:
		/**@name Accessors */
		//@{
		inline CGparameter 	GetParameter	()const { return m_cgParameter; }
		inline CGprogram 	GetProgram		()const { return m_cgProgram; }
		inline void SetProgram		( CGprogram val)	{ m_cgProgram = val; }
		inline void SetParameter	( CGparameter val)	{ m_cgParameter = val; }
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
		typedef Policy<T> Policy;

	public:
		/**
		 * Constructor
		 */
		TCgFrameVariable( CGprogram p_cgProgram, size_t p_uiOcc)
			:	CgFrameVariable( p_cgProgram, p_uiOcc)
		{}
		/**
		 * Copy constructor
		 */
		TCgFrameVariable( const TCgFrameVariable & p_rVariable)
			:	CgFrameVariable( p_rVariable)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~TCgFrameVariable()=0
		{}
		/**
		 *@return The type of the variable (int, bool, real)
		 */
		virtual const type_info & GetSubType() { return typeid( T); }
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
	public:
		T * m_tValue;	//!< The single value of the variable

	public:
		/**
		 * Constructor
		 */
		CgOneFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1)
			:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
		{
			m_tValue = new T[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		CgOneFrameVariable( const CgOneFrameVariable & p_rVariable)
			:	TCgFrameVariable<T>( p_rVariable)
		{
			m_tValue = new T[m_uiOcc];

			for (size_t i = 0 ; i < m_uiOcc ; i++)
			{
				Policy::assign( m_tValue[i], p_rVariable.m_tValue[i]);
			}
		}
		/**
		 * Destructor
		 */
		virtual ~CgOneFrameVariable()
		{
			delete [] m_tValue;
		}
		/**@name Accessors */
		//@{
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
		inline T &			operator []( size_t p_uiIndex)		{ return m_tValue[p_uiIndex]; }
		inline const T &	operator []( size_t p_uiIndex)const	{ return m_tValue[p_uiIndex]; }
		inline T &			GetValue( size_t p_uiIndex=0)		{ return m_tValue[p_uiIndex]; }
		inline const T &	GetValue( size_t p_uiIndex=0)const	{ return m_tValue[p_uiIndex]; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eTYPE GetType() { return eOne; }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0)
		{
			m_strValue[p_uiIndex] = p_strValue;
			Policy::assign<double>( m_tValue[p_uiIndex], atof( p_strValue.c_str()));
		}
		/**
		 * Defines the value of the variable, from a single value
		 *@param p_tValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( const T & p_tValue, size_t p_uiIndex=0) { Policy::assign( m_tValue[p_uiIndex], p_tValue);m_bChanged = true; }
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
	public:
		T * m_pValues;
		Point<T, Count> * m_ptValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		CgPointFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1)
			:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
		{
			m_pValues = new T[m_uiOcc * Count];
			m_ptValue = new Point<T, Count>[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		CgPointFrameVariable( const CgPointFrameVariable<T, Count> & p_rVariable)
			:	TCgFrameVariable<T>( p_rVariable)
		{
			m_pValues = new T[m_uiOcc * Count];
			m_ptValue = new Point<T, Count>[m_uiOcc];

			for (size_t i = 0 ; i < m_uiOcc * Count ; i++)
			{
				Policy::assign( m_pValues[i], p_rVariable.m_pValues[i]);
			}

			for (size_t i = 0 ; i < m_uiOcc ; i++)
			{
				m_ptValue[i].LinkCoords( & m_pValues[i * Count]);
			}
		}
		/**
		 * Destructor
		 */
		virtual ~CgPointFrameVariable()
		{
			delete [] m_ptValue;
		}
		/**@name Accessors */
		//@{
		inline Point<T, Count> &		operator []( size_t p_uiIndex)		{ return m_ptValue[p_uiIndex]; }
		inline const Point<T, Count> &	operator []( size_t p_uiIndex)const	{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> &		GetValue( size_t p_uiIndex=0)		{ return m_ptValue[p_uiIndex]; }
		inline const Point<T, Count> &	GetValue( size_t p_uiIndex=0)const	{ return m_ptValue[p_uiIndex]; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eTYPE GetType();
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0)
		{
			m_strValue[p_uiIndex] = p_strValue;
			StringArray l_arraySplitted = p_strValue.Split( ", \t");

			if (l_arraySplitted.size() == Count)
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					Policy::assign( m_ptValue[p_uiIndex][i], atof( l_arraySplitted[i].c_str()));
				}
			}
		}
		/**
		 * Defines the value of the variable, from a Point2D
		 *@param p_ptValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( const Point<T, Count> & p_ptValue, size_t p_uiIndex=0) { m_ptValue[p_uiIndex] = p_ptValue;m_bChanged = true; }
		//@}
	};

	template <size_t Count> struct CgPointVariableTyper;

	//! Cg Array of MxN dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class CgMatrixFrameVariable : public TCgFrameVariable<T>
	{
	public:
		T * m_pValues;
		Matrix <T, Rows, Columns> * m_mValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		CgMatrixFrameVariable( CGprogram p_cgProgram=NULL, size_t p_uiOcc=1)
			:	TCgFrameVariable<T>( p_cgProgram, p_uiOcc)
		{
			m_pValues = new T[Rows * Columns * m_uiOcc];
			m_mValue = new Matrix <T, Rows, Columns>[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		CgMatrixFrameVariable( const CgMatrixFrameVariable & p_rVariable)
			:	TCgFrameVariable<T>( p_rVariable)
		{
			m_pValues = new T[Rows * Columns * m_uiOcc];
			m_mValue = new Matrix <T, Rows, Columns>[m_uiOcc];

			for (size_t i = 0 ; i < Rows * Columns * m_uiOcc ; i++)
			{
				Policy::assign( m_pValues[i], p_rVariable.m_pValues[i]);
			}

			for (size_t i = 0 ; i < m_uiOcc ; i++)
			{
				m_mValue[i].LinkCoords( & m_pValues[i * Rows * Columns]);
			}
		}
		/**
		 * Destructor
		 */
		virtual ~CgMatrixFrameVariable()
		{
			delete [] m_mValue;
		}
		/**@name Accessors */
		//@{
		inline Matrix <T, Rows, Rows> &			operator []( size_t p_uiIndex)		{ return m_mValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	operator []( size_t p_uiIndex)const	{ return m_mValue[p_uiIndex]; }
		inline Matrix <T, Rows, Rows> &			GetValue( size_t p_uiIndex=0)		{ return m_mValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	GetValue( size_t p_uiIndex=0)const	{ return m_mValue[p_uiIndex]; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eTYPE GetType();
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0)
		{
			m_strValue[p_uiIndex] = p_strValue;
			StringArray l_arrayLines = p_strValue.Split( ";");

			if (l_arrayLines.size() == Rows)
			{
				bool l_bOK = true;

				for (size_t i = 0 ; i < Rows && l_bOK ; i++)
				{
					l_bOK = false;
					StringArray l_arraySplitted = l_arrayLines[i].Split( ", \t");

					if (l_arraySplitted.size() == Columns)
					{
						l_bOK = true;

						for (size_t j = 0 ; j < Columns ; j++)
						{
							Policy::assign( m_mValue[p_uiIndex][j][i], atof( l_arraySplitted[j].c_str()));
						}
					}
				}
			}
		}
		/**
		 * Defines the value of the variable, from a Matrix2
		 *@param p_mValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( const Matrix <T, Rows, Columns> & p_mValue, size_t p_uiIndex=0) { m_mValue[p_uiIndex] = p_mValue;m_bChanged = true; }
		//@}
	};

	template <size_t Rows, size_t Columns> struct CgMatrixVariableTyper;
}

#include "CgFrameVariable.inl"

#endif