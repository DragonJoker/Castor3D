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
	*\author	Sylvain Doremus
	*\version	0.8.0
	*\date		10/02/2016
	\~english
	*\brief		Basic signal class
	\~french
	*\brief		Classe basique de signal
	*/
	template< typename Function >
	class Signal
	{
	public:
		/**
		 *\~english
		 *\brief		Connects a new function that will be called if the signal is emitted
		 *\param[in]	p_function	The function
		 *\return		The function index, in order to be able to disconnect it
		 *\~french
		 *\brief		Connecte une nouvelle fonction, qui sera appelée lorsque le signal est émis
		 *\param[in]	p_function	La fonction
		 *\return		L'indice de la fonction, afin de pouvoir la déconnecter
		 */
		uint32_t connect( Function p_function )
		{
			uint32_t l_return = uint32_t( m_slots.size() ) + 1;
			m_slots.insert( std::make_pair( l_return, p_function ) );
			return l_return;
		}
		/**
		 *\~english
		 *\brief		Disconnects a function
		 *\param[in]	p_index		The function index
		 *\~french
		 *\brief		Déconnecte une fonction
		 *\param[in]	p_index		L'indice de la fonction
		 */
		void disconnect( uint32_t p_index )
		{
			auto it = m_slots.find( p_index );

			if ( it != m_slots.end() )
			{
				m_slots.erase( it );
			}
		}
		/**
		 *\~english
		 *\brief		Emits the signal, calls every connected function
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées
		 */
		void operator()()const
		{
			for ( auto it : m_slots )
			{
				it.second();
			}
		}
		/**
		 *\~english
		 *\brief		Emits the signal, calls every connected function
		 *\param[in]	p_params	The functions parameters
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées
		 *\param[in]	p_params	Les paramètres des fonctions
		 */
		template< typename ... Params >
		void operator()( Params && ... p_params )const
		{
			for ( auto it : m_slots )
			{
				it.second( std::forward< Params >( p_params )... );
			}
		}

	private:
		std::map< uint32_t, Function > m_slots;
	};
}

#endif
