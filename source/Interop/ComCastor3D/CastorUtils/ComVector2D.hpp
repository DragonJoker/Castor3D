/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_2D_H__
#define __COMC3D_COM_VECTOR_2D_H__

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( Vector2D, C3DVec2 );
	/*!
	\~english
	\brief		This class defines a CVector2D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector2D accessible depuis COM.
	*/
	class CVector2D
		: public CComAtlObjectT< Vector2D, C3DVec2 >
	{
	public:
		COM_PROPERTY_MBR( X, FLOAT, &m_internal.x );
		COM_PROPERTY_MBR( Y, FLOAT, &m_internal.y );

		STDMETHOD( Negate )()override;
		STDMETHOD( Normalise )()override;
		STDMETHOD( Dot )( IVector2D * rhs, FLOAT * pRet )override;
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pRet )override;
		STDMETHOD( Set )( /*[in]*/ FLOAT x, /*[in]*/ FLOAT y )override;
		STDMETHOD( CompMul )( IVector2D * rhs, IVector2D ** pRet )override;
		STDMETHOD( CompAdd )( IVector2D * rhs, IVector2D ** pRet )override;
		STDMETHOD( CompSub )( IVector2D * rhs, IVector2D ** pRet )override;
		STDMETHOD( Mul )( float rhs, IVector2D ** pRet )override;
		STDMETHOD( Div )( float rhs, IVector2D ** pRet )override;
		STDMETHOD( Add )( float rhs, IVector2D ** pRet )override;
		STDMETHOD( Sub )( float rhs, IVector2D ** pRet )override;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector2D ), CVector2D );
}

#endif
