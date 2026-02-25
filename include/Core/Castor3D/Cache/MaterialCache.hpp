/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialCache_H___
#define ___C3D_MaterialCache_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Material.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

namespace c3d
{
	/**
	\~english
	\brief		Material collection, with additional functions
	\~french
	\brief		Collection de matériaux, avec des fonctions additionnelles
	*/
	template<>
	class ResourceCacheT< Material, String, MaterialCacheTraits > final
		: public ResourceCacheBaseT< Material, String, MaterialCacheTraits >
	{
	public:
		using ElementT = Material;
		using ElementKeyT = String;
		using ElementCacheTraitsT = MaterialCacheTraits;
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
		C3D_API explicit ResourceCacheT( Engine & engine );
		/**
		 *\~english
		 *\brief		Intialises the default material.
		 *\param[in]	device		The GPU device.
		 *\~french
		 *\brief		Initialise le matériau par défaut.
		 *\param[in]	device		Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
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
		C3D_API void clear()noexcept;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater const & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater const & updater );
		/**
		 *\~english
		 *\brief		Uploads all GPU buffers to VRAM.
		 *\param[in]	uploader	Receives the upload requests.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU en VRAM.
		 *\param[in]	uploader	Reçoit les requêtes d'upload.
		 */
		C3D_API void upload( UploadData & uploader )const;
		/**
		 *\~english
		 *\brief			Registers a specific data shader buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\param[out]		buffer	The shader buffer.
		 *\~french
		 *\brief			Enregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 *\param[out]		buffer	Le shader buffer.
		 */
		C3D_API void registerSpecificsBuffer( String const & name
			, SpecificsBuffer buffer );
		/**
		 *\~english
		 *\brief			Unregisters a pass' specific data buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\~french
		 *\brief			Désenregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 */
		C3D_API void unregisterSpecificsBuffer( String const & name )noexcept;
		/**
		 *\~english
		 *\brief			Addw the pass' specific data buffer into the given descriptor layout bindings array.
		 *\param[in,out]	bindings		Receives the buffers descriptor layout bindings.
		 *\param[in]		shaderStages	The shader stage flags.
		 *\param[in,out]	index			The binding index.
		 *\~french
		 *\brief			Ecrit les shader buffers de données spécifiques dans le tableau de descriptor layout bindings donné.
		 *\param[in,out]	bindings		Reçoit les descriptor layout bindings des buffers.
		 *\param[in]		shaderStages	Les indicateurs de shader stage.
		 *\param[in,out]	index			L'indice de binding.
		 */
		C3D_API void addSpecificsBuffersLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index
			, VkShaderStageFlags shaderStages )const;
		/**
		 *\~english
		 *\brief			Writes the pass' specific data buffer into the given descriptor writes array.
		 *\param[in,out]	descriptorWrites	Receives the buffers descriptor writes.
		 *\param[in,out]	index				The binding index.
		 *\~french
		 *\brief			Ecrit les shader buffers de données spécifiques dans le tableau de descriptor writes donné.
		 *\param[in,out]	descriptorWrites	Reçoit les descriptor writes des buffers.
		 *\param[in,out]	index				L'indice de binding.
		 */
		C3D_API void addSpecificsBuffersDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;
		/**
		 *\~english
		 *\brief			Writes the pass' specific data buffer bindings into given frame pass.
		 *\param[in,out]	pass	The target frame pass.
		 *\param[in,out]	index	The binding index.
		 *\~french
		 *\brief			Ecrit les bindings des shader buffers de données spécifiques dans la frame pass donnée.
		 *\param[in,out]	pass	La frame pass ciblée.
		 *\param[in,out]	index	L'indice de binding.
		 */
		C3D_API void createSpecificsBuffersPassBindings( crg::FramePass & pass
			, uint32_t & index )const;
		/**
		 *\~english
		 *\brief			Declares pass' specific data shader buffers.
		 *\param[in,out]	writer	The shader writer.
		 *\param[out]		buffers	Receives the registered shader buffers.
		 *\param[in,out]	binding	The descriptor binding index.
		 *\param[in]		set		The descriptor set index.
		 *\~french
		 *\brief			Déclare dans les shaders les buffers spécifiques enregistrés.
		 *\param[in,out]	writer	Le writer de shader.
		 *\param[out]		buffers	Reçoit les shader buffers enregistrés.
		 *\param[in,out]	binding	L'indice de descripteur.
		 *\param[in]		set		L'indice de descriptor set.
		 */
		C3D_API void declareSpecificsShaderBuffers( sdw::ShaderWriter & writer
			, StringMap< shader::BufferBaseUPtr > & buffers
			, uint32_t & binding
			, uint32_t set )const;
		/**
		 *\~english
		 *\brief		Puts all the materials names in the given array
		 *\param[out]	names	The array of names to be filled
		 *\~french
		 *\brief		Remplit la liste des noms de tous les matériaux
		 *\param[out]	names	La liste de noms
		 */
		C3D_API void getNames( StringArray & names );
		/**
		 *\~english
		 *\return		The maximum pass types count (for visibility buffer use).
		 *\~french
		 *\brief		Le nombre maximum de types de passes (pour l'utilisation de vibility buffer).
		 */
		C3D_API uint32_t getMaxPassTypeCount()const;
		/**
		 *\~english
		 *\return		The pass type details for given pass type index (for visibility buffer use).
		 *\~french
		 *\brief		Les détails du type de passe pour l'index donné (pour l'utilisation de vibility buffer).
		 */
		C3D_API std::tuple< PassComponentCombineID, TextureCombineID > getPassTypeDetails( uint32_t passTypeIndex )const;
		/**
		 *\~english
		 *\return		The pass type index (for visibility buffer use).
		 *\~french
		 *\brief		L'indice de type de passe (pour l'utilisation de vibility buffer).
		 */
		C3D_API uint32_t getPassTypeIndex( PassComponentCombineID passType
			, TextureCombineID textureFlags )const;
		C3D_API bool registerPass( Pass & pass );
		C3D_API void unregisterPass( Pass & pass )noexcept;
		C3D_API bool registerUnit( TextureUnit & unit );
		C3D_API void unregisterUnit( TextureUnit & unit )noexcept;
		C3D_API bool registerTexture( AnimatedTexture const & texture );
		C3D_API void unregisterTexture( AnimatedTexture const & texture )noexcept;
		C3D_API uint32_t getCurrentPassTypeCount()const;

