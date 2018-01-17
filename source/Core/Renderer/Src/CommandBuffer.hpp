/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_CommandBuffer_HPP___
#define ___Renderer_CommandBuffer_HPP___
#pragma once

#include "ClearValue.hpp"

namespace renderer
{
	/**
	*\brief
	*	Tampon de commandes.
	*/
	class CommandBuffer
	{
	protected:
		CommandBuffer( CommandBuffer const & ) = delete;
		CommandBuffer & operator=( CommandBuffer const & ) = delete;
		/**
		*\brief
		*	Constructeur par défaut.
		*/
		CommandBuffer() = default;
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] pool
		*	Le pool de tampons de commandes.
		*\param[in] primary
		*	Dit si le tampon est un tampon de commandes primaire (\p true) ou secondaire (\p false).
		*/
		CommandBuffer( Device const & device
			, CommandPool const & pool
			, bool primary = true );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~CommandBuffer() = default;
		/**
		*\brief
		*	Démarre l'enregistrement du tampon de commandes.
		*\param[in] flags
		*	Les indicateurs de type de charge qui sera affectée au tampon.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool begin( CommandBufferUsageFlags flags = 0u )const = 0;
		/**
		*\brief
		*	Démarre l'enregistrement du tampon de commandes en tant que tampon secondaire.
		*\param[in] flags
		*	Les indicateurs de type de charge qui sera affectée au tampon.
		*\param[in] renderPass
		*	La passe de rendu avec laquelle le tampon sera compatible, et dans laquelle il peut s'exécuter.
		*\param[in] subpass
		*	L'indice de la sous-passe au sein de laquelle le tampon de commandes sera exécuté.
		*\param[in] frameBuffer
		*	Le tampon d'images dans lequel le tampon de commandes va effectuer son rendu.
		*\param[in] occlusionQueryEnable
		*	Indique si le tampon de commandes peut être exécuté alors qu'une requête d'occlusion est active sur le tampon principal.
		*\param[in] queryFlags
		*	Les indicateurs de requête d'occlusion pouvant être utilisées par une requête d'occlusion active sur le tampon principal,
		*	lorsque ce tampon est exécuté.
		*\param[in] pipelineStatistics
		*	Indique quelles statistique de pipeline peuvent être comptées par une requête active sur le tampon principal,
		*	lorsque ce tampon est exécuté.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool begin( CommandBufferUsageFlags flags
			, RenderPass const & renderPass
			, uint32_t subpass
			, FrameBuffer const & frameBuffer
			, bool occlusionQueryEnable = false
			, QueryControlFlags queryFlags = 0u
			, QueryPipelineStatisticFlags pipelineStatistics = 0u )const = 0;
		/**
		*\brief
		*	Termine l'enregistrement du tampon de commandes.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool end()const = 0;
		/**
		*\brief
		*	Réinitialise le tampon de commandes et le met dans un état où il peut à nouveau enregistrer des commandes.
		*\param[in] flags
		*	Les indicateurs contrôlant le comportement de la réinitialisation du tampon.
		*\return
		*	\p false en cas d'erreur.
		*/
		virtual bool reset( CommandBufferResetFlags flags = 0u )const = 0;
		/**
		*\brief
		*	Démarre une passe de rendu.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] frameBuffer
		*	Le tampon d'image affecté par le rendu.
		*\param[in] clearValues
		*	Les valeurs de vidage, une par attache de la passe de rendu.
		*\param[in] contents
		*	Indique la manière dont les commandes de la première sous-passe sont fournies.
		*/
		virtual void beginRenderPass( RenderPass const & renderPass
			, FrameBuffer const & frameBuffer
			, ClearValueArray const & clearValues
			, SubpassContents contents )const = 0;
		/**
		*\brief
		*	Passe à la sous-passe suivante.
		*\param[in] contents
		*	Indique la manière dont les commandes de la sous-passe suivante sont fournies.
		*/
		virtual void nextSubpass( SubpassContents contents )const = 0;
		/**
		*\brief
		*	Termine une passe de rendu.
		*/
		virtual void endRenderPass()const = 0;
		/**
		*\brief
		*	Execute des tampons de commande secondaires.
		*\param[in] commands
		*	Les tampons de commandes.
		*/
		virtual void executeCommands( CommandBufferCRefArray const & commands )const = 0;
		/**
		*\brief
		*	Vide l'image avec la couleur de vidage.
		*\param[in] image
		*	L'image à vider.
		*\param[in] colour
		*	La couleur de vidage.
		*/
		virtual void clear( Texture const & image
			, RgbaColour const & colour )const = 0;
		/**
		*\brief
		*	Met en place une barrière de transition d'état de tampon.
		*\param[in] after
		*	Les étapes devant être terminées avant l'exécution de la barrière.
		*\param[in] before
		*	Les étapes pouvant être commencées après l'exécution de la barrière.
		*\param[in] transitionBarrier
		*	La description de la transition.
		*/
		virtual void memoryBarrier( PipelineStageFlags after
			, PipelineStageFlags before
			, BufferMemoryBarrier const & transitionBarrier )const = 0;
		/**
		*\brief
		*	Met en place une barrière de transition de layout d'image.
		*\param[in] after
		*	Les étapes devant être terminées avant l'exécution de la barrière.
		*\param[in] before
		*	Les étapes pouvant être commencées après l'exécution de la barrière.
		*\param[in] transitionBarrier
		*	La description de la transition.
		*/
		virtual void memoryBarrier( PipelineStageFlags after
			, PipelineStageFlags before
			, ImageMemoryBarrier const & transitionBarrier )const = 0;
		/**
		*\brief
		*	Active un pipeline: shaders, tests, états, ...
		*\param[in] pipeline
		*	Le pipeline à activer.
		*\param[in] bindingPoint
		*	Le point d'attache du pipeline.
		*/
		virtual void bindPipeline( Pipeline const & pipeline
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const = 0;
		/**
		*\brief
		*	Active des tampons de géométrie.
		*\param[in] geometryBuffers
		*	Les tampons de géométrie.
		*/
		virtual void bindGeometryBuffers( GeometryBuffers const & geometryBuffers )const = 0;
		/**
		*\brief
		*	Active un descriptor set.
		*\param[in] descriptorSet
		*	Le descriptor set.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*/
		virtual void bindDescriptorSet( DescriptorSet const & descriptorSet
			, PipelineLayout const & layout
			, PipelineBindPoint bindingPoint = PipelineBindPoint::eGraphics )const = 0;
		/**
		*\brief
		*	Définit le viewport du pipeline.
		*\remarks
		*	Cette action n'est faisable que si le viewport est configuré comme dynamique.
		*\param[in] viewport
		*	Le viewport.
		*/
		virtual void setViewport( Viewport const & viewport )const = 0;
		/**
		*\brief
		*	Définit le ciseau du pipeline.
		*\remarks
		*	Cette action n'est faisable que si le ciseau est configuré comme dynamique.
		*\param[in] scissor
		*	Le ciseau.
		*/
		virtual void setScissor( Scissor const & scissor )const = 0;
		/**
		*\brief
		*	Dessine des sommets.
		*\param[in] vtxCount
		*	Nombre de sommets.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstVertex
		*	Index du premier sommet.
		*\param[in] firstInstance
		*	Index de la première instance.
		*/
		virtual void draw( uint32_t vtxCount
			, uint32_t instCount
			, uint32_t firstVertex
			, uint32_t firstInstance )const = 0;
		/**
		*\brief
		*	Dessine des sommets.
		*\param[in] indexCount
		*	Nombre d'indices.
		*\param[in] instCount
		*	Nombre d'instances.
		*\param[in] firstIndex
		*	Index du premier indice.
		*\param[in] vertexOffset
		*	La valeur ajoutée à l'indice du sommet avant d'indexer le tampon de sommets.
		*\param[in] firstInstance
		*	Index de la première instance.
		*/
		virtual void drawIndexed( uint32_t indexCount
			, uint32_t instCount
			, uint32_t firstIndex
			, uint32_t vertexOffset
			, uint32_t firstInstance )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( BufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( BufferBase const & src
			, VertexBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( VertexBufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( VertexBufferBase const & src
			, VertexBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( BufferBase const & src
			, UniformBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( UniformBufferBase const & src
			, BufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers un autre tampon.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*\param[in] size
		*	La taille des données à copier.
		*/
		virtual void copyBuffer( UniformBufferBase const & src
			, UniformBufferBase const & dst
			, uint32_t size
			, uint32_t offset = 0 )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*/
		virtual void copyImage( BufferBase const & src
			, Texture const & dst )const = 0;
		/**
		*\brief
		*	Copie les données d'un tampon vers une image.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	L'image destination.
		*/
		virtual void copyImage( StagingBuffer const & src
			, Texture const & dst )const = 0;
	};
}

#endif
