/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialCache_H___
#define ___C3D_MaterialCache_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Material/Material.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

namespace castor
{
	/**
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	template<>
	class ResourceCacheT< castor3d::Material, String, castor3d::MaterialCacheTraits > final
		: public ResourceCacheBaseT< castor3d::Material, String, castor3d::MaterialCacheTraits >
	{
	public:
		using ElementT = castor3d::Material;
		using ElementKeyT = String;
		using ElementCacheTraitsT = castor3d::MaterialCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ResourceCacheT( castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ResourceCacheT()override = default;
		/**
		 *\~english
		 *\brief		Intialises the default material.
		 *\param[in]	device		The GPU device.
		 *\param[in]	passType	The materials type.
		 *\~french
		 *\brief		Initialise le matériau par défaut.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	passType	Le type des matériaux.
		 */
		C3D_API void initialise( castor3d::RenderDevice const & device
			, castor3d::PassTypeID passType );
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Deletes the default material, flush the collection
		 *\~french
		 *\brief		Supprime le matériau par défaut, vide la collection
		 */
		C3D_API void clear();
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
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( castor3d::GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( ashes::CommandBuffer const & cb )const;
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	names	La liste de noms
		 */
		C3D_API void getNames( StringArray & names );
		C3D_API bool registerPass( castor3d::Pass & pass );
		C3D_API void unregisterPass( castor3d::Pass & pass );
		C3D_API bool registerUnit( castor3d::TextureUnit & unit );
		C3D_API void unregisterUnit( castor3d::TextureUnit & unit );
		C3D_API bool registerTexture( castor3d::AnimatedTexture const & texture );
		C3D_API void unregisterTexture( castor3d::AnimatedTexture const & texture );
		C3D_API uint32_t getMaxPassTypeCount()const;

		castor3d::MaterialRPtr getDefaultMaterial()const
		{
			return m_defaultMaterial;
		}

		castor3d::PassBuffer const & getPassBuffer()const
		{
			CU_Require( m_passBuffer );
			return *m_passBuffer;
		}

		castor3d::SssProfileBuffer const & getSssProfileBuffer()const
		{
			CU_Require( m_sssProfileBuffer );
			return *m_sssProfileBuffer;
		}

		castor3d::TextureConfigurationBuffer const & getTexConfigBuffer()const
		{
			CU_Require( m_texConfigBuffer );
			return *m_texConfigBuffer;
		}

		castor3d::TextureAnimationBuffer const & getTexAnimBuffer()const
		{
			CU_Require( m_texAnimBuffer );
			return *m_texAnimBuffer;
		}

	private:
		using ElementCacheT::cleanup;
		using ElementCacheT::clear;

	private:
		castor3d::Engine & m_engine;
		castor3d::MaterialRPtr m_defaultMaterial{};
		castor3d::PassBufferSPtr m_passBuffer;
		castor3d::SssProfileBufferSPtr m_sssProfileBuffer;
		castor3d::TextureConfigurationBufferSPtr m_texConfigBuffer;
		castor3d::TextureAnimationBufferSPtr m_texAnimBuffer;
		std::vector< castor3d::Pass * > m_pendingPasses;
		std::vector< castor3d::TextureUnit * > m_pendingUnits;
		std::vector< castor3d::AnimatedTexture const * > m_pendingTextures;
	};
}

#endif
