/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_UNIT_H__
#define __COMC3D_COM_TEXTURE_UNIT_H__

#include "ComTextureLayout.hpp"
#include "ComColour.hpp"

#include <Texture/TextureUnit.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CTextureUnit object accessible from COM.
	\~french
	\brief		Cette classe définit un CTextureUnit accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTextureUnit
		:	COM_ATL_OBJECT( TextureUnit )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTextureUnit();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTextureUnit();

		inline castor3d::TextureUnitSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::TextureUnitSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Texture, ITextureLayout *, make_getter( m_internal.get(), &castor3d::TextureUnit::getTexture ), make_putter( m_internal.get(), &castor3d::TextureUnit::setTexture ) );
		COM_PROPERTY( Sampler, ISampler *, make_getter( m_internal.get(), &castor3d::TextureUnit::getSampler ), make_putter( m_internal.get(), &castor3d::TextureUnit::setSampler ) );
		COM_PROPERTY( AlphaFunc, eCOMPARISON_FUNC, make_getter( m_internal.get(), &castor3d::TextureUnit::getAlphaFunc ), make_putter( m_internal.get(), &castor3d::TextureUnit::setAlphaFunc ) );
		COM_PROPERTY( AlphaValue, float, make_getter( m_internal.get(), &castor3d::TextureUnit::getAlphaValue ), make_putter( m_internal.get(), &castor3d::TextureUnit::setAlphaValue ) );
		COM_PROPERTY( AlpFunction, eALPHA_BLEND_FUNC, make_getter( m_internal.get(), &castor3d::TextureUnit::getAlpFunction ), make_putter( m_internal.get(), &castor3d::TextureUnit::setAlpFunction ) );
		COM_PROPERTY( RgbFunction, eRGB_BLEND_FUNC, make_getter( m_internal.get(), &castor3d::TextureUnit::getRgbFunction ), make_putter( m_internal.get(), &castor3d::TextureUnit::setRgbFunction ) );
		COM_PROPERTY( Channel, eTEXTURE_CHANNEL, make_getter( m_internal.get(), &castor3d::TextureUnit::getChannel ), make_putter( m_internal.get(), &castor3d::TextureUnit::setChannel ) );
		COM_PROPERTY( BlendColour, IColour *, make_getter( m_internal.get(), &castor3d::TextureUnit::getBlendColour ), make_putter( m_internal.get(), &castor3d::TextureUnit::setBlendColour ) );

		COM_PROPERTY_INDEXED( AlpArgument, unsigned int, eBLEND_SOURCE, make_indexed_getter( m_internal.get(), &castor3d::TextureUnit::getAlpArgument ), make_indexed_putter( m_internal.get(), &castor3d::TextureUnit::setAlpArgument ) );
		COM_PROPERTY_INDEXED( RgbArgument, unsigned int, eBLEND_SOURCE, make_indexed_getter( m_internal.get(), &castor3d::TextureUnit::getRgbArgument ), make_indexed_putter( m_internal.get(), &castor3d::TextureUnit::setRgbArgument ) );

		STDMETHOD( loadTexture )( /* [in] */ BSTR path );

	private:
		castor3d::TextureUnitSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureUnit ), CTextureUnit );

	DECLARE_VARIABLE_PTR_GETTER( TextureUnit, castor3d, TextureUnit );
	DECLARE_VARIABLE_PTR_PUTTER( TextureUnit, castor3d, TextureUnit );
}

#endif
