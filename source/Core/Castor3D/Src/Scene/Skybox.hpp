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
#include "Shader/UniformBuffer.hpp"

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Scene loader
		\~english
		\brief		Loader de scène
		*/
		class TextWriter
			: public Castor::TextWriter< Skybox >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a Skybox into a text file
			 *\param[in]	p_obj	the Skybox to save
			 *\param[in]	p_file	the file to write the Skybox in
			 *\~french
			 *\brief		Ecrit une Skybox dans un fichier texte
			 *\param[in]	p_obj	La Skybox
			 *\param[in]	p_file	Le fichier
			 */
			C3D_API bool operator()( Skybox const & p_obj, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit Skybox( Engine & p_engine );
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
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Renders the skybox.
		 *\param[in]	p_camera	The scene's camera.
		 *\~french
		 *\brief		Dessine la skybox.
		 *\param[in]	p_camera	La caméra de la scène.
		 */
		C3D_API void Render( Camera const & p_camera );
		/**
		 *\~english
		 *\return		The skybox's texture.
		 *\~french
		 *\return		La texture de la skybox.
		 */
		inline TextureLayout & GetTexture()
		{
			return *m_texture;
		}
		/**
		 *\~english
		 *\return		Sets the skybox's texture.
		 *\~french
		 *\return		Définit la texture de la skybox.
		 */
		inline void SetTexture( TextureLayoutSPtr p_texture )
		{
			m_texture = p_texture;
		}

	private:
		ShaderProgram & DoInitialiseShader();
		bool DoInitialiseTexture();
		bool DoInitialiseVertexBuffer();
		bool DoInitialisePipeline( ShaderProgram & p_program );

	private:
		//!\~english	The pipeline used while rendering the skybox.
		//!\~french		Le pipeline utilisé pour le rendu de la skybox.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The skybox cube map texture.
		//!\~french		La texture cube map de la skybox.
		TextureLayoutSPtr m_texture;
		//!\~english	The skybox cube map sampler.
		//!\~french		L'échantillonneur de la cube map de la skybox.
		SamplerWPtr m_sampler;
		//!\~english	The shader matrices constants buffer.
		//!\~french		Le tampon de constantes de shader contenant les matrices.
		UniformBuffer m_matricesBuffer;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer{ nullptr };
		//!\~english	The geomtry buffers.
		//!\~french		Les tampons de géométrie.
		GeometryBuffersSPtr m_geometryBuffers{ nullptr };
		//!\~english	Vertex elements declaration.
		//!\~french		Déclaration des éléments d'un sommet.
		Castor3D::BufferDeclaration m_declaration;
		//!\~english	Vertex array (cube definition).
		//!\~french		Tableau de vertex (définition du cube).
		std::array< Castor3D::BufferElementGroupSPtr, 36 > m_arrayVertex;
		//! 6 * 6 * [3(vertex position)].
		std::array< Castor::real, 108 > m_bufferVertex;
		//!\~english	The model matrix.
		//!\~french		La matrice modèle.
		Castor::Matrix4x4r m_mtxModel;
	};
}

#endif
