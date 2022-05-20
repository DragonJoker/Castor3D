/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshAnimationInstanceSubmesh_H___
#define ___C3D_MeshAnimationInstanceSubmesh_H___

#include "MeshAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	class MeshAnimationInstanceSubmesh
		: public castor::OwnedBy< MeshAnimationInstance >
	{
	public:
		/**
		*\~english
		*name Copy / Move.
		*\~french
		*name Copie / Déplacement.
		**/
		/**@{*/
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstanceSubmesh && rhs ) = default;
		C3D_API MeshAnimationInstanceSubmesh & operator=( MeshAnimationInstanceSubmesh && rhs ) = delete;
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstanceSubmesh const & rhs ) = delete;
		C3D_API MeshAnimationInstanceSubmesh & operator=( MeshAnimationInstanceSubmesh const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animationInstance	The parent skeleton animation instance.
		 *\param[in]	animationObject		The animation object.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	animationObject		L'animation d'objet.
		 */
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstance & animationInstance
			, MeshAnimationSubmesh & animationObject );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~MeshAnimationInstanceSubmesh() = default;
		/**
		 *\~english
		 *\brief		Updates the object, given to animation buffers.
		 *\param[in]	factor	The percentage between \p prv and \p cur.
		 *\param[in]	prv		The previous animation buffer (factor 0).
		 *\param[in]	cur		The current animation buffer (factor 1).
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné.
		 *\param[in]	factor	Le pourcentage entre \p prv et \p cur.
		 *\param[in]	prv		Le tampon d'animation précédent (pourcentage 0).
		 *\param[in]	cur		Le tampon d'animation courant (pourcentage 1).
		 */
		C3D_API void update( float factor
			, std::vector< float > const & prv
			, std::vector< float > const & cur
			, castor::BoundingBox const & prvbb
			, castor::BoundingBox const & curbb );
		/**
		 *\~english
		 *\brief		Moves the object back to initial state.
		 *\~french
		 *\brief		Remet l'objet à l'état initial.
		 */
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		The submesh.
		 *\~french
		 *\brief		Le sous-maillage.
		 */
		C3D_API Submesh const & getSubmesh()const;
		/**
		 *\~english
		 *\brief		The current weights.
		 *\~french
		 *\brief		Les poids courants.
		 */
		std::vector< float > getWeights()const
		{
			return m_cur;
		}

	protected:
		//!\~english	The animation object.
		//!\~french		L'animation d'objet.
		MeshAnimationSubmesh & m_animationObject;
		//!\~english	The current animation buffer.
		//!\~french		Le tampon d'animation actuel.
		std::vector< float > m_cur;
	};
}

#endif
