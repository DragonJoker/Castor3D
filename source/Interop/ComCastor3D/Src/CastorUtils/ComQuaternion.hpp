/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_QUATERNION_H__
#define __COMC3D_COM_QUATERNION_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include "ComMatrix4x4.hpp"
#include <Math/Quaternion.hpp>

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
		, public castor::Quaternion
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CQuaternion();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CQuaternion();

		typedef VariablePutter< castor::Quaternion, castor::Matrix4x4r const & > MtxPutter;
		COM_PROPERTY( RotationMatrix, IMatrix4x4 *, make_getter( this, &castor::Quaternion::toMatrix ), MtxPutter( this, &castor::Quaternion::fromMatrix ) );

		STDMETHOD( Transform )( /* [in] */ IVector3D * val, /* [out, retval] */ IVector3D ** pVal );
		STDMETHOD( ToAxisAngle )( /* [out] */ IVector3D ** pAxis, /* [out] */ IAngle ** pAngle );
		STDMETHOD( FromAxisAngle )( /* [in] */ IVector3D * axis, /* [in] */ IAngle * angle );
		STDMETHOD( ToAxes )( /* [out] */ IVector3D ** pX, /* [out] */ IVector3D ** pY, /* [out] */ IVector3D ** pZ );
		STDMETHOD( FromAxes )( /* [in] */ IVector3D * x, /* [in] */ IVector3D * y, /* [in] */ IVector3D * z );
		STDMETHOD( getMagnitude )( /* [out, retval] */ float * pVal );
		STDMETHOD( Conjugate )();
		STDMETHOD( Slerp )( /* [in] */ IQuaternion * quat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat );
		STDMETHOD( Mix )( /* [in] */ IQuaternion * quat, /* [in] */ float percent, /* [out, retval] */ IQuaternion ** pQuat );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Quaternion ), CQuaternion );

	DECLARE_VARIABLE_REF_GETTER( Quaternion, castor, Quaternion );
	DECLARE_VARIABLE_REF_PUTTER( Quaternion, castor, Quaternion );
}

#endif
