/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_2D_H__
#define __COMC3D_COM_VECTOR_2D_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Point.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Vector2D, castor, Point2f );
	/*!
	\~english
	\brief		This class defines a CVector2D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector2D accessible depuis COM.
	*/
	class CVector2D
		: public CComAtlObject< Vector2D, castor::Point2f >
	{
	public:
		COMEX_PROPERTY_IDX( X, 0, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );
		COMEX_PROPERTY_IDX( Y, 1, FLOAT, &m_internal, &Internal::operator[], &Internal::operator[] );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector2D * pVal, FLOAT * pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector2D ), CVector2D );
}

#endif
