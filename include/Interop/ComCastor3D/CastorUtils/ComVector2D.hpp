/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_2D_H__
#define __COMC3D_COM_VECTOR_2D_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Point.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CVector2D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector2D accessible depuis COM.
	*/
	class ATL_NO_VTABLE CVector2D
		:	COM_ATL_OBJECT( Vector2D )
		,	public castor::Point2r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CVector2D();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CVector2D();

		COM_PROPERTY( X, FLOAT, makeGetter( this, &castor::Point2r::operator[], 0 ), makePutter( this, &castor::Point2r::operator[], 0 ) );
		COM_PROPERTY( Y, FLOAT, makeGetter( this, &castor::Point2r::operator[], 1 ), makePutter( this, &castor::Point2r::operator[], 1 ) );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector2D * pVal, FLOAT * pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector2D ), CVector2D );

	DECLARE_VARIABLE_REF_GETTER( Vector2D, castor, Point2r );
	DECLARE_VARIABLE_REF_PUTTER( Vector2D, castor, Point2r );
}

#endif
