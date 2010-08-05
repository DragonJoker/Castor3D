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
#ifndef ___C3D_Matrix___
#define ___C3D_Matrix___

#include "Module_Utils.h"

namespace General
{	namespace Math
{
	/*!
	Matrix 2x2
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class Matrix2
	{
	public:
		T ** m_matrix;

	public:
		Matrix2()
		{
			m_matrix = new T * [2];

			for( int i = 0; i < 2; i++)
			{
				m_matrix[i] = new T[2];

				for (int j = 0; j < 2; j++)
				{
					m_matrix[i][j] = T( 0);
				}
			}
		}
		virtual ~Matrix2()
		{
			for (int i = 0 ; i < 2 ; i++)
			{
				delete m_matrix[i];
			}

			delete [] m_matrix;
		}
		inline bool operator ==( const Matrix2<T> & p_matrix)
		{
			bool l_bReturn = true;

			for (int i = 0 ; i < 2 && l_bReturn ; i++)
			{
				for (int j = 0 ; j < 2 && l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) < T( 0.00001f);
				}
			}

			return l_bReturn;
		}
		inline bool operator !=( const Matrix2<T> & p_matrix)
		{
			bool l_bReturn = false;

			for (int i = 0 ; i < 2 && ! l_bReturn ; i++)
			{
				for (int j = 0 ; j < 2 && ! l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) >= T( 0.00001f);
				}
			}

			return l_bReturn;
		}
	};
	/*!
	Matrix 3x3
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class Matrix3
	{
	public:
		T ** m_matrix;

	public:
		Matrix3()
		{
			m_matrix = new T * [2];

			for( int i = 0 ; i < 3 ; i++)
			{
				m_matrix[i] = new T[3];

				for (int j = 0 ; j < 3 ; j++)
				{
					m_matrix[i][j] = T( 0);
				}
			}
		}
		virtual ~Matrix3()
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				delete m_matrix[i];
			}

			delete [] m_matrix;
		}
		inline bool operator ==( const Matrix3<T> & p_matrix)
		{
			bool l_bReturn = true;

			for (int i = 0 ; i < 3 && l_bReturn ; i++)
			{
				for (int j = 0 ; j < 3 && l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) < T( 0.00001f);
				}
			}

			return l_bReturn;
		}
		inline bool operator !=( const Matrix3<T> & p_matrix)
		{
			bool l_bReturn = false;

			for (int i = 0 ; i < 3 && ! l_bReturn ; i++)
			{
				for (int j = 0 ; j < 3 && ! l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) >= T( 0.00001f);
				}
			}

			return l_bReturn;
		}
	};
	/*!
	Matrix 4x4
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class Matrix4
	{
	public:
		T ** m_matrix;

	public:
		Matrix4()
		{
			m_matrix = new T * [4];

			for( int i = 0 ; i < 4 ; i++)
			{
				m_matrix[i] = new T[4];

				for (int j = 0 ; j < 4 ; j++)
				{
					m_matrix[i][j] = T( 0);
				}
			}
		}
		virtual ~Matrix4()
		{
			for (int i = 0 ; i < 4 ; i++)
			{
				delete m_matrix[i];
			}

			delete [] m_matrix;
		}
		inline bool operator ==( const Matrix4<T> & p_matrix)
		{
			bool l_bReturn = true;

			for (int i = 0 ; i < 4 && l_bReturn ; i++)
			{
				for (int j = 0 ; j < 4 && l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) < T( 0.00001f);
				}
			}

			return l_bReturn;
		}
		inline bool operator !=( const Matrix4<T> & p_matrix)
		{
			bool l_bReturn = false;

			for (int i = 0 ; i < 4 && ! l_bReturn ; i++)
			{
				for (int j = 0 ; j < 4 && ! l_bReturn ; j++)
				{
					l_bReturn = abs( m_matrix[i][j] - p_matrix.m_matrix[i][j]) >= T( 0.00001f);
				}
			}

			return l_bReturn;
		}
	};
}
}

#endif