/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_SKYBOX_H___
#define ___C3D_SKYBOX_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"

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
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_camera	The scene's camera.
		 *\~french
		 *\brief		Dessine la skybox.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_camera	La caméra de la scène.
		 */
		C3D_API void Render( Camera const & p_camera, Pipeline & p_pipeline );
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

	private:
		bool DoInitialiseShader();
		bool DoInitialiseVertexBuffer();
		bool DoInitialiseTexture();

	private:
		//!\~english	The depth stencil state used while rendering the skybox.
		//!\~french		L'état de profondeur et stencil utilisé pour le rendu de la skybox.
		DepthStencilStateWPtr m_dss;
		//!\~english	The rasteriser state used while rendering the skybox.
		//!\~french		L'état de rastérisation utilisé pour le rendu de la skybox.
		RasteriserStateWPtr m_rs;
		//!\~english	The skybox cube map texture.
		//!\~french		La texture cube map de la skybox.
		TextureLayoutSPtr m_texture;
		//!\~english	The skybox cube map sampler.
		//!\~french		L'échantillonneur de la cube map de la skybox.
		SamplerWPtr m_sampler;
		//!\~english	The skybox shader.
		//!\~french		Le shader de la skybox.
		ShaderProgramWPtr m_program;
		//!\~english	The shader matrices constants buffer.
		//!\~french		Le tampon de constantes de shader contenant les matrices.
		FrameVariableBufferSPtr m_matricesBuffer;
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
