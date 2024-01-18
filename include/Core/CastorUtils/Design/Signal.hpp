/*
See LICENSE file in root folder
*/
#ifndef ___CU_SIGNAL_H___
#define ___CU_SIGNAL_H___

#include "CastorUtils/Design/DesignModule.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

#include "CastorUtils/Exception/Assertion.hpp"

#include <functional>
#include <map>
#include <set>

namespace castor
{
	template< typename MySignalT >
	class ConnectionT
		: public NonCopyable
	{
	private:
		using signal_type = MySignalT;
		using signal_ptr = signal_type *;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		ConnectionT()
			: m_connection{ 0u }
			, m_signal{ nullptr }
		{
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	connection	The connection to the signal.
		 *\param[in]	signal		The signal.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	connection	La connexion au signal.
		 *\param[in]	signal		Le signal.
		 */
		ConnectionT( uint32_t connection, signal_type & signal )
			: m_connection{ connection }
			, m_signal{ &signal }
		{
			signal.addConnection( *this );

#if !defined( NDEBUG )

			StringStream stream{ castor::makeStringStream() };
			stream << Debug::Backtrace{};
			m_stack = stream.str();

#endif
		}
		/**
		 *\~english
		 *\brief			Move constructor.
		 *\param[in,out]	rhs	The object to move.
		 *\~french
		 *\brief			Constructeur par déplacement.
		 *\param[in,out]	rhs	L'objet à déplacer.
		 */
		ConnectionT( ConnectionT< signal_type > && rhs )noexcept
			: m_connection{ rhs.m_connection }
			, m_signal{ rhs.m_signal }
#if !defined( NDEBUG )
			, m_stack{ std::move( rhs.m_stack ) }
#endif
		{
			if ( m_signal )
			{
				m_signal->replaceConnection( rhs, *this );
			}

			rhs.m_signal = nullptr;
			rhs.m_connection = 0u;
		}
		/**
		 *\~english
		 *\brief			Move assignment operator.
		 *\param[in,out]	rhs	The object to move.
		 *\~french
		 *\brief			Opérateur d'affectation par déplacement.
		 *\param[in,out]	rhs	L'objet à déplacer.
		 */
		ConnectionT & operator=( ConnectionT< signal_type > && rhs )noexcept
		{
			if ( &rhs != this )
			{
				disconnect();
				m_connection = rhs.m_connection;
				m_signal = rhs.m_signal;
#if !defined( NDEBUG )
				m_stack = std::move( rhs.m_stack );
#endif

				if ( m_signal )
				{
					m_signal->replaceConnection( rhs, *this );
				}

				rhs.m_signal = nullptr;
				rhs.m_connection = 0u;
			}

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
		~ConnectionT()noexcept
		{
			disconnect();
		}
		/**
		 *\~english
		 *\brief		Disconnects the function from the signal.
		 *\~french
		 *\brief		Déconnecte la fonction du signal.
		 */
		bool disconnect()noexcept
		{
			bool result{ false };

			if ( m_signal && m_connection )
			{
				m_signal->disconnect( m_connection );
				m_signal->removeConnection( *this );
				m_signal = nullptr;
				m_connection = 0u;
				result = true;
			}

			return result;
		}

		operator bool()const noexcept
		{
			return ( m_signal && m_connection );
		}

	private:
		/**
		 *\~english
		 *\brief		Swaps two connections.
		 *\~french
		 *\brief		Echange deux connexions.
		 */
		void swap( ConnectionT & lhs, ConnectionT & rhs )const noexcept
		{
			if ( &rhs != &lhs )
			{
				std::swap( lhs.m_signal, rhs.m_signal );
				std::swap( lhs.m_connection, rhs.m_connection );

#if !defined( NDEBUG )

				std::swap( lhs.m_stack, rhs.m_stack );

#endif
			}
		}

	private:
		//!\~english	The connection identifier.
		//!\~french		L'identifiant de la connection.
		uint32_t m_connection;
		//!\~english	The signal.
		//!\~french		Le signal.
		signal_ptr m_signal;
#if !defined( NDEBUG )
		//!\~english	The creation stack trace.
		//!\~french		La pile d'appels de la création.
		String m_stack;
#endif
	};

	template< typename Function >
	class SignalT
		: public NonCopyable
	{
		friend class ConnectionT< SignalT< Function > >;
		using my_connection = ConnectionT< SignalT< Function > >;
		using my_connection_ptr = my_connection *;

	public:
		using connection = my_connection;

	public:
		SignalT( SignalT && )noexcept = default;
		SignalT & operator=( SignalT && )noexcept = default;
		SignalT() = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects all remaining connections.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte toutes les connexions restantes.
		 */
		~SignalT()noexcept
		{
			// ConnectionT::disconnect appelle SignalT::remConnection, qui
			// supprime la connection de m_connections, invalidant ainsi
			// l'itérateur, donc on ne peut pas utiliser un for_each, ni
			// un range for loop.
			auto it = m_connections.begin();

			while ( it != m_connections.end() )
			{
#if !defined( NDEBUG )
				auto disco = ( *it )->disconnect();
				CU_Require( disco );
#else
				( *it )->disconnect();
#endif
				it = m_connections.begin();
			}
		}
		/**
		 *\~english
		 *\brief		Connects a new function that will be called if the signal is emitted.
		 *\param[in]	function	The function.
		 *\return		The function index, in order to be able to disconnect it.
		 *\~french
		 *\brief		Connecte une nouvelle fonction, qui sera appelée lorsque le signal est émis.
		 *\param[in]	function	La fonction.
		 *\return		L'indice de la fonction, afin de pouvoir la déconnecter.
		 */
		my_connection connect( Function function )
		{
			uint32_t index = 1u;

			if ( !m_slots.empty() )
			{
				index = m_slots.rbegin()->first + 1u;
			}

			m_slots.emplace( index, std::move( function ) );
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
		 *\param[in]	params	The functions parameters.
		 *\~french
		 *\brief		Emet le signal, appelant toutes les fonctions connectées.
		 *\param[in]	params	Les paramètres des fonctions.
		 */
		template< typename ... Params >
		void operator()( Params && ... params )const
		{
			for ( auto it : m_slots )
			{
				it.second( std::forward< Params >( params )... );
			}
		}

	private:
		/**
		 *\~english
		 *\brief		Disconnects a function.
		 *\param[in]	index	The function index.
		 *\~french
		 *\brief		Déconnecte une fonction.
		 *\param[in]	index	L'indice de la fonction.
		 */
		void disconnect( uint32_t index )noexcept
		{
			auto it = m_slots.find( index );

			if ( it != m_slots.end() )
			{
				m_slots.erase( it );
			}
		}
		/**
		 *\~english
		 *\brief		adds a connection to the list.
		 *\param[in]	connection	The connection to add.
		 *\~french
		 *\brief		Ajoute une connexion à la liste.
		 *\param[in]	connection	La connexion à ajouter.
		 */
		void addConnection( my_connection & connection )noexcept
		{
			try
			{
				assert( m_connections.find( &connection ) == m_connections.end() );
				m_connections.emplace( &connection );
			}
			catch ( ... )
			{
				// Nothing to do ?
			}
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	connection	The connection to remove.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	connection	La connexion à enlever.
		 */
		void removeConnection( my_connection & connection )noexcept
		{
			assert( m_connections.find( &connection ) != m_connections.end() );
			m_connections.erase( &connection );
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	oldConnection	The connection to remove.
		 *\param[in]	newConnection	The connection to add.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	oldConnection	La connexion à enlever.
		 *\param[in]	newConnection	La connexion à ajouter.
		 */
		void replaceConnection( my_connection & oldConnection
			, my_connection & newConnection )noexcept
		{
			removeConnection( oldConnection );
			addConnection( newConnection );
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
