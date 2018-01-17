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
	*	Attache de type sampler + texture.
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
			, Texture const & view
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
		inline Texture const & getTexture()const
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
		Texture const & m_view;
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
		*/
		SampledTextureBinding( DescriptorSetLayoutBinding const & layoutBinding
			, Texture const & view )
			: DescriptorSetBinding{ layoutBinding }
			, m_view{ view }
		{
		}
		/**
		*\return
		*	L'image.
		*/
		inline Texture const & getTexture()const
		{
			return m_view;
		}

	private:
		Texture const & m_view;
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
		*	Crée une attache de type tampon de variables uniformes.
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
}

#endif
