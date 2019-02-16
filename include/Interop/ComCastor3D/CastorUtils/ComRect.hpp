/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RECT_H__
#define __COMC3D_COM_RECT_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CRect object accessible from COM.
	\~french
	\brief		Cette classe définit un CRect accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRect
		:	COM_ATL_OBJECT( Rect )
		,	public castor::Rectangle
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRect();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRect();

		COM_PROPERTY( Left, INT, makeGetter( this, &castor::Rectangle::left ), makePutter( this, &castor::Rectangle::left ) );
		COM_PROPERTY( Right, INT, makeGetter( this, &castor::Rectangle::right ), makePutter( this, &castor::Rectangle::right ) );
		COM_PROPERTY( Top, INT, makeGetter( this, &castor::Rectangle::top ), makePutter( this, &castor::Rectangle::top ) );
		COM_PROPERTY( Bottom, INT, makeGetter( this, &castor::Rectangle::bottom ), makePutter( this, &castor::Rectangle::bottom ) );

		COM_PROPERTY_GET( Width, INT, makeGetter( this, &castor::Rectangle::getWidth ) );
		COM_PROPERTY_GET( Height, INT, makeGetter( this, &castor::Rectangle::getHeight ) );

		STDMETHOD( Set )( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom );
		STDMETHOD( IntersectsPosition )( /* [in] */ IPosition * pos, /* [retval][out] */ eINTERSECTION * pVal );
		STDMETHOD( IntersectsRectangle )( /* [in] */ IRect * pos, /* [retval][out] */ eINTERSECTION * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Rect ), CRect );

	DECLARE_VARIABLE_REF_GETTER( Rect, castor, Rectangle );
	DECLARE_VARIABLE_REF_PUTTER( Rect, castor, Rectangle );
}

#endif
