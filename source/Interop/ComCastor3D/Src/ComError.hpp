/* See LICENSE file in root folder */
#ifndef __C3DCOM_COM_ERROR_H__
#define __C3DCOM_COM_ERROR_H__

#include "ComCastor3DPrerequisites.hpp"

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		ScriptError class allows to use the COM error mechanism.
	\~french
	\brief		Classe permettant d'utiliser le mecanisme d'erreur COM
	*/
	class CComError
	{
	public:
		/**
		 *\~english
		 *\brief		Throws an exception corresponding to the given Win32 Error code.
		 *\param[in]	dwError			This represents the error.
		 *\param[in]	clsid			This is the GUID of PixelComponents throwing error.
		 *\param[in]	szSource		This is generally displayed as the title.
		 *\param[in]	dwHelpContext	This is the context in the help file.
		 *\param[in]	szHelpFileName	This is the name of the file containing help text.
		 *\~english
		 *\brief		Lance une exception correspondant au code d'erreur Win32 donn�.
		 *\param[in]	dwError			Repr�sente l'erreur.
		 *\param[in]	clsid			Le GUID de l'�l�ment d'o� provient l'erreur.
		 *\param[in]	szSource		Le titre.
		 *\param[in]	dwHelpContext	Le contexte, dans le fichier d'aide.
		 *\param[in]	szHelpFileName	Le nom du fichier contenant l'aide.
		*/
		static HRESULT DispatchWin32Error( DWORD dwError, REFCLSID clsid, LPCTSTR szSource, DWORD dwHelpContext, LPCTSTR szHelpFileName );

		/**
		 *\~english
		 *\brief		Throws the given error code and the message corresponding to the code.
		 *\remarks		If the code is a standard code and no message is provided, then the message is extracted from the system.
		 *\param[in]	hError			This represents the error.
		 *\param[in]	clsid			This is the GUID of PixelComponents throwing error.
		 *\param[in]	szSource		This is generally displayed as the title.
		 *\param[in]	szDescription	This is the description.
		 *\param[in]	dwHelpContext	This is the context in the help file.
		 *\param[in]	szHelpFileName	This is the name of the file containing help text.
		 *\~english
		 *\brief		Lance une exception contenant le code d'erreur ainsi que le message associ�.
		 *\remarks		Si le code est un code standard et qu'aucun message n'est donn�, le message est alors r�cup�r� � partir du syst�me.
		 *\param[in]	hError			Repr�sente l'erreur.
		 *\param[in]	clsid			Le GUID de l'�l�ment d'o� provient l'erreur.
		 *\param[in]	szSource		Le titre.
		 *\param[in]	szDescription	La description.
		 *\param[in]	dwHelpContext	Le contexte, dans le fichier d'aide.
		 *\param[in]	szHelpFileName	Le nom du fichier contenant l'aide.
		*/
		static HRESULT DispatchError( HRESULT hError, REFCLSID clsid, LPCTSTR szSource, LPCTSTR szDescription, DWORD dwHelpContext, LPCTSTR szHelpFileName );
	};
}

#endif
