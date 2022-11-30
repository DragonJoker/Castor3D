/*
See LICENSE file in root folder
*/
#ifndef ___CU_ThreadSafeSignal_H___
#define ___CU_ThreadSafeSignal_H___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Multithreading/SpinMutex.hpp"

#include <functional>
#include <map>
#include <set>

#pragma warning( push )
#pragma warning( disable:5262 )
#include <mutex>
#pragma warning( pop )

namespace castor
{
	template< typename SignalT >
	class TSConnectionT
	{
	private:
		using my_signal = SignalT;
		using my_signal_ptr = my_signal *;
		TSConnectionT( TSConnectionT< my_signal > const & ) = delete;
		TSConnectionT & operator=( TSConnectionT< my_signal > const & ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		TSConnectionT()
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
		TSConnectionT( uint32_t connection, my_signal & signal )
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
		TSConnectionT( TSConnectionT< my_signal > && rhs )noexcept
			: m_connection{ rhs.m_connection }
			, m_signal{ rhs.m_signal }
#if !defined( NDEBUG )
			, m_stack{ std::move( rhs.m_stack ) }
#endif
		{
			if ( m_signal )
			{
				auto lock( makeUniqueLock( m_signal->m_mutex ) );
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
		TSConnectionT & operator=( TSConnectionT< my_signal > && rhs )noexcept
		{
			if ( &rhs != this )
			{
				auto lhsSignal = m_signal;

				if ( lhsSignal )
				{
					lhsSignal->m_mutex.lock();
				}

				try
				{
					auto rhsSignal = rhs.m_signal;

					if ( rhsSignal )
					{
						rhsSignal->m_mutex.lock();
					}

					try
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
					catch ( ... )
					{
					}

					if ( rhsSignal )
					{
						rhsSignal->m_mutex.unlock();
					}
				}
				catch ( ... )
				{
				}

				if ( lhsSignal )
				{
					lhsSignal->m_mutex.unlock();
				}
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
		~TSConnectionT()noexcept
		{
			if ( m_signal )
			{
				auto lock( makeUniqueLock( m_signal->m_mutex ) );
				disconnect();
			}
			else
			{
				disconnect();
			}
		}
		/**
		 *\~english
		 *\brief		Disconnects the function from the signal.
		 *\~french
		 *\brief		Déconnecte la fonction du signal.
		 */
		bool disconnect()
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

		operator bool()const
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
		void swap( TSConnectionT & lhs, TSConnectionT & rhs )
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
		my_signal_ptr m_signal;

#if !defined( NDEBUG )

		//!\~english	The creation stack trace.
		//!\~french		La pile d'appels de la création.
		String m_stack;

#endif
	};

	template< typename Function >
	class TSSignalT
	{
		friend class TSConnectionT< TSSignalT< Function > >;
		using my_connection = TSConnectionT< TSSignalT< Function > >;
		using my_connection_ptr = my_connection *;

	public:
		using connection = my_connection;

	public:
		TSSignalT( TSSignalT const & ) = delete;
		TSSignalT & operator=( TSSignalT const & ) = delete;
		TSSignalT() = default;

		TSSignalT( TSSignalT && rhs )
		{
			auto rhsLock( makeUniqueLock( rhs.m_mutex ) );
			auto lhsLock( makeUniqueLock( m_mutex ) );
			m_connections = std::move( rhs.m_connections );
			m_slots = std::move( rhs.m_slots );
		}

		TSSignalT & operator=( TSSignalT && rhs )
		{
			auto rhsLock( makeUniqueLock( rhs.m_mutex ) );
			auto lhsLock( makeUniqueLock( m_mutex ) );
			m_connections = std::move( rhs.m_connections );
			m_slots = std::move( rhs.m_slots );

			return *this;
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Disconnects all remaining connections.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Déconnecte toutes les connexions restantes.
		 */
		~TSSignalT()noexcept
		{
			auto lock( makeUniqueLock( m_mutex ) );

			// TSConnectionT::disconnect appelle TSSignalT::remConnection, qui
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
			auto lock( makeUniqueLock( m_mutex ) );
			uint32_t index = 1u;

			if ( !m_slots.empty() )
			{
				index = m_slots.rbegin()->first + 1u;
			}

			m_slots.emplace( index, function );
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
			auto lock( makeUniqueLock( m_mutex ) );

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
			auto lock( makeUniqueLock( m_mutex ) );

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
		void disconnect( uint32_t index )
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
		void addConnection( my_connection & connection )
		{
			assert( m_connections.find( &connection ) == m_connections.end() );
			m_connections.insert( &connection );
		}
		/**
		 *\~english
		 *\brief		Removes a connection from the list.
		 *\param[in]	connection	The connection to remove.
		 *\~french
		 *\brief		Enlève une connexion de la liste.
		 *\param[in]	connection	La connexion à enlever.
		 */
		void removeConnection( my_connection & connection )
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
			, my_connection & newConnection )
		{
			removeConnection( oldConnection );
			addConnection( newConnection );
		}

	private:
		mutable SpinMutex m_mutex;
		//!\~english	The connected functions list.
		//!\~french		La liste des fonctions connectées.
		std::map< uint32_t, Function > m_slots;
		//!\~english	The connections list.
		//!\~french		La liste des connections à ce signal.
		std::set< my_connection_ptr > m_connections;
	};
}

#endif
