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
#ifndef ___CU_HDR_LOADER_H___
#define ___CU_HDR_LOADER_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		15/03/2016
	\~english
	\brief		HDR image loader.
	\~french
	\brief		Loader d'images HDR.
	*/
	class HdrLoader
	{
	public:
		/**
		 *\~english
		 *\brief		Loads the image at given file path.
		 *\param[in]	p_fileName	The image file path.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Charge l'image au chemin donné.
		 *\param[in]	p_fileName	Le chemin d'accès à l'image.
		 *\return		Le tampon de l'image.
		 */
		static PxBufferBaseSPtr Load( Path const & p_fileName );
	};
}

#endif
