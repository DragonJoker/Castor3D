/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#ifndef ___GlRenderer_GeometryBuffers_HPP___
#define ___GlRenderer_GeometryBuffers_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Pipeline/VertexInputAttributeDescription.hpp>
#include <Pipeline/VertexInputBindingDescription.hpp>
#include <Pipeline/VertexInputState.hpp>

namespace gl_renderer
{
	class GeometryBuffers
	{
	public:
		struct VBO
		{
			VBO( GLuint vbo
				, uint64_t offset
				, renderer::VertexInputBindingDescription binding
				, renderer::VertexInputAttributeDescriptionArray attributes )
				: vbo{ vbo }
				, offset{ offset }
				, binding{ binding }
				, attributes{ attributes }
			{
			}

			GLuint vbo;
			uint64_t offset;
			renderer::VertexInputBindingDescription binding;
			renderer::VertexInputAttributeDescriptionArray attributes;
		};

		struct IBO
		{
			IBO( GLuint ibo
				, uint64_t offset
				, renderer::IndexType type )
				: ibo{ ibo }
				, offset{ offset }
				, type{ type }
			{
			}

			GLuint ibo;
			uint64_t offset;
			renderer::IndexType type;
		};

	public:
		GeometryBuffers( VboBindings const & vbos
			, IboBinding const & ibo
			, renderer::VertexInputState const & vertexInputState
			, renderer::IndexType type );
		~GeometryBuffers()noexcept;

		void initialise();

		static std::vector< VBO > createVBOs( VboBindings const & vbos
			, renderer::VertexInputState const & vertexInputState );

		inline GLuint getVao()const
		{
			return m_vao;
		}

		inline std::vector< VBO > const & getVbos()const
		{
			return m_vbos;
		}

		inline bool hasIbo()const
		{
			return m_ibo != nullptr;
		}

		inline IBO const & getIbo()const
		{
			assert( m_ibo != nullptr );
			return *m_ibo;
		}

	private:

	protected:
		std::vector< VBO > m_vbos;
		std::unique_ptr< IBO > m_ibo;
		GLuint m_vao{ GL_INVALID_INDEX };
	};
}

#endif
