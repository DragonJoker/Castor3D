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
#include "Exception/Assertion.hpp"
#include "Miscellaneous/Debug.hpp"

#include <functional>
#include <map>
#include <set>

namespace Castor
{
	/*!
	*\author	Sylvain Doremus
	*\version	0.9.0
	*\date		01/03/2017
	\~english
	*\brief		Connection to a signal.
	\~french
	*\brief		Représente une connexion à un signal.
	*/
	template< typename SignalT >
	class Connection
	{
	private:
		using my_signal = SignalT;
		using my_signal_ptr = my_signal *;
		Connection( Connection< my_signal > const & ) = delete;
		Connection & operator=( Connection< my_signal > const & ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		Connection()
			: m_connection{ 0u }
			, m_signal{ nullptr }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_connection	The connection to the signal.
		 *\param[in]	p_signal		The signal.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_connection	La connexion au signal.
		 *\param[in]	p_signal		Le signal.
		 */
		Connection( uint32_t p_connection, my_signal & p_signal )
			: m_connection{ p_connection }
			, m_signal{ &p_signal }
		{
			p_signal.add_connection( *this );

#if !defined( NDEBUG )

			StringStream l_stream;
			l_stream << Debug::Backtrace{};
			m_stack = l_stream.str();

#endif
		}
		/**
		 *\~english
		 *\brief			Move constructor.
		 *\param[in,out]	p_rhs	The object to move.
		 *\~french
		 *\brief			Constructeur par déplacement.
		 *\param[in,out]	p_rhs	L'objet à déplacer.
		 */
		Connection( Connection< my_signal > && p_rhs )
			: m_connection{ p_rhs.m_connection }
			, m_signal{ p_rhs.m_signal }
#if !defined( NDEBUG )
			, m_stack{ std::move( p_rhs.m_stack ) }
#endif
		{
			p_rhs.m_signal = nullptr;
			p_rhs.m_connection = 0u;

			if ( m_signal )
			{
				m_signal->rem_connection( p_rhs );
				m_signal->add_connection( *this );
			}
		}
		/**
		 *\~english
		 *\brief			Move assignment operator.
		 *\param[in,out]	p_rhs	The object to move.
		 *\~french
		 *\brief			Opérateur d'affectation par déplacement.
		 *\param[in,out]	p_rhs	L'objet à déplacer.
		 */
		Connection & operator=( Connection< my_signal > && p_rhs )
		{
			Connection tmp{ std::move( p_rhs ) };
			swap( *this, tmp );
			m_signal->rem_connection( tmp );
			m_signal->add_connection( *this );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects the function from the signal.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte la fonction du signal.
		 */
		~Connection()
		{
			disconnect();
		}
		/**
		 *\~english
		 *\brief		Disconnects the function from the signal.
		 *\~french
		 *\brief		Déconnecte la fonction du signal.
		 */
		bool disconnect()
		{
			bool l_result{ false };

			if ( m_signal && m_connection )
			{
				m_signal->disconnect( m_connection );
				m_signal->rem_connection( *this );
				m_signal = nullptr;
				m_connection = 0u;
				l_result = true;
			}

			return l_result;
		}

	private:
		/**
		 *\~english
		 *\brief		Swaps two connections.
		 *\~french
		 *\brief		Echange deux connexions.
		 */
		void swap( Connection & p_lhs, Connection & p_rhs )
		{
			if ( &p_rhs != &p_lhs )
			{
				std::swap( p_lhs.m_signal, p_rhs.m_signal );
				std::swap( p_lhs.m_connection, p_rhs.m_connection );

#if !defined( NDEBUG )

				std::swap( p_lhs.m_stack, p_rhs.m_stack );

#endif
			}
		}

	private:
		//!\~english	The connection identifier.
		//!\~french		L'identifiant de la connection.
		uint32_t m_connection;
		//!\~english	The signal.
		//!\~french		Le signal.
		my_signal_ptr m_signal;

#if !defined( NDEBUG )

		//!\~english	The creation stack trace.
		//!\~french		La pile d'appels de la création.
		String m_stack;

#endif
	};
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
		friend class Connection< Signal< Function > >;
		using my_connection = Connection< Signal< Function > >;
		using my_connection_ptr = my_connection *;

	public:
		using connection = my_connection;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects all remaining connections.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte toutes les connexions restantes.
		 */
		~Signal()
		{
			// Connection::disconnect appelle Signal::remConnection, qui
			// supprime la connection de m_connections, invalidant ainsi
			// l'itérateur, donc on ne peut pas utiliser un for_each, ni
			// un range for loop.
			auto l_it = m_connections.begin();

			while ( l_it != m_connections.end() )
			{
				auto l_disco = ( *l_it )->disconnect();
				REQUIRE( l_disco );
				l_it = m_connections.begin();
			}
		}
		/**
		 *\~english
		 *\brief		Connects a new function that will be called if the signal is emitted.
		 *\param[in]	p_function	The function.
		 *\return		The function index, in order to be able to disconnect it.
		 *\~french
		 *\brief		Connecte une nouvelle fonction, qui sera appelée lorsque le signal est émis.
		 *\param[in]	p_function	La fonction.
		 *\return		L'indice de la fonction, afin de pouvoir la déconnecter.
		 */
		my_connection connect( Function p_function )
		{
			uint32_t index = uint32_t( m_slots.size() ) + 1u;
			m_slots.emplace( index, p_function );
			return my_connection{ index, *this };
		}
		/**
		 *\~english
		 *\brief		Emits the signal, calls every connected function.
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées.
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
		 *\brief		Emits the signal, calls every connected function.
		 *\param[in]	p_params	The functions parameters.
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées.
		 *\param[in]	p_params	Les paramètres des fonctions.
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
		/**
		 *\~english
		 *\brief		Disconnects a function.
		 *\param[in]	p_index	The function index.
		 *\~french
		 *\brief		Déconnecte une fonction.
		 *\param[in]	p_index	L'indice de la fonction.
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
		 *\brief		Adds a connection to the list.
		 *\param[in]	p_connection	The connection to add.
		 *\~french
		 *\brief		Ajoute une connexion à la liste.
		 *\param[in]	p_connection	La connexion à ajouter.
		 */
		void add_connection( my_connection & p_connection )
		{
			m_connections.insert( &p_connection );
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	p_connection	The connection to remove.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	p_connection	La connexion à enlever.
		 */
		void rem_connection( my_connection & p_connection )
		{
			assert( m_connections.find( &p_connection ) != m_connections.end() );
			m_connections.erase( &p_connection );
		}

	private:
		//!\~english	The connected functions list.
		//!\~french		La liste des fonctions connectées.
		std::map< uint32_t, Function > m_slots;
		//!\~english	The connections list.
		//!\~french		La liste des connections à ce signal.
		std::set< my_connection_ptr > m_connections;
	};
}

#endif
