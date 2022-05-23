/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshMorphTarget_H___
#define ___C3D_MeshMorphTarget_H___

#include "MeshAnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Binary/BinaryParser.hpp"

namespace castor3d
{
	class MeshMorphTarget
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
		C3D_API MeshMorphTarget( MeshAnimation & parent
			, castor::Milliseconds const & timeIndex );
		/**
		 *\~english
		 *\brief		Adds a submesh buffer.
		 *\~french
		 *\brief		Ajoute un tampon de sous-maillage.
		 */
		C3D_API void setTargetsWeights( Submesh const & submesh
			, std::vector< float > weights );
		/**
		 *\~english
		 *\brief		Adds a submesh buffer.
		 *\~french
		 *\brief		Ajoute un tampon de sous-maillage.
		 */
		C3D_API void setTargetWeight( Submesh const & submesh
			, uint32_t targetIndex
			, float targetWeight );
		/**
		 *\~english
		 *\return		The submesh buffer matching given submesh.
		 *\~french
		 *\return		Le tampon de sous-maillage correspondant au sous-maillage donné.
		 */
		C3D_API SubmeshTargetWeightMap::const_iterator find( Submesh const & submesh )const;
		/**
		 *\~english
		 *\return		The beginning of the submeshes buffers.
		 *\~french
		 *\return		Le début des tampons des sous-maillages.
		 */
		SubmeshTargetWeightMap::const_iterator begin()const
		{
			return m_submeshesTargets.begin();
		}
		/**
		 *\~english
		 *\return		The beginning of the submeshes buffers.
		 *\~french
		 *\return		Le début des tampons des sous-maillages.
		 */
		SubmeshTargetWeightMap::iterator begin()
		{
			return m_submeshesTargets.begin();
		}
		/**
		 *\~english
		 *\return		The end of the submeshes buffers.
		 *\~french
		 *\return		La fin des tampons des sous-maillages.
		 */
		SubmeshTargetWeightMap::const_iterator end()const
		{
			return m_submeshesTargets.end();
		}
		/**
		 *\~english
		 *\return		The end of the submeshes buffers.
		 *\~french
		 *\return		La fin des tampons des sous-maillages.
		 */
		SubmeshTargetWeightMap::iterator end()
		{
			return m_submeshesTargets.end();
		}

	private:
		void doSetTimeIndex( castor::Milliseconds const & time )
		{
			m_timeIndex = time;
		}

	private:
		//!\~english	The buffers, per submesh.
		//!\~french		Les tampons, par sous-maillage.
		SubmeshTargetWeightMap m_submeshesTargets;

		friend class BinaryParser< MeshMorphTarget >;
	};
}

#endif

