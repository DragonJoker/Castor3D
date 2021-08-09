/*
See LICENSE file in root folder
*/
#ifndef ___CU_FileParser_H___
#define ___CU_FileParser_H___

#include "CastorUtils/Design/Signal.hpp"
#include "CastorUtils/FileParser/AttributeParsersBySection.hpp"
#include "CastorUtils/FileParser/ParserParameterTypeException.hpp"

#include "CastorUtils/Log/LogModule.hpp"

namespace castor
{
#define CU_DO_WRITE_PARSER_NAME( funcname )\
	funcname( castor::FileParserContext & context\
		, castor::ParserParameterArray const & params )

#define CU_DO_WRITE_PARSER_CONTENT\
		bool result = false;

#define CU_DO_WRITE_PARSER_END( retval )\
		result = retval;\

	//!\~english	Define to ease the declaration of a parser.
	//!\~french		Un define pour faciliter la déclaration d'un analyseur.
#define CU_DeclareAttributeParser( funcname )\
	bool CU_DO_WRITE_PARSER_NAME( funcname );

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParser( funcname )\
	bool CU_DO_WRITE_PARSER_NAME( funcname )\
	{\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_ImplementAttributeParserNmspc( nmspc, funcname )\
	bool nmspc::CU_DO_WRITE_PARSER_NAME( funcname )\
	{\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePush( section )\
		CU_DO_WRITE_PARSER_END( true )\
		context.pendingSection = castor::SectionId( section );\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttribute()\
		CU_DO_WRITE_PARSER_END( false )\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur.
#define CU_EndAttributePop()\
		CU_DO_WRITE_PARSER_END( false )\
		context.sections.pop_back();\
		return result;\
	}

	//!\~english	Define to ease the implementation of a parser which starts a block.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur débutant un bloc.
#define CU_ImplementAttributeParserBlock( funcname, sectionname )\
	bool CU_DO_WRITE_PARSER_NAME( funcname )\
	{\
		auto sectionName = castor::SectionId( sectionname );\
		CU_DO_WRITE_PARSER_CONTENT

	//!\~english	Define to ease the implementation of a parser which starts a block.
	//!\~french		Un define pour faciliter l'implémentation d'un analyseur débutant un bloc.
#define CU_EndAttributeBlock()\
		CU_DO_WRITE_PARSER_END( false )\
		context.pendingSection = sectionName;\
		return result;\
	}

	//!\~english	Define to ease the call to FileParser::parseError.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseError.
#define CU_ParsingError( error )\
	context.preprocessed->parseError( error )

	//!\~english	Define to ease the call to FileParser::parseWarning.
	//!\~french		Un define pour faciliter l'appel de FileParser::parseWarning.
#define CU_ParsingWarning( warning )\
	context.preprocessed->parseWarning( warning )

	//!\~english	Define to ease creation of a section name.
	//!\~french		Un define pour faciliter la création d'un nom de section.
#define CU_MakeSectionName( a, b, c, d )\
	( (castor::SectionId( a ) << 24 ) | ( castor::SectionId( b ) << 16 ) | ( castor::SectionId( c ) << 8 ) | ( castor::SectionId( d ) << 0 ) )

	class PreprocessedFile
	{
	public:
		struct Action
		{
			Path file;
			int line;
			String name;
			SectionAttributeParsers functions;
			String params;
		};

	public:
		CU_API PreprocessedFile( PreprocessedFile const & rhs ) = default;
		CU_API PreprocessedFile( PreprocessedFile && rhs ) = default;
		CU_API PreprocessedFile & operator=( PreprocessedFile const & rhs );
		CU_API PreprocessedFile & operator=( PreprocessedFile && rhs );

		CU_API PreprocessedFile( FileParser & parser );
		CU_API PreprocessedFile( FileParser & parser
			, FileParserContextSPtr context );

		CU_API void addParser( Path file
			, int line
			, String name
			, SectionAttributeParsers functions
			, String params );
		CU_API bool parse();

		CU_API void parseError( String const & error );
		CU_API void parseWarning( String const & warning );

		FileParserContext & getContext()const
		{
			return *m_context;
		}

		uint32_t getCount()const
		{
			return uint32_t( m_actions.size() );
		}

		using ActionFunc = std::function< void( Action const & ) >;
		using ActionSignal = castor::Signal< ActionFunc >;
		using ActionConnection = castor::Connection< ActionSignal >;

		ActionSignal onAction;

	private:
		bool doCheckParams( String params
			, ParserParameterArray const & expected
			, ParserParameterArray & received );
		String doGetSectionsStack();

	private:
		FileParser & m_parser;
		FileParserContextSPtr m_context;
		std::vector< Action > m_actions;
		Action m_popAction;
	};

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
		CU_API FileParser( SectionId rootSectionId );
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
			, SectionId rootSectionId );
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
		CU_API PreprocessedFile processFile( Path const & path );
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
		CU_API PreprocessedFile processFile( Path const & path
			, String const & content );
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
		CU_API void processFile( Path const & path
			, PreprocessedFile & preprocessed );
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
		CU_API void processFile( Path const & path
			, String const & content
			, PreprocessedFile & preprocessed );
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
		CU_API bool parseFile( Path const & path
			, String const & content );
		/**
		 *\~english
		 *\brief		Logs an error in the log file.
		 *\param[in]	error	The error text.
		 *\~french
		 *\brief		Log une erreur dans le fichier de log.
		 *\param[in]	error	Le texte de l'erreur.
		 */
		CU_API void parseError( String const & functionName
			, int lineIndex
			, String const & error );
		/**
		 *\~english
		 *\brief		Logs a warning in the log file.
		 *\param[in]	warning	The warning text.
		 *\~french
		 *\brief		Log un avertissement dans le fichier de log.
		 *\param[in]	warning	Le texte de l'avertissement.
		 */
		CU_API void parseWarning( String const & functionName
			, int lineIndex
			, String const & warning );
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
		CU_API void addParser( SectionId section
			, String const & name
			, ParserFunction function
			, ParserParameterArray params = ParserParameterArray() );
		/**
		 *\~english
		 *\brief		Tells if the read lines are to be ignored.
		 *\~french
		 *\brief		Dit si les lignes suivantes doivent être ignorées.
		 */
		bool isIgnored()const
		{
			return m_ignored;
		}
		/**
		 *\~english
		 *\brief		Increments ignore level.
		 *\~french
		 *\brief		Incrémente le niveau d'ignorés.
		 */
		void ignore()
		{
			m_ignored = true;
		}

