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
		 *\~english
		 *\brief		Lance une exception correspondant au code d'erreur Win32 donné
		*/
		COMC3D_API static HRESULT DispatchWin32Error(
			DWORD dwError,
			REFCLSID clsid,
			LPCTSTR szSource,
			DWORD dwHelpContext,
			LPCTSTR szHelpFileName );

		/**
		 *\~english
		 *\brief		Throws the given error code and the message corresponding to the code.
		 *\remarks		If the code is a standard code and no message is provided, then the message is extracted from the system.
		 *\~english
		 *\brief		Lance une exception contenant le code d'erreur ainsi que le message associé.
		 *\remarks		Si le code est un code standard et qu'aucun message n'est donné, le message est alors récupéré à partir du système.
		*/
		COMC3D_API static HRESULT DispatchError(
			HRESULT hError,
			REFCLSID clsid,
			LPCTSTR szSource,
			LPCTSTR szDescription,
			DWORD dwHelpContext,
			LPCTSTR szHelpFileName );
	};
}

#endif
