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
#ifndef ___CU_LOG_TYPE_H___
#define ___CU_LOG_TYPE_H___

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Defines the various log types
	\~french
	\brief		Définit les différents types de log
	*/
	typedef enum ELogType
		: uint8_t
	{
		//!\~english Debug type log	\~french Log de type debug
		ELogType_DEBUG,
		//!\~english Information type log	\~french Log de type information
		ELogType_INFO,
		//!\~english Warning type log	\~french Log de type avertissement
		ELogType_WARNING,
		//!\~english Error type log	\~french Log de type erreur
		ELogType_ERROR,
		//!\~english Log types count	\~french Compte des logs
		ELogType_COUNT,
	}	ELogType;
}

#endif
