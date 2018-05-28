/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationKeyFrame_H___
#define ___C3D_MeshAnimationKeyFrame_H___

#include "Animation/AnimationKeyFrame.hpp"
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
}

#endif

