/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSet_HPP___
#define ___Renderer_DescriptorSet_HPP___
#pragma once

#include "Descriptor/DescriptorSetBinding.hpp"

#include <vector>

namespace renderer
{
	/**
	*\~french
	*\brief
	*	Set de descripteurs.
	*\~english
	*\brief
	*	A descriptor set.
	*/
	class DescriptorSet
	{
	protected:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] pool
		*	Le pool parent.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] pool
		*	The parent pool.
		*/
		DescriptorSet( DescriptorSetPool const & pool );

	public:
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		virtual ~DescriptorSet() = default;
		/**
		*\~french
		*\brief
		*	Crée une attache de type image et échantillonneur combinés.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] sampler
		*	L'échantillonneur.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a combined image and sampler binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The image.
		*\param[in] sampler
		*	The sampler.
		*\return
		*	The created binding.
		*/
		virtual CombinedTextureSamplerBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, Sampler const & sampler ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type échantillonneur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] sampler
		*	L'échantillonneur.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a sampler binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] sampler
		*	The sampler.
		*\return
		*	The created binding.
		*/
		virtual SamplerBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Sampler const & sampler ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type image échantillonée.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a sampled image binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The image.
		*\return
		*	The created binding.
		*/
		virtual SampledTextureBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, ImageLayout layout ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type image de stockage.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a storage image binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The image.
		*\return
		*	The created binding.
		*/
		virtual StorageTextureBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon uniforme.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\param[in] offset
		*	Le décalage de l'attache dans le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a uniform buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] uniformBuffer
		*	The buffer.
		*\param[in] offset
		*	The attach's offset in the buffer.
		*\return
		*	The created binding.
		*/
		virtual UniformBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBufferBase const & uniformBuffer
			, uint32_t offset ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de stockage.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] storageBuffer
		*	Le tampon.
		*\param[in] offset
		*	Le décalage de l'attache dans le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a storage buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] storageBuffer
		*	The buffer.
		*\param[in] offset
		*	The attach's offset in the buffer.
		*\return
		*	The created binding.
		*/
		virtual StorageBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & storageBuffer
			, uint32_t offset ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de texels.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] buffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a texel buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] buffer
		*	The buffer.
		*\param[in] view
		*	The view to the buffer.
		*\return
		*	The created binding.
		*/
		virtual TexelBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & buffer
			, BufferView const & view ) = 0;
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de variables uniformes.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a uniform buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] uniformBuffer
		*	The buffer.
		*\param[in] offset
		*	The attach's offset in the buffer.
		*\return
		*	The created binding.
		*/
		template< typename T >
		inline UniformBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBuffer< T > const & uniformBuffer
			, uint32_t offset )
		{
			return createBinding( layoutBinding
				, uniformBuffer.getUbo()
				, offset );
		}
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de stockage.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] storageBuffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a storage buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] storageBuffer
		*	The buffer.
		*\param[in] offset
		*	The attach's offset in the buffer.
		*\return
		*	The created binding.
		*/
		template< typename T >
		inline StorageBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Buffer< T > const & storageBuffer
			, uint32_t offset )
		{
			return createBinding( layoutBinding
				, storageBuffer.getBuffer()
				, offset );
		}
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de texels.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] view
		*	La vue sur le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a texel buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] buffer
		*	The buffer.
		*\param[in] view
		*	The view to the buffer.
		*\return
		*	The created binding.
		*/
		template< typename T >
		inline TexelBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBuffer< T > const & buffer
			, BufferView const & view )
		{
			return createBinding( layoutBinding
				, buffer.getUbo()
				, view );
		}
		/**
		*\~french
		*\brief
		*	Crée une attache de type tampon de texels.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] view
		*	La vue sur le tampon.
		*\return
		*	L'attache créée.
		*\~english
		*\brief
		*	Creates a texel buffer binding.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] buffer
		*	The buffer.
		*\param[in] view
		*	The view to the buffer.
		*\return
		*	The created binding.
		*/
		template< typename T >
		inline TexelBufferBinding const & createBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Buffer< T > const & buffer
			, BufferView const & view )
		{
			return createBinding( layoutBinding
				, buffer.getBuffer()
				, view );
		}
		/**
		*\~french
		*\brief
		*	Met à jour toutes les attaches du set de descripteurs.
		*\~english
		*\brief
		*	Updates all the bindings in the descriptor set.
		*/
		virtual void update()const = 0;
	};
}

#endif
