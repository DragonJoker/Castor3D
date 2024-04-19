/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightCache_H___
#define ___C3D_LightCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"

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
			, SceneNodeRPtr rootNode
			, SceneNodeRPtr rootCameraNode
			, SceneNodeRPtr rootObjectNode );
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
		 *\brief			Uploads all GPU buffers to VRAM.
		 *\param[in,out]	uploader	Receives the upload requests.
		 *\~french
		 *\brief			Met à jour tous les tampons GPU en VRAM.
		 *\param[in,out]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader )const;
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
		 *\param[in]	stages	The impacted shader stages.
		 *\param[in]	index	The binding point index.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	stages	Les shader stages impactés.
		 *\param[in]	index	L'indice du point d'attache.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( VkShaderStageFlags stages
			, uint32_t index )const;
		/**
		 *\~english
		 *\brief			Adds the descriptor set layout binding at given point.
		 *\param[in,out]	bindings	Receives the binding.
		 *\param[in]		stages		The impacted shader stages.
		 *\param[in,out]	index		The binding index.
		 *\~french
		 *\brief			Ajoute une attache de layout de set de descripteurs au point donné.
		 *\param[in,out]	bindings	Reçoit l'attache.
		 *\param[in]		stages		Les shader stages impactés.
		 *\param[in,out]	index		L'indice du point d'attache.
		 */
		C3D_API void addLayoutBinding( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags stages
			, uint32_t & index )const;
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
		C3D_API void addBinding( ashes::WriteDescriptorSetArray & writes
			, uint32_t & binding )const;
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
		 *\param[in]	type	The light type.
		 *\return		The number of light sources of the given type, in the buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in]	type	Le type de lumière.
		 *\return		Le nombre de sources lumineuses du type donné, dans le buffer.
		 */
		C3D_API uint32_t getLightsBufferCount( LightType type )const noexcept;

		uint32_t getLightsCount( LightType type )const
		{
			return uint32_t( getLights( type ).size() );
		}

		bool isDirty()const noexcept
		{
			return m_dirty || !m_dirtyLights.empty();
		}

		bool hasClusteredLights()const noexcept
		{
			return !getLights( LightType::ePoint ).empty()
				|| !getLights( LightType::eSpot ).empty();
		}

	private:
		bool doCheckUniqueDirectionalLight( LightType toAdd )const noexcept;
		bool doRegisterLight( Light & light );
		void doUnregisterLight( Light & light );

	private:
		LightsRefArray m_dirtyLights;
		LightBufferUPtr m_lightBuffer;
		castor::Vector< Light * > m_pendingLights;
		bool m_dirty{ true };
	};
}

#endif
