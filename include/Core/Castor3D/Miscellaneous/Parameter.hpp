/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Parameters_H___
#define ___C3D_Parameters_H___

#include "MiscellaneousModule.hpp"

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <cstring>

namespace castor3d
{
	template< typename KeyT >
	class ParametersT
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline ParametersT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~ParametersT()
		{
		}
		/**
		 *\~english
		 *\brief		Parses the given text into a parameters list.
		 *\param[in]	text	The source text.
		 *\~french
		 *\brief		Analyse le texte donné en une liste de paramètres.
		 *\param[in]	text	Le texte source.
		 */
		inline void parse( castor::String const & text )
		{
			if constexpr ( std::is_same_v< KeyT, castor::String > )
			{
				castor::StringArray params = castor::string::split( text, cuT( " " ), 0xFFFFFFFF, false );

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
		}
		/**
		 *\~english
		 *\brief		adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	name	The parameter name
		 *\param[in]	values	The parameter values
		 *\param[in]	count	The values count
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	name	Le nom du paramètre
		 *\param[in]	values	Les valeurs du paramètre
		 *\param[in]	count	Le compte des valeurs
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename ValueT >
		inline bool add( KeyT const & name
			, ValueT * values
			, uint32_t count )
		{
			bool result = false;
			ParamNameMapIt it = m_values.find( name );

			if ( it == m_values.end() )
			{
				ByteArray param( sizeof( ValueT ) * count, 0 );
				std::memcpy( &param[0], values, sizeof( ValueT ) * count );
				m_values.emplace( name, param );
				result = true;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		adds a parameters list.
		 *\param[in]	parameters	The parameters list.
		 *\~french
		 *\brief		Ajoute une liste de paramètres.
		 *\param[in]	parameters	La liste de paramètres.
		 */
		inline void add( ParametersT const & parameters )
		{
			for ( auto parameter : parameters.m_values )
			{
				m_values.emplace( parameter.first, parameter.second );
			}
		}
		/**
		 *\~english
		 *\brief		adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	name	The parameter name
		 *\param[in]	values	The parameter values
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	name	Le nom du paramètre
		 *\param[in]	values	Les valeurs du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename ValueT, uint32_t N >
		inline bool add( KeyT const & name
			, ValueT const( &values )[N] )
		{
			return add( name, values, N );
		}
		/**
		 *\~english
		 *\brief		adds a parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	name	The parameter name
		 *\param[in]	value	The parameter value
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	name	Le nom du paramètre
		 *\param[in]	value	La valeur du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		template< typename ValueT >
		inline bool add( KeyT const & name
			, ValueT const & value )
		{
			return add( name, &value, 1 );
		}
		/**
		 *\~english
		 *\brief		adds a string parameter
		 *\remarks		If a parameter with the given name already exists, nothing is done
		 *\param[in]	name	The parameter name
		 *\param[in]	value	The parameter value
		 *\return		\p false if a parameter with the given name already exists
		 *\~french
		 *\brief		Ajoute un paramètre chaîne de caractères
		 *\remarks		Si un paramètre avec le nom donné existe déjà, rien n'est fait
		 *\param[in]	name	Le nom du paramètre
		 *\param[in]	value	La valeur du paramètre
		 *\return		\p false si un paramètre avec le nom donné existe déjà
		 */
		inline bool add( KeyT const & name
			, castor::String const & value )
		{
			bool result = false;
			ParamNameMapIt it = m_values.find( name );

			if ( it == m_values.end() )
			{
				size_t size = sizeof( xchar ) * value.size();
				ByteArray param( size + 1, 0 );
				std::memcpy( &param[0], value.data(), size );
				m_values.emplace( name, param );
				result = true;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Sets a parameter value
		 *\param[in]	name	The parameter name
		 *\param[in]	value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Définit la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 *\param[in]	value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename ValueT >
		inline bool set( KeyT const & name
			, ValueT const & value )
		{
			bool result = false;
			ParamNameMapIt it = m_values.find( name );

			if ( it != m_values.end() )
			{
				std::memcpy( &it->second[0], &value, sizeof( ValueT ) );
				result = true;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	name	The parameter name
		 *\param[out]	values	The parameter values
		 *\param[in]	count	The values available count
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 *\param[out]	values	Les valeurs du paramètre
		 *\param[in]	count	La nombre disponible de valeurs
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename ValueT >
		inline bool get( KeyT const & name
			, ValueT * values
			, uint32_t count )const
		{
			bool result = false;
			ParamNameMapConstIt it = m_values.find( name );

			if ( it != m_values.end() )
			{
				if ( sizeof( ValueT ) * count >= it->second.size() )
				{
					std::memcpy( values, &it->second[0], it->second.size() );
					result = true;
				}
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	name	The parameter name
		 *\param[out]	value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 *\param[out]	value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename ValueT >
		inline bool get( KeyT const & name
			, ValueT & value )const
		{
			return get( name, &value, 1 );
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	name	The parameter name
		 *\param[out]	values	The parameter values
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 *\param[out]	values	Les valeurs du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		template< typename ValueT, uint32_t N >
		inline bool get( KeyT const & name
			, ValueT( &values )[N] )const
		{
			return get( name, values, N );
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	name	The parameter name
		 *\param[out]	value	The parameter value
		 *\return		\p false if there is no parameter with the given name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 *\param[out]	value	La valeur du paramètre
		 *\return		\p false s'il n'y a pas de paramètre avec le nom donné
		 */
		inline bool get( KeyT const & name
			, castor::String & value )const
		{
			bool result = false;
			ParamNameMapConstIt it = m_values.find( name );

			if ( it != m_values.end() )
			{
				value = reinterpret_cast< xchar const * const >( it->second.data() );
				result = true;
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Retrieves a parameter value
		 *\param[in]	name	The parameter name
		 *\~french
		 *\brief		Récupère la valeur d'un paramètre
		 *\param[in]	name	Le nom du paramètre
		 */
		template< typename ValueT >
		inline ValueT get( KeyT const & name )const
		{
			ValueT result;
			this->get( name, result );
			return result;
		}

		size_t size()const
		{
			return m_values.size();
		}

	private:
		CU_DeclareVector( uint8_t, Byte );
		CU_DeclareTemplateMap( KeyT, ByteArray, ParamName );
		ParamNameMap m_values;
	};
}

#endif
