/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_RGBCOLOUR_H__
#define __COMC3D_COM_RGBCOLOUR_H__

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Graphics/Colour.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CColour object accessible from COM.
	\~french
	\brief		Cette classe définit un CColour accessible depuis COM
	*/
	class ATL_NO_VTABLE CRgbColour
		: COM_ATL_OBJECT( RgbColour )
		, public castor::RgbColour
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRgbColour();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRgbColour();
		/**
		*\~english
		*\brief		Implicit conversion operator, to castor::Point4f.
		*\~french
		*\brief		Opérateur de conversion implicite vers castor::Point4f.
		*/
		inline operator castor::Point3f()const
		{
			return toBGRFloat( *this );
		}

		COM_PROPERTY( R, FLOAT, makeGetter( this, &castor::RgbColour::get, castor::RgbComponent::eRed ), makePutter( this, &castor::RgbColour::get, castor::RgbComponent::eRed ) );
		COM_PROPERTY( G, FLOAT, makeGetter( this, &castor::RgbColour::get, castor::RgbComponent::eGreen ), makePutter( this, &castor::RgbColour::get, castor::RgbComponent::eGreen ) );
		COM_PROPERTY( B, FLOAT, makeGetter( this, &castor::RgbColour::get, castor::RgbComponent::eBlue ), makePutter( this, &castor::RgbColour::get, castor::RgbComponent::eBlue ) );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RgbColour ), CRgbColour );

	DECLARE_VARIABLE_REF_GETTER( RgbColour, castor, RgbColour );
	DECLARE_VARIABLE_REF_PUTTER( RgbColour, castor, RgbColour );
}

#endif
