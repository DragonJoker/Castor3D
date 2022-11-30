/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComCastor3D/Castor3D/ComTextureLayout.hpp"
#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/CastorUtils/ComRgbaColour.hpp"

#include <Castor3D/Material/Texture/TextureUnit.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS_PTR( castor3d, TextureUnit );
	/*!
	\~english
	\brief		This class defines a CTextureUnit object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureUnit accessible depuis COM.
	*/
	class CTextureUnit
		: public CComAtlObject< TextureUnit, castor3d::TextureUnit >
	{
	public:
		COM_PROPERTY( Texture, ITextureLayout *, makeGetter( m_internal, &castor3d::TextureUnit::getTexture ), makePutter( m_internal, &castor3d::TextureUnit::setTexture ) );
		COM_PROPERTY( Sampler, ISampler *, makeGetter( m_internal, &castor3d::TextureUnit::getSampler ), makePutter( m_internal, &castor3d::TextureUnit::setSampler ) );

		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureUnit ), CTextureUnit );
}

#endif
