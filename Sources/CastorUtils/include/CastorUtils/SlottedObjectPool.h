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
#ifndef ___Castor_SlottedObjectPool___
#define ___Castor_SlottedObjectPool___

#include "Module_Utils.h"

namespace Castor
{ namespace Templates
{
	template <typename T>
	class SlottedObjectPool
	{
	private:
		T *** m_pools;
		T ** m_availableSlots;
		unsigned int * m_poolSizes;
		unsigned int m_numPools;
		unsigned int m_numObjects;
		unsigned int m_numAvailableSlots;

	public:
		SlottedObjectPool()
			:	m_pools				(NULL),
				m_availableSlots	(NULL),
				m_poolSizes			(NULL),
				m_numPools			(0),
				m_numObjects		(0),
				m_numAvailableSlots	(0)
		{}
		~SlottedObjectPool()
		{
			for (unsigned int i = 0 ; i < m_numPools ; i ++)
			{
				for (unsigned int j = 0 ; j < m_poolSizes[i] ; j ++)
				{
					delete m_pools[i][j];
				}

				delete [] m_pools[i];
			}

			delete [] m_poolSizes;
		}

	public:
		inline unsigned short NumAvailable()const	{ return m_numAvailableSlots; }
		inline unsigned short TotalNum()const		{ return m_numObjects; }
		inline void DoubleSize()					{ Allocate( m_numObjects); }

	public:
		T * Get()
		{
			if (m_numAvailableSlots != 0)
			{
				return m_availableSlots[-- m_numAvailableSlots];
			}

			DoubleSize();
			return Get();
		}

		void Release( T * p_ptr)
		{
			if (p_ptr != NULL)
			{
				m_availableSlots[m_numAvailableSlots ++] = p_ptr;
			}
		}

		void Allocate( unsigned int p_numObjects)
		{
			m_numObjects += p_numObjects;

			m_numPools ++;
			m_pools = static_cast <T ***> ( realloc( m_pools, m_numPools * sizeof( T **)));
			m_poolSizes = static_cast <unsigned int *> ( realloc( m_poolSizes, m_numPools * sizeof( unsigned int)));

			T ** l_newPool = new T * [p_numObjects];

			m_pools[m_numPools - 1] = l_newPool;
			m_poolSizes[m_numPools - 1] = p_numObjects;

			m_availableSlots = static_cast <T **> ( realloc( m_availableSlots, m_numObjects * sizeof( T *)));

			for (unsigned int i = 0 ; i < p_numObjects ; i ++)
			{
				l_newPool[i] = new T;
				m_availableSlots[m_numAvailableSlots ++] = l_newPool[i];
			}
		}
	};
}
}

#endif
