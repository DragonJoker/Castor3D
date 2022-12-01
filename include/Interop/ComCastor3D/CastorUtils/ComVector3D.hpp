/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_3D_H__
#define __COMC3D_COM_VECTOR_3D_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Point.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Vector3D, castor, Point3f );
	/*!
	\~english
	\brief		This class defines a CVector3D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector3D accessible depuis COM.
	*/
	class CVector3D
		: public CComAtlObject< Vector3D, castor::Point3f >
	{
	public:
		COMEX_PROPERTY_IDX( X, 0, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( Y, 1, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( Z, 2, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector3D * pVal, FLOAT * pRet );
		STDMETHOD( Cross )( /* [in] */ IVector3D * val, /* [out,retval] */ IVector3D ** pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
		STDMETHOD( Set )( /* [in] */ FLOAT x, /* [in] */ FLOAT y, /* [in] */ FLOAT z );
		STDMETHOD( CompMul )( IVector3D * rhs, IVector3D ** pVal );
		STDMETHOD( CompAdd )( IVector3D * rhs, IVector3D ** pVal );
		STDMETHOD( CompSub )( IVector3D * rhs, IVector3D ** pVal );
		STDMETHOD( Mul )( float rhs, IVector3D ** pVal );
		STDMETHOD( Div )( float rhs, IVector3D ** pVal );
		STDMETHOD( Add )( float rhs, IVector3D ** pVal );
		STDMETHOD( Sub )( float rhs, IVector3D ** pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector3D ), CVector3D );
}

#endif
