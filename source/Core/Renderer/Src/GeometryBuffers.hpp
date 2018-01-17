/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_GeometryBuffers_HPP___
#define ___Renderer_GeometryBuffers_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

#include <vector>

namespace renderer
{
	/**
	*\brief
	*	Classe encapsulant les VBOs et l'IBO d'un mesh.
	*/
	class GeometryBuffers
	{
	public:
		struct VBO
		{
			VertexBufferBase const & vbo;
			uint64_t offset;
			VertexLayout const & layout;
		};

		struct IBO
		{
			IBO( BufferBase const & buffer
				, uint64_t offset
				, IndexType type )
				: buffer{ buffer }
				, offset{ offset }
				, type{ type }
			{
			}

			BufferBase const & buffer;
			uint64_t offset;
			IndexType type;
		};

	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] vboOffset
		*	L'offset du premier sommet.
		*\param[in] layout
		*	Le layout.
		*/
		GeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbo
		*	Le VBO.
		*\param[in] vboOffset
		*	L'offset du premier sommet dans le VBO.
		*\param[in] layout
		*	Le layout.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		GeometryBuffers( VertexBufferBase const & vbo
			, uint64_t vboOffset
			, VertexLayout const & layout
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*/
		GeometryBuffers( VertexBufferCRefArray const & vbos
			, UInt64Array vboOffsets
			, VertexLayoutCRefArray const & layouts );
		/**
		*\brief
		*	Constructeur.
		*\param[in] vbos
		*	Les VBOs.
		*\param[in] vboOffsets
		*	L'offset du premier sommet pour chaque VBO.
		*\param[in] layouts
		*	Les layouts, un par vbo de \p vbos.
		*\param[in] ibo
		*	L'IBO.
		*\param[in] iboOffset
		*	L'offset du premier sommet dans l'IBO.
		*\param[in] type
		*	Le type des indices.
		*/
		GeometryBuffers( VertexBufferCRefArray const & vbos
			, UInt64Array vboOffsets
			, VertexLayoutCRefArray const & layouts
			, BufferBase const & ibo
			, uint64_t iboOffset
			, IndexType type );
		/**
		*\brief
		*	Destructeur.
		*/
		virtual ~GeometryBuffers() = default;
		/**
		*\return
		*	Les VBOs.
		*/
		inline std::vector< VBO > const & getVbos()const
		{
			return m_vbos;
		}
		/**
		*\return
		*	\p true si l'IBO est d�fini.
		*/
		inline bool hasIbo()const
		{
			return m_ibo != nullptr;
		}
		/**
		*\return
		*	L'IBO.
		*/
		inline IBO const & getIbo()const
		{
		    assert( m_ibo != nullptr );
			return *m_ibo;
		}

	protected:
		std::vector< VBO > m_vbos;
		std::unique_ptr< IBO > m_ibo;
	};
}

#endif
