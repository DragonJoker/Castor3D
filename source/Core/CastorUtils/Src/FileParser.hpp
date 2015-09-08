/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CASTOR_FILE_PARSER_H___
#define ___CASTOR_FILE_PARSER_H___

#include "ParserParameter.hpp"
#include "ParserParameterTypeException.hpp"

namespace Castor
{
	/**
	 *\~english
	 *\brief		Parser function definition
	 *\param[in]	p_arrayParams	The params contained in the line
	 *\param[in]	p_pContext		The parsing context
	 *\return		\p true if a brace is to be opened on next line
	 *\~french
	 *\brief		Définition d'une fonction d'analyse
	 *\param[in]	p_arrayParams	Les paramètres contenus dans la ligne
	 *\param[in]	p_pContext		Le contexte d'analyse
	 *\return		\p true si une accolade doit être ouverte à la ligne suivante
	 */
	typedef bool ( ParserFunction )( Castor::FileParser * p_pParser, ParserParameterArray const & p_arrayParams );

	//!\~english Pointer over a parser function	\~french Pointeur sur une fonction d'analyse
	typedef ParserFunction * PParserFunction;

	//!\~english Define to ease the declaration of a parser	\~french Un define pour faciliter la déclaration d'un analyseur
#	define DECLARE_ATTRIBUTE_PARSER( X ) bool X( Castor::FileParser * p_pParser, Castor::ParserParameterArray const & p_arrayParams );

	//!\~english Define to ease the implementation of a parser	\~french Un define pour faciliter l'implémentation d'un analyseur
#	define IMPLEMENT_ATTRIBUTE_PARSER( nmspc, X )\
		bool nmspc::X( Castor::FileParser * p_pParser, Castor::ParserParameterArray const & p_arrayParams )\
		{\
			bool l_return = false;\
			FileParserContextSPtr p_pContext = p_pParser->GetContext();\
			try\
			{\
				if( !p_pParser->IsIgnored() )

	//!\~english Define to ease the implementation of a parser	\~french Un define pour faciliter l'implémentation d'un analyseur
#	define END_ATTRIBUTE_PUSH( x )\
			}\
			catch( ParserParameterTypeException p_exc )\
			{\
				PARSING_ERROR( string::from_str( p_exc.what() ) );\
			}\
			l_return = true;\
			p_pContext->stackSections.push( x );\
			return l_return;\
		}

	//!\~english Define to ease the implementation of a parser	\~french Un define pour faciliter l'implémentation d'un analyseur
#	define END_ATTRIBUTE()\
			}\
			catch( ParserParameterTypeException p_exc )\
			{\
				PARSING_ERROR( string::from_str( p_exc.what() ) );\
			}\
			l_return = false;\
			return l_return;\
		}

	//!\~english Define to ease the implementation of a parser	\~french Un define pour faciliter l'implémentation d'un analyseur
#	define END_ATTRIBUTE_POP()\
			}\
			catch( ParserParameterTypeException p_exc )\
			{\
				PARSING_ERROR( string::from_str( p_exc.what() ) );\
			}\
			l_return = false;\
			p_pContext->stackSections.pop();\
			return l_return;\
		}

	//!\~english Define to ease the call to FileParser::ParseError	\~french Un define pour faciliter l'appel de FileParser::ParseError
#	define PARSING_ERROR( p_strError) p_pParser->ParseError( (p_strError) )

	//!\~english Define to ease the call to FileParser::ParseWarning	\~french Un define pour faciliter l'appel de FileParser::ParseWarning
