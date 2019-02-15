/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_4D_H__
#define __COMC3D_COM_VECTOR_4D_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"

#include <Math/Point.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CVector4D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector4D accessible depuis COM.
	*/
	class ATL_NO_VTABLE CVector4D
		:	COM_ATL_OBJECT( Vector4D )
		,	public castor::Point4r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CVector4D();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CVector4D();

		COM_PROPERTY( X, FLOAT, makeGetter( this, &castor::Point4r::operator[], 0 ), makePutter( this, &castor::Point4r::operator[], 0 ) );
		COM_PROPERTY( Y, FLOAT, makeGetter( this, &castor::Point4r::operator[], 1 ), makePutter( this, &castor::Point4r::operator[], 1 ) );
		COM_PROPERTY( Z, FLOAT, makeGetter( this, &castor::Point4r::operator[], 2 ), makePutter( this, &castor::Point4r::operator[], 2 ) );
		COM_PROPERTY( W, FLOAT, makeGetter( this, &castor::Point4r::operator[], 3 ), makePutter( this, &castor::Point4r::operator[], 3 ) );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector4D * pVal, FLOAT * pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector4D ), CVector4D );

	DECLARE_VARIABLE_VAL_GETTER( Vector4D, castor, Point4r );
	DECLARE_VARIABLE_REF_GETTER( Vector4D, castor, Point4r );
	DECLARE_VARIABLE_REF_PUTTER( Vector4D, castor, Point4r );
}

#endif
