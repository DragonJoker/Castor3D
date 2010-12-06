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
#ifndef ___C3D_FrameVariable___
#define ___C3D_FrameVariable___

namespace Castor3D
{
	//! Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class FrameVariable
	{
	public:
		//! The different supported variable counts
		/*!
		Can be 1, 2, 3, 4 variables, 2, 3, 4 dimensions matrices
		*/
		typedef enum eTYPE
		{
			eOne,		//!< One variable
			eVec1,		//!< 1 variable array
			eVec2,		//!< 2 variables array
			eVec3,		//!< 3 variables array
			eVec4,		//!< 4 variables array
			eMat1,		//!< 1x1 matrix
			eMat2x2,	//!< 2x2 matrix
			eMat2x3,	//!< 2x3 matrix
			eMat2x4,	//!< 2x4 matrix
			eMat3x2,	//!< 3x2 matrix
			eMat3x3,	//!< 3x3 matrix
			eMat3x4,	//!< 3x4 matrix
			eMat4x2,	//!< 4x2 matrix
			eMat4x3,	//!< 4x3 matrix
			eMat4x4		//!< 4x4 matrix
		}
		eTYPE;

	protected:
		String m_strName;		//!< The variable name as it appears in the shader program
		bool m_bChanged;
		size_t m_uiOcc;
		String * m_strValue;	//!< The value of the variable,

	public:
		/**
		 * Constructor
		 */
		FrameVariable( size_t p_uiOcc=1)
			:	m_bChanged( false),
				m_uiOcc( p_uiOcc)
		{
			m_strValue = new String[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		FrameVariable( const FrameVariable & p_rVariable)
			:	m_strName( p_rVariable.m_strName),
				m_uiOcc( p_rVariable.m_uiOcc)
		{
			m_strValue = new String[m_uiOcc];
		}
		/**
		 * Destructor
		 */
		virtual ~FrameVariable()=0
		{
			delete [] m_strValue;
		}
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eTYPE GetType()=0;
		/**
		 * Gives the type of the variable (essentially bool, int or real)
		 *@return The type of the variable
		 */
		virtual const type_info & GetSubType()=0;
		/**
		 * Gives the name of the variable, as it appears in the shader program
		 *@return The variable name
		 */
		inline String GetName()const { return m_strName; }
		/**
		 * Defines the name of the variable, as it appears in the shader program
		 *@param p_strName : [in] The variable name
		 */
		inline void SetName( const String & p_strName) { m_strName = p_strName; }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 */
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0)=0;
		/**
		 * Gives the value string of the variable
		 *@return The string containing the value of the variable
		 */
		inline String GetStrValue( size_t p_uiIndex=0)const { return m_strValue[p_uiIndex]; }
	};

	//! Variable type shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution.
	It has a variable type, so it can be derived in int, bool or real (essentially)
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class TFrameVariable : public FrameVariable
	{
	protected:
		typedef Value<T> Policy;

	public:
		/**
		 * Constructor
		 */
		TFrameVariable( size_t p_uiOcc=1)
			:	FrameVariable( p_uiOcc)
		{}
		/**
		 * Copy constructor
		 */
		TFrameVariable( const TFrameVariable & p_rVariable)
			:	FrameVariable( p_rVariable)
		{
			for (size_t i = 0 ; i < m_uiOcc ; i++)
			{
				m_strValue[i] = p_rVariable.m_strValue[i];
			}
		}
		/**
		 * Destructor
		 */
		virtual ~TFrameVariable()=0
		{}
		/**
		 *@return The type of the variable (int, bool, real)
		 */
		virtual const type_info & GetSubType() { return typeid( T); }
	};

	//! Single variable typed variable
	/*!
	This is a single variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class OneFrameVariable : public TFrameVariable<T>
	{
	public:
		T * m_tValue;	//!< The single value of the variable

	public:
		/**
		 * Constructor
		 */
		OneFrameVariable( size_t p_uiOcc=1)
			:	TFrameVariable<T>( p_uiOcc)
		{
			m_tValue = new T[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		OneFrameVariable( const OneFrameVariable & p_rVariable)
			:	TFrameVariable<T>( p_rVariable)
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
		virtual ~OneFrameVariable()
		{
			delete [] m_tValue;
		}
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
		 */
		virtual void SetValue( const String & p_strValue, size_t p_uiIndex=0)
		{
			m_strValue[p_uiIndex] = p_strValue;
			Policy::assign<double>( m_tValue[p_uiIndex], atof( p_strValue.c_str()));
		}
		/**
		 * Defines the value of the variable, from a single value
		 *@param p_tValue : [in] The new value
		 */
		inline void SetValue( const T & p_tValue, size_t p_uiIndex=0) { Policy::assign( m_tValue[p_uiIndex], p_tValue);m_bChanged = true; }
	};

	//! Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class PointFrameVariable : public TFrameVariable<T>
	{
	public:
		T * m_pValues;
		Point<T, Count> * m_ptValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		PointFrameVariable( size_t p_uiOcc=1)
			:	TFrameVariable<T>( p_uiOcc)
		{
			m_pValues = new T[m_uiOcc * Count];
			m_ptValue = new Point<T, Count>[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		PointFrameVariable( const PointFrameVariable<T, Count> & p_rVariable)
			:	TFrameVariable<T>( p_rVariable)
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
		virtual ~PointFrameVariable()
		{
			delete [] m_ptValue;
		}
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
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
		 */
		inline void SetValue( const Point<T, Count> & p_ptValue, size_t p_uiIndex=0) { m_ptValue[p_uiIndex] = p_ptValue;m_bChanged = true; }
	};

	template <size_t Count> struct PointVariableTyper;

	//! N dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class MatrixFrameVariable : public TFrameVariable<T>
	{
	public:
		T * m_pValues;
		Matrix <T, Rows, Columns> * m_mValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		MatrixFrameVariable( size_t p_uiOcc=1)
			:	TFrameVariable<T>( p_uiOcc)
		{
			m_pValues = new T[Rows * Columns * m_uiOcc];
			m_mValue = new Matrix <T, Rows, Columns>[m_uiOcc];
		}
		/**
		 * Copy constructor
		 */
		MatrixFrameVariable( const MatrixFrameVariable & p_rVariable)
			:	TFrameVariable<T>( p_rVariable)
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
		virtual ~MatrixFrameVariable()
		{
			delete [] m_mValue;
		}
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
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
		 */
		inline void SetValue( const Matrix <T, Rows, Columns> & p_mValue, size_t p_uiIndex=0) { m_mValue[p_uiIndex] = p_mValue;m_bChanged = true; }
	};

	template <size_t Rows, size_t Columns> struct MatrixVariableTyper;
}

#include "FrameVariable.inl"

#endif