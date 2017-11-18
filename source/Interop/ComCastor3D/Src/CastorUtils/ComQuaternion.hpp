/* See LICENSE file in root folder */
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
		COM_PROPERTY_GET( RotationMatrix, IMatrix4x4 *, makeGetter( this, &castor::Quaternion::toMatrix ) );

		STDMETHOD( Transform )( /* [in] */ IVector3D * val, /* [out, retval] */ IVector3D ** pVal );
		STDMETHOD( ToAxisAngle )( /* [out] */ IVector3D ** pAxis, /* [out] */ IAngle ** pAngle );
		STDMETHOD( FromAxisAngle )( /* [in] */ IVector3D * axis, /* [in] */ IAngle * angle );
		STDMETHOD( ToAxes )( /* [out] */ IVector3D ** pX, /* [out] */ IVector3D ** pY, /* [out] */ IVector3D ** pZ );
		STDMETHOD( FromAxes )( /* [in] */ IVector3D * x, /* [in] */ IVector3D * y, /* [in] */ IVector3D * z );
		STDMETHOD( GetMagnitude )( /* [out, retval] */ float * pVal );
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
