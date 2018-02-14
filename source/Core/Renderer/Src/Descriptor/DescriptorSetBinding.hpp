/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_DescriptorSetBinding_HPP___
#define ___Renderer_DescriptorSetBinding_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	A binding un a descriptor set.
	*\~french
	*\brief
	*	Attache dans un set descripteur.
	*/
	class DescriptorSetBinding
	{
	public:
		DescriptorSetBinding( DescriptorSetBinding const & ) = delete;
		DescriptorSetBinding( DescriptorSetBinding && ) = default;
		DescriptorSetBinding & operator=( DescriptorSetBinding const & ) = delete;
		DescriptorSetBinding & operator=( DescriptorSetBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] index
		*	L'index de tableau.
		*/
		DescriptorSetBinding( DescriptorSetLayoutBinding const & layoutBinding
			, uint32_t index )
			: m_binding{ layoutBinding }
			, m_index{ index }
		{
		}
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~DescriptorSetBinding() = default;
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline DescriptorSetLayoutBinding const & getBinding()const
		{
			return m_binding;
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**\}*/

	private:
		DescriptorSetLayoutBinding const & m_binding;
		uint32_t m_index;
	};
	/**
	*\~english
	*\brief
	*	Combined texture and sampler descriptor.
	*\~french
	*\brief
	*	Attache de type échantillonneur + texture.
	*/
	class CombinedTextureSamplerBinding
		: public DescriptorSetBinding
	{
	public:
		CombinedTextureSamplerBinding( CombinedTextureSamplerBinding const & ) = delete;
		CombinedTextureSamplerBinding( CombinedTextureSamplerBinding && ) = default;
		CombinedTextureSamplerBinding & operator=( CombinedTextureSamplerBinding const & ) = delete;
		CombinedTextureSamplerBinding & operator=( CombinedTextureSamplerBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The texture view.
		*\param[in] sampler
		*	The sampler.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] sampler
		*	L'échantillonneur.
		*\param[in] index
		*	L'index de tableau.
		*/
		CombinedTextureSamplerBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, Sampler const & sampler
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_view{ view }
			, m_sampler{ sampler }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline TextureView const & getView()const
		{
			return m_view;
		}

		inline Sampler const & getSampler()const
		{
			return m_sampler;
		}
		/**\}*/

	private:
		TextureView const & m_view;
		Sampler const & m_sampler;
	};
	/**
	*\~english
	*\brief
	*	Sampler descriptor.
	*\~french
	*\brief
	*	Attache de type échantillonneur.
	*/
	class SamplerBinding
		: public DescriptorSetBinding
	{
	public:
		SamplerBinding( SamplerBinding const & ) = delete;
		SamplerBinding( SamplerBinding && ) = default;
		SamplerBinding & operator=( SamplerBinding const & ) = delete;
		SamplerBinding & operator=( SamplerBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] sampler
		*	The sampler.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] sampler
		*	L'échantillonneur.
		*\param[in] index
		*	L'index de tableau.
		*/
		SamplerBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Sampler const & sampler
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_sampler{ sampler }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline Sampler const & getSampler()const
		{
			return m_sampler;
		}
		/**\}*/

	private:
		Sampler const & m_sampler;
	};
	/**
	*\~english
	*\brief
	*	Sampled texture descriptor.
	*\~french
	*\brief
	*	Attache de type texture échantillonnée.
	*/
	class SampledTextureBinding
		: public DescriptorSetBinding
	{
	public:
		SampledTextureBinding( SampledTextureBinding const & ) = delete;
		SampledTextureBinding( SampledTextureBinding && ) = default;
		SampledTextureBinding & operator=( SampledTextureBinding const & ) = delete;
		SampledTextureBinding & operator=( SampledTextureBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The texture view.
		*\param[in] layout
		*	The image layout.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] layout
		*	Le layout de l'image.
		*\param[in] index
		*	L'index de tableau.
		*/
		SampledTextureBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, ImageLayout layout
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_view{ view }
			, m_layout{ layout }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline TextureView const & getView()const
		{
			return m_view;
		}

		inline ImageLayout getLayout()const
		{
			return m_layout;
		}
		/**\}*/

	private:
		TextureView const & m_view;
		ImageLayout m_layout;
	};
	/**
	*\~english
	*\brief
	*	Storage texture descriptor.
	*\~french
	*\brief
	*	Attache de type texture de stockage.
	*/
	class StorageTextureBinding
		: public DescriptorSetBinding
	{
	public:
		StorageTextureBinding( StorageTextureBinding const & ) = delete;
		StorageTextureBinding( StorageTextureBinding && ) = default;
		StorageTextureBinding & operator=( StorageTextureBinding const & ) = delete;
		StorageTextureBinding & operator=( StorageTextureBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] view
		*	The texture view.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] index
		*	L'index de tableau.
		*/
		StorageTextureBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_view{ view }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline TextureView const & getView()const
		{
			return m_view;
		}
		/**\}*/

	private:
		TextureView const & m_view;
	};
	/**
	*\~english
	*\brief
	*	Uniform buffer descriptor.
	*\~french
	*\brief
	*	Attache de type tampon de variables uniformes.
	*/
	class UniformBufferBinding
		: public DescriptorSetBinding
	{
	public:
		UniformBufferBinding( UniformBufferBinding const & ) = delete;
		UniformBufferBinding( UniformBufferBinding && ) = default;
		UniformBufferBinding & operator=( UniformBufferBinding const & ) = delete;
		UniformBufferBinding & operator=( UniformBufferBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] uniformBuffer
		*	The uniform buffer.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The range size in the buffer.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\param[in] offset
		*	Le décalage dans tampon.
		*\param[in] range
		*	La taille de l'intrvalle dans le tampon.
		*\param[in] index
		*	L'index de tableau.
		*/
		UniformBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBufferBase const & uniformBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_uniformBuffer{ uniformBuffer }
			, m_offset{ offset }
			, m_range{ range }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline UniformBufferBase const & getUniformBuffer()const
		{
			return m_uniformBuffer;
		}

		inline uint32_t getOffset()const
		{
			return m_offset;
		}

		inline uint32_t getRange()const
		{
			return m_range;
		}
		/**\}*/

	private:
		UniformBufferBase const & m_uniformBuffer;
		uint32_t m_offset;
		uint32_t m_range;
	};
	/**
	*\~english
	*\brief
	*	Storage buffer descriptor.
	*\~french
	*\brief
	*	Attache de type tampon de stockage.
	*/
	class StorageBufferBinding
		: public DescriptorSetBinding
	{
	public:
		StorageBufferBinding( StorageBufferBinding const & ) = delete;
		StorageBufferBinding( StorageBufferBinding && ) = default;
		StorageBufferBinding & operator=( StorageBufferBinding const & ) = delete;
		StorageBufferBinding & operator=( StorageBufferBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] storageBuffer
		*	The GPU buffer.
		*\param[in] offset
		*	The offset in the buffer.
		*\param[in] range
		*	The range size in the buffer.
		*\param[in] index
		*	The array index.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] storageBuffer
		*	Le tampon GPU.
		*\param[in] offset
		*	Le décalage dans tampon.
		*\param[in] range
		*	La taille de l'intrvalle dans le tampon.
		*\param[in] index
		*	L'index de tableau.
		*/
		StorageBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & storageBuffer
			, uint32_t offset
			, uint32_t range
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_buffer{ storageBuffer }
			, m_offset{ offset }
			, m_range{ range }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}

		inline uint32_t getOffset()const
		{
			return m_offset;
		}

		inline uint32_t getRange()const
		{
			return m_range;
		}
		/**\}*/

	private:
		BufferBase const & m_buffer;
		uint32_t m_offset;
		uint32_t m_range;
	};
	/**
	*\~english
	*\brief
	*	Texel buffer descriptor.
	*\~french
	*\brief
	*	Attache de type tampon de texels.
	*/
	class TexelBufferBinding
		: public DescriptorSetBinding
	{
	public:
		TexelBufferBinding( TexelBufferBinding const & ) = delete;
		TexelBufferBinding( TexelBufferBinding && ) = default;
		TexelBufferBinding & operator=( TexelBufferBinding const & ) = delete;
		TexelBufferBinding & operator=( TexelBufferBinding && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] layoutBinding
		*	The layout binding.
		*\param[in] storageBuffer
		*	The GPU buffer.
		*\param[in] view
		*	The view to the buffer.
		*\param[in] index
		*	The array index.
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] view
		*	La vue sur le tampon.
		*\param[in] index
		*	L'index de tableau.
		*/
		TexelBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & buffer
			, BufferView const & view
			, uint32_t index )
			: DescriptorSetBinding{ layoutBinding, index }
			, m_buffer{ buffer }
			, m_view{ view }
		{
		}
		/**
		*\~english
		*\name Getters.
		*\~french
		*\name Accesseurs.
		*/
		/**\{*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}

		inline BufferView const & getView()const
		{
			return m_view;
		}
		/**\}*/

	private:
		BufferBase const & m_buffer;
		BufferView const & m_view;
	};
}

#endif
