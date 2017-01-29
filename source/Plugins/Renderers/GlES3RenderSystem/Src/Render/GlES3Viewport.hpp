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
#ifndef ___C3DGLES3_VIEWPORT_H___
#define ___C3DGLES3_VIEWPORT_H___

#include "Common/GlES3Holder.hpp"

#include <Render/Viewport.hpp>

namespace GlES3Render
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/04/2016
	\~english
	\brief		Render API specific viewport implementation.
	\~french
	\brief		Implémentation de viewport spàcifique à l'API de rendu.
	*/
	class GlES3Viewport
		: public Castor3D::IViewportImpl
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_viewport		The parent viewport.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_viewport		Le viewport parent.
		 */
		GlES3Viewport( GlES3RenderSystem & p_renderSystem, Castor3D::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlES3Viewport();
		/**
		 *\copydoc		Castor3D::IViewportImpl::Apply
		 */
		void Apply()const override;
	};
}

#endif
