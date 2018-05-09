/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationKeyFrame_H___
#define ___C3D_MeshAnimationKeyFrame_H___

#include "Animation/AnimationKeyFrame.hpp"

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Mesh/Submesh.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		07/12/2017
	\~english
	\brief		A keyframe for a mesh animation.
	\~french
	\brief		Une keyframe pour une animation de maillage.
	*/
	class MeshAnimationKeyFrame
		: public AnimationKeyFrame
		, public castor::OwnedBy< MeshAnimation >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent		The parent animation.
		 *\param[in]	timeIndex	When the key frame starts.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent		L'animation parente.
		 *\param[in]	timeIndex	Quand la key frame commence.
		 */
		C3D_API MeshAnimationKeyFrame( MeshAnimation & parent
			, castor::Milliseconds const & timeIndex = 0_ms );
		/**
		 *\~english
		 *\brief		Adds a submesh buffer.
		 *\~french
		 *\brief		Ajoute un tampon de sous-maillage.
		 */
		C3D_API void addSubmeshBuffer( Submesh const & submesh
			, InterleavedVertexArray const & buffer );
		/**
		 *\~english
		 *\return		The submesh buffer matching given submesh.
		 *\~french
		 *\return		Le tampon de sous-maillage correspondant au sous-maillage donné.
		 */
		inline SubmeshAnimationBufferMap::const_iterator find( Submesh const & submesh )const
		{
			return m_submeshesBuffers.find( submesh.getId() );
		}
		/**
		 *\~english
		 *\return		The beginning of the submeshes buffers.
		 *\~french
		 *\return		Le début des tampons des sous-maillages.
		 */
		inline SubmeshAnimationBufferMap::const_iterator begin()const
		{
			return m_submeshesBuffers.begin();
		}
		/**
		 *\~english
		 *\return		The end of the submeshes buffers.
		 *\~french
		 *\return		La fin des tampons des sous-maillages.
		 */
		inline SubmeshAnimationBufferMap::const_iterator end()const
		{
			return m_submeshesBuffers.end();
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		//!\~english	The buffers, per submesh.
		//!\~french		Les tampons, par sous-maillage.
		SubmeshAnimationBufferMap m_submeshesBuffers;

		friend class BinaryParser< MeshAnimationKeyFrame >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		Helper structure to find ChunkType from a type.
	\remarks	Specialisation for MeshAnimationKeyFrame.
	\~french
	\brief		Classe d'aide pour récupéer un ChunkType depuis un type.
	\remarks	Spécialisation pour MeshAnimationKeyFrame.
	*/
	template<>
	struct ChunkTyper< MeshAnimationKeyFrame >
	{
		static ChunkType const Value = ChunkType::eMeshAnimationKeyFrame;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		MeshAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationKeyFrame.
	*/
	template<>
	class BinaryWriter< MeshAnimationKeyFrame >
		: public BinaryWriterBase< MeshAnimationKeyFrame >
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
		C3D_API bool doWrite( MeshAnimationKeyFrame const & obj )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date 		07/12/2017
	\~english
	\brief		MeshAnimationKeyFrame binary loader.
	\~english
	\brief		Loader binaire de MeshAnimationKeyFrame.
	*/
	template<>
	class BinaryParser< MeshAnimationKeyFrame >
		: public BinaryParserBase< MeshAnimationKeyFrame >
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
		C3D_API bool doParse( MeshAnimationKeyFrame & obj )override;
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
		C3D_API bool doParse_v1_3( MeshAnimationKeyFrame & obj )override;
	};
}

#endif

