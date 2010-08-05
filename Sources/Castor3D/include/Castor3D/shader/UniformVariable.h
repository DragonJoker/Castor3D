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
#ifndef ___C3D_UniformVariable___
#define ___C3D_UniformVariable___

namespace Castor3D
{
	//! Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/02/2010
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
			eVec2,	//!< 2 variables array
			eVec3,	//!< 3 variables array
			eVec4,	//!< 4 variables array
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
		 * Gives the count type of this variable
		 */
		virtual eUNIFORM_VARIABLE_TYPE GetType()=0;
		/**
		 * Gives the type of this variable (essentially bool, int or float)
		 */
		virtual const type_info & GetSubType()=0;
		/**
		 * Gives the value string of the variable
		 */
		inline String GetStrValue()const { return m_strValue; }
		/**
		 * Gives the name of the variable
		 */
		inline String GetName()const { return m_strName; }
		/**
		 * Defines the name of this variable
		 */
		inline void SetName( const String & p_strName) { m_strName = p_strName; }
		/**
		 * Defines the value of this variable, from a string
		 */
		virtual void SetValue( const String & p_strValue)=0;
	};

	template <typename T>
	class TUniformVariable : public UniformVariable
	{
	public:
		TUniformVariable(){}
		TUniformVariable( const TUniformVariable & p_rVariable)
			:	UniformVariable( p_rVariable)
		{
		}
		virtual const type_info & GetSubType() { return typeid( T); }
	};

	template <typename T>
	class OneUniformVariable : public TUniformVariable<T>
	{
	protected:
		T m_tValue;

	public:
		OneUniformVariable(){}
		OneUniformVariable( const OneUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_tValue( p_rVariable.m_tValue)
		{
		}
		inline T GetValue() { return m_tValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eOne; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			m_tValue = T( atof( p_strValue.char_str()));
		}
		inline void SetValue( T p_tValue) { m_tValue = p_tValue; }
	};

	template <typename T>
	class Point2DUniformVariable : public TUniformVariable<T>
	{
	protected:
		Point2D <T> m_ptValue;

	public:
		Point2DUniformVariable(){}
		Point2DUniformVariable( const Point2DUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_ptValue( p_rVariable.m_ptValue)
		{
		}
		inline Point2D <T> GetValue() { return m_ptValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eVec2; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arraySplitted = p_strValue.Split( ", \t");

			if (l_arraySplitted.size() == 2)
			{
				for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
				{
					m_ptValue.ptr()[i] = T( atof( l_arraySplitted[i].char_str()));
				}
			}
		}
		inline void SetValue( Point2D <T> p_ptValue) { m_ptValue = p_ptValue; }
	};

	template <typename T>
	class Point3DUniformVariable : public TUniformVariable<T>
	{
	protected:
		Point3D <T> m_ptValue;

	public:
		Point3DUniformVariable(){}
		Point3DUniformVariable( const Point3DUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_ptValue( p_rVariable.m_ptValue)
		{
		}
		inline Point3D <T> GetValue() { return m_ptValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eVec3; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arraySplitted = p_strValue.Split( ", \t");

			if (l_arraySplitted.size() == 3)
			{
				for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
				{
					m_ptValue.ptr()[i] = T( atof( l_arraySplitted[i].char_str()));
				}
			}
		}
		inline void SetValue( Point3D <T> p_ptValue) { m_ptValue = p_ptValue; }
	};

	template <typename T>
	class Point4DUniformVariable : public TUniformVariable<T>
	{
	protected:
		Point4D <T> m_ptValue;

	public:
		Point4DUniformVariable(){}
		Point4DUniformVariable( const Point4DUniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_ptValue( p_rVariable.m_ptValue)
		{
		}
		inline Point4D <T> GetValue() { return m_ptValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eVec4; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arraySplitted = p_strValue.Split( ", \t");

			if (l_arraySplitted.size() == 4)
			{
				for (size_t i = 0 ; i < l_arraySplitted.size() ; i++)
				{
					m_ptValue.ptr()[i] = T( atof( l_arraySplitted[i].char_str()));
				}
			}
		}
		inline void SetValue( Point4D <T> p_ptValue) { m_ptValue = p_ptValue; }
	};

	template <typename T>
	class Matrix2UniformVariable : public TUniformVariable<T>
	{
	protected:
		Matrix2 <T> m_mValue;

	public:
		Matrix2UniformVariable(){}
		Matrix2UniformVariable( const Matrix2UniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_mValue( p_rVariable.m_mValue)
		{
		}
		inline Matrix2 <T> GetValue() { return m_mValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eMat2; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arrayLines = p_strValue.Split( ";");

			if (l_arrayLines.size() == 2)
			{
				bool l_bOK = true;

				for (size_t i = 0 ; i < l_arrayLines.size() && l_bOK ; i++)
				{
					l_bOK = false;
					StringArray l_arraySplitted = l_arrayLines[i].Split( ", \t");

					if (l_arraySplitted.size() == 2)
					{
						l_bOK = true;

						for (size_t j = 0 ; j < l_arraySplitted.size() ; j++)
						{
							m_mValue.m_matrix[i][j] = T( atof( l_arraySplitted[j].char_str()));
						}
					}
				}
			}
		}
		inline void SetValue( Matrix2 <T> p_mValue) { m_mValue = p_mValue; }
	};

	template <typename T>
	class Matrix3UniformVariable : public TUniformVariable<T>
	{
	protected:
		Matrix3 <T> m_mValue;

	public:
		Matrix3UniformVariable(){}
		Matrix3UniformVariable( const Matrix3UniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_mValue( p_rVariable.m_mValue)
		{
		}
		inline Matrix3 <T> GetValue() { return m_mValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eMat3; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arrayLines = p_strValue.Split( ";");

			if (l_arrayLines.size() == 3)
			{
				bool l_bOK = true;

				for (size_t i = 0 ; i < l_arrayLines.size() && l_bOK ; i++)
				{
					l_bOK = false;
					StringArray l_arraySplitted = l_arrayLines[i].Split( ", \t");

					if (l_arraySplitted.size() == 3)
					{
						l_bOK = true;

						for (size_t j = 0 ; j < l_arraySplitted.size() ; j++)
						{
							m_mValue.m_matrix[i][j] = T( atof( l_arraySplitted[j].char_str()));
						}
					}
				}
			}
		}
		inline void SetValue( Matrix3 <T> p_mValue) { m_mValue = p_mValue; }
	};

	template <typename T>
	class Matrix4UniformVariable : public TUniformVariable<T>
	{
	protected:
		Matrix4 <T> m_mValue;

	public:
		Matrix4UniformVariable(){}
		Matrix4UniformVariable( const Matrix4UniformVariable & p_rVariable)
			:	TUniformVariable<T>( p_rVariable),
				m_mValue( p_rVariable.m_mValue)
		{
		}
		inline Matrix4 <T> GetValue() { return m_mValue; }

		virtual eUNIFORM_VARIABLE_TYPE GetType() { return eMat4; }

		virtual void SetValue( const String & p_strValue)
		{
			m_strValue = p_strValue;
			StringArray l_arrayLines = p_strValue.Split( ";");

			if (l_arrayLines.size() == 4)
			{
				bool l_bOK = true;

				for (size_t i = 0 ; i < l_arrayLines.size() && l_bOK ; i++)
				{
					l_bOK = false;
					StringArray l_arraySplitted = l_arrayLines[i].Split( ", \t");

					if (l_arraySplitted.size() == 4)
					{
						l_bOK = true;

						for (size_t j = 0 ; j < l_arraySplitted.size() ; j++)
						{
							m_mValue.m_matrix[i][j] = T( atof( l_arraySplitted[j].char_str()));
						}
					}
				}
			}
		}
		inline void SetValue( Matrix4 <T> p_mValue) { m_mValue = p_mValue; }
	};
}

#endif
