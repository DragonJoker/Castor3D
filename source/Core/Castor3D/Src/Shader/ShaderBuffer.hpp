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
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "Shader/ShaderStorageBuffer.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Wrapper class to select between SSBO or TBO.
	\remarks	Allows to user either one or the other in the same way.
	\~french
	\brief		Classe permettant de choisir entre SSBO et TBO.
	\remarks	Permet d'utiliser l'un comme l'autre de la même manière.
	*/
	class ShaderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	size	The buffer size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	size	La taille du tampon.
		 */
		C3D_API ShaderBuffer( Engine & engine
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderBuffer();
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Binds the buffer.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Active le tampon.
		 *\param[in]	index	Le point d'activation.
		 */
		C3D_API void bind( uint32_t index )const;
		/**
		 *\~english
		 *\return		The pointer the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		C3D_API uint8_t * ptr();

	protected:
		//!\~english	The SSBO.
		//!\~french		Le SSBO.
		ShaderStorageBufferUPtr m_ssbo;
		//!\~english	The TBO.
		//!\~french		Le TBO.
		TextureUnit m_tbo;
		//!\~english	The TBO's buffer.
		//!\~french		Le tampon du TBO.
		castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
