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
#ifndef ___C3D_ForwardRenderTechniquePass_H___
#define ___C3D_ForwardRenderTechniquePass_H___

#include "RenderTechniquePass.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/12/2016
	\~english
	\brief		Forward render technique pass class.
	\~french
	\brief		Classe de passe de technique de rendu de type forward.
	*/
	class ForwardRenderTechniquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_renderTarget	The render target for this technique.
		 *\param[in]	p_technique		The parent render technique.
		 *\param[in]	p_opaque		Tells if this pass if for opaque nodes.
		 *\param[in]	p_multisampling	The multisampling status
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_renderTarget	La render target pour cette technique.
		 *\param[in]	p_technique		La technique de rendu parente.
		 *\param[in]	p_opaque		Dit si cette passe de rendu est pour les noeuds opaques.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 */
		C3D_API ForwardRenderTechniquePass( Castor::String const & p_name
			, RenderTarget & p_renderTarget
			, RenderTechnique & p_technique
			, bool p_opaque
			, bool p_multisampling = false );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ForwardRenderTechniquePass();
		/**
		 *\copydoc		Castor3D::RenderPass::RenderShadowMaps
		 */
		C3D_API void RenderShadowMaps()override;

	private:
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetDepthMaps
		 */
		C3D_API void DoGetDepthMaps( DepthMapArray & p_depthMaps )override;
	};
}

#endif
