/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_MESH_H___
#define ___C3D_ANIMATED_MESH_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Mesh/MeshAnimationModule.hpp"

namespace castor3d
{
	class AnimatedMesh
		: public AnimatedObject
	{
	public:
		/**
		 *\~english
		 *name Copy / Move.
		 *\~french
		 *name Copie / Déplacement.
		 **/
		/**@{*/
		C3D_API AnimatedMesh( AnimatedMesh && rhs ) = default;
		C3D_API AnimatedMesh & operator=( AnimatedMesh && rhs ) = delete;
		C3D_API AnimatedMesh( AnimatedMesh const & rhs ) = delete;
		C3D_API AnimatedMesh & operator=( AnimatedMesh const & rhs ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The object name.
		 *\param[in]	mesh		The mesh.
		 *\param[in]	geometry	The geometry instantiating the mesh.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom de l'objet.
		 *\param[in]	mesh		Le maillage.
		 *\param[in]	geometry	La géométrie instanciant le maillage.
		 */
		C3D_API AnimatedMesh( castor::String const & name
			, Mesh & mesh
			, Geometry & geometry );
		/**
		 *\copydoc		castor3d::AnimatedObject::update
		 */
		C3D_API void update( castor::Milliseconds const & elapsed )override;
		/**
		 *\copydoc		castor3d::AnimatedObject::isPlayingAnimation
		 */
		C3D_API bool isPlayingAnimation()const override
		{
			return m_playingAnimation != nullptr;
		}
		/**
		 *\~english
		 *\return		The mesh.
		 *\~french
		 *\return		Le maillage.
		 */
		Mesh const & getMesh()const
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The mesh.
		 *\~french
		 *\return		Le maillage.
		 */
		Mesh & getMesh()
		{
			return m_mesh;
		}
		/**
		 *\~english
		 *\return		The geometry instantiating the mesh.
		 *\~french
		 *\return		La géométrie instanciant le maillage.
		 */
		Geometry const & getGeometry()const
		{
			return m_geometry;
		}
		/**
		 *\~english
		 *\return		The geometry instantiating the mesh.
		 *\~french
		 *\return		La géométrie instanciant le maillage.
		 */
		Geometry & getGeometry()
		{
			return m_geometry;
		}
		/**
		 *\~english
		 *\return		The currently playing animation for this object.
		 *\~french
		 *\return		L'animation en cours de lecture sur cet objet.
		 */
		MeshAnimationInstance const & getPlayingAnimation()const
		{
			return *m_playingAnimation;
		}

	private:
		void doAddAnimation( castor::String const & name )override;
		void doStartAnimation( AnimationInstance & animation )override;
		void doStopAnimation( AnimationInstance & animation )override;
		void doClearAnimations()override;

	protected:
		//!\~english	The submesh affected by the animations.
		//!\~french		Le sous-maillage affecté par les animations.
		Mesh & m_mesh;
		//!\~english	The geometry instantiating the mesh.
		//!\~french		La géométrie instanciant le maillage.
		Geometry & m_geometry;
		//!\~english	Currently playing animation.
		//!\~french		L'animation en cours de lecture.
		MeshAnimationInstanceRPtr m_playingAnimation{ nullptr };
	};
}

#endif

