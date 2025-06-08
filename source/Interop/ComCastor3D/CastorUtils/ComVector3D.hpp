/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_3D_H__
#define __COMC3D_COM_VECTOR_3D_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( Vector3D, C3DVec3 );
	/*!
	\~english
	\brief		This class defines a CVector3D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector3D accessible depuis COM.
	*/
	class CVector3D
		: public CComAtlObjectT< Vector3D, C3DVec3 >
	{
	public:
		COM_PROPERTY_MBR( X, FLOAT, &m_internal.x );
		COM_PROPERTY_MBR( Y, FLOAT, &m_internal.y );
		COM_PROPERTY_MBR( Z, FLOAT, &m_internal.z );

		STDMETHOD( Negate )()override;
		STDMETHOD( Normalise )()override;
		STDMETHOD( Dot )( IVector3D * rhs, FLOAT * pRet )override;
		STDMETHOD( Cross )( /*[in]*/ IVector3D * rhs, /* [out,retval] */ IVector3D ** pRet )override;
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pRet )override;
		STDMETHOD( Set )( /*[in]*/ FLOAT x, /*[in]*/ FLOAT y, /*[in]*/ FLOAT z )override;
		STDMETHOD( CompMul )( IVector3D * rhs, IVector3D ** pRet )override;
		STDMETHOD( CompAdd )( IVector3D * rhs, IVector3D ** pRet )override;
		STDMETHOD( CompSub )( IVector3D * rhs, IVector3D ** pRet )override;
		STDMETHOD( Mul )( float rhs, IVector3D ** pRet )override;
		STDMETHOD( Div )( float rhs, IVector3D ** pRet )override;
		STDMETHOD( Add )( float rhs, IVector3D ** pRet )override;
		STDMETHOD( Sub )( float rhs, IVector3D ** pRet )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector3D ), CVector3D );
}

#endif
