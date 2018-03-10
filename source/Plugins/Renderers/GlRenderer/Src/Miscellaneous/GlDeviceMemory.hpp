/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Miscellaneous/DeviceMemory.hpp>
#include <Miscellaneous/MemoryRequirements.hpp>

namespace gl_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant le stockage alloué à un tampon de données.
	*\~english
	*\brief
	*	Class wrapping a storage allocated to a data buffer.
	*/
	class DeviceMemory
		: public renderer::DeviceMemory
	{
	public:
		class DeviceMemoryImpl
		{
		public:
			DeviceMemoryImpl( renderer::MemoryRequirements const & requirements
				, renderer::MemoryPropertyFlags flags
				, GLuint boundResource
				, GLuint boundTarget );
			virtual ~DeviceMemoryImpl() = default;
			virtual uint8_t * lock( uint32_t offset
				, uint32_t size
				, renderer::MemoryMapFlags flags )const = 0;
			virtual void flush( uint32_t offset
				, uint32_t size )const = 0;
			virtual void invalidate( uint32_t offset
				, uint32_t size )const = 0;
			virtual void unlock()const = 0;

		protected:
			renderer::MemoryRequirements m_requirements;
			renderer::MemoryPropertyFlags m_flags;
			GlMemoryMapFlags m_mapFlags;
			GLuint m_boundResource;
			GLenum m_boundTarget;
		};

	public:
		DeviceMemory( Device const & device
			, renderer::MemoryRequirements const & requirements
			, renderer::MemoryPropertyFlags flags );
		~DeviceMemory();
		void bindToBuffer( GLuint resource, GLenum target );
		void bindToImage( Texture const & texture
			, GLenum target
			, renderer::ImageCreateInfo const & createInfo );
		/**
		*\copydoc	renderer::DeviceMemory::lock
		*/
		uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags flags )const override;
		/**
		*\copydoc	renderer::DeviceMemory::flush
		*/
		void flush( uint32_t offset
			, uint32_t size )const override;
		/**
		*\copydoc	renderer::DeviceMemory::invalidate
		*/
		void invalidate( uint32_t offset
			, uint32_t size )const override;
		/**
		*\copydoc	renderer::DeviceMemory::unlock
		*/
		void unlock()const override;

	private:
		void doSetImage1D( uint32_t width
			, renderer::ImageCreateInfo const & createInfo );
		void doSetImage2D( uint32_t width
			, uint32_t height
			, renderer::ImageCreateInfo const & createInfo );
		void doSetImage3D( uint32_t width
			, uint32_t height
			, uint32_t depth
			, renderer::ImageCreateInfo const & createInfo );
		void doSetImage2DMS( uint32_t width
			, uint32_t height
			, renderer::ImageCreateInfo const & createInfo );
		void doSetImage3DMS( uint32_t width
			, uint32_t height
			, uint32_t depth
			, renderer::ImageCreateInfo const & createInfo );
		void updateOneLayer( renderer::BufferImageCopy const & copyInfo )const;

	private:
		Device const & m_device;
		renderer::MemoryRequirements m_requirements;
		std::unique_ptr< DeviceMemoryImpl > m_impl;
	};
}