		PreprocessedFile & getPreprocessed()const
		{
			CU_Require( m_preprocessed );
			return *m_preprocessed;
		}

		LoggerInstance & getLogger()const
		{
			return m_logger;
		}

		SectionId getRootSectionId()const
		{
			return m_rootSectionId;
		}

		void validate()
		{
			doValidate();
		}
		
		String getSectionName( SectionId section )const
		{
			return doGetSectionName( section );
		}

		void cleanupParser( PreprocessedFile & preprocessed )
		{
			doCleanupParser( preprocessed );
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
		CU_API virtual FileParserContextSPtr doInitialiseParser( Path const & path ) = 0;
		/**
		 *\~english
		 *\brief		Specific cleanup.
		 *\~french
		 *\brief		Nettoyage spécifique.
		 */
		CU_API virtual void doCleanupParser( PreprocessedFile & preprocessed ) = 0;
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
		CU_API virtual bool doDiscardParser( PreprocessedFile & preprocessed
			, String const & line );
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
		CU_API virtual String doGetSectionName( SectionId section )const = 0;
		CU_API virtual std::unique_ptr< FileParser > doCreateParser()const = 0;

	private:
		bool doParseScriptLine( PreprocessedFile & preprocessed
			, String & line
			, int lineIndex );
		void doParseScriptBlockBegin( PreprocessedFile & preprocessed
			, int lineIndex );
		bool doParseScriptBlockEnd( PreprocessedFile & preprocessed
			, int lineIndex );
		bool doInvokeParser( PreprocessedFile & preprocessed
			, String & line
			, int lineIndex );
		void doEnterBlock( PreprocessedFile & preprocessed
			, int lineIndex );
		void doLeaveBlock( PreprocessedFile & preprocessed
			, int lineIndex );
		bool doIsInIgnoredBlock();
		void doIncludeFile( PreprocessedFile & preprocessed
			, int lineIndex
			, String const & param );
		void doAddDefine( int lineIndex
			, String const & param );
		void doCheckDefines( String & text );

	private:
		SectionId m_rootSectionId;
		int m_ignoreLevel{ 0 };
		Path m_path;
		String m_functionName;

	protected:
		LoggerInstance & m_logger;
		//!\~english	The parser functions.
		//!\~french		Les fonctions du parseur.
		AttributeParsers m_parsers;
		//!\~english	Tells the lines parsed are to be ignored.
		//!\~french		Dit que les ligne slues sont ignorées.
		bool m_ignored{ false };
		//!\~english	The defines map.
		//!\~french		La map de defines.
		StrStrMap m_defines;
		//!\~english	The parser context.
		//!\~french		Le contexte du parseur.
		PreprocessedFile * m_preprocessed{ nullptr };
	};
}

#endif
