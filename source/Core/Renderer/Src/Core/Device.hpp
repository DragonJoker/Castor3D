/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Device_HPP___
#define ___Renderer_Device_HPP___
#pragma once

#include "Command/CommandPool.hpp"
#include "Command/Queue.hpp"
#include "Core/Connection.hpp"
#include "Core/PhysicalDevice.hpp"
#include "Image/ImageCreateInfo.hpp"
#include "Image/SamplerCreateInfo.hpp"
#include "Pipeline/ColourBlendState.hpp"
#include "Pipeline/RasterisationState.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	The class containing the informations related to the logical device.
	*\remarks
	*	It creates most of the rendering API objects.
	*\~french
	*\brief
	*	Classe contenant les informations liées au GPU logique.
	*\remarks
	*	Elle crée la plupart des objets de rendu.
	*/
	class Device
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] renderer
		*	The Renderer instance.
		*\param[in] connection
		*	The connection to the application.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] connection
		*	La connection à l'application.
		*/
		Device( Renderer const & renderer
			, PhysicalDevice const & gpu
			, Connection const & connection );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Device() = default;
		/**
		*\~english
		*\return
		*	The clip direction for the rendering API.
		*\~french
		*\return
		*	La direction de clipping pour l'API de rendu.
		*/
		ClipDirection getClipDirection()const;
		/**
		*\~english
		*\brief
		*	Enables the device's context (for OpenGL).
		*\~french
		*\brief
		*	Active le contexte du périphérique (pour OpenGL).
		*/
		void enable()const;
		/**
		*\~french
		*\brief
		*	Désctive le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Disables the device's context (for OpenGL).
		*/
		void disable()const;
		/**
		*\~french
		*\brief
		*	Crée une passe de rendu.
		*\param[in] createInfo
		*	Les informations de création.
		*\return
		*	La passe créée.
		*\~english
		*\brief
		*	Creates a render pass.
		*\param[in] createInfo
		*	The creation informations.
		*\return
		*	The created pass.
		*/
		virtual RenderPassPtr createRenderPass( RenderPassCreateInfo createInfo )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] setLayouts
		*	The descriptor sets layouts.
		*\param[in] pushConstantRanges
		*	The push constants ranges.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] setLayouts
		*	Les layouts des descripteurs du pipeline.
		*\param[in] pushConstantRanges
		*	Les intervalles de push constants.
		*\return
		*	Le layout créé.
		*/
		virtual PipelineLayoutPtr createPipelineLayout( DescriptorSetLayoutCRefArray const & setLayouts
			, PushConstantRangeCRefArray const & pushConstantRanges )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a descriptor set layout.
		*\param[in] bindings
		*	The layout bindings.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de set de descripteurs.
		*\param[in] bindings
		*	Les attaches du layout.
		*\return
		*	Le layout créé.
		*/
		virtual DescriptorSetLayoutPtr createDescriptorSetLayout( DescriptorSetLayoutBindingArray && bindings )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a descriptor pool.
		*\param[in] flags
		*	Bitmask specifying certain supported operations on a descriptor pool.
		*\param[in] maxSets
		*	The maximum number of descriptor sets that can be allocated from the pool.
		*\param[in] poolSizes
		*	The array of DescriptorPoolSize describing the type and count of descriptors to be allocated in the pool.
		*\return
		*	The created pool.
		*\~french
		*\brief
		*	Crée un pool de descripteurs.
		*\param[in] flags
		*	Masque de bits définissant les opérations supportées sur un pool de descripteurs.
		*\param[in] maxSets
		*	Le nombre maximum d'ensembles de descripteurs pouvant être alloués par le pool.
		*\param[in] poolSizes
		*	Le tableau de DescriptorPoolSize décrivant les types et nombre de descripteurs à allouer dans le pool.
		*\return
		*	Le pool créé.
		*/
		virtual DescriptorPoolPtr createDescriptorPool( DescriptorPoolCreateFlags flags
			, uint32_t maxSets
			, DescriptorPoolSizeArray poolSizes )const = 0;
		/**
		*\~english
		*\brief
		*	Allocates memory on the device.
		*\param[in] requirements
		*	The memory allocation requirements.
		*\param[in] flags
		*	The memory type.
		*\return
		*	The DeviceMemory object holding the allocated memory.
		*\~french
		*\brief
		*	Alloue de la mémoire sur le périphérique.
		*\param[in] requirements
		*	Les exigences d'allocation mémoire.
		*\param[in] flags
		*	Le type de mémoire.
		*\return
		*	L'objet DeviceMemory contenant la mémoire allouée.
		*/
		virtual DeviceMemoryPtr allocateMemory( MemoryRequirements const & requirements
			, MemoryPropertyFlags flags )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a texture.
		*\param[in] createInfo
		*	The creation informations.
		*\~french
		*\brief
		*	Crée une texture.
		*\param[in] createInfo
		*	Les informations de création.
		*/
		virtual TexturePtr createTexture( ImageCreateInfo const & createInfo )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a texture.
		*\param[in] image
		*	The image whose layout is being queried.
		*\param[out] subresource
		*	Receives the image subresource.
		*\param[out] layout
		*	Receives the subresource layout.
		*\~french
		*\brief
		*	Crée une texture.
		*\param[in] image
		*	L'image pour laquelle le layout est demandé.
		*\param[out] subresource
		*	Reçoit la sous-ressource de l'image.
		*\param[out] layout
		*	Reçoit le layout de la sous-ressource.
		*/
		virtual void getImageSubresourceLayout( Texture const & image
			, ImageSubresource const & subresource
			, SubresourceLayout & layout )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a sampler.
		*\param[in] createInfo
		*	The creation informations.
		*\~french
		*\brief
		*	Crée un échantillonneur.
		*\param[in] createInfo
		*	Les informations de création.
		*/
		virtual SamplerPtr createSampler( SamplerCreateInfo const & createInfo )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a GPU buffer.
		*\param[in] size
		*	The buffer size.
		*\param[in] target
		*	The buffer usage flags.
		*\~french
		*\brief
		*	Crée un tampon GPU.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*/
		virtual BufferBasePtr createBuffer( uint32_t size
			, BufferTargets target )const = 0;
		/**
		*\~french
		*\brief
		*	Crée une vue sur un tampon GPU.
		*\param[in] buffer
		*	Le tampon sur lequel la vue est créée.
		*\param[in] format
		*	Le format des pixels de la vue.
		*\param[in] offset
		*	Le décalage dans le tampon.
		*\param[in] range
		*	Le nombre d'éléments dans le tampon.
		*\~english
		*\brief
		*	Creates a view on a GPU buffer.
		*\param[in] buffer
		*	The buffer from which the view is created.
		*\param[in] format
		*	The view's pixels format.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The number of elements from the buffer.
		*/
		virtual BufferViewPtr createBufferView( BufferBase const & buffer
			, Format format
			, uint32_t offset
			, uint32_t range )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a uniform buffer.
		*\param[in] count
		*	The buffer elements count.
		*\param[in] size
		*	The size of one element in the buffer.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] memoryFlags
		*	The buffer memory flags.
		*\~french
		*\brief
		*	Crée un tampon d'uniformes.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] size
		*	La taille d'un élément.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual UniformBufferBasePtr createUniformBuffer( uint32_t count
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags memoryFlags )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a swap chain.
		*\param[in] size
		*	The wanted dimensions.
		*\~french
		*\brief
		*	Crée une swap chain.
		*\param[in] size
		*	Les dimensions souhaitées.
		*/
		virtual SwapChainPtr createSwapChain( Extent2D const & size )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a semaphore.
		*\~french
		*\brief
		*	Crée un sémaphore.
		*/
		virtual SemaphorePtr createSemaphore()const = 0;
		/**
		*\~english
		*\brief
		*	Creates a fence.
		*\param[in] flags
		*	The fence creation flags.
		*\~french
		*\brief
		*	Crée une barrière.
		*\param[in] flags
		*	Les indicateurs de création de la barrière.
		*/
		virtual FencePtr createFence( FenceCreateFlags flags = 0 )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a command buffer pool.
		*\param[in] queueFamilyIndex
		*	The family index of the queue to which the pool belongs.
		*\param[in] flags
		*	The command pool creation flags.
		*\~french
		*\brief
		*	Crée un pool de tampons de commandes.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Les indicateurs de création du pool.
		*/
		virtual CommandPoolPtr createCommandPool( uint32_t queueFamilyIndex
			, CommandPoolCreateFlags const & flags = 0 )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a shader module.
		*\param[in] stage
		*	The module's shader stage.
		*\~french
		*\brief
		*	Crée un module shader.
		*\param[in] stage
		*	Le niveau de shader utilisé pour le module.
		*/
		virtual ShaderModulePtr createShaderModule( ShaderStageFlag stage )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a query pool.
		*\param[in] type
		*	The query type.
		*\param[in] count
		*	The number of queries managed by the pool.
		*\param[in] pipelineStatistics
		*	Specifies the counters to be returned in queries.
		*\~french
		*\brief
		*	Crée un pool de requêtes.
		*\param[in] type
		*	Le type de requête.
		*\param[in] count
		*	Le nombre de requêtes gérées par le pool.
		*\param[in] pipelineStatistics
		*	Définit les compteurs que les requêtes retournent.
		*/
		virtual QueryPoolPtr createQueryPool( QueryType type
			, uint32_t count
			, QueryPipelineStatisticFlags pipelineStatistics )const = 0;
		/**
		*\~english
		*\brief
		*	Computes an frustum projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
		*\brief
		*	Calcule une matrice de projection frustum.
		*\param[in] left, right
		*	La position des plans gauche et droite.
		*\param[in] top, bottom
		*	La position des plans haut et bas.
		*\param[in] zNear, zFar
		*	La position des premier et arrière plans.
		*/
		virtual Mat4 frustum( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const = 0;
		/**
		*\~english
		*	Computes a perspective projection matrix.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\param[in] zFar
		*	The far plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*\param[in] zFar
		*	La position de l'arrière plan (pour le clipping).
		*/
		virtual Mat4 perspective( Angle fovy
			, float aspect
			, float zNear
			, float zFar )const = 0;
		/**
		*\~english
		*\brief
		*	Computes an orthographic projection matrix.
		*\param[in] left, right
		*	The left and right planes position.
		*\param[in] top, bottom
		*	The top and bottom planes position.
		*\param[in] zNear, zFar
		*	The near and far planes position.
		*\~french
		*\brief
		*	Calcule une matrice de projection orthographique.
		*\param[in] left, right
		*	La position des plans gauche et droite.
		*\param[in] top, bottom
		*	La position des plans haut et bas.
		*\param[in] zNear, zFar
		*	La position des premier et arrière plans.
		*/
		virtual Mat4 ortho( float left
			, float right
			, float bottom
			, float top
			, float zNear
			, float zFar )const = 0;
		/**
		*\~english
		*	Computes a perspective projection matrix with no far plane clipping.
		*\param[in] fovy
		*	The vertical aperture angle.
		*\param[in] aspect
		*	The width / height ratio.
		*\param[in] zNear
		*	The near plane position.
		*\~french
		*\brief
		*	Calcule une matrice de projection en perspective sans clipping
		*	d'arrière plan.
		*\param[in] fovy
		*	L'angle d'ouverture verticale.
		*\param[in] aspect
		*	Le ratio largeur / hauteur.
		*\param[in] zNear
		*	La position du premier plan (pour le clipping).
		*/
		Mat4 infinitePerspective( Angle fovy
			, float aspect
			, float zNear )const;
		/**
		*\~english
		*\brief
		*	Creates a GPU buffer.
		*\param[in] size
		*	The buffer size.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] memoryFlags
		*	The buffer memory flags.
		*\~french
		*\brief
		*	Crée un tampon GPU.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire du tampon.
		*/
		BufferBasePtr createBuffer( uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags flags )const;
		/**
		*\~english
		*\brief
		*	Creates a texture.
		*\param[in] createInfo
		*	The creation informations.
		*\~french
		*\brief
		*	Crée une texture.
		*\param[in] createInfo
		*	Les informations de création.
		*/
		TexturePtr createTexture( ImageCreateInfo const & createInfo
			, MemoryPropertyFlags flags )const;
		/**
		*\~french
		*\brief
		*	Crée une passe de rendu.
		*\param[in] attaches
		*	Les attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécessaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état atteint en fin de passe.
		*\return
		*	La passe créée.
		*\~english
		*\brief
		*	Creates a render pass.
		*\param[in] attaches
		*	The attachments pixels formats.
		*\param[in] subpasses
		*	The rendersubpasses (at least one is necessary).
		*\param[in] initialState
		*	The state wanted at the beginning of the pass.
		*\param[in] finalState
		*	The state attained at the end of the pass.
		*\return
		*	The created pass.
		*/
		RenderPassPtr createRenderPass( AttachmentDescriptionArray const & attaches
			, RenderSubpassPtrArray && subpasses
			, RenderSubpassState const & initialState
			, RenderSubpassState const & finalState )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout()const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] layout
		*	The descriptor set layout.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayout const & layout )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] pushConstantRange
		*	The push constants range.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] pushConstantRange
		*	L'intervalle de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( PushConstantRange const & pushConstantRange )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] layout
		*	The descriptor set layout.
		*\param[in] pushConstantRange
		*	The push constants range.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\param[in] pushConstantRange
		*	L'intervalle de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayout const & layout
			, PushConstantRange const & pushConstantRange )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] layouts
		*	The descriptor sets layouts.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layouts
		*	Les layouts des descripteurs du pipeline.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( DescriptorSetLayoutCRefArray const & layouts )const;
		/**
		*\~english
		*\brief
		*	Creates a pipeline layout.
		*\param[in] pushConstantRanges
		*	The push constants ranges.
		*\return
		*	The created layout.
		*\~french
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] pushConstantRanges
		*	Les intervalles de push constants.
		*\return
		*	Le layout créé.
		*/
		PipelineLayoutPtr createPipelineLayout( PushConstantRangeCRefArray const & pushConstantRanges )const;
		/**
		*\~english
		*\brief
		*	Creates a sampler.
		*\param[in] wrapS, wrapT, wrapR
		*	The texture wrap modes.
		*\param[in] minFilter, magFilter
		*	The minification and magnification filters.
		*\param[in] mipFilter
		*	The mipmap filter.
		*\param[in] minLod
		*	Minimal LOD Level.
		*\param[in] maxLod
		*	Maximal LOD Level.
		*\param[in] lodBias
		*	The texture LOD offset.
		*\param[in] borderColour
		*	Texture border colour.
		*\param[in] maxAnisotropy
		*	Maximal anisotropic filtering value.
		*\param[in] compareOp
		*	The comparison operator, for depth maps.
		*\~french
		*\brief
		*	Crée un échantillonneur.
		*\param[in] wrapS, wrapT, wrapR
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le filtre de mipmap.
		*\param[in] minLod
		*	Niveau de LOD minimal.
		*\param[in] maxLod
		*	Niveau de LOD maximal.
		*\param[in] lodBias
		*	Le décalage de LOD de la texture.
		*\param[in] borderColour
		*	Couleur des bords de la texture.
		*\param[in] maxAnisotropy
		*	Valeur maximale pour le filtrage anisotropique.
		*\param[in] compareOp
		*	L'opérateur de comparaison, pour les textures de profondeur.
		*/
		SamplerPtr createSampler( WrapMode wrapS
			, WrapMode wrapT
			, WrapMode wrapR
			, Filter minFilter
			, Filter magFilter
			, MipmapMode mipFilter = MipmapMode::eNone
			, float minLod = -1000.0f
			, float maxLod = 1000.0f
			, float lodBias = 0.0f
			, BorderColour borderColour = BorderColour::eFloatOpaqueBlack
			, float maxAnisotropy = 1.0f
			, CompareOp compareOp = CompareOp::eAlways )const;
		/**
		*\~english
		*\brief
		*	Waits for the device to be idle.
		*\~french
		*\brief
		*	Attend que le périphérique soit inactif.
		*/
		virtual void waitIdle()const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline std::vector< QueueFamilyProperties > const & getQueueProperties()const
		{
			return m_gpu.getQueueProperties();
		}

		inline PhysicalDeviceProperties const & getProperties()const
		{
			return m_gpu.getProperties();
		}

		inline PhysicalDeviceMemoryProperties const & getMemoryProperties()const
		{
			return m_gpu.getMemoryProperties();
		}

		inline PhysicalDeviceFeatures const & getFeatures()const
		{
			return m_gpu.getFeatures();
		}

		inline Queue const & getPresentQueue()const
		{
			return *m_presentQueue;
		}

		inline Queue const & getComputeQueue()const
		{
			return *m_computeQueue;
		}

		inline Queue const & getGraphicsQueue()const
		{
			return *m_graphicsQueue;
		}

		inline CommandPool const & getPresentCommandPool()const
		{
			return *m_presentCommandPool;
		}

		inline CommandPool const & getComputeCommandPool()const
		{
			return *m_computeCommandPool;
		}

		inline CommandPool const & getGraphicsCommandPool()const
		{
			return *m_graphicsCommandPool;
		}

		inline Renderer const & getRenderer()const
		{
			return m_renderer;
		}

		inline PhysicalDevice const & getPhysicalDevice()const
		{
			return m_gpu;
		}
		/**@}*/
		/**
		*\~english
		*\return
		*	The number of nanoseconds it takes for timestamp value to be incremented by 1.
		*\~french
		*\return
		*	Le nombre de nanosecondes qu'il faut pour que la valeur d'un timestamp soit incrémentée de 1.
		*/
		inline float getTimestampPeriod()const
		{
			return m_timestampPeriod;
		}

	private:
		/**
		*\~french
		*\brief
		*	Active le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Enables the device's context (for OpenGL).
		*/
		virtual void doEnable()const = 0;
		/**
		*\~french
		*\brief
		*	Désactive le contexte du périphérique (pour OpenGL).
		*\~english
		*\brief
		*	Disables the device's context (for OpenGL).
		*/
		virtual void doDisable()const = 0;

	public:
		DeviceEnabledSignal onEnabled;
		DeviceDisabledSignal onDisabled;

	protected:
		Renderer const & m_renderer;
		PhysicalDevice const & m_gpu;
		QueuePtr m_presentQueue;
		QueuePtr m_computeQueue;
		QueuePtr m_graphicsQueue;
		CommandPoolPtr m_presentCommandPool;
		CommandPoolPtr m_computeCommandPool;
		CommandPoolPtr m_graphicsCommandPool;
		float m_timestampPeriod;
	};
}

#endif
