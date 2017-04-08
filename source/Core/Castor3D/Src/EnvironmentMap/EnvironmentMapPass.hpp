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
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "Technique/RenderTechniquePass.hpp"

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
	class EnvironmentMapPass
		: Castor::OwnedBy< EnvironmentMap >
	{
	public:
		EnvironmentMapPass( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass & operator=( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass( EnvironmentMapPass && )= default;
		EnvironmentMapPass & operator=( EnvironmentMapPass && )=default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_reflectionMap	The parent reflection map.
		 *\param[in]	p_node			The node from which the camera is created.
		 *\param[in]	p_objectNode	The node to which the object is attached.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_reflectionMap	Le reflection map parente.
		 *\param[in]	p_node			Le noeud depuis lequel on crée la caméra.
		 *\param[in]	p_objectNode	Le noeud auquel l'objet est attaché.
		 */
		C3D_API EnvironmentMapPass( EnvironmentMap & p_reflectionMap
			, SceneNodeSPtr p_node
			, SceneNode const & p_objectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~EnvironmentMapPass();
		/**
		 *\copydoc		Castor3D::RenderPass::DoInitialise
		 */
		bool Initialise( Castor::Size const & p_size );
		/**
		 *\copydoc		Castor3D::ShadowMapPass::DoCleanup
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	p_node		The base node.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	p_node		Le noeud de base.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void Update( SceneNode const & p_node, RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API void Render();

	private:
		//!\~english	The camera node.
		//!\~french		Le noeud de la camera.
		SceneNodeSPtr m_node;
		//!\~english	The camera.
		//!\~french		La camera.
		CameraSPtr m_camera;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		//!\~english	The pass used to render transparent nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds transparents.
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
	};
}

#endif
