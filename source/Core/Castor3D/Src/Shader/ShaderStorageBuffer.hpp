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
#ifndef ___C3D_ShaderStorageBuffer_H___
#define ___C3D_ShaderStorageBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include "Mesh/Buffer/CpuBuffer.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		22/11/2016
	\~english
	\brief		Shader storage buffer interface

	\~french
	\brief		Interface de tampon de stockage shader.
	*/
	class ShaderStorageBuffer
		: public CpuBuffer< uint8_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	p_declaration	The buffer declaration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_declaration	La déclaration du tampon.
		 */
		C3D_API ShaderStorageBuffer( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderStorageBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	type	Buffer access type.
		 *\param[in]	nature	Buffer access nature.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	type	Type d'accès du tampon.
		 *\param[in]	nature	Nature d'accès du tampon.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( BufferAccessType type
			, BufferAccessNature nature );
		/**
		 *\~english
		 *\brief		Clean up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		sets the buffer's binding point.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Définit le point d'attache du tampon.
		 *\param[in]	index	Le point d'attache.
		 */
		C3D_API void bindTo( uint32_t index )const;
	};
}

#endif
