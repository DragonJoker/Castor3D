/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AnimatedObjectGroupCache_H___
#define ___C3D_AnimatedObjectGroupCache_H___

#include "Castor3D/Cache/CacheBase.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		AnimatedObjectGroup cache.
	\~french
	\brief		Cache de AnimatedObjectGroup.
	*/
	class AnimatedObjectGroupCache
		: public CacheBase< AnimatedObjectGroup, castor::String >
	{
	public:
		struct SkeletonPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedSkeleton const & skeleton;
			UniformBufferOffsetT< SkinningUboConfiguration > skinningUbo;
		};
		struct MeshPoolsEntry
		{
			AnimatedObjectGroup const & group;
			AnimatedMesh const & mesh;
			UniformBufferOffsetT< MorphingUboConfiguration > morphingUbo;
		};
		using MyCache = CacheBase< AnimatedObjectGroup, castor::String >;
		using MyCacheTraits = typename MyCacheType::MyCacheTraits;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using LockType = typename MyCacheType::LockType;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
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
		C3D_API AnimatedObjectGroupCache( Engine & engine
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~AnimatedObjectGroupCache();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the UBO pools in VRAM.
		 *\param[in]	commandBuffer	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour les pools d'UBO en VRAM.
		 *\param[in]	commandBuffer	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void uploadUbos( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Cleans up the UBO pools.
		 *\~french
		 *\brief		Nettoie les pools d'UBO.
		 */
		C3D_API void cleanupUbos();
		/**
		 *\~english
		 *\return		The entry related to given AnimatedMesh.
		 *\param[in]	mesh	The mexh.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedMesh donné.
		 *\param[in]	mesh	Le mesh.
		 */
		C3D_API MeshPoolsEntry getUbos( AnimatedMesh const & mesh )const;
		/**
		 *\~english
		 *\return		The entry related to given AnimatedSkeleton.
		 *\param[in]	skeleton	The skeleton.
		 *\~french
		 *\brief		L'entrée relative à l'AnimatedSkeleton donné.
		 *\param[in]	skeleton	Le squelette.
		 */
		C3D_API SkeletonPoolsEntry getUbos( AnimatedSkeleton const & skeleton )const;
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Vide la collection.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void clear( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Adds an object.
		 *\param[in]	element	The object.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	element	L'objet.
		 */
		C3D_API void add( ElementPtr element );
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name	The object name.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\return		L'objet créé.
		 */
		C3D_API ElementPtr add( Key const & name );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name	The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name	Le nom d'objet.
		 */
		C3D_API void remove( Key const & name );

	private:
		using MyCache::add;

	private:
		SkeletonPoolsEntry doCreateEntry( RenderDevice const & device
			, AnimatedObjectGroup const & group
			, AnimatedSkeleton const & skeleton );
		MeshPoolsEntry doCreateEntry( RenderDevice const & device
			, AnimatedObjectGroup const & group
			, AnimatedMesh const & mesh );
		void doRemoveEntry( RenderDevice const & device
			, AnimatedSkeleton const & skeleton );
		void doRemoveEntry( RenderDevice const & device
			, AnimatedMesh const & mesh );
		void doRegister( AnimatedObjectGroup & group );
		void doUnregister( AnimatedObjectGroup & group );

	private:
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
		std::map< AnimatedSkeleton const *, SkeletonPoolsEntry > m_skeletonEntries;
		std::map< AnimatedMesh const *, MeshPoolsEntry > m_meshEntries;
		std::map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonAddedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedSkeletonChangeConnection > m_skeletonRemovedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshAddedConnections;
		std::map< AnimatedObjectGroup *, OnAnimatedMeshChangeConnection > m_meshRemovedConnections;
	};
}

#endif
