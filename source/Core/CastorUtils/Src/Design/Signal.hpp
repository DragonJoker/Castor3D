/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
