/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedObjectGroupCache_H___
#define ___C3D_AnimatedObjectGroupCache_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/ResourceCacheBase.hpp>

namespace castor
{
	/**
	\~english
	\brief		AnimatedObjectGroup cache.
	\~french
	\brief		Cache de AnimatedObjectGroup.
	*/
	template<>
	class ResourceCacheT< castor3d::AnimatedObjectGroup, String, castor3d::AnimatedObjectGroupCacheTraits >
		: public OwnedBy< castor3d::Scene >
		, public ResourceCacheBaseT< castor3d::AnimatedObjectGroup, String, castor3d::AnimatedObjectGroupCacheTraits >
	{
	public:
		struct SkeletonPoolsEntry
		{
			castor3d::AnimatedObjectGroup const & group;
			castor3d::AnimatedSkeleton const & skeleton;
			castor3d::UniformBufferOffsetT< castor3d::SkinningUboConfiguration > skinningUbo;
		};
		struct MeshPoolsEntry
		{
			castor3d::AnimatedObjectGroup const & group;
			castor3d::AnimatedMesh const & mesh;
			castor3d::UniformBufferOffsetT< castor3d::MorphingUboConfiguration > morphingUbo;
		};

		using ElementT = castor3d::AnimatedObjectGroup;
		using ElementKeyT = String;
		using ElementCacheTraitsT = castor3d::AnimatedObjectGroupCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementObsT = typename ElementCacheT::ElementObsT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	produce			The element producer.
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	produce			Le créateur d'objet.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 */
		C3D_API explicit ResourceCacheT( castor3d::Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ResourceCacheT() = default;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( castor3d::CpuUpdater & updater );
		/**
		 *\~english
		 *\return		The entry related to given AnimatedMesh.
		 *\param[in]	mesh	The mexh.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedMesh donné.
		 *\param[in]	mesh	Le mesh.
		 */
		C3D_API MeshPoolsEntry getUbos( castor3d::AnimatedMesh const & mesh )const;
		/**
		 *\~english
		 *\return		The entry related to given AnimatedSkeleton.
		 *\param[in]	skeleton	The skeleton.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedSkeleton donné.
		 *\param[in]	skeleton	Le squelette.
		 */
		C3D_API SkeletonPoolsEntry getUbos( castor3d::AnimatedSkeleton const & skeleton )const;
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Vide la collection.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void clear( castor3d::RenderDevice const & device );

	private:
		using ElementCacheT::clear;

	private:
		SkeletonPoolsEntry doCreateEntry( castor3d::RenderDevice const & device
			, castor3d::AnimatedObjectGroup const & group
			, castor3d::AnimatedSkeleton const & skeleton );
		MeshPoolsEntry doCreateEntry( castor3d::RenderDevice const & device
			, castor3d::AnimatedObjectGroup const & group
			, castor3d::AnimatedMesh const & mesh );
		void doRemoveEntry( castor3d::RenderDevice const & device
			, castor3d::AnimatedSkeleton const & skeleton );
		void doRemoveEntry( castor3d::RenderDevice const & device
			, castor3d::AnimatedMesh const & mesh );
		void doRegister( castor3d::AnimatedObjectGroup & group );
		void doUnregister( castor3d::AnimatedObjectGroup & group );

	private:
		castor3d::Engine & m_engine;
		std::map< castor3d::AnimatedSkeleton const *, SkeletonPoolsEntry > m_skeletonEntries;
		std::map< castor3d::AnimatedMesh const *, MeshPoolsEntry > m_meshEntries;
		std::map< castor3d::AnimatedObjectGroup *, castor3d::OnAnimatedSkeletonChangeConnection > m_skeletonAddedConnections;
		std::map< castor3d::AnimatedObjectGroup *, castor3d::OnAnimatedSkeletonChangeConnection > m_skeletonRemovedConnections;
		std::map< castor3d::AnimatedObjectGroup *, castor3d::OnAnimatedMeshChangeConnection > m_meshAddedConnections;
		std::map< castor3d::AnimatedObjectGroup *, castor3d::OnAnimatedMeshChangeConnection > m_meshRemovedConnections;
	};
}

#endif
