/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RECT_H__
#define __COMC3D_COM_RECT_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_EX( Rect, castor, Rectangle );
	/*!
	\~english
	\brief		This class defines a CRect object accessible from COM.
	\~french
	\brief		Cette classe définit un CRect accessible depuis COM.
	*/
	class CRect
		: public CComAtlObject< Rect, castor::Rectangle >
	{
	public:
		COMEX_PROPERTY( Left, INT, &m_internal, &Internal::left, &Internal::left );
		COMEX_PROPERTY( Right, INT, &m_internal, &Internal::right, &Internal::right );
		COMEX_PROPERTY( Top, INT, &m_internal, &Internal::top, &Internal::top );
		COMEX_PROPERTY( Bottom, INT, &m_internal, &Internal::bottom, &Internal::bottom );

		COMEX_PROPERTY_GET( Width, INT, &m_internal, &castor::Rectangle::getWidth );
		COMEX_PROPERTY_GET( Height, INT, &m_internal, &castor::Rectangle::getHeight );

		STDMETHOD( Set )( /* [in] */ INT left, /* [in] */ INT top, /* [in] */ INT right, /* [in] */ INT bottom );
		STDMETHOD( IntersectsPosition )( /* [in] */ IPosition * pos, /* [retval][out] */ eINTERSECTION * pVal );
		STDMETHOD( IntersectsRectangle )( /* [in] */ IRect * pos, /* [retval][out] */ eINTERSECTION * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Rect ), CRect );
}

#endif
