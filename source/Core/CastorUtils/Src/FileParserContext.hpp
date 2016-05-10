/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CASTOR_FILE_PARSER_CONTEXT_H___
#define ___CASTOR_FILE_PARSER_CONTEXT_H___

#include <deque>
#include <limits>
#include "File.hpp"

#if defined( min )
#	undef min
#endif
#if defined( max )
#	undef max
#endif
#if defined( abs )
#	undef abs
#endif

namespace Castor
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
		CU_API FileParserContext( TextFile * p_pFile );
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
		CU_API void RegisterUserContext( String const & p_name, void * p_data );
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
		CU_API void * UnregisterUserContext( String const & p_name );
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
		CU_API void * GetUserContext( String const & p_name );

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
