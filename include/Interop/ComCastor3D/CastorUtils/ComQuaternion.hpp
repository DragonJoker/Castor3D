/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_QUATERNION_H__
#define __COMC3D_COM_QUATERNION_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComMatrix4x4.hpp"

#include <CastorUtils/Math/Quaternion.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, Quaternion );
	/*!
	\~english
	\brief		This class defines a CQuaternion object accessible from COM.
	\~french
	\brief		Cette classe définit un CQuaternion accessible depuis COM
	*/
	class CQuaternion
		: public CComAtlObject< Quaternion, castor::Quaternion >
	{
	public:
		//typedef VariablePutter< castor::Quaternion, castor::Matrix4x4f const & > MtxPutter;
		STDMETHOD( get_RotationMatrix )( IMatrix4x4 ** /* [out, retval] */ pVal );

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
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Quaternion ), CQuaternion );
}

#endif
