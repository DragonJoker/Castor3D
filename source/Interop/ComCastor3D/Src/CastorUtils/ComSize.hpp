/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SIZE_H__
#define __COMC3D_COM_SIZE_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Size.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CSize object accessible from COM.
	\~french
	\brief		Cette classe définit un CSize accessible depuis COM.
	*/
	class ATL_NO_VTABLE CSize
		:	COM_ATL_OBJECT( Size )
		,	public castor::Size
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CSize();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CSize();

		COM_PROPERTY( Width, UINT, makeGetter( this, &castor::Size::getWidth ), makePutter( this, &castor::Size::getWidth ) );
		COM_PROPERTY( Height, UINT, makeGetter( this, &castor::Size::getHeight ), makePutter( this, &castor::Size::getHeight ) );

		STDMETHOD( Set )( /* [in] */ UINT x, /* [in] */ UINT y );
		STDMETHOD( Grow )( /* [in] */ INT x, /* [in] */ INT y );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Size ), CSize );

	DECLARE_VARIABLE_VAL_GETTER( Size, castor, Size );
	DECLARE_VARIABLE_REF_GETTER( Size, castor, Size );
	DECLARE_VARIABLE_REF_PUTTER( Size, castor, Size );
}

#endif
