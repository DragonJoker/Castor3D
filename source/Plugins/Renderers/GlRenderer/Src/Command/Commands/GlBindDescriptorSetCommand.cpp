/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBindDescriptorSetCommand.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Buffer/GlBufferView.hpp"
#include "Descriptor/GlDescriptorSet.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Image/GlSampler.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"
#include "Buffer/GlUniformBuffer.hpp"

#include <Descriptor/DescriptorSetLayoutBinding.hpp>

namespace gl_renderer
{
	namespace
	{
		void bind( renderer::CombinedTextureSamplerBinding const & binding )
		{
			glLogCall( gl::ActiveTexture, GL_TEXTURE0 + binding.getBinding().getBindingPoint() );
			glLogCall( gl::BindTexture
				, convert( binding.getView().getType() )
				, static_cast< TextureView const & >( binding.getView() ).getImage() );
			glLogCall( gl::BindSampler
				, binding.getBinding().getBindingPoint()
				, static_cast< Sampler const & >( binding.getSampler() ).getSampler() );
		}

		void bind( renderer::SamplerBinding const & binding )
		{
			glLogCall( gl::BindSampler
				, binding.getBinding().getBindingPoint()
				, static_cast< Sampler const & >( binding.getSampler() ).getSampler() );
		}

		void bind( renderer::SampledTextureBinding const & binding )
		{
			glLogCall( gl::ActiveTexture, GL_TEXTURE0 + binding.getBinding().getBindingPoint() );
			glLogCall( gl::BindTexture
				, convert( binding.getView().getType() )
				, static_cast< TextureView const & >( binding.getView() ).getImage() );
		}

		void bind( renderer::StorageTextureBinding const & binding )
		{
			auto & view = binding.getView();
			auto & range = view.getSubResourceRange();
			glLogCall( gl::ActiveTexture, GL_TEXTURE0 + binding.getBinding().getBindingPoint() );
			glLogCall( gl::BindImageTexture
				, binding.getBinding().getBindingPoint()
				, static_cast< TextureView const & >( view ).getImage()
				, range.getBaseMipLevel()
				, range.getLayerCount() > 0
				, range.getBaseArrayLayer()
				, GL_ACCESS_TYPE_READ_WRITE
				, getFormat( view.getFormat() ) );
		}

		void bind( renderer::UniformBufferBinding const & binding )
		{
			glLogCall( gl::BindBufferRange
				, GL_BUFFER_TARGET_UNIFORM
				, binding.getBinding().getBindingPoint()
				, static_cast< Buffer const & >( binding.getUniformBuffer().getBuffer() ).getBuffer()
				, binding.getOffset()
				, binding.getRange() );
		}

		void bind( renderer::StorageBufferBinding const & binding )
		{
			glLogCall( gl::BindBufferRange
				, GL_BUFFER_TARGET_SHADER_STORAGE
				, binding.getBinding().getBindingPoint()
				, static_cast< Buffer const & >( binding.getBuffer() ).getBuffer()
				, binding.getOffset()
				, binding.getRange() );
		}

		void bind( renderer::TexelBufferBinding const & binding )
		{
			glLogCall( gl::ActiveTexture, GL_TEXTURE0 + binding.getBinding().getBindingPoint() );
			glLogCall( gl::BindTexture
				, GL_BUFFER_TARGET_TEXTURE
				, static_cast< BufferView const & >( binding.getView() ).getImage() );
		}
	}

	BindDescriptorSetCommand::BindDescriptorSetCommand( renderer::DescriptorSet const & descriptorSet
		, renderer::PipelineLayout const & layout
		, renderer::PipelineBindPoint bindingPoint )
		: m_descriptorSet{ static_cast< DescriptorSet const & >( descriptorSet ) }
		, m_layout{ static_cast< PipelineLayout const & >( layout ) }
		, m_bindingPoint{ bindingPoint }
	{
	}

	void BindDescriptorSetCommand::apply()const
	{
		glLogCommand( "BindDescriptorSetCommand" );
		for ( auto & descriptor : m_descriptorSet.getCombinedTextureSamplers() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getSamplers() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getSampledTextures() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getStorageTextures() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getUniformBuffers() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getStorageBuffers() )
		{
			bind( descriptor );
		}

		for ( auto & descriptor : m_descriptorSet.getTexelBuffers() )
		{
			bind( descriptor );
		}
	}

	CommandPtr BindDescriptorSetCommand::clone()const
	{
		return std::make_unique< BindDescriptorSetCommand >( *this );
	}
}
