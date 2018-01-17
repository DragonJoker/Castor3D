/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Device_HPP___
#define ___Renderer_Device_HPP___
#pragma once

#include "ColourBlendState.hpp"
#include "CommandPool.hpp"
#include "Connection.hpp"
#include "Queue.hpp"
#include "RasterisationState.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe contenant les informations liées au GPU logique.
	*/
	class Device
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] connection
		*	La connection à l'application.
		*/
		Device( Renderer const & renderer
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
		*\brief
		*	Crée une passe de rendu.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] clear
		*	Dit si l'on veut vider le contenu des images au chargement de la passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*\return
		*	La passe créée.
		*/
		virtual RenderPassPtr createRenderPass( std::vector< PixelFormat > const & formats
			, RenderSubpassPtrArray const & subpasses
			, RenderPassState const & initialState
			, RenderPassState const & finalState
			, bool clear = true
			, SampleCountFlag samplesCount = SampleCountFlag::e1 )const = 0;
		/**
		*\~french
		*\brief
		*	Crée une sous-passe de rendu.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] neededState
		*	L'état voulu pour l'exécution de cette sous passe.
		*\return
		*	La sous-passe créée.
		*\~english
		*\brief
		*	Creates a render subpass.
		*\param[in] formats
		*	The attachments pixels formats.
		*\param[in] neededState
		*	The state wanted for this subpass execution.
		*\return
		*	The created subpass
		*/
		virtual RenderSubpassPtr createRenderSubpass( std::vector< PixelFormat > const & formats
			, RenderSubpassState const & neededState )const = 0;
		/**
		*\brief
		*	Crée un layout de sommets.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*\return
		*	Le layout créé.
		*/
		virtual VertexLayoutPtr createVertexLayout( uint32_t bindingSlot
			, uint32_t stride )const = 0;
		/**
		*\brief
		*	Crée un layout de pipeline.
		*\return
		*	Le layout créé.
		*/
		virtual PipelineLayoutPtr createPipelineLayout()const = 0;
		/**
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\return
		*	Le layout créé.
		*/
		virtual PipelineLayoutPtr createPipelineLayout( DescriptorSetLayout const & layout )const = 0;
		/**
		*\brief
		*	Crée tampon de géométries.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\return
		*	Le layout créé.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout )const = 0;
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] vboOffset
		*	L'offset du premier sommet dans le VBO.
		*\param[in] layout
		*	Le layout.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type )const = 0;
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, VertexLayoutCRefArray const & layouts )const = 0;
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		virtual GeometryBuffersPtr createGeometryBuffers( VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > vboOffsets
			, VertexLayoutCRefArray const & layouts
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type )const = 0;
		/**
		*\brief
		*	Crée un pipeline.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] program
		*	Le programme shader.
		*\param[in] vertexBuffers
		*	Les tampons de sommets utilisés.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*\return
		*	Le pipeline créé.
		*/
		virtual PipelinePtr createPipeline( PipelineLayout const & layout
			, ShaderProgram const & program
			, VertexLayoutCRefArray const & vertexLayouts
			, RenderPass const & renderPass
			, PrimitiveTopology topology
			, RasterisationState const & rasterisationState = RasterisationState{}
			, ColourBlendState const & colourBlendState = ColourBlendState::createDefault() )const = 0;
		/**
		*\brief
		*	Crée un layout de set de descripteurs.
		*\param[in] bindings
		*	Les attaches du layout.
		*\return
		*	Le layout créé.
		*/
		virtual DescriptorSetLayoutPtr createDescriptorSetLayout( DescriptorSetLayoutBindingArray && bindings )const = 0;
		/**
		*\brief
		*	Crée une texture.
		*/
		virtual TexturePtr createTexture()const = 0;
		/**
		*\brief
		*	Crée un échantillonneur.
		*\param[in] wrapS, wrapT, wrapR
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le filtre de mipmap.
		*/
		virtual SamplerPtr createSampler( WrapMode wrapS
			, WrapMode wrapT
			, WrapMode wrapR
			, Filter minFilter
			, Filter magFilter
			, MipmapMode mipFilter = MipmapMode::eNone )const = 0;
		/**
		*\brief
		*	Crée un tampon GPU.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual BufferBasePtr createBuffer( uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags memoryFlags )const = 0;
		/**
		*\brief
		*	Crée un tampon d'uniformes.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] size
		*	La taille d'un élément.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual UniformBufferBasePtr createUniformBuffer( uint32_t count
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags memoryFlags )const = 0;
		/**
		*\brief
		*	Crée une swap chain.
		*\param[in] size
		*	Les dimensions souhaitées.
		*/
		virtual SwapChainPtr createSwapChain( IVec2 const & size )const = 0;
		/**
		*\brief
		*	Crée un sémaphore.
		*/
		virtual SemaphorePtr createSemaphore()const = 0;
		/**
		*\brief
		*	Crée un pool de tampons de commandes.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Combinaison binaire de CommandPoolCreateFlag.
		*/
		virtual CommandPoolPtr createCommandPool( uint32_t queueFamilyIndex
			, CommandPoolCreateFlags const & flags = 0 )const = 0;
		/**
		*\brief
		*	Crée un programme shader.
		*/
		virtual ShaderProgramPtr createShaderProgram()const = 0;
		/**
		*\brief
		*	Attend que le périphérique soit inactif.
		*/
		virtual void waitIdle()const = 0;
		/**
		*\brief
		*	Le numéro de version.
		*/
		inline std::string const & getVersion()const
		{
			return m_version;
		}
		/**
		*\return
		*	La file de présentation.
		*/
		inline Queue const & getPresentQueue()const
		{
			return *m_presentQueue;
		}
		/**
		*\return
		*	La file de dessin.
		*/
		inline Queue const & getGraphicsQueue()const
		{
			return *m_graphicsQueue;
		}
		/**
		*\return
		*	Le pool de tampons de commandes pour la file de présentation.
		*/
		inline CommandPool const & getPresentCommandPool()const
		{
			return *m_presentCommandPool;
		}
		/**
		*\return
		*	Le pool de tampons de commandes pour la file de dessin.
		*/
		inline CommandPool const & getGraphicsCommandPool()const
		{
			return *m_graphicsCommandPool;
		}

	protected:
		QueuePtr m_presentQueue;
		QueuePtr m_graphicsQueue;
		CommandPoolPtr m_presentCommandPool;
		CommandPoolPtr m_graphicsCommandPool;
		std::string m_version;
	};
}

#endif
