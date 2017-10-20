/*
See LICENSE file in root folder
*/
#ifndef ___GL_ATTRIBUTE_BASE_H___
#define ___GL_ATTRIBUTE_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Mesh/Buffer/Buffer.hpp>

namespace GlRender
{
	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlAttributeBase
		: public Holder
	{
	public:
		GlAttributeBase( OpenGl & gl
			, castor3d::ShaderProgram const & program
			, uint32_t stride
			, castor::String const & attributeName
			, GlType glType
			, uint32_t count
			, uint32_t divisor );
		virtual ~GlAttributeBase();

		virtual void bind( bool normalised = false );
		virtual void unbind();

		inline void setOffset( uint32_t offset )
		{
			m_offset = offset;
		}

		inline uint32_t getLocation()const
		{
			return m_attributeLocation;
		}

	protected:
		castor3d::ShaderProgram const & m_program;
		uint32_t m_stride;
		castor::String m_attributeName;
		uint32_t m_attributeLocation;
		uint32_t m_count;
		uint32_t m_divisor;
		uint32_t m_offset;
		GlType m_glType;
	};
}

#endif
