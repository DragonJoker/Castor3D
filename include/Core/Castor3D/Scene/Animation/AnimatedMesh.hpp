/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ANIMATED_MESH_H___
#define ___C3D_ANIMATED_MESH_H___

#include "AnimatedObject.hpp"

#include "Castor3D/Scene/Animation/Mesh/MeshAnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	class AnimatedMesh
		: public AnimatedObject
	{
	public:
		C3D_API AnimatedMesh( AnimatedMesh && rhs )noexcept = default;
		C3D_API AnimatedMesh & operator=( AnimatedMesh && rhs )noexcept = delete;
		C3D_API AnimatedMesh( AnimatedMesh const & rhs ) = delete;
		C3D_API AnimatedMesh & operator=( AnimatedMesh const & rhs ) = delete;
		C3D_API ~AnimatedMesh()noexcept override = default;
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
		 *\~english
		 *\brief		Fills a buffer with this object's morph targets weights transforms.
		 *\param[out]	submesh	The submesh from which the morph targets are retrieved.
		 *\param[out]	buffer	Receives the transforms.
		 *\~french
		 *\brief		Remplit un buffer avec les poids de cibles de morph de cet objet.
		 *\param[out]	submesh	Les submesh depuis lequel les morph targets sont récupérées.
		 *\param[out]	buffer	Reçoit les transformations.
		 */
		C3D_API uint32_t fillBuffer( Submesh const & submesh
			, MorphingWeightsConfiguration * buffer )const;
		/**
		 *\copydoc		castor3d::AnimatedObject::update
		 */
		C3D_API void update( castor::Milliseconds const & elapsed )override;

		C3D_API uint32_t getId( Submesh const & submesh )const;

		C3D_API void setId( Submesh const & submesh
			, uint32_t id );

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
		std::vector< uint32_t > m_ids;
		mutable bool m_reinit{ false };
	};
}

#endif

