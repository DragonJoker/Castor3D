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
#ifndef ___C3D_ReflectionMapPass_H___
#define ___C3D_ReflectionMapPass_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Geometry.hpp"
#include "Texture/TextureUnit.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Reflection mapping pass implementation.
	\~french
	\brief		Implémentation d'une passe de reflection mapping.
	*/
	class ReflectionMapPass
		: public RenderTarget
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
		C3D_API ReflectionMapPass( Engine & p_engine
			, SceneNode & p_node
			, ReflectionMap const & p_reflectionMap );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ReflectionMapPass();
		/**
		 *\copydoc		Castor3D::RenderPass::DoInitialise
		 */
		bool Initialise( Castor::Size const & p_size )override;
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoCleanup
		 */
		void Cleanup()override;
		/**
		 *\~english
		 *\brief		Render function
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void Render( uint32_t p_face = 0 );
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\param[out]	p_index		The pass index.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 *\param[out]	p_index		L'indice de la passe.
		 */
		C3D_API void Update( RenderQueueArray & p_queues
			, int32_t p_index );

	private:
		/**
		 *\copydoc		Castor3D::RenderPass::DoRender
		 */
		void DoRender( uint32_t p_face );

	private:
		//!\~english	The parent reflection map.
		//!\~french		La reflection map parente.
		ReflectionMap const & m_reflectionMap;
		//!\~english	The node.
		//!\~french		Le noeud.
		SceneNode & m_node;
		//!\~english	Tells if the pass is initialised.
		//!\~french		Dit si la passe est initialisée.
		bool m_initialised{ false };
		//!\~english	The pass index.
		//!\~french		L'indice de la passe.
		int32_t m_index{ 0u };
		//!\~english	The connection to node changed signal.
		//!\~french		La connexion au signal de changement du noeud.
		SceneNode::OnChanged::connection m_onNodeChanged;
		//!\~english	The projection matrix.
		//!\~french		La matrice de projection.
		Castor::Matrix4x4r m_projection;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!\~english	The view matrices for the render of each cube face.
		//!\~french		Les matrices vue pour le dessin de chaque face du cube.
		std::array< Castor::Matrix4x4r, size_t( CubeMapFace::eCount ) > m_matrices;
	};
}

#endif
