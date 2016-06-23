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
#ifndef ___C3D_MESH_ANIMATION_SUBMESH_H___
#define ___C3D_MESH_ANIMATION_SUBMESH_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Class which represents the mesh animation submeshes buffers.
	\~french
	\brief		Classe de représentation des tampons de sous-maillages d'animations de maillage.
	*/
	struct SubmeshAnimationBuffer
	{
		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		real m_timeIndex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		InterleavedVertexArray m_buffer;
	};
	using SubmeshAnimationBufferArray = std::vector< SubmeshAnimationBuffer >;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Class which represents the mesh animation submeshes.
	\~french
	\brief		Classe de représentation de sous-maillages d'animations de maillage.
	*/
	class MeshAnimationSubmesh
		: public Castor::OwnedBy< MeshAnimation >
		, public std::enable_shared_from_this< MeshAnimationSubmesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animation	The parent animation.
		 *\param[in]	p_type		The skeleton animation object type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animation	L'animation parente.
		 *\param[in]	p_type		Le type d'objet d'animation de squelette.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimation & p_animation, Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~MeshAnimationSubmesh();
		/**
		 *\~english
		 *\brief		Creates and adds an animation submesh buffer.
		 *\param[in]	p_timeIndex		The time index.
		 *\param[in]	p_buffer		The submesh buffer.
		 *\~french
		 *\brief		Crée et ajoute un tampon de sous-maillage d'animation.
		 *\param[in]	p_timeIndex		L'index de temps.
		 *\param[in]	p_buffer		Le tampon du sous-maillage.
		 */
		C3D_API bool AddBuffer( real p_timeIndex, InterleavedVertexArray && p_buffer );
		/**
		 *\~english
		 *\return		The vertex buffers.
		 *\~french
		 *\return		Les tampons de sommets.
		 */
		inline SubmeshAnimationBufferArray const & GetBuffers()const
		{
			return m_buffers;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh & GetSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh const & GetSubmesh()const
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The animation length.
		 *\~french
		 *\return		La durée de l'animation.
		 */
		inline real GetLength()const
		{
			return m_length;
		}
		/**
		 *\~english
		 *\brief		Sets the animation length.
		 *\param[in]	p_length	The new value.
		 *\~french
		 *\brief		Définit la durée de l'animation.
		 *\param[in]	p_length	La nouvelle valeur.
		 */
		inline void	SetLength( real p_length )
		{
			m_length = p_length;
		}
		/**
		 *\~english
		 *\brief		Tells whether or not the object has keyframes.
		 *\return		\p false if no keyframes.
		 *\~french
		 *\brief		Dit si l'objet a des keyframes.
		 *\return		\p false si pas de keyframes.
		 */
		inline bool HasBuffers()const
		{
			return !m_buffers.empty();
		}

	protected:
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		real m_length{ 0.0_r };
		//!\~english	The buffers.
		//!\~french		Les tampons.
		SubmeshAnimationBufferArray m_buffers;
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;

		friend class BinaryWriter< MeshAnimationSubmesh >;
		friend class BinaryParser< MeshAnimationSubmesh >;
		friend class MeshAnimationInstanceSubmesh;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find eCHUNK_TYPE from a type.
	\remarks	Specialisation for MeshAnimationSubmesh.
	\~french
	\brief		Classe d'aide pour récupéer un eCHUNK_TYPE depuis un type.
	\remarks	Spécialisation pour MeshAnimationSubmesh.
	*/
	template<>
	struct ChunkTyper< MeshAnimationSubmesh >
	{
		static eCHUNK_TYPE const Value = eCHUNK_TYPE_MESH_ANIMATION_SUBMESH;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MeshAnimationSubmesh binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationSubmesh.
	*/
	template<>
	class BinaryWriter< MeshAnimationSubmesh >
		: public BinaryWriterBase< MeshAnimationSubmesh >
	{
	protected:
		/**
		 *\~english
		 *\brief		Function used to fill the chunk from specific data.
		 *\param[in]	p_obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	p_obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoWrite( MeshAnimationSubmesh const & p_obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		26/01/2016
	\~english
	\brief		MeshAnimationSubmesh binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationSubmesh.
	*/
	template<>
	class BinaryParser< MeshAnimationSubmesh >
		: public BinaryParserBase< MeshAnimationSubmesh >
	{
	private:
		/**
		 *\~english
		 *\brief		Function used to retrieve specific data from the chunk.
		 *\param[out]	p_obj	The object to read.
		 *\param[in]	p_chunk	The chunk containing data.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	p_obj	L'objet à lire.
		 *\param[in]	p_chunk	Le chunk contenant les données.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool DoParse( MeshAnimationSubmesh & p_obj )override;
	};
}

#endif
