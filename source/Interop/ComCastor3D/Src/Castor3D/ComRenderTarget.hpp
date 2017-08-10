/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef __COMC3D_COM_RENDER_TARGET_H__
#define __COMC3D_COM_RENDER_TARGET_H__

#include "ComSize.hpp"
#include "ComCamera.hpp"
#include "ComScene.hpp"

#include <Render/RenderTarget.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CRenderWindow object accessible from COM.
	\~french
	\brief		Cette classe définit un CRenderWindow accessible depuis COM.
	*/
	class ATL_NO_VTABLE CRenderTarget
		:	COM_ATL_OBJECT( RenderTarget )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CRenderTarget();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CRenderTarget();

		inline castor3d::RenderTargetSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::RenderTargetSPtr value )
		{
			m_internal = value;
		}

		COM_PROPERTY( SamplesCount, unsigned int, make_getter( m_internal.get(), &castor3d::RenderTarget::getSamplesCount ), make_putter( m_internal.get(), &castor3d::RenderTarget::setSamplesCount ) );
		COM_PROPERTY( Camera, ICamera *, make_getter( m_internal.get(), &castor3d::RenderTarget::getCamera ), make_putter( m_internal.get(), &castor3d::RenderTarget::setCamera ) );
		COM_PROPERTY( ViewportType, eVIEWPORT_TYPE, make_getter( m_internal.get(), &castor3d::RenderTarget::getViewportType ), make_putter( m_internal.get(), &castor3d::RenderTarget::setViewportType ) );
		COM_PROPERTY( Scene, IScene *, make_getter( m_internal.get(), &castor3d::RenderTarget::getScene ), make_putter( m_internal.get(), &castor3d::RenderTarget::setScene ) );
		COM_PROPERTY( PixelFormat, ePIXEL_FORMAT, make_getter( m_internal.get(), &castor3d::RenderTarget::getPixelFormat ), make_putter( m_internal.get(), &castor3d::RenderTarget::setPixelFormat ) );

		COM_PROPERTY_GET( Size, ISize *, make_getter( m_internal.get(), &castor3d::RenderTarget::getSize ) );

		STDMETHOD( Initialise )( /* [in] */ unsigned int index );
		STDMETHOD( Cleanup )();

	private:
		castor3d::RenderTargetSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( RenderTarget ), CRenderTarget );

	DECLARE_VARIABLE_PTR_GETTER( RenderTarget, castor3d, RenderTarget );
	DECLARE_VARIABLE_PTR_PUTTER( RenderTarget, castor3d, RenderTarget );
}

#endif
