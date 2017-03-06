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

		inline Castor3D::TextureUnitSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::TextureUnitSPtr internal )
		{
			m_internal = internal;
		}

		COM_PROPERTY( Texture, ITextureLayout *, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetTexture ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetTexture ) );
		COM_PROPERTY( Sampler, ISampler *, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetSampler ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetSampler ) );
		COM_PROPERTY( AlphaFunc, eALPHA_FUNC, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetAlphaFunc ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetAlphaFunc ) );
		COM_PROPERTY( AlphaValue, float, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetAlphaValue ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetAlphaValue ) );
		COM_PROPERTY( AlpFunction, eALPHA_BLEND_FUNC, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetAlpFunction ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetAlpFunction ) );
		COM_PROPERTY( RgbFunction, eRGB_BLEND_FUNC, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetRgbFunction ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetRgbFunction ) );
		COM_PROPERTY( Channel, eTEXTURE_CHANNEL, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetChannel ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetChannel ) );
		COM_PROPERTY( BlendColour, IColour *, make_getter( m_internal.get(), &Castor3D::TextureUnit::GetBlendColour ), make_putter( m_internal.get(), &Castor3D::TextureUnit::SetBlendColour ) );

		COM_PROPERTY_INDEXED( AlpArgument, unsigned int, eBLEND_SOURCE, make_indexed_getter( m_internal.get(), &Castor3D::TextureUnit::GetAlpArgument ), make_indexed_putter( m_internal.get(), &Castor3D::TextureUnit::SetAlpArgument ) );
		COM_PROPERTY_INDEXED( RgbArgument, unsigned int, eBLEND_SOURCE, make_indexed_getter( m_internal.get(), &Castor3D::TextureUnit::GetRgbArgument ), make_indexed_putter( m_internal.get(), &Castor3D::TextureUnit::SetRgbArgument ) );

		STDMETHOD( LoadTexture )( /* [in] */ BSTR path );

	private:
		Castor3D::TextureUnitSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( TextureUnit ), CTextureUnit );

	DECLARE_VARIABLE_PTR_GETTER( TextureUnit, Castor3D, TextureUnit );
	DECLARE_VARIABLE_PTR_PUTTER( TextureUnit, Castor3D, TextureUnit );
}

#endif
