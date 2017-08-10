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
#ifndef __COMC3D_COM_SHADER_PROGRAM_H__
#define __COMC3D_COM_SHADER_PROGRAM_H__

#include "ComAtlObject.hpp"

#include <Shader/ShaderProgram.hpp>

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

		inline castor3d::ShaderProgramSPtr getInternal()const
		{
			return m_internal;
		}

		inline void setInternal( castor3d::ShaderProgramSPtr pass )
		{
			m_internal = pass;
		}

		STDMETHOD( get_File )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal );
		STDMETHOD( put_File )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val );
		STDMETHOD( get_Source )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [out, retval] */ BSTR * pVal );
		STDMETHOD( put_Source )( /* [in] */ eSHADER_TYPE target, /* [in] */ eSHADER_MODEL model, /* [in] */ BSTR val );
		STDMETHOD( Initialise )();
		STDMETHOD( Cleanup )();
		STDMETHOD( createObject )( /* [in] */ eSHADER_TYPE val );

	private:
		castor3d::ShaderProgramSPtr m_internal;
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( ShaderProgram ), CShaderProgram );

	DECLARE_VARIABLE_PTR_GETTER( ShaderProgram, castor3d, ShaderProgram );
	DECLARE_VARIABLE_PTR_PUTTER( ShaderProgram, castor3d, ShaderProgram );
}

#endif
