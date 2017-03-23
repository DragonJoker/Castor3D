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
#ifndef ___C3D_ReflectionMap_H___
#define ___C3D_ReflectionMap_H___

#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Render/Viewport.hpp"
#include "Texture/TextureUnit.hpp"

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
	class ReflectionMap
		: public Castor::OwnedBy< Engine >
	{
	protected:
		using ReflectionMapNodeMap = std::map< SceneNode const *, ReflectionMapPassSPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API ReflectionMap( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ReflectionMap();
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
		 *\param[in]	p_index	The map index.
		 *\~english
		 *\return		La map de réflexion.
		 *\param[in]	p_index	L'indice de la map.
		 */
		inline TextureUnit & GetTexture( uint32_t p_index )
		{
			REQUIRE( p_index < m_reflectionMaps.size() );
			return m_reflectionMaps[p_index];
		}
		/**
		 *\~english
		 *\return		The reflection map.
		 *\param[in]	p_index	The map index.
		 *\~english
		 *\return		La map de réflexion.
		 *\param[in]	p_index	L'indice de la map.
		 */
		inline TextureUnit const & GetTexture( uint32_t p_index )const
		{
			REQUIRE( p_index < m_reflectionMaps.size() );
			return m_reflectionMaps[p_index];
		}

	private:
		using CubeColourAttachment = std::array< TextureAttachmentSPtr, size_t( CubeMapFace::eCount ) >;
		using CubeDepthAttachment = RenderBufferAttachmentSPtr;
		//!\~english	The attach between depth buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de profondeur et le tampon principal.
		CubeDepthAttachment m_depthAttach;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The attach between colour buffer and main frame buffer.
		//!\~french		L'attache entre le tampon de couleur et le tampon principal.
		std::vector< CubeColourAttachment > m_colourAttachs;
		//!\~english	The shadow map texture.
		//!\~french		La texture de mappage d'ombres.
		std::vector< TextureUnit > m_reflectionMaps;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The geometry buffer.
		//!\~french		Les tampons de géométrie.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	The reflection mapping passes used during the render.
		//!\~french		Les passes de reflection mapping utilisées pendant le rendu.
		ReflectionMapNodeMap m_passes;
	};
}

#endif
