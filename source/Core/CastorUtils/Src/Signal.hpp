/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/ )

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_SIGNAL_H___
#define ___CU_SIGNAL_H___

#include "CastorUtilsPrerequisites.hpp"

#include <functional>

namespace Castor
{
	/*!
	 *\author		Sylvain Doremu
	 *\version		0.1.
	 *\date		10/02/201
	 *\brief		Basic signal clas
	*/
	template< typename Function >
	class Signal
	{
	public:
		/** Connects a new function that will be called if the signal is raised
		*\param[in]	p_function		The functio
		 *\return		The function index, in order to be able to disconnect it
		*/
		uint32_t connect( Function p_function )
		{
			uint32_t l_return = uint32_t( m_slots.size() ) + 1;
			m_slots.insert( std::make_pair( l_return, p_function ) );
			return l_return;
		}

		/** Disconnects a function
		*\param[in]	p_index		The function inde
		*/
		void disconnect( uint32_t p_index )
		{
			auto it = m_slots.find( p_index );

			if ( it != m_slots.end() )
			{
				m_slots.erase( it );
			}
		}

		/** Raise the signal, calls every connected function
		*/
		void operator()()const
		{
			for ( auto it = m_slots.begin(); it != m_slots.end(); ++it )
			{
				it->second();
			}
		}

		/** Raise the signal, calls every connected function
		*\param[in]	p_params		The function parameter
		*/
		template< typename ... Params >
		void operator()( Params && ... p_params )const
		{
			for ( auto it = m_slots.begin(); it != m_slots.end(); ++it )
			{
				it->second( std::move( p_params )... );
			}
		}

	private:
		std::map< uint32_t, Function > m_slots;
	};
}

#endif
