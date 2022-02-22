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

		bool isPlayingAnimation()const override
		{
			return m_playingAnimation != nullptr;
		}

		Mesh & getMesh()const
		{
			return m_mesh;
		}

		Geometry & getGeometry()const
		{
			return m_geometry;
		}

		uint32_t getId()const
		{
			return m_id;
		}

		void setId( uint32_t id )
		{
			m_id = id;
		}

		MeshAnimationInstance & getPlayingAnimation()const
		{
			return *m_playingAnimation;
		}

	private:
		void doAddAnimation( castor::String const & name )override;
		void doStartAnimation( AnimationInstance & animation )override;
		void doStopAnimation( AnimationInstance & animation )override;
		void doClearAnimations()override;

	protected:
		Mesh & m_mesh;
		Geometry & m_geometry;
		MeshAnimationInstanceRPtr m_playingAnimation{ nullptr };
		uint32_t m_id{};
	};
}

#endif

