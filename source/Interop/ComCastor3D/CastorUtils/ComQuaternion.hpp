/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_QUATERNION_H__
#define __COMC3D_COM_QUATERNION_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"
#include "ComCastor3D/CastorUtils/ComMatrix4x4.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( Quaternion, C3DQuat );
	/*!
	\~english
	\brief		This class defines a CQuaternion object accessible from COM.
	\~french
	\brief		Cette classe définit un CQuaternion accessible depuis COM
	*/
	class CQuaternion
		: public CComAtlObjectT< Quaternion, C3DQuat >
	{
	public:
		COM_PROPERTY_GET_SPTR( RotationMatrix, IMatrix4x4, c3dQuat_getRotationMatrix );
		COM_PROPERTY_GET( Pitch, FLOAT, c3dQuat_getPitch );
		COM_PROPERTY_GET( Yaw, FLOAT, c3dQuat_getYaw );
		COM_PROPERTY_GET( Roll, FLOAT, c3dQuat_getRoll );

		STDMETHOD( Transform )( /*[in]*/ IVector3D * val, /*[out, retval]*/ IVector3D ** pRet )override;
		STDMETHOD( ToAxisAngle )( /*[out]*/ IVector3D ** pAxis, /*[out]*/ FLOAT * pRet )override;
		STDMETHOD( FromAxisAngle )( /*[in]*/ IVector3D * axis, /*[in]*/ FLOAT angle )override;
		STDMETHOD( ToAxes )( /*[out]*/ IVector3D ** pX, /*[out]*/ IVector3D ** pY, /*[out]*/ IVector3D ** pZ )override;
		STDMETHOD( FromAxes )( /*[in]*/ IVector3D * x, /*[in]*/ IVector3D * y, /*[in]*/ IVector3D * z )override;
		STDMETHOD( GetMagnitude )( /*[out, retval]*/ FLOAT * pRet )override;
		STDMETHOD( Conjugate )( )override;
		STDMETHOD( Slerp )( /*[in]*/ IQuaternion * quat, /*[in]*/ FLOAT percent, /*[out, retval]*/ IQuaternion ** pRet )override;
		STDMETHOD( Mix )( /*[in]*/ IQuaternion * quat, /*[in]*/ FLOAT percent, /*[out, retval]*/ IQuaternion ** pRet )override;
		STDMETHOD( Mul )( /*[in]*/ IQuaternion * rhs, /*[out, retval]*/ IQuaternion ** pRet )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Quaternion ), CQuaternion );
}

#endif
