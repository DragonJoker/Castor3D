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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*/
		DescriptorSetBinding( DescriptorSetLayoutBinding const & layoutBinding )
			: m_binding{ layoutBinding }
		{
		}
		/**
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*/
		virtual ~DescriptorSetBinding() = default;
		/**
		*\return
		*	L'attache de layout.
		*/
		inline DescriptorSetLayoutBinding const & getBinding()const
		{
			return m_binding;
		}

	private:
		DescriptorSetLayoutBinding const & m_binding;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] sampler
		*	L'échantillonneur.
		*/
		CombinedTextureSamplerBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, Sampler const & sampler )
			: DescriptorSetBinding{ layoutBinding }
			, m_view{ view }
			, m_sampler{ sampler }
		{
		}
		/**
		*\return
		*	L'image.
		*/
		inline TextureView const & getView()const
		{
			return m_view;
		}
		/**
		*\return
		*	L'échantillonneur.
		*/
		inline Sampler const & getSampler()const
		{
			return m_sampler;
		}

	private:
		TextureView const & m_view;
		Sampler const & m_sampler;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] sampler
		*	L'échantillonneur.
		*/
		SamplerBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Sampler const & sampler )
			: DescriptorSetBinding{ layoutBinding }
			, m_sampler{ sampler }
		{
		}
		/**
		*\return
		*	L'échantillonneur.
		*/
		inline Sampler const & getSampler()const
		{
			return m_sampler;
		}

	private:
		Sampler const & m_sampler;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*\param[in] layout
		*	Le layout de l'image.
		*/
		SampledTextureBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view
			, ImageLayout layout )
			: DescriptorSetBinding{ layoutBinding }
			, m_view{ view }
			, m_layout{ layout }
		{
		}
		/**
		*\return
		*	L'image.
		*/
		inline TextureView const & getView()const
		{
			return m_view;
		}
		/**
		*\return
		*	Le layout de l'image.
		*/
		inline ImageLayout getLayout()const
		{
			return m_layout;
		}

	private:
		TextureView const & m_view;
		ImageLayout m_layout;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] view
		*	L'image.
		*/
		StorageTextureBinding( DescriptorSetLayoutBinding const & layoutBinding
			, TextureView const & view )
			: DescriptorSetBinding{ layoutBinding }
			, m_view{ view }
		{
		}
		/**
		*\return
		*	L'image.
		*/
		inline TextureView const & getView()const
		{
			return m_view;
		}

	private:
		TextureView const & m_view;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] uniformBuffer
		*	Le tampon.
		*\return
		*	L'attache créée.
		*/
		UniformBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, UniformBufferBase const & uniformBuffer
			, uint32_t offset )
			: DescriptorSetBinding{ layoutBinding }
			, m_uniformBuffer{ uniformBuffer }
			, m_offset{ offset }
		{
		}
		/**
		*\return
		*	Le tampon d'uniformes.
		*/
		inline UniformBufferBase const & getUniformBuffer()const
		{
			return m_uniformBuffer;
		}
		/**
		*\return
		*	L'offset.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}

	private:
		UniformBufferBase const & m_uniformBuffer;
		uint32_t m_offset;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] storageBuffer
		*	Le tampon GPU.
		*\return
		*	L'attache créée.
		*/
		StorageBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & storageBuffer
			, uint32_t offset )
			: DescriptorSetBinding{ layoutBinding }
			, m_buffer{ storageBuffer }
			, m_offset{ offset }
		{
		}
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}
		/**
		*\return
		*	L'offset.
		*/
		inline uint32_t getOffset()const
		{
			return m_offset;
		}

	private:
		BufferBase const & m_buffer;
		uint32_t m_offset;
	};
	/**
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
		*\brief
		*	Constructeur.
		*\param[in] layoutBinding
		*	L'attache de layout.
		*\param[in] buffer
		*	Le tampon.
		*\param[in] view
		*	La vue sur le tampon.
		*\return
		*	L'attache créée.
		*/
		TexelBufferBinding( DescriptorSetLayoutBinding const & layoutBinding
			, BufferBase const & buffer
			, BufferView const & view )
			: DescriptorSetBinding{ layoutBinding }
			, m_buffer{ buffer }
			, m_view{ view }
		{
		}
		/**
		*\return
		*	Le tampon.
		*/
		inline BufferBase const & getBuffer()const
		{
			return m_buffer;
		}
		/**
		*\return
		*	La vue.
		*/
		inline BufferView const & getView()const
		{
			return m_view;
		}

	private:
		BufferBase const & m_buffer;
		BufferView const & m_view;
	};
}

#endif
