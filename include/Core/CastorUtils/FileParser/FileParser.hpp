/*
See LICENSE file in root folder
*/
#ifndef ___CU_FileParser_H___
#define ___CU_FileParser_H___

#include "CastorUtils/FileParser/AttributeParsersBySection.hpp"
#include "CastorUtils/FileParser/ParserParameterTypeException.hpp"

#include "CastorUtils/Log/LogModule.hpp"

namespace castor
{
#define DO_WRITE_PARSER_NAME( funcname )\
	funcname( castor::FileParser * parser, castor::ParserParameterArray const & params )

#define DO_WRITE_PARSER_END( retval )\
		result = retval;

	//!\~english	Define to ease the declaration of a parser.
	//!\~french		Un define pour faciliter la déclaration d'un analyseur.
#define CU_DeclareAttributeParser( funcname )\
	bool DO_WRITE_PARSER_NAME( funcname );

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParser( funcname )\
	bool DO_WRITE_PARSER_NAME( funcname )\
	{\
		bool result = false;\
		castor::FileParserContextSPtr context = parser->getContext();

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParserNmspc( nmspc, funcname )\
	bool nmspc::DO_WRITE_PARSER_NAME( funcname )\
	{\
		bool result = false;\
		castor::FileParserContextSPtr context = parser->getContext();

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePush( section )\
		DO_WRITE_PARSER_END( true )\
		context->m_sections.push_back( uint32_t( section ) );\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttribute()\
		DO_WRITE_PARSER_END( false )\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePop()\
		DO_WRITE_PARSER_END( false )\
		context->m_sections.pop_back();\
		return result;\
	}

	//!\~english	Define to ease the call to FileParser::parseError.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseError.
#define CU_ParsingError( error )\
	parser->parseError( error )

	//!\~english	Define to ease the call to FileParser::parseWarning.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseWarning.
#define CU_ParsingWarning( warning )\
	parser->parseWarning( warning )

	//!\~english	Define to ease creation of a section name.
	//!\~french		Un define pour faciliter la création d'un nom de section.
#define CU_MakeSectionName( a, b, c, d )\
	( (uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 ) | ( uint32_t( c ) << 8 ) | ( uint32_t( d ) << 0 ) )

	class FileParser
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	rootSectionId	The root section id.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	rootSectionId	L'id de la section de root.
		 */
		CU_API FileParser( uint32_t rootSectionId );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	logger			The logger instance.
		 *\param[in]	rootSectionId	The root section id.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	logger			L'instance de logging.
		 *\param[in]	rootSectionId	L'id de la section de root.
		 */
		CU_API FileParser( LoggerInstance & logger
			, uint32_t rootSectionId );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API virtual ~FileParser();
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path	The file access path.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		\p true si tout s'est bien passé.
		 */
		CU_API bool parseFile( Path const & path );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path	The file access path.
		 *\param[in]	content	The file content.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\param[in]	content	Le contenu du fichier.
		 *\return		\p true si tout s'est bien passé.
		 */
		CU_API bool parseFile( Path const & path, String const & content );
		/**
		 *\~english
		 *\brief		Logs an error in the log file.
		 *\param[in]	error	The error text.
		 *\~french
		 *\brief		Log une erreur dans le fichier de log.
		 *\param[in]	error	Le texte de l'erreur.
		 */
		CU_API void parseError( String const & error );
		/**
		 *\~english
		 *\brief		Logs a warning in the log file.
		 *\param[in]	warning	The warning text.
		 *\~french
		 *\brief		Log un avertissement dans le fichier de log.
		 *\param[in]	warning	Le texte de l'avertissement.
		 */
		CU_API void parseWarning( String const & warning );
		/**
		 *\~english
		 *\brief		Tests if the params given to the function will be appropriate.
		 *\param[in]	params		The given parameters.
		 *\param[in]	expected	The expected parameters.
		 *\param[in]	received	The filled parameters.
		 *\return		\p false if \p params doesn't contain all expected parameters types.
		 *\~french
		 *\brief		Vérifie si les paramètres donnés à la fonction correspondent à ceux qu'elle attend.
		 *\param[in]	params		Les paramètres donnés.
		 *\param[in]	expected	Les paramètres attendus.
		 *\param[in]	received	Les paramètres remplis.
		 *\return		\p false si \p params ne contient pas tous les types de paramètres attendus par la fonction.
		 */
		CU_API bool checkParams( String params
			, ParserParameterArray const & expected
			, ParserParameterArray & received );
		/**
		 *\~english
		 *\brief		adds a parser function to the parsers list.
		 *\param[in]	section		The parser section.
		 *\param[in]	name		The parser name.
		 *\param[in]	function	The parser function.
		 *\param[in]	params		The expected parameters.
		 *\~french
		 *\brief		Ajoute une fonction d'analyse à la liste.
		 *\param[in]	section		La section.
		 *\param[in]	name		Le nom de la fonction.
		 *\param[in]	function	La fonction d'analyse.
		 *\param[in]	params		Les paramètres attendus.
		 */
		CU_API void addParser( uint32_t section
			, String const & name
			, ParserFunction function
			, ParserParameterArray && params = ParserParameterArray() );
		/**
		 *\~english
		 *\brief		Tells if the read lines are to be ignored.
		 *\~french
		 *\brief		Dit si les lignes suivantes doivent être ignorées.
		 */
		inline bool isIgnored()const
		{
			return m_ignored;
		}
		/**
		 *\~english
		 *\brief		Increments ignore level.
		 *\~french
		 *\brief		Incrémente le niveau d'ignorés.
		 */
		inline void ignore()
		{
			m_ignored = true;
		}
		/**
		 *\~english
		 *\return		The parser context.
		 *\~french
		 *\return		Le contexte d'analyse.
		 */
		inline FileParserContextSPtr getContext()
		{
			return m_context;
		}

	protected:
		/**
		 *\~english
		 *\brief		Specific initialisation function.
		 *\param[in]	path	The file access path.
		 *\~french
		 *\brief		Initialisation spécifique.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 */
		CU_API virtual void doInitialiseParser( Path const & path ) = 0;
		/**
		 *\~english
		 *\brief		Specific cleanup.
		 *\~french
		 *\brief		Nettoyage spécifique.
		 */
		CU_API virtual void doCleanupParser() = 0;
		/**
		 *\~english
		 *\brief		Function called when current section is out of bounds.
		 *\param[in]	line	The current line.
		 *\return		\p true if a brace is opened after this line, \p false if not.
		 *\~french
		 *\brief		Fonction appelée lorsque la section courante est hors limites.
		 *\param[in]	line	La ligne en cours d'analyse.
		 *\return		\p true si une accolade doit être ouverte à la ligne suivant, \p false sinon.
		 */
		CU_API virtual bool doDelegateParser( String const & line ) = 0;
		/**
		 *\~english
		 *\brief		Function called when no parser is found for the line.
		 *\param[in]	error	The error text.
		 *\return		false if the line must be ignored.
		 *\~french
		 *\brief		Fonction appelée si aucun analyseur n'est trouvé pour traiter la ligne.
		 *\param[in]	error	Le texte de l'erreur.
		 *\return		false si la ligne doit être ignorée.
		 */
		CU_API virtual bool doDiscardParser( String const & error ) = 0;
		/**
		 *\~english
		 *\brief		Function called when file parsing is completed with no error.
		 *\~french
		 *\brief		Fonction appelée si l'analyse est complétée sans erreurs.
		 */
		CU_API virtual void doValidate() = 0;
		/**
		 *\~english
		 *\brief		Gives the name associated to section ID.
		 *\param[in]	section	The section ID.
		 *\return		The name.
		 *\~french
		 *\brief		donne le nom associé à l'ID de section.
		 *\param[in]	section	L'ID de section
		 *\return		Le nom.
		 */
		CU_API virtual String doGetSectionName( uint32_t section ) = 0;

	private:
		bool doParseScriptLine( String & line );
		bool doParseScriptBlockEnd();
		bool doInvokeParser( String & line, AttributeParserMap const & parsers );
		void doEnterBlock();
		void doLeaveBlock();
		bool doIsInIgnoredBlock();
		String doGetSectionsStack();
		void doAddDefine( String const & param );
		void doCheckDefines( String & text );

	private:
		uint32_t m_rootSectionId;
		int m_ignoreLevel;

	protected:
		LoggerInstance & m_logger;
		//!\~english	The map holding the parsers, sorted by section.
		//!\~french		La map de parseurs, triés par section.
		AttributeParsersBySection m_parsers;
		//!\~english	The parser context.
		//!\~french		Le contexte du parseur.
		FileParserContextSPtr m_context;
		//!\~english	Tells the lines parsed are to be ignored.
		//!\~french		Dit que les ligne slues sont ignorées.
		bool m_ignored;
		//!\~english	The defines map.
		//!\~french		La map de defines.
		StrStrMap m_defines;
	};
}

#endif
