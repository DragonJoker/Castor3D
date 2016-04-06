/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_RENDER_SYSTEM_H___
#define ___GL_RENDER_SYSTEM_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "GlPipeline.hpp"
#include "GlContext.hpp"

#include <RenderSystem.hpp>

namespace GlRender
{
	class GlRenderSystem
		: public Castor3D::RenderSystem
	{
	public:
		GlRenderSystem( Castor3D::Engine & p_engine );
		virtual ~GlRenderSystem();
		//!< Initialize OpenGL Extensions
		bool InitOpenGlExtensions();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateContext
		 */
		virtual Castor3D::ContextSPtr CreateContext();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateGeometryBuffers
		 */
		virtual Castor3D::GeometryBuffersSPtr CreateGeometryBuffers( Castor3D::eTOPOLOGY p_topology, Castor3D::ShaderProgram const & p_program, Castor3D::VertexBuffer * p_vtx, Castor3D::IndexBuffer * p_idx, Castor3D::VertexBuffer * p_bones, Castor3D::VertexBuffer * p_inst );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateDepthStencilState
		 */
		virtual Castor3D::DepthStencilStateSPtr CreateDepthStencilState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateRasteriserState
		 */
		virtual Castor3D::RasteriserStateSPtr CreateRasteriserState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBlendState
		 */
		virtual Castor3D::BlendStateSPtr CreateBlendState();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateSampler
		 */
		virtual Castor3D::SamplerSPtr CreateSampler( Castor::String const & p_name );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateShaderProgram
		 */
		virtual Castor3D::ShaderProgramSPtr CreateShaderProgram();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateIndexBuffer
		 */
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint32_t > > CreateIndexBuffer( Castor3D::CpuBuffer<uint32_t> * p_buffer );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateVertexBuffer
		 */
		virtual std::shared_ptr< Castor3D::GpuBuffer< uint8_t > > CreateVertexBuffer( Castor3D::CpuBuffer< uint8_t > * p_buffer );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateStaticTexture
		 */
		virtual Castor3D::StaticTextureSPtr CreateStaticTexture();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateDynamicTexture
		 */
		virtual Castor3D::DynamicTextureSPtr CreateDynamicTexture( uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameVariableBuffer
		 */
		virtual Castor3D::FrameVariableBufferSPtr CreateFrameVariableBuffer( Castor::String const & p_name );
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateFrameBuffer
		 */
		virtual Castor3D::FrameBufferSPtr CreateFrameBuffer();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateBackBuffers
		 */
		virtual Castor3D::BackBuffersSPtr CreateBackBuffers();
		/**
		 *\copydoc		Castor3D::RenderSystem::CreateQuery
		 */
		virtual Castor3D::GpuQuerySPtr CreateQuery( Castor3D::eQUERY_TYPE p_type );
		/**
		 *\copydoc		Castor3D::RenderSystem::GetPipelineImpl
		 */
		virtual Castor3D::IPipelineImplSPtr GetPipelineImpl()
		{
			return m_pipelineImpl;
		}
		/**
		 *\copydoc		Castor3D::RenderSystem::UseVertexBufferObjects
		 */
		inline bool UseVertexBufferObjects()
		{
			return m_useVertexBufferObjects;
		}

		inline bool IsExtensionInit()
		{
			return m_extensionsInit;
		}

		inline int GetOpenGlMajor()
		{
			return m_openGlMajor;
		}

		inline int GetOpenGlMinor()
		{
			return m_openGlMinor;
		}

		inline void SetOpenGlVersion( int p_iMajor, int p_iMinor )
		{
			m_openGlMajor = p_iMajor;
			m_openGlMinor = p_iMinor;
		}

		inline OpenGl & GetOpenGl()
		{
			return m_openGl;
		}

	private:
		virtual void DoInitialise();
		virtual void DoCleanup();

	protected:
		std::shared_ptr< GlPipelineImpl > m_pipelineImpl;
		bool m_useVertexBufferObjects;
		bool m_extensionsInit;
		int m_openGlMajor;
		int m_openGlMinor;
		OpenGl m_openGl;

#if C3D_TRACE_OBJECTS

	public:
		void Track( void * p_object, std::string const & p_name, std::string const & p_file, int line );
		void Untrack( void * p_object );

#else

	public:
		template< typename T >
		void Track( T * p_object, std::string const & p_name, std::string const & p_file, int line )
		{
		}

		template< typename T >
		void Untrack( T * p_object )
		{
		}

#endif
	};
}

#endif
