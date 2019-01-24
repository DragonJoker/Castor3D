/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PARAMETER_H___
#define ___C3D_PARAMETER_H___

#include "Castor3DPrerequisites.hpp"

#include <cstring>

namespace castor3d
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
		 *\brief		Parses the given text into a parameters list.
		 *\param[in]	p_text	The source text.
		 *\~french
		 *\brief		Analyse le texte donné en une liste de paramètres.
		 *\param[in]	p_text	Le texte source.
		 */
		C3D_API void parse( castor::String const & p_text )
		{
			castor::StringArray params = castor::string::split( p_text, cuT( " " ), 0xFFFFFFFF, false );

			for ( auto param : params )
			{
				if ( param.find( cuT( "-" ) ) != castor::String::npos )
				{
					param = param.substr( 1 );
				}

				castor::StringArray paramNameValue = castor::string::split( param, cuT( "=" ), 2, false );

				if ( paramNameValue.size() > 1 )
				{
					add( paramNameValue[0], paramNameValue[1] );
				}
				else
				{
					add( param, true );
				}
			}
		}
		/**
		 *\~english
		 *\brief		adds a parameter
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
		inline bool add( castor::String const & p_name, T * p_values, uint32_t p_count )
		{
			bool result = false;
			ParamNameMapIt it = m_mapParameters.find( p_name );

			if ( it == m_mapParameters.end() )
			{
				ByteArray param( sizeof( T ) * p_count, 0 );
				std::memcpy( &param[0], p_values, sizeof( T ) * p_count );
				m_mapParameters.insert( std::make_pair( p_name, param ) );
				result = true;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		adds a parameters list.
		 *\param[in]	p_parameters	The parameters list.
		 *\~french
		 *\brief		Ajoute une liste de paramètres.
		 *\param[in]	p_parameters	La liste de paramètres.
		 */
		inline void add( Parameters const & p_parameters )
		{
			for ( auto parameter : p_parameters.m_mapParameters )
			{
				m_mapParameters.insert( { parameter.first, parameter.second } );
			}
		}
		/**
		 *\~english
		 *\brief		adds a parameter
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
		inline bool add( castor::String const & p_name, T const( &p_values )[N] )
		{
			return add( p_name, p_values, N );
		}
		/**
		 *\~english
		 *\brief		adds a parameter
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
		inline bool add( castor::String const & p_name, T const & p_value )
		{
			return add( p_name, &p_value, 1 );
		}
		/**
		 *\~english
		 *\brief		adds a string parameter
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
		inline bool add( castor::String const & p_name, castor::String const & p_value )
		{
			bool result = false;
			ParamNameMapIt it = m_mapParameters.find( p_name );

			if ( it == m_mapParameters.end() )
			{
				size_t size = sizeof( xchar ) * p_value.size();
				ByteArray param( size + 1, 0 );
				std::memcpy( &param[0], p_value.data(), size );
				m_mapParameters.insert( std::make_pair( p_name, param ) );
				result = true;
			}

			return result;
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
		inline bool set( castor::String const & p_name, T const & p_value )
		{
			bool result = false;
			ParamNameMapIt it = m_mapParameters.find( p_name );

			if ( it != m_mapParameters.end() )
			{
				std::memcpy( &it->second[0], &p_value, sizeof( T ) );
				result = true;
			}

			return result;
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
		inline bool get( castor::String const & p_name, T * p_values, uint32_t p_count )const
		{
			bool result = false;
			ParamNameMapConstIt it = m_mapParameters.find( p_name );

			if ( it != m_mapParameters.end() )
			{
				if ( sizeof( T ) * p_count >= it->second.size() )
				{
					std::memcpy( p_values, &it->second[0], it->second.size() );
					result = true;
				}
			}

			return result;
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
		inline bool get( castor::String const & p_name, T & p_value )const
		{
			return get( p_name, &p_value, 1 );
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
		inline bool get( castor::String const & p_name, T( &p_values )[N] )const
		{
			return get( p_name, p_values, N );
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
		inline bool get( castor::String const & p_name, castor::String & p_value )const
		{
			bool result = false;
			ParamNameMapConstIt it = m_mapParameters.find( p_name );

			if ( it != m_mapParameters.end() )
			{
				p_value = reinterpret_cast< xchar const * const >( it->second.data() );
				result = true;
			}

			return result;
		}

	private:
		CU_DeclareVector( uint8_t, Byte );
		CU_DeclareMap( castor::String, ByteArray, ParamName );
		ParamNameMap m_mapParameters;
	};
}

#endif
