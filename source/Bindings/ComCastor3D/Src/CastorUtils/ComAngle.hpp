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
#ifndef __COMC3D_COM_ANGLE_H__
#define __COMC3D_COM_ANGLE_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Angle.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CAngle object accessible from COM.
	\~french
	\brief		Cette classe définit un CAngle accessible depuis COM
	*/
	class CAngle
		:	COM_ATL_OBJECT( Angle )
		,	public Castor::Angle
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CAngle();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CAngle();

		COM_PROPERTY( Degrees, FLOAT, make_getter( this, &Castor::Angle::degrees ), make_putter( this, &Castor::Angle::degrees ) );
		COM_PROPERTY( Radians, FLOAT, make_getter( this, &Castor::Angle::radians ), make_putter( this, &Castor::Angle::radians ) );
		COM_PROPERTY( Grads, FLOAT, make_getter( this, &Castor::Angle::grads ), make_putter( this, &Castor::Angle::grads ) );

		STDMETHOD( Cos )( /* [out, retval] */ float * pVal );
		STDMETHOD( Sin )( /* [out, retval] */ float * pVal );
		STDMETHOD( Tan )( /* [out, retval] */ float * pVal );
		STDMETHOD( ACos )( /* [in] */ float val );
		STDMETHOD( ASin )( /* [in] */ float val );
		STDMETHOD( ATan )( /* [in] */ float val );
		STDMETHOD( Cosh )( /* [out, retval] */ float * pVal );
		STDMETHOD( Sinh )( /* [out, retval] */ float * pVal );
		STDMETHOD( Tanh )( /* [out, retval] */ float * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Angle ), CAngle );
		
	DECLARE_VARIABLE_REF_GETTER( Angle, Castor, Angle );
	DECLARE_VARIABLE_REF_PUTTER( Angle, Castor, Angle );
}

#endif
