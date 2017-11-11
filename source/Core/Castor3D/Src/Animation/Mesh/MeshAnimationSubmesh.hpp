/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_SUBMESH_H___
#define ___C3D_MESH_ANIMATION_SUBMESH_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

namespace castor3d
{
	class MorphComponent;
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
		: public castor::OwnedBy< MeshAnimation >
		, public std::enable_shared_from_this< MeshAnimationSubmesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\param[in]	submesh		The submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 *\param[in]	submesh		Le sous-maillage.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimation & animation, Submesh & submesh );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh && rhs ) = default;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationSubmesh( MeshAnimationSubmesh const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API MeshAnimationSubmesh & operator=( MeshAnimationSubmesh const & rhs ) = delete;
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
		 *\param[in]	timeIndex	The time index.
		 *\param[in]	buffer		The submesh buffer.
		 *\~french
		 *\brief		Crée et ajoute un tampon de sous-maillage d'animation.
		 *\param[in]	timeIndex	L'index de temps.
		 *\param[in]	buffer		Le tampon du sous-maillage.
		 */
		C3D_API bool addBuffer( castor::Milliseconds const & timeIndex
			, InterleavedVertexArray && buffer );
		/**
		 *\~english
		 *\return		The vertex buffers.
		 *\~french
		 *\return		Les tampons de sommets.
		 */
		inline SubmeshAnimationBufferArray const & getBuffers()const
		{
			return m_buffers;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh & getSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The submesh.
		 *\~french
		 *\return		Le sous-maillage.
		 */
		inline Submesh const & getSubmesh()const
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\return		The animation length.
		 *\~french
		 *\return		La durée de l'animation.
		 */
		inline castor::Milliseconds getLength()const
		{
			return m_length;
		}
		/**
		 *\~english
		 *\brief		sets the animation length.
		 *\param[in]	length	The new value.
		 *\~french
		 *\brief		Définit la durée de l'animation.
		 *\param[in]	length	La nouvelle valeur.
		 */
		inline void setLength( castor::Milliseconds const & length )
		{
			m_length = length;
		}
		/**
		 *\~english
		 *\brief		Tells whether or not the object has keyframes.
		 *\return		\p false if no keyframes.
		 *\~french
		 *\brief		Dit si l'objet a des keyframes.
		 *\return		\p false si pas de keyframes.
		 */
		inline bool hasBuffers()const
		{
			return !m_buffers.empty();
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		inline MorphComponent const & getComponent()const
		{
			return *m_component;
		}
		/**
		 *\~english
		 *\return		The bones component.
		 *\~french
		 *\return		Le composant des os.
		 */
		inline MorphComponent & getComponent()
		{
			return *m_component;
		}

	protected:
		//!\~english	The animation length.
		//!\~french		La durée de l'animation.
		castor::Milliseconds m_length{ 0 };
		//!\~english	The buffers.
		//!\~french		Les tampons.
		SubmeshAnimationBufferArray m_buffers;
		//!\~english	The submesh.
		//!\~french		Le sous-maillage.
		Submesh & m_submesh;
		//!\~english	The bones component.
		//!\~french		Le composant des os.
		std::shared_ptr< MorphComponent > m_component;

		friend class BinaryWriter< MeshAnimationSubmesh >;
		friend class BinaryParser< MeshAnimationSubmesh >;
		friend class MeshAnimationInstanceSubmesh;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date 		28/05/2016
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for MeshAnimationSubmesh.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour MeshAnimationSubmesh.
	*/
	template<>
	struct ChunkTyper< MeshAnimationSubmesh >
	{
		static ChunkType const Value = ChunkType::eMeshAnimationSubmesh;
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
		 *\param[in]	obj	The object to write.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques.
		 *\param[in]	obj	L'objet à écrire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doWrite( MeshAnimationSubmesh const & obj )override;
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
		 *\param[out]	obj	The object to read.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk.
		 *\param[out]	obj	L'objet à lire.
		 *\return		\p false si une erreur quelconque est arrivée.
		 */
		C3D_API bool doParse( MeshAnimationSubmesh & obj )override;
	};
}

#endif
