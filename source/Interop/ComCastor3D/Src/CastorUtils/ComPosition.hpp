/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_POSITION_H__
#define __COMC3D_COM_POSITION_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Position.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CPosition object accessible from COM.
	\~french
	\brief		Cette classe définit un CPosition accessible depuis COM
	*/
	class ATL_NO_VTABLE CPosition
		:	COM_ATL_OBJECT( Position )
		,	public castor::Position
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPosition();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPosition();

		COM_PROPERTY( X, INT, makeGetter( this, &castor::Position::x ), makePutter( this, &castor::Position::x ) );
		COM_PROPERTY( Y, INT, makeGetter( this, &castor::Position::y ), makePutter( this, &castor::Position::y ) );

		STDMETHOD( Set )( /* [in] */ INT x, /* [in] */ INT y );
		STDMETHOD( Offset )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Position ), CPosition );

	DECLARE_VARIABLE_REF_GETTER( Position, castor, Position );
	DECLARE_VARIABLE_REF_PUTTER( Position, castor, Position );
}

#endif
