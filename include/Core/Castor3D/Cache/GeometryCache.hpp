/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryCache_H___
#define ___C3D_GeometryCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <set>

namespace castor3d
{
	using GeometryCacheTraits = ObjectCacheTraitsT< Geometry, castor::String >;

	C3D_API size_t hash( Geometry const & geometry
		, Submesh const & submesh
		, Pass const & pass );
	/**
	\~english
	\brief		Geometry cache.
	\~french
	\brief		Cache de Geometry.
	*/
	template<>
	class ObjectCacheT< Geometry, castor::String, GeometryCacheTraits >
		: public ObjectCacheBaseT< Geometry, castor::String, GeometryCacheTraits >
	{
	public:
		struct PoolsEntry
		{
			size_t hash;
			Geometry const & geometry;
			Submesh const & submesh;
			Pass const & pass;
		};
		using ElementT = Geometry;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = GeometryCacheTraits;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
		using ElementContT = typename ElementObjectCacheT::ElementContT;
		using ElementInitialiserT = typename ElementObjectCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementObjectCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementObjectCacheT::ElementMergerT;
		using ElementAttacherT = typename ElementObjectCacheT::ElementAttacherT;
		using ElementDetacherT = typename ElementObjectCacheT::ElementDetacherT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
		 *\param[in]	produce			The element producer.
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\param[in]	attach			The element attacher (to a scene node).
		 *\param[in]	detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
		 *\param[in]	produce			Le créateur d'objet.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		C3D_API ObjectCacheT( Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCacheT() = default;

		C3D_API void fillInfo( RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Adds an object.
		 *\param[in]	element	The object.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	element	L'objet.
		 */
		C3D_API void add( ElementPtrT element );

	public:
		using ElementObjectCacheT::add;

	private:
		uint32_t m_faceCount{ 0 };
		uint32_t m_vertexCount{ 0 };
	};
}

#endif
