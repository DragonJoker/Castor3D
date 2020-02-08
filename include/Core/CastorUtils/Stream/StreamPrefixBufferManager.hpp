/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_PREFIX_BUFFER_CACHE_H___
#define ___CASTOR_STREAM_PREFIX_BUFFER_CACHE_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"
#include "CastorUtils/Config/MultiThreadConfig.hpp"

#include <atomic>

namespace castor
{
	namespace format
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		Holds associated streams and stream buffers
		\~french
		\brief		Garde les associations flux/tampon de flux
		*/
		template< typename prefix_type, typename char_type, typename traits = std::char_traits< char_type > >
		class BasicPrefixBufferManager
		{
		private:
			using bos = std::ios_base;
			using bsb = std::basic_streambuf< char_type, traits >;
			using table_type = std::map< bos *, bsb * >;
			using value_type = typename table_type::value_type;
			using iterator = typename table_type::iterator;
			using const_iterator = typename table_type::const_iterator;
			using lock_type = std::unique_lock< std::mutex >;

			BasicPrefixBufferManager( BasicPrefixBufferManager< prefix_type, char_type, traits > & obj ) = delete;

			/**
			 *\~english
			 *\brief		Default constructor
			 *\~french
			 *\brief		Constructeur par défaut
			 */
			/** Default constructor
			*/
			BasicPrefixBufferManager()
			{
				++sm_instances;
			}

		public:
			/**
			 *\~english
			 *\brief		Destructor
			 *\~french
			 *\brief		Destructeur
			 */
			~BasicPrefixBufferManager()
			{
				--sm_instances;
				lock_type lock{ makeUniqueLock( m_mutex ) };

				for ( auto buffer : m_list )
				{
					delete buffer.second;
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
				return m_list.insert( std::make_pair( &o_s, b_s ) ).second;
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
				return ( m_list.erase( &io_s ) == 1 );
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
			 *\brief		Retrieves an instance of the BasicPrefixBufferManager
			 *\~french
			 *\brief		Récupère une instance du BasicPrefixBufferManager
			 */
			static BasicPrefixBufferManager< prefix_type, char_type, traits > * instance()
			{
				static BasicPrefixBufferManager< prefix_type, char_type, traits > ibm;
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

		template< typename prefix_type, typename char_type, typename traits >
		std::atomic_int BasicPrefixBufferManager< prefix_type, char_type, traits >::sm_instances = 0;
	}
}

#endif
