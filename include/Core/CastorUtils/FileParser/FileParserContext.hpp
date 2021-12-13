/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FILE_PARSER_CONTEXT_H___
#define ___CASTOR_FILE_PARSER_CONTEXT_H___

#include "CastorUtils/FileParser/FileParserModule.hpp"
#include "CastorUtils/Log/LogModule.hpp"

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Data/Path.hpp"

#include <deque>
#include <limits>

namespace castor
{
	class FileParserContext
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	logger	The logger instance.
		 *\param[in]	path	The file access path.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	logger	L'instance de logger.
		 *\param[in]	path	Le chemin d'accès au fichier.
		 */
		CU_API explicit FileParserContext( FileParser & parser
			, Path const & path );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API virtual ~FileParserContext() = default;
		/**
		 *\~english
		 *\brief		Registers a user context.
		 *\param[in]	name	The context name, must be unique.
		 *\param[in]	data	The user context data.
		 *\~french
		 *\brief		Enregistre un contexte utilisateur.
		 *\param[in]	name	Le nom du contexte, doit être unique.
		 *\param[in]	data	Les données du contexte utilisateur.
		 */
		CU_API void registerUserContext( String const & name, void * data );
		/**
		 *\~english
		 *\brief		Unregisters a user context.
		 *\param[in]	name	The context name.
		 *\return		The user context data.
		 *\~french
		 *\brief		Désenregistre un contexte utilisateur.
		 *\param[in]	name	Le nom du contexte.
		 *\return		Les données du contexte utilisateur.
		 */
		CU_API void * unregisterUserContext( String const & name );
		/**
		 *\~english
		 *\brief		Retrieves a user context.
		 *\param[in]	name	The context name.
		 *\return		The user context data.
		 *\~french
		 *\brief		Récupère un contexte utilisateur.
		 *\param[in]	name	Le nom du contexte.
		 *\return		Les données du contexte utilisateur.
		 */
		CU_API bool hasUserContext( String const & name );
		/**
		 *\~english
		 *\brief		Retrieves a user context.
		 *\param[in]	name	The context name.
		 *\return		The user context data.
		 *\~french
		 *\brief		Récupère un contexte utilisateur.
		 *\param[in]	name	Le nom du contexte.
		 *\return		Les données du contexte utilisateur.
		 */
		CU_API void * getUserContext( String const & name );

	public:
		//!\~english	The file currently parsed.
		//!\~french		Le fichier en cours d'analyse.
		Path file{};
		//!\~english	The current line.
		//!\~french		La ligne en cours d'analyse.
		unsigned long long line{};
		//!\~english	The sections stack.
		//!\~french		La pile de sections.
		std::deque< SectionId > sections{};
		//!\~english	The sections stack.
		//!\~french		La pile de sections.
		SectionId pendingSection{};
		//!\~english	The current function name.
		//!\~french		Le nom de la fonction actuelle.
		String functionName{};
		//!\~english	The user context data, useful in plug-ins.
		//!\~french		Les données de contexte utilisateur, utile dans les plug-ins.
		std::map< String, void * > userContexts{};
		//!\~english	The parser.
		//!\~french		Le parseur.
		FileParser * parser{};
		//!\~english	The logger instance.
		//!\~french		L'instance de logger.
		LoggerInstance * logger{};
		PreprocessedFile * preprocessed{};
	};
}

#endif
