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

namespace castor3d
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
		: public castor::OwnedBy< Engine >
	{
	protected:
		using ShadowMapLightMap = std::map< Light const *, ShadowMapPassSPtr >;
		using SortedPasses = std::map< double, ShadowMapPassSPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ShadowMap( Engine & engine );
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
		 *\~french
		 *\brief		Initialise le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer and light type specific data.
		 *\~french
		 *\brief		Nettoie le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		adds a light source, creating a shadow map pass for it.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse, créant une passe de shadow mapping pour elle.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void addLight( Light & light );
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Une combinaison de SceneFlag.
		 */
		C3D_API void updateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\param[in]	programFlags	Combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API GLSL::Shader getVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API GLSL::Shader getGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API GLSL::Shader getPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;
		/**
		 *\~english
		 *\return		The sorted shadow mapping passes.
		 *\~english
		 *\return		Les passes de shadow mapping triées.
		 */
		inline SortedPasses & getPasses()
		{
			return m_sorted;
		}
		/**
		 *\~english
		 *\return		The sorted shadow mapping passes.
		 *\~english
		 *\return		Les passes de shadow mapping triées.
		 */
		inline SortedPasses const & getPasses()const
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
		C3D_API virtual int32_t doGetMaxPasses()const = 0;
		/**
		 *\~english
		 *\return		The shadow map texture dimensions.
		 *\~english
		 *\return		Les dimensions de la texture de map d'ombres.
		 */
		C3D_API virtual castor::Size doGetSize()const = 0;
		/**
		 *\~english
		 *\brief		Initialises the light type specific data.
		 *\param[in]	p_size	The wanted frame buffer dimensions.
		 *\~french
		 *\brief		Initialise les données spécifiques au type de source lumineuse.
		 *\param[in]	p_size	Les dimensions voulues pour le frame buffer.
		 */
		C3D_API virtual void doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the light type specific data.
		 *\~french
		 *\brief		Nettoie les données spécifiques au type de source lumineuse.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Creates a light type specific shadow map pass.
		 *\param[in]	light	The light source.
		 *\return		The shadow map pass.
		 *\~french
		 *\brief		Crée une passe de shadow mapping spécifique au type de source lumineuse.
		 *\param[in]	light	La source lumineuse.
		 *\return		La passe de shadow mapping.
		 */
		C3D_API virtual ShadowMapPassSPtr doCreatePass( Light & light )const = 0;
		/**
		 *\copydoc		castor3d::ShadowMap::updateFlags
		 */
		C3D_API virtual void doUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const = 0;
		/**
		 *\copydoc		castor3d::ShadowMap::getVertexShaderSource
		 */
		C3D_API virtual GLSL::Shader doGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\copydoc		castor3d::ShadowMap::getGeometryShaderSource
		 */
		C3D_API virtual GLSL::Shader doGetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\copydoc		castor3d::ShadowMap::getPixelShaderSource
		 */
		C3D_API virtual GLSL::Shader doGetPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const = 0;

		void doApplyAlphaFunc( GLSL::GlslWriter & writer
			, ComparisonFunc alphaFunc
			, GLSL::Float const & alpha
			, GLSL::Int const & material
			, GLSL::Materials const & materials );

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
