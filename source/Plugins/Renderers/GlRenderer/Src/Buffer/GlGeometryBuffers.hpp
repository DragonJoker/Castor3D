/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#ifndef ___GlRenderer_GeometryBuffers_HPP___
#define ___GlRenderer_GeometryBuffers_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Buffer/GeometryBuffers.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe encapsulant les VBOs et l'IBO d'un mesh.
	*/
	class GeometryBuffers
		: public renderer::GeometryBuffers
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] offset
		*	L'offset du premier sommet.
		*\param[in] layout
		*	Le layout.
		*/
		GeometryBuffers( renderer::VertexBufferBase const & vbo
			, uint64_t offset
			, renderer::VertexLayout const & layout );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] offset
		*	L'offset du premier sommet dans le VBO.
		*\param[in] layout
		*	Le layout.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] offset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		GeometryBuffers( renderer::VertexBufferBase const & vbo
			, uint64_t vboOffset
			, renderer::VertexLayout const & layout
			, renderer::BufferBase const & ibo
			, uint64_t iboOffset
			, renderer::IndexType type );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] offsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*/
		GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > offsets
			, renderer::VertexLayoutCRefArray const & layouts );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] offsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] offset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
			, std::vector< uint64_t > offsets
			, renderer::VertexLayoutCRefArray const & layouts
			, renderer::BufferBase const & ibo
			, uint64_t offset
			, renderer::IndexType type );
		/**
		*\brief
		*	Destructeur.
		*/
		~GeometryBuffers()noexcept;
		/**
		*\return
		*	Le VAO OpenGL
		*/
		inline GLuint getVao()const
		{
			return m_vao;
		}

	private:
		void doInitialise();

	private:
		GLuint m_vao;
	};
}

#endif
