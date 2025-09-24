/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightGroupCache_H___
#define ___C3D_LightGroupCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace c3d
{
	/**
	\~english
	\brief		LightGroup cache.
	\~french
	\brief		Cache de LightGroup.
	*/
	template<>
	class ResourceCacheT< LightGroup, String, LightGroupCacheTraits > final
		: public ResourceCacheBaseT< LightGroup, String, LightGroupCacheTraits >
	{
	public:
		using ElementT = LightGroup;
		using ElementKeyT = String;
		using ElementCacheTraitsT = LightGroupCacheTraits;

		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene			The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène.
		 */
		C3D_API explicit ResourceCacheT( Scene & scene );
		/**
		 *\~english
		 *\brief		Intialises the elements.
		 *\~french
		 *\brief		Initialise les éléments.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves the lights of given type.
		 *\param[in]	type	The light type.
		 *\return		The lights.
		 *\~french
		 *\brief		Récupère les lumières du type donné.
		 *\param[in]	type	Le type de lumière.
		 *\return		Les lumières.
		 */
		C3D_API Vector< LightGroup * > const & getLightGroups( LightType type )const;

		bool isDirty()const noexcept
		{
			return m_dirty;
		}

	private:
		void doRegisterLightGroup( LightGroup & light );
		void doUnregisterLightGroup( LightGroup const & light );

	private:
		Scene & m_scene;
		LightBufferRPtr m_lightBuffer{};
		Vector< LightGroup * > m_pendingLights;
		Array< Vector< LightGroup * >, size_t( LightType::eCount ) > m_lightsPerType;
		bool m_dirty{ true };
	};
}

#endif
