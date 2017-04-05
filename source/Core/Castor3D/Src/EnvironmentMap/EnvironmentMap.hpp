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
#ifndef ___C3D_EnvironmentMap_H___
#define ___C3D_EnvironmentMap_H___

#include "EnvironmentMap/EnvironmentMapPass.hpp"
#include "Scene/SceneNode.hpp"
#include "Texture/TextureUnit.hpp"
#include "Texture/TextureLayout.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Reflection mapping implementation.
	\~french
	\brief		Implémentation du reflection mapping.
	*/
	class EnvironmentMap
		: public Castor::OwnedBy< Engine >
	{
	public:
		using CubeMatrices = std::array< Castor::Matrix4x4r, size_t (CubeMapFace::eCount) >;
		using CubeColourAttachment = std::array< TextureAttachmentSPtr, size_t (CubeMapFace::eCount) >;
		using CubeCameras = std::array< CameraSPtr, size_t (CubeMapFace::eCount) >;
		using CubeDepthAttachment = RenderBufferAttachmentSPtr;
		using EnvironmentMapPasses = std::array< EnvironmentMapPass, size_t (CubeMapFace::eCount) >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_node		The scene node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_node		Le noeud de scène.
		 */
		C3D_API EnvironmentMap( Engine & p_engine, SceneNode & p_node );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~EnvironmentMap();
		/**
		 *\~english
		 *\brief		Initialises the frame buffer.
		 *\~french
		 *\brief		Initialise le frame buffer.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer.
		 *\~french
		 *\brief		Nettoie le frame buffer.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the passes.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour les passes.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void Update( RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Renders the selected lights shadow map.
		 *\~french
		 *\brief		Dessine les shadow maps des lumières sélectionnées.
		 */
		C3D_API void Render();
		/**
		 *\~english
		 *\brief		Adds a reflection source, creating a reflection map pass for it.
		 *\param[in]	p_node	The source node.
		 *\~french
		 *\brief		Ajoute une source de réflexion, créant une passe de reflection mapping pour elle.
		 *\param[in]	p_node	Le noeud de la source.
		 */
		C3D_API void AddSource( SceneNode & p_node );
		/**
		 *\~english
		 *\return		The reflection map.
		 *\~english
		 *\return		La map de réflexion.
		 */
		inline TextureUnit & GetTexture()
		{
			return m_environmentMap;
		}
		/**
		 *\~english
		 *\return		The reflection map.
		 *\~english
		 *\return		La map de réflexion.
		 */
		inline TextureUnit const & GetTexture()const
		{
			return m_environmentMap;
		}
		/**
		 *\~english
		 *\return		The reflection map dimensions.
		 *\~english
		 *\return		Les dimensions de la map de réflexion.
		 */
		inline Castor::Size GetSize()const
		{
			return m_environmentMap.GetTexture()->GetDimensions();
		}

	private:
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de profondeur et le tampon principal.
		CubeDepthAttachment m_depthAttach;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The attach between colour buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de couleur et le tampon principal.
		CubeColourAttachment m_colourAttachs;
		//!\~english	The reflection mapping texture.
		//!\~french		La texture de reflcetion mapping.
		TextureUnit m_environmentMap;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The node.
		//!\~french		Le noeud.
		SceneNode const & m_node;
		//!\~english	The connection to node changed signal.
		//!\~french		La connexion au signal de changement du noeud.
		SceneNode::OnChanged::connection m_onNodeChanged;
		//!\~english	The view matrices for the render of each cube face.
		//!\~french		Les matrices vue pour le dessin de chaque face du cube.
		CubeMatrices m_matrices;
		//!\~english	The target size.
		//!\~french		Les dimensions de la cible.
		Castor::Size m_size;
		//!\~english	The render pass for each cube face.
		//!\~french		La passe de rendu pour chaque face du cube.
		EnvironmentMapPasses m_passes;
	};
}

#endif
