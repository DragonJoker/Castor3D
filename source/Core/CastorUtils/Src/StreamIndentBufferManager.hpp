/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_STREAM_INDENT_BUFFER_MANAGER_H___
#define ___CASTOR_STREAM_INDENT_BUFFER_MANAGER_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
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
		template< typename char_type, typename traits = std::char_traits< char_type > >
		class basic_indent_buffer_manager
		{
		private:
			typedef std::ios_base bos;
			typedef std::basic_streambuf< char_type, traits > bsb;
			typedef std::map< bos *, bsb * > table_type;
			typedef typename table_type::value_type value_type;
			typedef typename table_type::iterator iterator;
			typedef typename table_type::const_iterator const_iterator;

			/**
			 *\~english
			 *\brief		Default constructor
			 *\~french
			 *\brief		Constructeur par défaut
			 */
			/** Default constructor
			*/
			basic_indent_buffer_manager()
			{
				++sm_instances;
			}

			/**
			 *\~english
			 *\brief		Copy constructor
			 *\remarks		Not implemented to deactivate it
			 *\~french
			 *\brief		Constructeur par copie
			 *\remarks		Non implémenté afin de le désactiver
			 */
			basic_indent_buffer_manager( basic_indent_buffer_manager< char_type, traits > & obj );

		public:
			/**
			 *\~english
			 *\brief		Destructor
			 *\~french
			 *\brief		Destructeur
			 */
			~basic_indent_buffer_manager()
			{
				--sm_instances;

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
			bsb * get_buffer( std::ios_base & io_s )
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
				delete get_buffer( io_s );
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
			 *\brief		Retrieves an instance of the basic_indent_buffer_manager
			 *\~french
			 *\brief		Récupère une instance du basic_indent_buffer_manager
			 */
			static basic_indent_buffer_manager< char_type, traits > * instance()
			{
				static basic_indent_buffer_manager< char_type, traits > ibm;
				return &ibm;
			}

		private:
			//!\~english The instance count	\~french Le compte des instances
			static int sm_instances;
			//!\~english The associated elements list	\~french Les éléments associés
			table_type m_list;
		};

		template< typename char_type, typename traits >
		int basic_indent_buffer_manager< char_type, traits >::sm_instances = 0;

		typedef basic_indent_buffer_manager< char, std::char_traits< char > > indent_buffer_manager;
		typedef basic_indent_buffer_manager< wchar_t, std::char_traits< wchar_t > > windent_buffer_manager;
	}
}

#endif
