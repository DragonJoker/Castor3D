/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_ATTRIBUTE_BASE_H___
#define ___C3DGLES3_ATTRIBUTE_BASE_H___

#include "GlES3RenderSystemPrerequisites.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

#include <Mesh/Buffer/Buffer.hpp>

namespace GlES3Render
{
	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlES3AttributeBase
		: public Holder
	{
	public:
		GlES3AttributeBase( OpenGlES3 & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, GlES3Type p_glType, uint32_t p_count, uint32_t p_divisor );
		virtual ~GlES3AttributeBase();

		virtual void Bind( bool p_bNormalised = false );
		virtual void Unbind();

		inline void SetOffset( uint32_t p_offset )
		{
			m_offset = p_offset;
		}

		inline uint32_t GetLocation()const
		{
			return m_attributeLocation;
		}

	protected:
		Castor3D::ShaderProgram const & m_program;
		uint32_t m_stride;
		Castor::String m_attributeName;
		uint32_t m_attributeLocation;
		uint32_t m_count;
		uint32_t m_divisor;
		uint32_t m_offset;
		GlES3Type m_glType;
	};
}

#endif
