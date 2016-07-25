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
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComColour.hpp"
#include "ComShaderProgram.hpp"

#include <Material/Pass.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CPass object accessible from COM.
	\~french
	\brief		Cette classe définit un CPass accessible depuis COM.
	*/
	class ATL_NO_VTABLE CPass
		:	COM_ATL_OBJECT( Pass )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CPass();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CPass();

		inline Castor3D::PassSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::PassSPtr pass )
		{
			m_internal = pass;
		}

		COM_PROPERTY( Ambient, IColour *, make_getter( m_internal.get(), &Castor3D::Pass::GetAmbient ), make_putter( m_internal.get(), &Castor3D::Pass::SetAmbient ) );
		COM_PROPERTY( Diffuse, IColour *, make_getter( m_internal.get(), &Castor3D::Pass::GetDiffuse ), make_putter( m_internal.get(), &Castor3D::Pass::SetDiffuse ) );
		COM_PROPERTY( Specular, IColour *, make_getter( m_internal.get(), &Castor3D::Pass::GetSpecular ), make_putter( m_internal.get(), &Castor3D::Pass::SetSpecular ) );
		COM_PROPERTY( Emissive, IColour *, make_getter( m_internal.get(), &Castor3D::Pass::GetEmissive ), make_putter( m_internal.get(), &Castor3D::Pass::SetEmissive ) );
		COM_PROPERTY( Shininess, float, make_getter( m_internal.get(), &Castor3D::Pass::GetShininess ), make_putter( m_internal.get(), &Castor3D::Pass::SetShininess ) );
		COM_PROPERTY( TwoSided, boolean, make_getter( m_internal.get(), &Castor3D::Pass::IsTwoSided ), make_putter( m_internal.get(), &Castor3D::Pass::SetTwoSided ) );
		COM_PROPERTY( Alpha, float, make_getter( m_internal.get(), &Castor3D::Pass::GetAlpha ), make_putter( m_internal.get(), &Castor3D::Pass::SetAlpha ) );

		COM_PROPERTY_GET( TextureUnitCount, unsigned int, make_getter( m_internal.get(), &Castor3D::Pass::GetTextureUnitsCount ) );

		STDMETHOD( CreateTextureUnit )( /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( GetTextureUnitByIndex )( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal );
		STDMETHOD( DestroyTextureUnit )( /* [in] */ ITextureUnit * val );
		STDMETHOD( GetTextureUnitByChannel )( /* [in] */ eTEXTURE_CHANNEL channel, /* [out, retval] */ ITextureUnit ** pVal );

	private:
		Castor3D::PassSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Pass ), CPass );

	DECLARE_VARIABLE_PTR_GETTER( Pass, Castor3D, Pass );
	DECLARE_VARIABLE_PTR_PUTTER( Pass, Castor3D, Pass );
}

#endif
