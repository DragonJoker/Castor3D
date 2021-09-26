/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_INDENT_BUFFER_CACHE_H___
#define ___CASTOR_STREAM_INDENT_BUFFER_CACHE_H___

#include "CastorUtils/Stream/StreamModule.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor
{
	namespace format
	{
		template< typename char_type, typename traits >
		class BasicIndentBufferManager
		{
		private:
			using bos = std::ios_base;
			using bsb = std::basic_streambuf< char_type, traits >;
			using table_type = std::map< bos *, bsb * >;
			using value_type = typename table_type::value_type;
			using iterator = typename table_type::iterator;
			using const_iterator = typename table_type::const_iterator;
			using lock_type = std::unique_lock< std::mutex >;
			/**
			 *\~english
			 *\brief		Default constructor
			 *\~french
			 *\brief		Constructeur par défaut
			 */
			/** Default constructor
			*/
			BasicIndentBufferManager()
			{
				++sm_instances;
			}

			/**
			 *\~english
			 *\brief		Copy constructor
			 *\~french
			 *\brief		Constructeur par copie
			 */
			BasicIndentBufferManager( BasicIndentBufferManager< char_type, traits > & obj ) = delete;

		public:
			/**
			 *\~english
			 *\brief		Destructor
			 *\~french
			 *\brief		Destructeur
			 */
			~BasicIndentBufferManager()
			{
				--sm_instances;
				lock_type lock{ makeUniqueLock( m_mutex ) };

				for ( iterator it = m_list.begin(); it != m_list.end(); ++it )
				{
					delete it->second;
				}
			}

			/**
			 *\~english
			 *\brief		Inserts a stream and a stream buffer
			 *\param[in]	o_s	The stream
			 *\param[in]	b_s	The stream buffer
			 *\return		true if the pair wa successfully added
			 *\~french
			 *\brief		Insère une association flux/tampon
			 *\param[in]	o_s	Le flux
			 *\param[in]	b_s	Le tampon
			 *\return		true si la paire a été correctement insérée
			 */
			bool insert( bos & o_s, bsb * b_s )
			{
				lock_type lock{ makeUniqueLock( m_mutex ) };
				return m_list.emplace( &o_s, b_s ).second;
			}

			/**
			 *\~english
			 *\brief		Retrieves the items count
			 *\return		The count
			 *\~french
			 *\brief		Récupère le nombre d'éléments
			 *\return		La valeur
			 */
			size_t size()
			{
				lock_type lock{ makeUniqueLock( m_mutex ) };
				return m_list.size();
			}

			/**
			 *\~english
			 *\brief		Retrieves the stream buffer associated to given stream
			 *\param[in]	io_s	The stream
			 *\~french
			 *\brief		Récupère le tampon associé au flux donné
			 *\param[in]	io_s	Le flux
			 */
			bsb * getBuffer( std::ios_base & io_s )
			{
				lock_type lock{ makeUniqueLock( m_mutex ) };
				const_iterator cb_iter( m_list.find( &io_s ) );

				if ( cb_iter == m_list.end() )
				{
					return nullptr;
				}

				return cb_iter->second;
			}

			/*
			 *\~english
			 *\brief		Erases the stream buffer associated to given stream
			 *\param[in]	io_s	The stream
			 *\return		true if the stream was associated to a stream buffer
			 *\~french
			 *\brief		Efface le tampon associé au flux donné
			 *\param[in]	io_s	Le flux
			 *\return		true si le flux était associé à un tampon
			 */
			bool erase( std::ios_base & io_s )
			{
				delete getBuffer( io_s );
				lock_type lock{ makeUniqueLock( m_mutex ) };
				return ( m_list.erase( &io_s ) == 1u );
			}

			/**
			 *\~english
			 *\brief		Retrieves the instances count
			 *\return		The count
			 *\~french
			 *\brief		Récupère le nompbre d'instances
			 *\return		La valeur
			 */
			static size_t instances()
			{
				return sm_instances;
			}

			/**
			 *\~english
			 *\brief		Retrieves an instance of the BasicIndentBufferManager
			 *\~french
			 *\brief		Récupère une instance du BasicIndentBufferManager
			 */
			static BasicIndentBufferManager< char_type, traits > * instance()
			{
				static BasicIndentBufferManager< char_type, traits > ibm;
				return &ibm;
			}

		private:
			//!\~english	The instance count.
			//!\~french		Le compte des instances.
			static std::atomic_int sm_instances;
			//!\~english	The associated elements list.
			//\~french		Les éléments associés.
			table_type m_list;
			//!\~english	mutex protecting the associated elements list.
			//!\~french		Le mutex protégeant les éléments associés.
			std::mutex m_mutex;
		};

		template< typename char_type, typename traits >
		std::atomic_int BasicIndentBufferManager< char_type, traits >::sm_instances = 0;
	}
}

#endif
