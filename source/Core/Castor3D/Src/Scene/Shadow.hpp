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
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/03/2017
	\~english
	\brief		Shadows configuration class.
	\~french
	\brief		Classe de configuration des ombres.
	*/
	class Shadow
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Shadow();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Shadow();
		/**
		 *\~english
		 *\return		The fog's density.
		 *\~french
		 *\return		La densité du brouillard.
		 */
		inline GLSL::ShadowType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the fog type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le type de brouillard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetType( GLSL::ShadowType p_value )
		{
			m_type = p_value;
		}

	private:
		//!\~english	The shadows filter type type.
		//!\~french		Le type de filtrage des ombres.
		GLSL::ShadowType m_type{ GLSL::ShadowType::eRaw };
	};
}

#endif
