/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_TEXTURE_IMAGE_H__
#define __COMC3D_COM_TEXTURE_IMAGE_H__

#include "ComCastor3D/Castor3D/ComSampler.hpp"
#include "ComCastor3D/CastorUtils/ComPixelBuffer.hpp"

#include <Castor3D/Texture/TextureView.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CTexture object accessible from COM.
	\~french
	\brief		Cette classe définit un CTexture accessible depuis COM.
	*/
	class ATL_NO_VTABLE CTextureView
		:	COM_ATL_OBJECT( TextureView )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CTextureView();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CTextureView();

		inline castor3d::TextureViewSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::TextureViewSPtr state )
		{
			m_internal = state;
		}

		COM_PROPERTY_GET( Buffer, IPixelBuffer *, makeGetter( m_internal.get(), &castor3d::TextureView::getBuffer ) );

		STDMETHOD( StaticSource )( /* [in] */ IPixelBuffer * val );
		STDMETHOD( Dynamic2DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ ePIXEL_FORMAT format );
		STDMETHOD( Dynamic3DSource )( /* [in] */ unsigned int w, /* [in] */ unsigned int h, /* [in] */ unsigned int d, /* [in] */ ePIXEL_FORMAT format );

	private:
		castor3d::TextureViewSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureView ), CTextureView );

	DECLARE_VARIABLE_PTR_GETTER( TextureView, castor3d, TextureView );
}

#endif
