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
#ifndef ___C3D_UniformVariable___
#define ___C3D_UniformVariable___

namespace Castor3D
{
	//! Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class UniformVariable
	{
	public:
		//! The different supported variable counts
		/*!
		Can be 1, 2, 3, 4 variables, 2, 3, 4 dimensions matrices
		*/
		typedef enum eUNIFORM_VARIABLE_TYPE
		{
			eOne,	//!< One variable
			eVec,	//!< array
			eVec1,	//!< 1 variable array
			eVec2,	//!< 2 variables array
			eVec3,	//!< 3 variables array
			eVec4,	//!< 4 variables array
			eMat,	//!< matrix
			eMat1,	//!< 1x1 matrix
			eMat2,	//!< 2x2 matrix
			eMat3,	//!< 3x3 matrix
			eMat4	//!< 4x4 matrix
		} eUNIFORM_VARIABLE_TYPE;

	protected:
		String m_strName;	//!< The variable name as it appears in the shader program
		String m_strValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		UniformVariable(){}
		/**
		 * Copy constructor
		 */
		UniformVariable( const UniformVariable & p_rVariable)
			:	m_strName( p_rVariable.m_strName),
				m_strValue( p_rVariable.m_strValue)
		{}
		/**
		 * Destructor
		 */
		~UniformVariable(){}
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eUNIFORM_VARIABLE_TYPE GetType()=0;
		/**
		 * Gives the type of the variable (essentially bool, int or real)
		 *@return The type of the variable
		 */
		virtual const type_info & GetSubType()=0;
		/**
		 * Gives the value string of the variable
		 *@return The string containing the value of the variable
		 */
		inline String GetStrValue()const { return m_strValue; }
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
		virtual void SetValue( const String & p_strValue)=0;
	};

	//! Variable type shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution.
	It has a variable type, so it can be derived in int, bool or real (essentially)
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class TUniformVariable : public UniformVariable
	{
	public:
		/**
		 * Constructor
		 */
		TUniformVariable(){}
		/**
		 * Copy constructor
		 */
		TUniformVariable( const TUniformVariable & p_rVariable)
			:	UniformVariable( p_rVariable)
		{
		}
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
	class OneUniformVariable : public TUniformVariable<T>
	{
	public:
		T m_tValue;	//!< The single value of the variable

	public:
		/**
		 * Constructor
		 */
		OneUniformVariable(){}
		/**
		 * Copy constructor
		 */
		OneUniformVariable( const OneUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_tValue( p_rVariable.m_tValue)
		{
		}
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
		inline T GetValue() { return m_tValue; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eOne; }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 */
		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			m_tValue = T( atof( p_strValue.c_str()));
		}
		/**
		 * Defines the value of the variable, from a single value
		 *@param p_tValue : [in] The new value
		 */
		inline void SetValue( T p_tValue) { m_tValue = p_tValue; }
	};

	//! Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class PointUniformVariable : public TUniformVariable<T>
	{
	public:
		Point<T, Count> m_ptValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		PointUniformVariable(){}
		/**
		 * Copy constructor
		 */
		PointUniformVariable( const PointUniformVariable<T, Count> & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_ptValue( p_rVariable.m_ptValue)
		{
		}
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
		inline Point<T, Count> GetValue() { return m_ptValue; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eUNIFORM_VARIABLE_TYPE( eVec + Count); }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 */
		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arraySplitted = p_strValue.Split( ", \t");

			if (l_arraySplitted.size() == Count)
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					m_ptValue[i] = T( atof( l_arraySplitted[i].c_str()));
				}
			}
		}
		/**
		 * Defines the value of the variable, from a Point2D
		 *@param p_ptValue : [in] The new value
		 */
		inline void SetValue( Point<T, Count> p_ptValue) { m_ptValue = p_ptValue; }
	};

	//! N dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class MatrixUniformVariable : public TUniformVariable<T>
	{
	public:
		Matrix <T, Count, Count> m_mValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		MatrixUniformVariable(){}
		/**
		 * Copy constructor
		 */
		MatrixUniformVariable( const MatrixUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_mValue( p_rVariable.m_mValue)
		{
		}
		/**
		 * Gives the value of the variable
		 *@return The variable value
		 */
		inline Matrix <T, Count, Count> GetValue() { return m_mValue; }
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eUNIFORM_VARIABLE_TYPE( eMat + Count); }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 */
		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arrayLines = p_strValue.Split( ";");

			if (l_arrayLines.size() == Count)
			{
				bool l_bOK = true;

				for (size_t i = 0 ; i < Count && l_bOK ; i++)
				{
					l_bOK = false;
					StringArray l_arraySplitted = l_arrayLines[i].Split( ", \t");

					if (l_arraySplitted.size() == Count)
					{
						l_bOK = true;

						for (size_t j = 0 ; j < Count ; j++)
						{
							m_mValue.m_matrix[i * Count + j] = T( atof( l_arraySplitted[j].c_str()));
						}
					}
				}
			}
		}
		/**
		 * Defines the value of the variable, from a Matrix2
		 *@param p_mValue : [in] The new value
		 */
		inline void SetValue( Matrix <T, Count, Count> p_mValue) { m_mValue = p_mValue; }
	};
}

#endif