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
#ifndef ___C3D_MATRIX_BUFFER_H___
#define ___C3D_MATRIX_BUFFER_H___

#include "CpuBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Matrix buffer representation
	\remark		Holds the matrices of a submesh instance
	\~french
	\brief		Représentation d'un tampon de matrices
	\remark		Contient les matrices d'une instance de submesh
	*/
	class MatrixBuffer
		: public CpuBuffer< real >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur
		 */
		C3D_API MatrixBuffer( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MatrixBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Create();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\remark		Used for instanciation
		 *\param[in]	p_count	Instances count
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\remark		Utilisé pour l'instanciation
		 *\param[in]	p_count	Nombre d'instances
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Bind( uint32_t p_count );

	private:
		using CpuBuffer< real >::Bind;
	};
}

#endif
