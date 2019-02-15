/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_VECTOR_3D_H__
#define __COMC3D_COM_VECTOR_3D_H__

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
	\brief		This class defines a CVector3D object accessible from COM.
	\~french
	\brief		Cette classe définit un CVector3D accessible depuis COM.
	*/
	class ATL_NO_VTABLE CVector3D
		:	COM_ATL_OBJECT( Vector3D )
		,	public castor::Point3r
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CVector3D();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CVector3D();

		COM_PROPERTY( X, FLOAT, makeGetter( this, &castor::Point3r::operator[], 0 ), makePutter( this, &castor::Point3r::operator[], 0 ) );
		COM_PROPERTY( Y, FLOAT, makeGetter( this, &castor::Point3r::operator[], 1 ), makePutter( this, &castor::Point3r::operator[], 1 ) );
		COM_PROPERTY( Z, FLOAT, makeGetter( this, &castor::Point3r::operator[], 2 ), makePutter( this, &castor::Point3r::operator[], 2 ) );

		STDMETHOD( Negate )();
		STDMETHOD( Normalise )();
		STDMETHOD( Dot )( IVector3D * pVal, FLOAT * pRet );
		STDMETHOD( Cross )( /* [in] */ IVector3D * val, /* [out,retval] */ IVector3D ** pRet );
		STDMETHOD( Length )( /* [out,retval] */ FLOAT * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Vector3D ), CVector3D );

	DECLARE_VARIABLE_VAL_GETTER( Vector3D, castor, Point3r );
	DECLARE_VARIABLE_REF_GETTER( Vector3D, castor, Point3r );
	DECLARE_VARIABLE_REF_PUTTER( Vector3D, castor, Point3r );
}

#endif
