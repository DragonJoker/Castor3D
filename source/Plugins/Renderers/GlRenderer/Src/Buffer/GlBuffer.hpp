/**
*\file
*	RenderingResources.h
*\author
*	Sylvain Doremus
*/
#ifndef ___GlRenderer_Buffer_HPP___
#define ___GlRenderer_Buffer_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Buffer/Buffer.hpp>

namespace gl_renderer
{
	class Buffer
		: public renderer::BufferBase
	{
	public:
		Buffer( renderer::Device const & device
			, uint32_t size
			, renderer::BufferTargets target );
		~Buffer();
		/**
		*\copydoc	renderer::BufferBase::getMemoryRequirements
		*/
		renderer::MemoryRequirements getMemoryRequirements()const override;
		/**
		*\copydoc	renderer::BufferBase::makeTransferDestination
		*/
		renderer::BufferMemoryBarrier makeTransferDestination()const override;
		/**
		*\copydoc	renderer::BufferBase::makeTransferSource
		*/
		renderer::BufferMemoryBarrier makeTransferSource()const override;
		/**
		*\copydoc	renderer::BufferBase::makeVertexShaderInputResource
		*/
		renderer::BufferMemoryBarrier makeVertexShaderInputResource()const override;
		/**
		*\copydoc	renderer::BufferBase::makeUniformBufferInput
		*/
		renderer::BufferMemoryBarrier makeUniformBufferInput()const override;
		/**
		*\copydoc	renderer::BufferBase::makeMemoryTransitionBarrier
		*/
		renderer::BufferMemoryBarrier makeMemoryTransitionBarrier( renderer::AccessFlags dstAccess )const override;
		/**
		*\return
		*	Le tampon.
		*/
		inline GLuint getBuffer()const
		{
			assert( m_name != GL_INVALID_INDEX );
			return m_name;
		}
		/**
		*\return
		*	La cible du tampon.
		*/
		inline GlBufferTarget getTarget()const
		{
			return m_target;
		}

	private:
		void doBindMemory()override;

	public:
		mutable BufferDestroySignal onDestroy;

	private:
		GLuint m_name{ GL_INVALID_INDEX };
		GlBufferTarget m_target;
		mutable GlBufferTarget m_copyTarget;
	};
}

#endif
