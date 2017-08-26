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
#ifndef ___C3D_SKYBOX_H___
#define ___C3D_SKYBOX_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "PBR/IblTextures.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		Skybox implementation.
	\~french
	\brief		Implémentation de Skybox.
	*/
	class Skybox
		: public castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Skybox loader.
		\~english
		\brief		Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< Skybox >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a Skybox into a text file.
			 *\param[in]	obj		The Skybox to save.
			 *\param[in]	file	The file to write the Skybox in.
			 *\~french
			 *\brief		Ecrit une Skybox dans un fichier texte.
			 *\param[in]	obj		La Skybox.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( Skybox const & obj, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit Skybox( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Skybox();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void cleanup();
		/**
		 *\~english
		 *\brief		Renders the skybox.
		 *\param[in]	camera	The scene's camera.
		 *\~french
		 *\brief		Dessine la skybox.
		 *\param[in]	camera	La caméra de la scène.
		 */
		C3D_API virtual void render( Camera const & camera );
		/**
		*\~english
		*\return		sets the skybox's equirectangular texture.
		*\~french
		*\return		Définit la texture équirectangulaire de la skybox.
		*/
		C3D_API void setEquiTexture( TextureLayoutSPtr texture
			, castor::Size const & size );
		/**
		 *\~english
		 *\return		The skybox's equirectangular texture path.
		 *\~french
		 *\return		Le chemin de l'image équirectangulaire de la skybox.
		 */
		inline castor::Path const & getEquiTexturePath()const
		{
			return m_equiTexturePath;
		}
		/**
		 *\~english
		 *\return		The skybox's texture.
		 *\~french
		 *\return		La texture de la skybox.
		 */
		inline TextureLayout & getTexture()
		{
			REQUIRE( m_texture );
			return *m_texture;
		}
		/**
		 *\~english
		 *\return		The skybox's texture.
		 *\~french
		 *\return		La texture de la skybox.
		 */
		inline TextureLayoutSPtr getTexture()const
		{
			return m_texture;
		}
		/**
		 *\~english
		 *\return		The skybox's IBL textures.
		 *\~french
		 *\return		Les texture d'IBL de la skybox.
		 */
		inline IblTextures const & getIbl()const
		{
			REQUIRE( m_ibl );
			return *m_ibl;
		}
		/**
		 *\~english
		 *\return		sets the skybox's texture.
		 *\~french
		 *\return		Définit la texture de la skybox.
		 */
		inline void setTexture( TextureLayoutSPtr texture )
		{
			m_texture = texture;
		}
		/**
		 *\~english
		 *\return		sets the skybox's scene.
		 *\~french
		 *\return		Définit la scène de la skybox.
		 */
		inline void setScene( Scene & scene )
		{
			m_scene = &scene;
		}

	protected:
		virtual ShaderProgram & doInitialiseShader();
		bool doInitialiseTexture();
		void doInitialiseEquiTexture();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( ShaderProgram & program );

	protected:
		//!\~english	The skybox's scene.
		//!\~french		La scène de la skybox.
		SceneRPtr m_scene{ nullptr };
		//!\~english	The pipeline used while rendering the skybox.
		//!\~french		Le pipeline utilisé pour le rendu de la skybox.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The skybox equirectangular map texture.
		//!\~french		La texture équirectangulaire de la skybox.
		TextureLayoutSPtr m_equiTexture;
		//!\~english	The skybox equirectangular image path.
		//!\~french		Le chemin de l'image équirectangulaire de la skybox.
		castor::Path m_equiTexturePath;
		//!\~english	The skybox equirectangular map texture wanted face size.
		//!\~french		La taille voulue pour les faces de la texture équirectangulaire de la skybox.
		castor::Size m_equiSize;
		//!\~english	The skybox cube map texture.
		//!\~french		La texture cube map de la skybox.
		TextureLayoutSPtr m_texture;
		//!\~english	The skybox cube map sampler.
		//!\~french		L'échantillonneur de la cube map de la skybox.
		SamplerWPtr m_sampler;
		//!\~english	The shader matrices constants buffer.
		//!\~french		Le tampon de constantes de shader contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		ModelMatrixUbo m_modelMatrixUbo;
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		HdrConfigUbo m_configUbo;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer{ nullptr };
		//!\~english	The geomtry buffers.
		//!\~french		Les tampons de géométrie.
		GeometryBuffersSPtr m_geometryBuffers{ nullptr };
		//!\~english	Vertex elements declaration.
		//!\~french		Déclaration des éléments d'un sommet.
		castor3d::BufferDeclaration m_declaration;
		//!\~english	Vertex array (cube definition).
		//!\~french		Tableau de vertex (définition du cube).
		std::array< castor3d::BufferElementGroupSPtr, 36 > m_arrayVertex;
		//! 6 * 6 * [3(vertex position)].
		std::array< castor::real, 108 > m_bufferVertex;
		//!\~english	The model matrix.
		//!\~french		La matrice modèle.
		castor::Matrix4x4r m_mtxModel;
		//!\~english	The IBL textures.
		//!\~french		Les textures l'IBL.
		std::unique_ptr< IblTextures > m_ibl;
		//!\~english	Tells if the skybox's texture is HDR.
		//!\~french		Dit si la texture de la skybox est HDR.
		bool m_hdr{ false };
	};
}

#endif
