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
#ifndef ___C3D_ShadowMapPoint_H___
#define ___C3D_ShadowMapPoint_H___

#include "ShadowMap.hpp"
#include "Render/Viewport.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMapPoint
		: public ShadowMap
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_scene		The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_scene		La scène.
		 */
		C3D_API ShadowMapPoint( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPoint();

	private:
		/**
		 *\copydoc		Castor3D::ShadowMap::DoInitialise
		 */
		bool DoInitialise( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoCreatePass
		 */
		ShadowMapPassSPtr DoCreatePass( Light & p_light )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoUpdateFlags
		 */
		void DoUpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetVertexShaderSource
		 */
		Castor::String DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags
			, bool p_invertNormals )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetGeometryShaderSource
		 */
		Castor::String DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::ShadowMap::DoGetPixelShaderSource
		 */
		Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, uint8_t p_sceneFlags )const override;

	private:
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon profondeur et le tampon principal.
		TextureAttachmentSPtr m_depthAttach;
	};
}

#endif
