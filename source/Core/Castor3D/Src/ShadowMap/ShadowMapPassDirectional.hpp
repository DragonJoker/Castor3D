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
#ifndef ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___
#define ___C3D_SHADOW_MAP_PASS_DIRECTIONAL_H___

#include "ShadowMapPass.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation for spot lights.
	\~french
	\brief		Implémentation du mappage d'ombres pour les lumières spot.
	*/
	class ShadowMapPassDirectional
		: public ShadowMapPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_light		The light source.
		 *\param[in]	p_shadowMap	The parent shadow map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_light		La source lumineuse.
		 *\param[in]	p_shadowMap	La shadow map parente.
		 */
		C3D_API ShadowMapPassDirectional( Engine & p_engine
			, Light & p_light
			, ShadowMap const & p_shadowMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMapPassDirectional();
		/**
		 *\~english
		 *\return		The camera.
		 *\~french
		 *\return		La caméra.
		 */
		inline CameraSPtr GetCamera()const
		{
			return m_camera;
		}

	private:
		/**
		 *\copydoc		Castor3D::RenderPass::DoInitialise
		 */
		bool DoInitialise( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoCleanup
		 */
		void DoCleanup()override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdate
		 */
		void DoUpdate( RenderQueueArray & p_queues )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoRender
		 */
		void DoRender( uint32_t p_face )override;
		/**
		 *\copydoc		Castor3D::RenderPass::DoPrepareBackPipeline
		 */
		void DoPrepareBackPipeline( ShaderProgram & p_program
			, PipelineFlags const & p_flags )override;

	private:
		//!\~english	The camera created from the light.
		//!\~french		La caméra créée à partir de la lumière.
		CameraSPtr m_camera;
		//!\~english	The view matrix.
		//!\~french		La matrice vue.
		Castor::Matrix4x4r m_view;
	};
}

#endif
