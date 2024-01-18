/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FILE_PARSER_CONTEXT_H___
#define ___CASTOR_FILE_PARSER_CONTEXT_H___

#include "CastorUtils/FileParser/FileParserModule.hpp"
#include "CastorUtils/Log/LogModule.hpp"

#include "CastorUtils/Data/File.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Design/NonCopyable.hpp"

#include <deque>
#include <limits>

namespace castor
{
	struct ContextDeleter
	{
		CU_API virtual ~ContextDeleter()noexcept = default;
		CU_API virtual void destroy( void * data )noexcept = 0;
	};

	using ContextDeleterPtr = std::unique_ptr< ContextDeleter >;

	template< typename ContextT >
	struct ContextDeleterT
		: public ContextDeleter
	{
		using ContextPtr = ContextT *;

		void destroy( void * data )noexcept override
		{
			delete ContextPtr( data );
		}
	};

	template< typename ContextT >
	ContextDeleterPtr makeContextDeleter()
	{
		return std::make_unique< ContextDeleterT< ContextT > >();
	}

	struct BlockContext
	{
		CU_API BlockContext( BlockContext const & rhs ) = delete;
		CU_API BlockContext & operator=( BlockContext const & rhs ) = delete;
		CU_API BlockContext( BlockContext && rhs )noexcept = default;
		CU_API BlockContext & operator=( BlockContext && rhs )noexcept = default;

		BlockContext( void * pcontext
			, ContextDeleterPtr pdtor )
			: context{ pcontext }
			, dtor{ std::move( pdtor ) }
		{
		}

		~BlockContext()noexcept
		{
			if ( dtor && context )
			{
				dtor->destroy( context );
			}
		}

		void * context;
		ContextDeleterPtr dtor;
	};

	class FileParserContext
		: public NonMovable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parser	The parser.
		 *\param[in]	path	The file access path.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parser	L'analyseur.
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
		CU_API virtual ~FileParserContext()noexcept = default;
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
		//!\~english	The context blocks stack.
		//!\~french		La pile de contextes de blocs.
		std::deque< void * > blocks{};
		//!\~english	The section to push on the stack.
		//!\~french		La section à mettre sur la pile.
		SectionId pendingSection{};
		//!\~english	The block context to push on the stack.
		//!\~french		Le contexte de bloc à mettre sur la pile.
		void * pendingBlock{};
		//!\~english	The current function name.
		//!\~french		Le nom de la fonction actuelle.
		String functionName{};
		//!\~english	The user context data, useful in plug-ins.
		//!\~french		Les données de contexte utilisateur, utile dans les plug-ins.
		StringMap< void * > userContexts{};
		//!\~english	The parser.
		//!\~french		Le parseur.
		FileParser * parser{};
		//!\~english	The logger instance.
		//!\~french		L'instance de logger.
		LoggerInstance * logger{};
		//!\~english	The preprocessed file.
		//!\~french		Le fichier prétraité.
		PreprocessedFile * preprocessed{};
		//!\~english	The allocated block contexts.
		//!\~french		Les contextes de bloc alloués.
		std::vector< BlockContext > allocatedBlocks;
	};
}

#endif
