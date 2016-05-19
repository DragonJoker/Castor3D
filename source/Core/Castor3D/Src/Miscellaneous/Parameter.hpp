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
#ifndef ___C3D_PARAMETER_H___
#define ___C3D_PARAMETER_H___

#include "Castor3DPrerequisites.hpp"

#include <cstring>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Configuration parameters
	\remark		Pair of a name and binary data
	\~french
	\brief		Paramètres de configuration
	\remark		Couple nom/données binaires
	*/
	class Parameters
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Parameters()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Parameters()
		{
		}
		/**
		 *\~english
		 *\brief		Adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	p_name		The parameter name
		 *\param[in]	p_values	The parameter values
		 *\param[in]	p_count		The values count
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	p_name		Le nom du paramètre
		 *\param[in]	p_values	Les valeurs du paramètre
		 *\param[in]	p_count		Le compte des valeurs
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename T >
		inline bool Add( Castor::String const & p_name, T * p_values, uint32_t p_count )
		{
			bool l_return = false;
			ParamNameMapIt l_it = m_mapParameters.find( p_name );

			if ( l_it == m_mapParameters.end() )
			{
				ByteArray l_param( sizeof( T ) * p_count, 0 );
				std::memcpy( &l_param[0], p_values, sizeof( T ) * p_count );
				m_mapParameters.insert( std::make_pair( p_name, l_param ) );
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	p_name		The parameter name
		 *\param[in]	p_values	The parameter values
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	p_name		Le nom du paramètre
		 *\param[in]	p_values	Les valeurs du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename T, uint32_t N >
		inline bool Add( Castor::String const & p_name, T const( &p_values )[N] )
		{
			return Add( p_name, p_values, N );
		}
		/**
		 *\~english
		 *\brief		Adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename T >
		inline bool Add( Castor::String const & p_name, T const & p_value )
		{
			return Add( p_name, &p_value, 1 );
		}
		/**
		 *\~english
		 *\brief		Adds a string parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre chaîne de caractères
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		inline bool Add( Castor::String const & p_name, Castor::String const & p_value )
		{
			bool l_return = false;
			ParamNameMapIt l_it = m_mapParameters.find( p_name );

			if ( l_it == m_mapParameters.end() )
			{
				size_t l_size = sizeof( xchar ) * p_value.size();
				ByteArray l_param( l_size + 1, 0 );
				std::memcpy( &l_param[0], p_value.data(), l_size );
				m_mapParameters.insert( std::make_pair( p_name, l_param ) );
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Sets a parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[in]	p_value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Définit la valeur d'un paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[in]	p_value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename T >
		inline bool Set( Castor::String const & p_name, T const & p_value )
		{
			bool l_return = false;
			ParamNameMapIt l_it = m_mapParameters.find( p_name );

			if ( l_it != m_mapParameters.end() )
			{
				std::memcpy( &l_it->second[0], &p_value, sizeof( T ) );
				l_return = true;
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	p_name		The parameter name
		 *\param[out]	p_values	The parameter values
		 *\param[in]	p_count		The values available count
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	p_name		Le nom du paramètre
		 *\param[out]	p_values	Ls valeurs du paramètre
		 *\param[in]	p_count		La nombre disponible de valeurs
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename T >
		inline bool Get( Castor::String const & p_name, T * p_values, uint32_t p_count )const
		{
			bool l_return = false;
			ParamNameMapConstIt l_it = m_mapParameters.find( p_name );

			if ( l_it != m_mapParameters.end() )
			{
				if ( sizeof( T ) * p_count >= l_it->second.size() )
				{
					std::memcpy( p_values, &l_it->second[0], l_it->second.size() );
					l_return = true;
				}
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[out]	p_value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[out]	p_value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename T >
		inline bool Get( Castor::String const & p_name, T & p_value )const
		{
			return Get( p_name, &p_value, 1 );
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	p_name		The parameter name
		 *\param[out]	p_values	The parameter values
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	p_name		Le nom du paramètre
		 *\param[out]	p_values	Les valeurs du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename T, uint32_t N >
		inline bool Get( Castor::String const & p_name, T( &p_values )[N] )const
		{
			return Get( p_name, p_values, N );
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	p_name	The parameter name
		 *\param[out]	p_value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	p_name	Le nom du paramètre
		 *\param[out]	p_value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		inline bool Get( Castor::String const & p_name, Castor::String & p_value )const
		{
			bool l_return = false;
			ParamNameMapConstIt l_it = m_mapParameters.find( p_name );

			if ( l_it != m_mapParameters.end() )
			{
				p_value = reinterpret_cast< xchar const * const >( l_it->second.data() );
				l_return = true;
			}

			return l_return;
		}

	private:
		DECLARE_VECTOR( uint8_t, Byte );
		DECLARE_MAP( Castor::String, ByteArray, ParamName );
		ParamNameMap m_mapParameters;
	};
}

#endif
