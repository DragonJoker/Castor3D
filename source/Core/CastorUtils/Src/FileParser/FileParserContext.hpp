/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FILE_PARSER_CONTEXT_H___
#define ___CASTOR_FILE_PARSER_CONTEXT_H___

#include <deque>
#include <limits>
#include "Data/File.hpp"

#if defined( min )
#	undef min
#endif
#if defined( max )
#	undef max
#endif
#if defined( abs )
#	undef abs
#endif

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		The context used into file parsing functions
	\remark		While parsing a "brace file", the context holds the important data retrieved
	\~french
	\brief		Contexte utilisé dans les fonctions d'analyse
	\remark		Lorsqu'on analyse un fichier, le contexte contient les informations importantes qui ont été récupérées.
	*/
	class FileParserContext
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pFile	The file in parse
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pFile	Le fichier en cours de traitement
		 */
		CU_API explicit FileParserContext( TextFile * p_pFile );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~FileParserContext();
		/**
		 *\~english
		 *\brief		Registers a user context.
		 *\param[in]	p_name	The context name, must be unique.
		 *\param[in]	p_data	The user context data.
		 *\~french
		 *\brief		Enregistre un contexte utilisateur.
		 *\param[in]	p_name	Le nom du contexte, doit être unique.
		 *\param[in]	p_data	Les données du contexte utilisateur.
		 */
		CU_API void registerUserContext( String const & p_name, void * p_data );
		/**
		 *\~english
		 *\brief		Unregisters a user context.
		 *\param[in]	p_name	The context name.
		 *\return		The user context data.
		 *\~french
		 *\brief		Désenregistre un contexte utilisateur.
		 *\param[in]	p_name	Le nom du contexte.
		 *\return		Les données du contexte utilisateur.
		 */
		CU_API void * unregisterUserContext( String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves a user context.
		 *\param[in]	p_name	The context name.
		 *\return		The user context data.
		 *\~french
		 *\brief		Récupère un contexte utilisateur.
		 *\param[in]	p_name	Le nom du contexte.
		 *\return		Les données du contexte utilisateur.
		 */
		CU_API void * getUserContext( String const & p_name );

	public:
		//!\~english The file currently parsed	\~french Le fichier en cours d'analyse
		TextFile * m_file;
		//!\~english The current line	\~french La ligne en cours d'analyse
		unsigned long long m_line;
		//!\~english The sections stack	\~french La pile de sections
		std::deque< uint32_t > m_sections;
		//!\~english The current function name	\~french Le nom de la fonction actuelle
		String m_functionName;
		//!\~english The user context data, useful in plug-ins.	\~french Les données de contexte utilisateur, utile dans les plug-ins.
		std::map< String, void * > m_userContexts;
	};
}

#endif
