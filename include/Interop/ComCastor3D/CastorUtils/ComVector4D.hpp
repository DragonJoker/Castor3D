/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_4D_H__
#define __COMC3D_COM_VECTOR_4D_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Point.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Vector4D, castor, Point4f );
	/*!
	\~english
	\brief		This class defines a CVector4D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector4D accessible depuis COM.
	*/
	class CVector4D
		: public CComAtlObject< Vector4D, castor::Point4f >
	{
	public:
		COMEX_PROPERTY_IDX( X, 0, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( Y, 1, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( Z, 2, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( W, 3, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector4D * pVal, FLOAT * pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
		STDMETHOD( Set )( /* [in] */ FLOAT x, /* [in] */ FLOAT y, /* [in] */ FLOAT z, /* [in] */ FLOAT w );
		STDMETHOD( CompMul )( IVector4D * rhs, IVector4D ** pVal );
		STDMETHOD( CompAdd )( IVector4D * rhs, IVector4D ** pVal );
		STDMETHOD( CompSub )( IVector4D * rhs, IVector4D ** pVal );
		STDMETHOD( Mul )( float rhs, IVector4D ** pVal );
		STDMETHOD( Div )( float rhs, IVector4D ** pVal );
		STDMETHOD( Add )( float rhs, IVector4D ** pVal );
		STDMETHOD( Sub )( float rhs, IVector4D ** pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector4D ), CVector4D );
}

#endif
