/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightCache_H___
#define ___C3D_LightCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	using LightCacheTraits = ObjectCacheTraitsT< Light, castor::String >;
	/**
	\~english
	\brief		Light cache.
	\~french
	\brief		Cache de Light.
	*/
	template<>
	class ObjectCacheT< Light, castor::String, LightCacheTraits >
		: public ObjectCacheBaseT< Light, castor::String, LightCacheTraits >
	{
	public:
		using ElementT = Light;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = LightCacheTraits;
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
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
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
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
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
		 *\brief		Retrieves the lights of given type.
		 *\param[in]	type	The light type.
		 *\return		The lights.
		 *\~french
		 *\brief		Récupère les lumières du type donné.
		 *\param[in]	type	Le type de lumière.
		 *\return		Les lumières.
		 */
		C3D_API LightsArray getLights( LightType type )const;
		/**
		 *\~english
		 *\brief		Creates a frame pass binding.
		 *\~french
		 *\brief		Crée une attache de frame pass.
		 */
		C3D_API void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding point index.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index = 0u )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for the lights buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for the lights buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding
			, VkDeviceSize offset
			, VkDeviceSize size )const;
		/**
		 *\~english
		 *\brief		Retrieves the count of the lights of given type.
		 *\param[in]	type	The light type.
		 *\return		The count.
		 *\~french
		 *\brief		Récupère le nombre de lumières du type donné.
		 *\param[in]	type	Le type de lumière.
		 *\return		Le compte.
		 */
		uint32_t getLightsCount( LightType type )const
		{
			return uint32_t( getLights( type ).size() );
		}

	private:
		bool doCheckUniqueDirectionalLight( LightType toAdd );
		bool doRegisterLight( Light & light );
		void doUnregisterLight( Light & light );

	private:
		LightsRefArray m_dirtyLights;
		std::map< Light *, OnLightChangedConnection > m_connections;
		LightBufferUPtr m_lightBuffer;
		std::vector< Light * > m_pendingLights;
	};
}

#endif
