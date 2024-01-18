/*
See LICENSE file in root folder
*/
#ifndef ___CU_FileParser_H___
#define ___CU_FileParser_H___

#include "CastorUtils/Design/Signal.hpp"
#include "CastorUtils/FileParser/AttributeParsersBySection.hpp"
#include "CastorUtils/FileParser/ParserParameter.hpp"

#include "CastorUtils/Log/LogModule.hpp"

namespace castor
{
	class PreprocessedFile
	{
	public:
		struct Action
		{
			Action( Path file = {}
				, uint64_t line = {}
				, String name = {}
				, uint32_t section = {}
				, ParserFunctionAndParams function = {}
				, String params = {}
				, bool implicit = {} )
				: file{ std::move( file ) }
				, line{ line }
				, name{ std::move( name ) }
				, section{ section }
				, function{ std::move( function ) }
				, params{ std::move( params ) }
				, implicit{ implicit }
			{
			}

			Path file{};
			uint64_t line{};
			String name{};
			uint32_t section{};
			ParserFunctionAndParams function{};
			String params{};
			bool implicit{};
		};

	public:
		CU_API PreprocessedFile( PreprocessedFile const & rhs ) = delete;
		CU_API PreprocessedFile & operator=( PreprocessedFile const & rhs ) = delete;
		CU_API PreprocessedFile( PreprocessedFile && rhs )noexcept;
		CU_API PreprocessedFile & operator=( PreprocessedFile && rhs )noexcept;
		CU_API virtual ~PreprocessedFile()noexcept = default;

		CU_API explicit PreprocessedFile( FileParser & parser );
		CU_API PreprocessedFile( FileParser & parser
			, FileParserContextUPtr context );

		CU_API void addParserAction( Path file
			, uint64_t line
			, String name
			, SectionId section
			, ParserFunctionAndParams function
			, String params
			, bool implicit );
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

		using ActionFunc = std::function< void( SectionId, Action const & ) >;
		using ActionSignal = SignalT< ActionFunc >;
		using ActionConnection = ConnectionT< ActionSignal >;

		ActionSignal onAction;

	protected:
		CU_API virtual bool doIsEmpty()const;
		CU_API virtual void doAddParserAction( Path file
			, uint64_t line
			, String name
			, SectionId section
			, ParserFunctionAndParams function
			, String params
			, bool implicit );
		CU_API virtual Action * doGetFirstAction();
		CU_API virtual Action * doGetNextAction();

	private:
		bool doCheckParams( String params
			, ParserParameterArray const & expected
			, ParserParameterArray & received );
		String doGetSectionsStack();

	protected:
		FileParser & m_parser;
		FileParserContextUPtr m_context;
		std::vector< Action > m_actions;
		std::vector< Action >::iterator m_current;
		Action m_popAction;
	};

