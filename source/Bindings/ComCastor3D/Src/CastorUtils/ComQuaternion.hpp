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
#ifndef __COMC3D_COM_QUATERNION_H__
#define __COMC3D_COM_QUATERNION_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include "ComMatrix4x4.hpp"
#include <Quaternion.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CQuaternion object accessible from COM.
	\~french
	\brief		Cette classe définit un CQuaternion accessible depuis COM
	*/
	class ATL_NO_VTABLE CQuaternion
		: COM_ATL_OBJECT( Quaternion )
		, public Castor::Quaternion
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		COMC3D_API CQuaternion();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		COMC3D_API virtual ~CQuaternion();

		typedef VariablePutter< Castor::Quaternion, Castor::Matrix4x4r const & > MtxPutter;
		COM_PROPERTY( RotationMatrix, IMatrix4x4 *, make_getter( this, &Castor::Quaternion::ToRotationMatrix ), MtxPutter( this, &Castor::Quaternion::FromRotationMatrix ) );

		STDMETHOD( Transform )( /* [in] */ IVector3D * val, /* [out, retval] */ IVector3D ** pVal );
		STDMETHOD( ToAxisAngle )( /* [out] */ IVector3D ** pAxis, /* [out] */ IAngle ** pAngle );
		STDMETHOD( FromAxisAngle )( /* [in] */ IVector3D * axis, /* [in] */ IAngle * angle );
		STDMETHOD( ToAxes )( /* [out] */ IVector3D ** pX, /* [out] */ IVector3D ** pY, /* [out] */ IVector3D ** pZ );
		STDMETHOD( FromAxes )( /* [in] */ IVector3D * x, /* [in] */ IVector3D * y, /* [in] */ IVector3D * z );
		STDMETHOD( GetYaw )( /* [out, retval] */ IAngle ** pVal );
		STDMETHOD( GetPitch )( /* [out, retval] */ IAngle ** pVal );
		STDMETHOD( GetRoll )( /* [out, retval] */ IAngle ** pVal );
		STDMETHOD( GetMagnitude )( /* [out, retval] */ float * pVal );
		STDMETHOD( Conjugate )();
		STDMETHOD( Slerp )( /* [in] */ IQuaternion * quat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat );
		STDMETHOD( Mix )( /* [in] */ IQuaternion * quat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Quaternion ), CQuaternion );

	DECLARE_VARIABLE_REF_GETTER( Quaternion, Castor, Quaternion );
	DECLARE_VARIABLE_REF_PUTTER( Quaternion, Castor, Quaternion );
}

#endif
