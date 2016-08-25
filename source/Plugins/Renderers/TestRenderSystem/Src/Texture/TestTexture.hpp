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
#ifndef ___TRS_TEXTURE_H___
#define ___TRS_TEXTURE_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Texture/TextureLayout.hpp>

namespace TestRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class TestTexture
		: public Castor3D::TextureLayout
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of AccessType).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of AccessType).
		 */
		TestTexture( TestRenderSystem & p_renderSystem, Castor3D::TextureType p_type, Castor3D::AccessType p_cpuAccess, Castor3D::AccessType p_gpuAccess );
		/**
		 *\brief		Destructor.
		 */
		~TestTexture();
		/**
		 *\copydoc		Castor3D::TextureLayout::Create
		 */
		virtual bool Create();
		/**
		 *\copydoc		Castor3D::TextureLayout::Destroy
		 */
		virtual void Destroy();
		/**
		*\brief		Forces mipmaps generation.
		*/
		void GenerateMipmaps()const;

	private:
		/**
		 *\copydoc		Castor3D::TextureLayout::DoBind
		 */
		virtual bool DoBind( uint32_t p_index )const;
		/**
		 *\copydoc		Castor3D::TextureLayout::DoUnbind
		 */
		virtual void DoUnbind( uint32_t p_index )const;
	};
}

#endif
