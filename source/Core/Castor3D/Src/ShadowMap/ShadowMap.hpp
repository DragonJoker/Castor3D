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
#ifndef ___C3D_ShadowMap_H___
#define ___C3D_ShadowMap_H___

#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/OwnedBy.hpp>

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
	class ShadowMap
		: public Castor::OwnedBy< Engine >
	{
	protected:
		using ShadowMapLightMap = std::map< LightRPtr, ShadowMapPassSPtr >;
		using SortedPasses = std::map< double, ShadowMapPassSPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API ShadowMap( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMap();
		/**
		 *\~english
		 *\brief		Initialises the frame buffer and light type specific data.
		 *\param[in]	p_size	The wanted frame buffer dimensions.
		 *\~french
		 *\brief		Initialise le frame buffer et les données spécifiques au type de source lumineuse.
		 *\param[in]	p_size	Les dimensions voulues pour le frame buffer.
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer and light type specific data.
		 *\~french
		 *\brief		Nettoie le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Adds a light source, creating a shadow map pass for it.
		 *\param[in]	p_light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse, créant une passe de shadow mapping pour elle.
		 *\param[in]	p_light	La source lumineuse.
		 */
		C3D_API void AddLight( Light & p_light );
		/**
		 *\copydoc		Castor3D::RenderPass::DoUpdateFlags
		 */
		C3D_API void UpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags
			, SceneFlags & p_sceneFlags )const;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetVertexShaderSource
		 */
		C3D_API Castor::String GetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetGeometryShaderSource
		 */
		C3D_API Castor::String GetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const;
		/**
		 *\copydoc		Castor3D::RenderPass::DoGetPixelShaderSource
		 */
		C3D_API Castor::String GetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const;
		/**
		 *\~english
		 *\return		The sorted shadow mapping passes.
		 *\~english
		 *\return		Les passes de shadow mapping triées.
		 */
		inline SortedPasses & GetPasses()
		{
			return m_sorted;
		}
		/**
		 *\~english
		 *\return		The sorted shadow mapping passes.
		 *\~english
		 *\return		Les passes de shadow mapping triées.
		 */
		inline SortedPasses const & GetPasses()const
		{
			return m_sorted;
		}

	private:
		/**
		 *\~english
		 *\return		The maximum light type specific shadow map passes.
		 *\~english
		 *\return		Le nombre maximum de passes de shadow maps pour le type de source lumineuse.
		 */
		C3D_API virtual int32_t DoGetMaxPasses()const = 0;
		/**
		 *\~english
		 *\return		The shadow map texture dimensions.
		 *\~english
		 *\return		Les dimensions de la texture de map d'ombres.
		 */
		C3D_API virtual Castor::Size DoGetSize()const = 0;
		/**
		 *\~english
		 *\brief		Initialises the light type specific data.
		 *\param[in]	p_size	The wanted frame buffer dimensions.
		 *\~french
		 *\brief		Initialise les données spécifiques au type de source lumineuse.
		 *\param[in]	p_size	Les dimensions voulues pour le frame buffer.
		 */
		C3D_API virtual void DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the light type specific data.
		 *\~french
		 *\brief		Nettoie les données spécifiques au type de source lumineuse.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Creates a light type specific shadow map pass.
		 *\param[in]	p_light	The light source.
		 *\return		The shadow map pass.
		 *\~french
		 *\brief		Crée une passe de shadow mapping spécifique au type de source lumineuse.
		 *\param[in]	p_light	La source lumineuse.
		 *\return		La passe de shadow mapping.
		 */
		C3D_API virtual ShadowMapPassSPtr DoCreatePass( Light & p_light )const = 0;
		/**
		 *\copydoc		Castor3D::ShadowMap::UpdateFlags
		 */
		C3D_API virtual void DoUpdateFlags( TextureChannels & p_textureFlags
			, ProgramFlags & p_programFlags
			, SceneFlags & p_sceneFlags )const = 0;
		/**
		 *\copydoc		Castor3D::ShadowMap::GetVertexShaderSource
		 */
		C3D_API virtual Castor::String DoGetVertexShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags
			, bool p_invertNormals )const;
		/**
		 *\copydoc		Castor3D::ShadowMap::GetGeometryShaderSource
		 */
		C3D_API virtual Castor::String DoGetGeometryShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const;
		/**
		 *\copydoc		Castor3D::ShadowMap::GetPixelShaderSource
		 */
		C3D_API virtual Castor::String DoGetPixelShaderSource( TextureChannels const & p_textureFlags
			, ProgramFlags const & p_programFlags
			, SceneFlags const & p_sceneFlags )const = 0;

	protected:
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The geometry buffer.
		//!\~french		Les tampons de géométrie.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	The shadow mapping passes used during the render.
		//!\~french		Les passes de shadow mapping utilisées pendant le rendu.
		ShadowMapLightMap m_passes;
		//!~english		The shadow mapping passes, sorted by light source's distance to the camera.
		//!\~french		Les passes de shadow mapping, triées par distance de la source lumineuse à la caméra.
		SortedPasses m_sorted;
	};
}

#endif
