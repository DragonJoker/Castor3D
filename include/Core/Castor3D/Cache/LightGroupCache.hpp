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

namespace castor
{
	/**
	\~english
	\brief		LightGroup cache.
	\~french
	\brief		Cache de LightGroup.
	*/
	template<>
	class ResourceCacheT< castor3d::LightGroup, String, castor3d::LightGroupCacheTraits > final
		: public castor::ResourceCacheBaseT< castor3d::LightGroup, String, castor3d::LightGroupCacheTraits >
	{
	public:
		using ElementT = castor3d::LightGroup;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = castor3d::LightGroupCacheTraits;

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
		C3D_API ResourceCacheT( castor3d::Scene & scene );
		/**
		 *\~english
		 *\brief		Intialises GPU buffer.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise le buffer GPU.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( castor3d::RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();

		bool isDirty()const noexcept
		{
			return m_dirty;
		}

	private:
		bool doRegisterLightGroup( castor3d::LightGroup & light );
		void doUnregisterLightGroup( castor3d::LightGroup & light );

	private:
		castor3d::Scene & m_scene;
		castor3d::LightBufferRPtr m_lightBuffer;
		castor::Vector< castor3d::LightGroup * > m_pendingLights;
		bool m_dirty{ true };
	};
}

#endif
