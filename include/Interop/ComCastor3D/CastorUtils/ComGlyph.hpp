/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_GLYPH_H__
#define __COMC3D_COM_GLYPH_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/CastorUtils/ComSize.hpp"
#include "ComCastor3D/CastorUtils/ComPosition.hpp"

#include <CastorUtils/Graphics/Glyph.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CGlyph object accessible from COM.
	\~french
	\brief		Cette classe définit un CGlyph accessible depuis COM.
	*/
	class ATL_NO_VTABLE CGlyph
		: COM_ATL_OBJECT( Glyph )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CGlyph();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CGlyph();

		void setInternal( castor::Glyph * p_glyph )
		{
			m_glyph = p_glyph;
		}

		COM_PROPERTY_GET( Size, ISize *, makeGetter( m_glyph, &castor::Glyph::getSize ) );
		COM_PROPERTY_GET( Bearing, IPosition *, makeGetter( m_glyph, &castor::Glyph::getBearing ) );
		COM_PROPERTY_GET( Advance, INT, makeGetter( m_glyph, &castor::Glyph::getAdvance ) );

	private:
		castor::Glyph * m_glyph;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Glyph ), CGlyph )
}

#endif