		MaterialObs getDefaultMaterial()const
		{
			return m_defaultMaterial;
		}

		PassBuffer & getPassBuffer()const
		{
			CU_Require( m_passBuffer );
			return *m_passBuffer;
		}

		SssProfileBuffer & getSssProfileBuffer()const
		{
			CU_Require( m_sssProfileBuffer );
			return *m_sssProfileBuffer;
		}

		TextureConfigurationBuffer & getTexConfigBuffer()const
		{
			CU_Require( m_texConfigBuffer );
			return *m_texConfigBuffer;
		}

		TextureAnimationBuffer & getTexAnimBuffer()const
		{
			CU_Require( m_texAnimBuffer );
			return *m_texAnimBuffer;
		}

	private:
		using ElementCacheT::cleanup;
		using ElementCacheT::clear;

		class PassDataBuffers
		{
		public:
			/**
			*\name
			*	Initialisation / Update.
			*/
			/**@{*/
			void initialise( RenderDevice const & device );
			void cleanup();
			void update( PassBuffer & passBuffer
				, UploadData & uploader )const;
			/**@}*/
			/**
			*\~english
			*\name
			*	Registration.
			*\~french
			*\name
			*	Enregistrement.
			*/
			/**@{*/
			void registerBuffer( String const & name
				, SpecificsBuffer buffer );
			void unregisterBuffer( String const & name );
			/**@}*/
			/**
			*\name
			*	Descriptor layout / set.
			*/
			/**@{*/
			void addLayoutBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
				, uint32_t & index
				, VkShaderStageFlags shaderStages )const;
			void addDescriptorWrites( ashes::WriteDescriptorSetArray & descriptorWrites
				, uint32_t & index )const;
			void createPassBindings( crg::FramePass & pass
				, uint32_t & index )const;
			/**@}*/
			/**
			*\name
			*	Shader declarations.
			*/
			/**@{*/
			void declareShaderBuffers( sdw::ShaderWriter & writer
				, StringMap< shader::BufferBaseUPtr > & buffers
				, uint32_t & binding
				, uint32_t set )const;
			/**@}*/

		private:
			c3d::SpecificsBuffers m_buffers;
		};

		void doUpdatePending();

	private:
		Engine & m_engine;
		MaterialObs m_defaultMaterial{};
		PassBufferUPtr m_passBuffer;
		SssProfileBufferUPtr m_sssProfileBuffer;
		TextureConfigurationBufferUPtr m_texConfigBuffer;
		TextureAnimationBufferUPtr m_texAnimBuffer;
		Vector< Pass * > m_pendingPasses;
		Vector< TextureUnit * > m_pendingUnits;
		Vector< AnimatedTexture const * > m_pendingTextures;
		PassDataBuffers m_specificsBuffers;
	};
}

#endif
