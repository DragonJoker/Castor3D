/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things.
	\remark		Manages translation, scaling, rotation of the thing.
	\~french
	\brief		Classe de représentation de choses mouvantes.
	\remark		Gère les translations, mises à l'échelle, rotations de la chose.
	*/
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
		C3D_API MeshAnimationInstanceSubmesh & operator=( MeshAnimationInstanceSubmesh && rhs ) = default;
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
		C3D_API virtual ~MeshAnimationInstanceSubmesh();
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
			, SubmeshAnimationBuffer const & prv
			, SubmeshAnimationBuffer const & cur );
		/**
		 *\~english
		 *\brief		The submesh.
		 *\~french
		 *\brief		Le sous-maillage.
		 */
		C3D_API Submesh const & getSubmesh()const;
		/**
		 *\~english
		 *\brief		The current factor.
		 *\~french
		 *\brief		Le facteur courant.
		 */
		inline float getCurrentFactor()const
		{
			return m_currentFactor;
		}

	protected:
		//!\~english	The animation object.
		//!\~french		L'animation d'objet.
		MeshAnimationSubmesh & m_animationObject;
		//!\~english	The time elapsed since the last keyframe.
		//!\~french		Le temps écoulé depuis la dernière keyframe.
		float m_currentFactor{ 0.0f };
		//!\~english	The current animation buffer.
		//!\~french		Le tampon d'animation actuel.
		SubmeshAnimationBuffer const * m_cur{ nullptr };
	};
}

#endif
