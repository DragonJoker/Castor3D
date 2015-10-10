/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef __COMC3D_COM_PASS_H__
#define __COMC3D_COM_PASS_H__

#include "ComColour.hpp"
#include "ComShaderProgram.hpp"

#include <Pass.hpp>

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
		COM_PROPERTY( Shader, IShaderProgram *, make_getter( m_internal.get(), &Castor3D::Pass::GetShader< Castor3D::ShaderProgramBase > ), make_putter( m_internal.get(), &Castor3D::Pass::SetShader ) );

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
