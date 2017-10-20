/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_BUFFER_H___
#define ___C3DGLES3_BUFFER_H___

#include "Buffer/GlES3BufferBase.hpp"

#include <Mesh/Buffer/GpuBuffer.hpp>

namespace GlES3Render
{
	/*!
	\author		Sylvain DOREMUS
	\version 	0.6.1.0
	\date 		03/01/2011
	\~english
	\brief 		Class implementing OpenGL VBO.
	\~french
	\brief 		Classe implémentant les VBO OpenGL.
	*/
	template< typename T >
	class GlES3Buffer
		: public Castor3D::GpuBuffer< T >
		, public Holder
	{
	public:
		GlES3Buffer( GlES3RenderSystem & p_renderSystem, OpenGlES3 & p_gl, GlES3BufferTarget p_target );
		virtual ~GlES3Buffer();
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Create
		 */
		bool Create()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Destroy
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::InitialiseStorage
		 */
		void InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::SetBindingPoint
		 */
		void SetBindingPoint( uint32_t p_index )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Bind
		 */
		void Bind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Unbind
		 */
		void Unbind()const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Copy
		 */
		void Copy( Castor3D::GpuBuffer< T > const & p_src, uint32_t p_size )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Upload
		 */
		void Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Upload
		 */
		void Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Lock
		 */
		T * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const override;
		/**
		 *\copydoc		Castor3D::GpuBuffer< T >::Unlock
		 */
		void Unlock()const override;

		inline uint32_t GetGlES3Name()const
		{
			return m_glBuffer.GetGlES3Name();
		}

		inline uint32_t GetBindingPoint()const
		{
			return m_glBuffer.GetBindingPoint();
		}

	protected:
		GlES3BufferBase< T > m_glBuffer;
	};
}

#include "GlES3Buffer.inl"

#endif
