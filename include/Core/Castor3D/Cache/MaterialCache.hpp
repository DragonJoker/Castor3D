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
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

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
		 *\~french
		 *\brief		Initialise le matériau par défaut.
		 *\param[in]	device		Le device GPU.
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
		 *\brief			Registers a specific data shader buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\param[out]		buffer	The shader buffer.
		 *\~french
		 *\brief			Enregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 *\param[out]		buffer	Le shader buffer.
		 */
		C3D_API void registerSpecificsBuffer( std::string const & name
			, castor3d::SpecificsBuffer buffer );
		/**
		 *\~english
		 *\brief			Unregisters a pass' specific data buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\~french
		 *\brief			Désenregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 */
		C3D_API void unregisterSpecificsBuffer( std::string const & name );
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
		C3D_API void addSpecificsBuffersBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const;
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
		C3D_API void addSpecificsBuffersDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
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
			, std::map< std::string, castor3d::shader::BufferBaseUPtr > & buffers
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
		C3D_API std::tuple< castor3d::PassComponentCombineID, castor3d::TextureCombineID > getPassTypeDetails( uint32_t passTypeIndex )const;
		/**
		 *\~english
		 *\return		The pass type index (for visibility buffer use).
		 *\~french
		 *\brief		L'indice de type de passe (pour l'utilisation de vibility buffer).
		 */
		C3D_API uint32_t getPassTypeIndex( castor3d::PassComponentCombineID passType
			, castor3d::TextureCombineID textureFlags )const;
		C3D_API bool registerPass( castor3d::Pass & pass );
		C3D_API void unregisterPass( castor3d::Pass & pass );
		C3D_API bool registerUnit( castor3d::TextureUnit & unit );
		C3D_API void unregisterUnit( castor3d::TextureUnit & unit );
		C3D_API bool registerTexture( castor3d::AnimatedTexture const & texture );
		C3D_API void unregisterTexture( castor3d::AnimatedTexture const & texture );
		C3D_API uint32_t getCurrentPassTypeCount()const;

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

		class PassDataBuffers
		{
		public:
			/**
			*\name
			*	Initialisation / Update.
			*/
			/**@{*/
			void initialise( castor3d::RenderDevice const & device );
			void cleanup();
			void update( castor3d::PassBuffer & passBuffer
				, ashes::CommandBuffer const & cb )const;
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
			void registerBuffer( std::string const & name
				, castor3d::SpecificsBuffer buffer );
			void unregisterBuffer( std::string const & name );
			/**@}*/
			/**
			*\name
			*	Descriptor layout / set.
			*/
			/**@{*/
			void addBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
				, VkShaderStageFlags shaderStages
				, uint32_t & index )const;
			void addDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
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
				, std::map< std::string, castor3d::shader::BufferBaseUPtr > & buffers
				, uint32_t & binding
				, uint32_t set )const;
			/**@}*/

		private:
			castor3d::SpecificsBuffers m_buffers;
		};

	private:
		castor3d::Engine & m_engine;
		castor3d::MaterialRPtr m_defaultMaterial{};
		castor3d::PassBufferUPtr m_passBuffer;
		castor3d::SssProfileBufferUPtr m_sssProfileBuffer;
		castor3d::TextureConfigurationBufferUPtr m_texConfigBuffer;
		castor3d::TextureAnimationBufferUPtr m_texAnimBuffer;
		std::vector< castor3d::Pass * > m_pendingPasses;
		std::vector< castor3d::TextureUnit * > m_pendingUnits;
		std::vector< castor3d::AnimatedTexture const * > m_pendingTextures;
		PassDataBuffers m_specificsBuffers;
	};
}

#endif
