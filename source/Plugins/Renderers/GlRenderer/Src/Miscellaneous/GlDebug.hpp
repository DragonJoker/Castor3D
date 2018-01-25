/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"
#include <string>

#if !defined( CALLBACK )
#	if defined( _WIN32 )
#		define CALLBACK __stdcall
#	else
#		define CALLBACK
#	endif
#endif

namespace gl_renderer
{
	/**
	*\brief
	*	Charge les fonctions de debug disponibles dans certaines extensions OpenGL.
	*\remarks
	*	Les extensions concernées sont : GL_ARB_debug_output et GL_AMDX_debug_output
	*	Il faut que le contexte OpenGL ne soit pas actif
	*/
	void loadDebugFunctions();
	/**
	*\brief
	*	Affecte à la fonction de debug disponible sa callback d'affichage
	*\remarks
	*	Il faut que le contexte OpenGL soit actif
	*/
	void initialiseDebugFunctions();
	/**
	\brief
	*	Callback d'affichage d'une information de debug, avec l'extension ARB
	*\param[in] source
	*	La source de l'info
	*\param[in] type
	*	Le type de l'info
	*\param[in] id
	*	L'id de l'info
	*\param[in] severity
	*	L'importance de l'info
	*\param[in] length
	*	La longueur du message qui suit
	*\param[in] message
	*	Un message informatif
	*\param[in] userParam
	*	Paramètre donné par l'utilisateur
	*/
	static void CALLBACK callbackDebugLog( uint32_t source
		, uint32_t type
		, uint32_t id
		, uint32_t severity
		, int length
		, const char * message
		, void * userParam );
	/**
	*\brief
	*	Callback d'affichage d'une information de debug, avec l'extension AMDX
	*\param[in] id
	*	L'id de l'info
	*\param[in] category
	*	La catégorie de l'info
	*\param[in] severity
	*	L'importance de l'info
	*\param[in] length
	*	La longueur du message qui suit
	*\param[in] message
	*	Un message informatif
	*\param[in] userParam
	*	Paramètre donné par l'utilisateur
	*/
	static void CALLBACK callbackDebugLogAMD( uint32_t id
		, uint32_t category
		, uint32_t severity
		, int length
		, const char * message
		, void * userParam );
}
