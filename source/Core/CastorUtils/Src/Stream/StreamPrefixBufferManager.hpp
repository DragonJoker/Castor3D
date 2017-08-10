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
#ifndef ___CASTOR_STREAM_PREFIX_BUFFER_CACHE_H___
#define ___CASTOR_STREAM_PREFIX_BUFFER_CACHE_H___

#include "CastorUtilsPrerequisites.hpp"

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
			typedef std::ios_base bos;
			typedef std::basic_streambuf< char_type, traits > bsb;
			typedef std::map< bos *, bsb * > table_type;
			typedef typename table_type::value_type value_type;
			typedef typename table_type::iterator iterator;
			typedef typename table_type::const_iterator const_iterator;

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
			//!\~english The instance count	\~french Le compte des instances
			static int sm_instances;
			//!\~english The associated elements list	\~french Les éléments associés
			table_type m_list;
		};

		template< typename prefix_type, typename char_type, typename traits >
		int BasicPrefixBufferManager< prefix_type, char_type, traits >::sm_instances = 0;
	}
}

#endif
