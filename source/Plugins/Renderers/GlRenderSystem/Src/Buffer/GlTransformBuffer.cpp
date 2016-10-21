#include "GlTransformBuffer.hpp"

#include "Render/GlRenderSystem.hpp"
#include "Mesh/GlAttribute.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlTransformBuffer::GlTransformBuffer( OpenGl & p_gl, GlRenderSystem & p_renderSystem, ShaderProgram & p_program, TransformBufferDeclaration const & p_declaration )
		: Castor3D::GpuTransformBuffer( p_renderSystem, p_program, p_declaration )
		, BindableType{ p_gl,
						"GlTransformBuffer",
						std::bind( &OpenGl::GenTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsTransformFeedback, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindTransformFeedback( eGL_BUFFER_TARGET_TRANSFORM_FEEDBACK, p_glName );
						} }
		, m_glBuffer{ p_gl, eGL_BUFFER_TARGET_ARRAY }
	{
	}

	GlTransformBuffer::~GlTransformBuffer()
	{
	}

	bool GlTransformBuffer::Create()
	{
		bool l_return = BindableType::Create();

		if ( l_return )
		{
			l_return = m_glBuffer.Create();
		}

		return l_return;
	}

	void GlTransformBuffer::Destroy()
	{
		m_glBuffer.Destroy();
		BindableType::Destroy();
	}

	bool GlTransformBuffer::Initialise()
	{
		bool l_return = BindableType::Bind();

		if ( l_return )
		{
			l_return = m_glBuffer.Bind();

			if ( l_return )
			{
				l_return = GetOpenGl().BindBufferBase( eGL_BUFFER_TARGET_TRANSFORM_FEEDBACK_BUFFER, 0, m_glBuffer.GetGlName() );
				m_glBuffer.Unbind();
			}
				
			if ( l_return )
			{
				l_return = DoCreateAttributes();
			}

			BindableType::Unbind();
		}

		return l_return;
	}

	void GlTransformBuffer::Cleanup()
	{
		m_glBuffer.Cleanup();
	}

	void GlTransformBuffer::Update()
	{
	}

	bool GlTransformBuffer::Bind()
	{
		return BindableType::Bind();
	}

	void GlTransformBuffer::Unbind()
	{
		BindableType::Unbind();
	}

	bool GlTransformBuffer::Fill( uint8_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		return m_glBuffer.Fill( p_buffer, p_size, p_type, p_nature );
	}

	uint8_t * GlTransformBuffer::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	void GlTransformBuffer::Unlock()
	{
		m_glBuffer.Unlock();
	}

	GlAttributeBaseSPtr GlTransformBuffer::DoCreateAttribute( TransformBufferElementDeclaration const & p_element )
	{
		bool l_return = true;
		auto const & l_renderSystem = GetOpenGl().GetRenderSystem();
		auto & l_program = static_cast< GlShaderProgram & >( m_program );
		GlAttributeBaseSPtr l_attribute;
		uint32_t l_stride = m_declaration.GetStride();

		switch ( p_element.m_dataType )
		{
		case ElementType::Float:
			l_attribute = std::make_shared< GlAttributeVec1r >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Vec2:
			l_attribute = std::make_shared< GlAttributeVec2r >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Vec3:
			l_attribute = std::make_shared< GlAttributeVec3r >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Vec4:
			l_attribute = std::make_shared< GlAttributeVec4r >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Colour:
			l_attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Int:
			l_attribute = std::make_shared< GlAttributeVec1i >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::IVec2:
			l_attribute = std::make_shared< GlAttributeVec2i >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::IVec3:
			l_attribute = std::make_shared< GlAttributeVec3i >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::IVec4:
			l_attribute = std::make_shared< GlAttributeVec4i >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::UInt:
			l_attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::UIVec2:
			l_attribute = std::make_shared< GlAttributeVec2ui >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::UIVec3:
			l_attribute = std::make_shared< GlAttributeVec3ui >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::UIVec4:
			l_attribute = std::make_shared< GlAttributeVec4ui >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Mat2:
			l_attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Mat3:
			l_attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::Mat4:
			l_attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( l_attribute )
		{
			l_attribute->SetOffset( p_element.m_offset );
		}

		return l_attribute;
	}

	bool GlTransformBuffer::DoCreateAttributes()
	{
		for ( auto & l_element : m_declaration )
		{
			auto l_attribute = DoCreateAttribute( l_element );

			if ( l_attribute )
			{
				m_attributes.push_back( l_attribute );
			}
		}

		return !m_attributes.empty();
	}
}
