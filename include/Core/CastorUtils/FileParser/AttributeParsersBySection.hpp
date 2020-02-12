/*
See LICENSE file in root folder
*/
#ifndef ___CU_AttributeParsersBySection_H___
#define ___CU_AttributeParsersBySection_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
	 *\~english
	 *\brief		Parser function definition.
	 *\param[in]	parser	The file parser.
	 *\param[in]	params	The params contained in the line.
	 *\return		\p true if a brace is to be opened on next line.
	 *\~french
	 *\brief		Définition d'une fonction d'analyse.
	 *\param[in]	parser	L'analyseur de fichier.
	 *\param[in]	params	Les paramètres contenus dans la ligne.
	 *\return		\p true si une accolade doit être ouverte à la ligne suivante.
	 */
	using ParserFunction = std::function< bool( FileParser * parser, ParserParameterArray const & params ) >;

	struct ParserFunctionAndParams
	{
		ParserFunction m_function;
		ParserParameterArray m_params;
	};

#if defined( CU_CompilerMSVC )

	/**
	\~english
	\brief		Helper class used with MSVC to avoid warning 4503.
	\remark		It forwards few functions and typedefs of the original map.
	\~french
	\brief		Classe d'aide à la disparition du warning 4503 pour MSVC.
	\remark		Elle expose quelques fonctions et types de la map originale.
	*/
	class AttributeParserMap
	{
	private:
		CU_DeclareMap( String, ParserFunctionAndParams, );
		Map m_map;

	public:
		typedef MapIt iterator;
		typedef MapConstIt const_iterator;

		ParserFunctionAndParams & operator []( String const & name )
		{
			MapIt it = m_map.find( name );

			if ( it == m_map.end() )
			{
				m_map.insert( std::make_pair( name, ParserFunctionAndParams() ) );
				it = m_map.find( name );
			}

			return it->second;
		}

		iterator find( String const & name )
		{
			return m_map.find( name );
		}

		const_iterator find( String const & name )const
		{
			return m_map.find( name );
		}

		iterator begin()
		{
			return m_map.begin();
		}

		const_iterator begin()const
		{
			return m_map.begin();
		}

		iterator end()
		{
			return m_map.end();
		}

		const_iterator end()const
		{
			return m_map.end();
		}
	};

#else

	CU_DeclareMap( String, ParserFunctionAndParams, AttributeParser );

#endif

	typedef std::map< uint32_t, AttributeParserMap > AttributeParsersBySection;
}

#endif
