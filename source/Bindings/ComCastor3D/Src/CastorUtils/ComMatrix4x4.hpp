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
#ifndef __COMC3D_COM_SQUARE_MATRIX_H__
#define __COMC3D_COM_SQUARE_MATRIX_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <SquareMatrix.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CMatrix object accessible from COM.
	\~french
	\brief		Cette classe définit un CMatrix accessible depuis COM
	*/
	class ATL_NO_VTABLE CMatrix4x4
		: COM_ATL_OBJECT( Matrix4x4 )
		, public Castor::Matrix4x4r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CMatrix4x4();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CMatrix4x4();

		STDMETHOD( Transpose )();
		STDMETHOD( Invert )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Matrix4x4 ), CMatrix4x4 );

	DECLARE_VARIABLE_REF_GETTER( Matrix4x4, Castor, Matrix4x4r );
	DECLARE_VARIABLE_REF_PUTTER( Matrix4x4, Castor, Matrix4x4r );
	DECLARE_VARIABLE_REF_PARAM_GETTER( Matrix4x4, Castor, Matrix4x4r );
}

#endif
