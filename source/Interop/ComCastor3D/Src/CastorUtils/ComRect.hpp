/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RECT_H__
#define __COMC3D_COM_RECT_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Graphics/Rectangle.hpp>

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

		COM_PROPERTY( Left, INT, make_getter( this, &castor::Rectangle::left ), make_putter( this, &castor::Rectangle::left ) );
		COM_PROPERTY( Right, INT, make_getter( this, &castor::Rectangle::right ), make_putter( this, &castor::Rectangle::right ) );
		COM_PROPERTY( Top, INT, make_getter( this, &castor::Rectangle::top ), make_putter( this, &castor::Rectangle::top ) );
		COM_PROPERTY( Bottom, INT, make_getter( this, &castor::Rectangle::bottom ), make_putter( this, &castor::Rectangle::bottom ) );

		COM_PROPERTY_GET( Width, INT, make_getter( this, &castor::Rectangle::width ) );
		COM_PROPERTY_GET( Height, INT, make_getter( this, &castor::Rectangle::width ) );

		STDMETHOD( set )( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom );
		STDMETHOD( IntersectsPosition )( /* [in] */ IPosition * pos, /* [retval][out] */ eINTERSECTION * pVal );
		STDMETHOD( IntersectsRectangle )( /* [in] */ IRect * pos, /* [retval][out] */ eINTERSECTION * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Rect ), CRect );

	DECLARE_VARIABLE_REF_GETTER( Rect, castor, Rectangle );
	DECLARE_VARIABLE_REF_PUTTER( Rect, castor, Rectangle );
}

#endif
