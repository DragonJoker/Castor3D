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
#ifndef __COMC3D_COM_SHADER_PROGRAM_H__
#define __COMC3D_COM_SHADER_PROGRAM_H__

#include "ComAtlObject.hpp"

#include <ShaderProgram.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CShaderProgram object accessible from COM.
	\~french
	\brief		Cette classe définit un CShaderProgram accessible depuis COM.
	*/
	class ATL_NO_VTABLE CShaderProgram
		:	COM_ATL_OBJECT( ShaderProgram )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CShaderProgram();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CShaderProgram();

		inline Castor3D::ShaderProgramBaseSPtr GetInternal()const
		{
			return m_internal;
		}

		inline void SetInternal( Castor3D::ShaderProgramBaseSPtr pass )
		{
			m_internal = pass;
		}

		STDMETHOD( get_File )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal );
		STDMETHOD( put_File )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val );
		STDMETHOD( get_Source )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal );
		STDMETHOD( put_Source )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val );
		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( CreateObject )( /* [in] */ eSHADER_TYPE val );

	private:
		Castor3D::ShaderProgramBaseSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( ShaderProgram ), CShaderProgram );

	DECLARE_VARIABLE_PTR_GETTER( ShaderProgram, Castor3D, ShaderProgramBase );
	DECLARE_VARIABLE_PTR_PUTTER( ShaderProgram, Castor3D, ShaderProgramBase );
}

#endif