	class FileParser
		: public NonMovable
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
		CU_API explicit FileParser( SectionId rootSectionId
			, void * rootBlockContext = nullptr );
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
			, SectionId rootSectionId
			, void * rootBlockContext = nullptr );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API virtual ~FileParser()noexcept = default;
		/**
		 *\~english
		 *\brief		Registers additional parsers.
		 *\param[in]	name		The registering name.
		 *\param[in]	parsers		The parsers.
		 *\~french
		 *\brief		Enregistre des analyseurs supplémentaires.
		 *\param[in]	name		Le nom d'enregistrement.
		 *\param[in]	parsers		Les analyseurs.
		 */
		CU_API void registerParsers( castor::String const & name
			, AdditionalParsers const & parsers );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	appName			The application name, if a dezip is needed.
		 *\param[in]	path			The file access path.
		 *\param[in]	preprocessed	The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	appName			Le nom de l'application, si un dézip est nécessaire.
		 *\param[in]	path			Le chemin d'accès au fichier.
		 *\param[in]	preprocessed	Le fichier pré-traité.
		 */
		CU_API void processFile( String const & appName
			, Path path
			, PreprocessedFile & preprocessed );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path			The file access path.
		 *\param[in]	preprocessed	The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path			Le chemin d'accès au fichier.
		 *\param[in]	preprocessed	Le fichier pré-traité.
		 */
		CU_API void processFile( Path const & path
			, PreprocessedFile & preprocessed );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path			The file access path.
		 *\param[in]	content			The file content.
		 *\param[in]	preprocessed	The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path			Le chemin d'accès au fichier.
		 *\param[in]	content			Le contenu du fichier.
		 *\param[in]	preprocessed	Le fichier pré-traité.
		 */
		CU_API void processFile( Path const & path
			, String const & content
			, PreprocessedFile & preprocessed );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	appName	The application name, if a dezip is needed.
		 *\param[in]	path	The file access path.
		 *\return		The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	appName	Le nom de l'application, si un dézip est nécessaire.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		Le fichier pré-traité.
		 */
		CU_API PreprocessedFile processFile( String const & appName
			, Path const & path );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path	The file access path.
		 *\return		The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		Le fichier pré-traité.
		 */
		CU_API PreprocessedFile processFile( Path const & path );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	path	The file access path.
		 *\param[in]	content	The file content.
		 *\return		The preprocessed file.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\param[in]	content	Le contenu du fichier.
		 *\return		Le fichier pré-traité.
		 */
		CU_API PreprocessedFile processFile( Path const & path
			, String const & content );
		/**
		 *\~english
		 *\brief		Parsing function.
		 *\param[in]	appName	The application name, if a dezip is needed.
		 *\param[in]	path	The file access path.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de traitement.
		 *\param[in]	appName	Le nom de l'application, si un dézip est nécessaire.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 *\return		\p true si tout s'est bien passé.
		 */
		CU_API bool parseFile( String const & appName
			, Path const & path );
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
		 *\param[in]	functionName	The error function.
		 *\param[in]	lineIndex		The error line.
		 *\param[in]	error			The error text.
		 *\~french
		 *\brief		Log une erreur dans le fichier de log.
		 *\param[in]	functionName	La fonction de l'erreur.
		 *\param[in]	lineIndex		La ligne de l'erreur.
		 *\param[in]	error	Le texte de l'erreur.
		 */
		CU_API void parseError( String const & functionName
			, uint64_t lineIndex
			, String const & error );
		/**
		 *\~english
		 *\brief		Logs a warning in the log file.
		 *\param[in]	functionName	The warning function.
		 *\param[in]	lineIndex		The warning line.
		 *\param[in]	warning			The warning text.
		 *\~french
		 *\brief		Log un avertissement dans le fichier de log.
		 *\param[in]	functionName	La fonction de l'avertissement.
		 *\param[in]	lineIndex		La ligne de l'avertissement.
		 *\param[in]	warning			Le texte de l'avertissement.
		 */
		CU_API void parseWarning( String const & functionName
			, uint64_t lineIndex
			, String const & warning );
		/**
		 *\~english
		 *\brief		adds a parser function to the parsers list.
		 *\param[in]	oldSection	The parser section onto which the function is applied.
		 *\param[in]	newSection	The parser section resulting of the function application.
		 *\param[in]	name		The parser name.
		 *\param[in]	function	The parser function.
		 *\param[in]	params		The expected parameters.
		 *\~french
		 *\brief		Ajoute une fonction d'analyse à la liste.
		 *\param[in]	oldSection	La section sur laquelle la fonction est appliquée.
		 *\param[in]	newSection	La section résultant de l'application de la fonction.
		 *\param[in]	name		Le nom de la fonction.
		 *\param[in]	function	La fonction d'analyse.
		 *\param[in]	params		Les paramètres attendus.
		 */
		CU_API void addParser( SectionId oldSection
			, SectionId newSection
			, String const & name
			, ParserFunction const & function
			, ParserParameterArray const & params = ParserParameterArray() );
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
			, ParserFunction const & function
			, ParserParameterArray const & params = ParserParameterArray() );
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

		LoggerInstance & getLogger()const
		{
			return m_logger;
		}

		SectionId getRootSectionId()const
		{
			return m_rootSectionId;
		}

		void * getRootBlockContext()const
		{
			return m_rootBlockContext;
		}

		void validate( PreprocessedFile & preprocessed )
		{
			doValidate( preprocessed );
		}
		
		String getSectionName( SectionId section )const
		{
			return doGetSectionName( section );
		}

		void cleanupParser( PreprocessedFile & preprocessed )
		{
			doCleanupParser( preprocessed );
		}

		auto const & getAdditionalParsers()const
		{
			return m_additionalParsers;
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
		CU_API virtual FileParserContextUPtr doInitialiseParser( Path const & path );
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
		 *\param[in]	preprocessed	The preprocessed file.
		 *\param[in]	line			The line.
		 *\return		false if the line must be ignored.
		 *\~french
		 *\brief		Fonction appelée si aucun analyseur n'est trouvé pour traiter la ligne.
		 *\param[in]	preprocessed	Le fichier pré-traité.
		 *\param[in]	line			La ligne.
		 *\return		false si la ligne doit être ignorée.
		 */
		CU_API virtual bool doDiscardParser( PreprocessedFile & preprocessed
			, StringView line );
		/**
		 *\~english
		 *\brief		Function called when file parsing is completed with no error.
		 *\~french
		 *\brief		Fonction appelée si l'analyse est complétée sans erreurs.
		 */
		CU_API virtual void doValidate( PreprocessedFile & preprocessed ) = 0;
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
		void doProcessNoBlockLine( StringView curLine
			, PreprocessedFile & preprocessed
			, std::vector< StringView > const & work
			, std::vector< StringView > const & nextWork
			, uint64_t lineIndex
			, SectionId & pendingSection
			, bool & isNextOpenBrace );
		void doProcessLine( StringView curLine
			, PreprocessedFile & preprocessed
			, std::vector< StringView > const & work
			, std::vector< StringView > const & nextWork
			, uint64_t lineIndex
			, SectionId & pendingSection
			, bool & commented
			, bool & isNextOpenBrace );
		void doParseScriptBlockBegin( PreprocessedFile & preprocessed
			, uint32_t newSection
			, uint64_t lineIndex
			, bool implicit );
		bool doParseScriptBlockEnd( PreprocessedFile & preprocessed
			, uint64_t lineIndex
			, bool implicit );
		std::pair< bool, SectionId > doInvokeParser( PreprocessedFile & preprocessed
			, StringView line
			, StringView nextToken
			, uint64_t lineIndex );
		void doEnterBlock( PreprocessedFile & preprocessed
			, SectionId newSection
			, uint64_t lineIndex
			, bool implicit );
		void doLeaveBlock( PreprocessedFile & preprocessed
			, uint64_t lineIndex
			, bool implicit );
		bool doIsInIgnoredBlock()const;
		void doIncludeFile( PreprocessedFile & preprocessed
			, uint64_t lineIndex
			, StringView param );
		void doAddDefine( uint64_t lineIndex
			, StringView param );
		void doCheckDefines( String & text )const;

	private:
		SectionId m_rootSectionId{};
		void * m_rootBlockContext{};
		int m_ignoreLevel{ 0 };
		Path m_path;
		String m_fileName;
		StringMap< AdditionalParsers > m_additionalParsers;
		std::deque< SectionId > m_sections{};

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
	};
}

#endif