#	define PARSING_WARNING( p_strWarning) p_pParser->ParseWarning( (p_strWarning) )

	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		"Brace file" parser base class
	\~french
	\brief		Classe de base pour les analyseurs de fichier à accolades
	*/
	class FileParser
	{
	private:
		typedef std::pair< PParserFunction, ParserParameterArray > ParserFunctionAndParams;
#if defined( _MSC_VER )
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		20/02/2013
		\~english
		\brief		Helper class used with MSVC to avoid warning 4503
		\remark		It forwards few functions and typedefs of the original map
		\~french
		\brief		Classe d'aide à la disparition du warning 4503 pour MSVC
		\remark		Elle expose quelques fonctions et types de la map originale
		*/
		class AttributeParserMap
		{
		private:
			DECLARE_MAP( String, ParserFunctionAndParams, );
			Map m_map;

		public:
			typedef MapIt iterator;
			typedef MapConstIt const_iterator;

			ParserFunctionAndParams & operator []( String const & p_strName )
			{
				MapIt l_it = m_map.find( p_strName );

				if ( l_it == m_map.end() )
				{
					m_map.insert( std::make_pair( p_strName, ParserFunctionAndParams() ) );
					l_it = m_map.find( p_strName );
				}

				return l_it->second;
			}

			iterator find( String const & p_strName )
			{
				return m_map.find( p_strName );
			}

			const_iterator find( String const & p_strName )const
			{
				return m_map.find( p_strName );
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
		DECLARE_MAP( String, ParserFunctionAndParams, AttributeParser );
#endif

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_iRootSectionNumber	The root section id
		 *\param[in]	p_iSectionCount			The sections count
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_iRootSectionNumber	L'id de la section de root
		 *\param[in]	p_iSectionCount			Le nombre de sections
		 */
		CU_API FileParser( int p_iRootSectionNumber, int p_iSectionCount );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~FileParser();
		/**
		 *\~english
		 *\brief		Parsing function
		 *\param[in]	p_strFileName	The file name
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de traitement
		 *\param[in]	p_strFileName	Le nom du fichier
		 *\return		\p true si tout s'est bien passé
		 */
		CU_API bool ParseFile( String const & p_strFileName );
		/**
		 *\~english
		 *\brief		Parsing function
		 *\param[in]	p_file	The file
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de traitement
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		CU_API bool ParseFile( TextFile & p_file );
		/**
		 *\~english
		 *\brief		Logs an error in the log file
		 *\param[in]	p_strError	The error text
		 *\param[in]	p_pContext	The current parsing context
		 *\~french
		 *\brief		Log une erreur dans le fichier de log
		 *\param[in]	p_strError	Le texte de l'erreur
		 *\param[in]	p_pContext	Le context d'analyse
		 */
		CU_API void ParseError( String const & p_strError );
		/**
		 *\~english
		 *\brief		Logs a warning in the log file
		 *\param[in]	p_strWarning	The warning text
		 *\param[in]	p_pContext		The current parsing context
		 *\~french
		 *\brief		Log un avertissement dans le fichier de log
		 *\param[in]	p_strWarning	Le texte de l'avertissement
		 *\param[in]	p_pContext		Le context d'analyse
		 */
		CU_API void ParseWarning( String const & p_strWarning );
		/**
		 *\~english
		 *\brief		Tests if the params given to the function will be appropriate
		 *\param[in]	p_strParams		The given parameters
		 *\param[in]	p_itBegin		An iterator to the first expected parameter
		 *\param[in]	p_itEnd			An iterator to the end of the expected parameters
		 *\return		\p false if \p p_strParams doesn't contain all expected parameters types
		 *\~french
		 *\brief		Vérifie si les paramètres donnés à la fonction correspondent à ceux qu'elle attend
		 *\param[in]	p_strParams		Les paramètres donnés
		 *\param[in]	p_itBegin		Un itérateur sur le premier des paramètres attendus
		 *\param[in]	p_itEnd			Un itérateur sur la fin des paramètres attendus
		 *\return		\p false si \p p_strParams ne contient pas tous les types de paramètres attendus par la fonction
		 */
		CU_API bool CheckParams( String const & p_strParams, ParserParameterArrayConstIt p_itBegin, ParserParameterArrayConstIt p_itEnd );
		/**
		 *\~english
		 *\brief		Adds a parser function to the parsers list
		 *\param[in]	p_iSection		The parser section
		 *\param[in]	p_strName		The parser name
		 *\param[in]	p_pfnFunction	The parser function
		 *\param[in]	p_iCount		The expected parameters list count
		 *\param[in]	...				The expected parameters list
		 *\~french
		 *\brief		Ajoute une foncction d'analyse à la liste
		 *\param[in]	p_iSection		La section
		 *\param[in]	p_strName		Le nom
		 *\param[in]	p_pfnFunction	La fonction d'analyse
		 *\param[in]	p_iCount		Le compte des paramètres attendus
		 *\param[in]	...				La liste des paramètres attendus
		 */
		CU_API void AddParser( int p_iSection, String const & p_strName, PParserFunction p_pfnFunction, int p_iCount = 0, ... );
		/**
		 *\~english
		 *\brief		Tells if the read lines are to be ignored
		 *\~french
		 *\brief		Dit si les lignes suivantes doivent être ignorées
		 */
		inline bool IsIgnored()const
		{
			return m_bIgnored;
		}
		/**
		 *\~english
		 *\brief		Increments ignore level
		 *\~french
		 *\brief		Incrémente le niveau d'ignorés
		 */
		inline void Ignore()
		{
			m_bIgnored = true;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parser pontext
		 *\return		The context
		 *\~french
		 *\brief		Récupère le contexte d'analyse
		 *\return		Le contexte
		 */
		inline FileParserContextSPtr GetContext()
		{
			return m_pParsingContext;
		}

	protected:
		/**
		 *\~english
		 *\brief		Specific initialisation function
		 *\~french
		 *\brief		Initialisation spécifique
		 */
		CU_API virtual void DoInitialiseParser( Castor::TextFile & p_file ) = 0;
		/**
		 *\~english
		 *\brief		Specific cleanup
		 *\~french
		 *\brief		Nettoyage spécifique
		 */
		CU_API virtual void DoCleanupParser() = 0;
		/**
		 *\~english
		 *\brief		Function called when \p m_iSection is out of bounds
		 *\param[in]	p_strLine	The current line
		 *\return		\p true if a brace is opened after this line, \false if not
		 *\~french
		 *\brief		Fonction appelée lorsque \p m_iSection est hors limites
		 *\param[in]	p_strLine	La ligne en cours d'analyse
		 *\return		\p true si une accolade doit être ouverte à la ligne suivant, \p false sinon
		 */
		CU_API virtual bool DoDelegateParser( String const & p_strLine ) = 0;
		/**
		 *\~english
		 *\brief		Function called when no parser is found for the line
		 *\param[in]	p_strLine	The current line
		 *\~french
		 *\brief		Fonction appelée si aucun analyseur n'est trouvé pour traiter la ligne
		 *\param[in]	p_strLine	La ligne en cours d'analyse
		 */
		CU_API virtual void DoDiscardParser( String const & p_strLine ) = 0;
		/**
		 *\~english
		 *\brief		Function called when file parsing is completed with no error
		 *\~french
		 *\brief		Fonction appelée si l'analyse est complétée sans erreurs
		 */
		CU_API virtual void DoValidate() = 0;

	private:
		bool DoParseScriptLine( String & p_strLine );
		bool DoParseScriptBlockEnd();
		bool DoInvokeParser( Castor::String & p_line, AttributeParserMap const & p_parsers );

	private:
		int m_iRootSectionNumber;
		int m_iSectionCount;
		int m_iIgnoreLevel;

	protected:
		//!\~english The map holding the parsers, sorted by section	\~french La map de parseurs, triés par section
		std::map< int, AttributeParserMap > m_mapParsers;
		//!\~english Th parser context	\~french Le contexte du parseur
		FileParserContextSPtr m_pParsingContext;
		//!\~english Tells the lines parsed are to be ignored	\~french Dit que les ligne slues sont ignorées
		bool m_bIgnored;
	};
}

#endif
