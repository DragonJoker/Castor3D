#include "GlVertexBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "GlAttribute.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlVertexBufferObject::GlVertexBufferObject( GlRenderSystem & p_renderSystem, OpenGl & p_gl, BufferDeclaration const & p_declaration, HardwareBufferPtr p_buffer )
		: GlBuffer< uint8_t >( p_renderSystem, p_gl, eGL_BUFFER_TARGET_ARRAY, p_buffer )
		, m_bufferDeclaration( p_declaration )
	{
		//for ( auto l_element : m_bufferDeclaration )
		//{
		//	GlAttributeBaseSPtr l_attribute;

		//	switch ( l_element.m_dataType )
		//	{
		//	case eELEMENT_TYPE_1FLOAT:
		//		l_attribute = std::make_shared< GlAttribute1r >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_2FLOATS:
		//		l_attribute = std::make_shared< GlAttribute2r >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_3FLOATS:
		//		l_attribute = std::make_shared< GlAttribute3r >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_4FLOATS:
		//		l_attribute = std::make_shared< GlAttribute4r >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_COLOUR:
		//		l_attribute = std::make_shared< GlAttribute1ui >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_1INT:
		//		l_attribute = std::make_shared< GlAttribute1i >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_2INTS:
		//		l_attribute = std::make_shared< GlAttribute2i >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_3INTS:
		//		l_attribute = std::make_shared< GlAttribute3i >( p_gl, &p_renderSystem, l_name );
		//		break;

		//	case eELEMENT_TYPE_4INTS:
		//		l_attribute = std::make_shared< GlAttribute4i >( p_gl, &p_renderSystem, l_name );
		//		break;
		//	}

		//	l_attribute->SetOffset( l_element.m_offset );
		//	l_attribute->SetStride( m_bufferDeclaration.GetStride() );
		//	m_arrayAttributes.push_back( l_attribute );
		//}
	}

	GlVertexBufferObject::~GlVertexBufferObject()
	{
	}

	bool GlVertexBufferObject::Create()
	{
		return GlBuffer< uint8_t >::DoCreate();
	}

	void GlVertexBufferObject::Destroy()
	{
		GlBuffer< uint8_t >::DoDestroy();
	}

	bool GlVertexBufferObject::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
	{
		return GlBuffer< uint8_t >::DoInitialise( p_type, p_nature );
	}

	bool GlVertexBufferObject::AttachTo( ShaderProgramBaseSPtr p_program )
	{
		bool l_return = true;
		GlShaderProgramSPtr l_pNewProgram = std::static_pointer_cast< GlShaderProgram >( p_program );
		GlShaderProgramSPtr l_pOldProgram = m_program.lock();

		if ( l_pOldProgram != l_pNewProgram )
		{
			for ( auto l_attribute : m_arrayAttributes )
			{
				l_attribute->SetShader( l_pNewProgram );
			}

			m_program = l_pNewProgram;

			if ( l_pNewProgram )
			{
				l_return = DoAttributesInitialise();
			}
		}

		return l_return;
	}

	void GlVertexBufferObject::Cleanup()
	{
		DoAttributesCleanup();
		GlBuffer< uint8_t >::DoCleanup();
	}

	bool GlVertexBufferObject::Bind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();
		bool l_return = l_pBuffer && l_pBuffer->IsAssigned();

		if ( l_return )
		{
			l_return = DoBind();
		}

		if ( l_return )
		{
			REQUIRE( !m_program.expired() );
			l_return = DoAttributesBind();
		}

		return l_return;
	}

	void GlVertexBufferObject::Unbind()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			REQUIRE( !m_program.expired() );
			DoAttributesUnbind();
		}
	}

	uint8_t * GlVertexBufferObject::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		uint8_t * l_return = NULL;
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			l_return = GlBuffer< uint8_t >::DoLock( p_offset, p_count, p_flags );
		}

		return l_return;
	}

	void GlVertexBufferObject::Unlock()
	{
		HardwareBufferPtr l_pBuffer = GetCpuBuffer();

		if ( l_pBuffer && l_pBuffer->IsAssigned() )
		{
			GlBuffer< uint8_t >::DoUnlock();
		}
	}

	void GlVertexBufferObject::DoAttributesCleanup()
	{
		for ( auto && l_attribute : m_arrayAttributes )
		{
			l_attribute->Cleanup();
		}
	}

	bool GlVertexBufferObject::DoAttributesInitialise()
	{
		m_valid = 0;

		for ( auto && l_attribute : m_arrayAttributes )
		{
			m_valid += ( l_attribute->Initialise() ? 1 : 0 );
		}

		return m_valid > 0;
	}

	bool GlVertexBufferObject::DoAttributesBind()
	{
		bool l_return = true;

		for ( auto && l_it = m_arrayAttributes.begin(); l_it != m_arrayAttributes.end() && l_return; ++l_it )
		{
			if ( ( *l_it )->GetLocation() != eGL_INVALID_INDEX )
			{
				l_return = ( *l_it )->Bind( false );
			}
		}

		return l_return;
	}

	void GlVertexBufferObject::DoAttributesUnbind()
	{
		for ( auto && l_attribute : m_arrayAttributes )
		{
			if ( l_attribute->GetLocation() != eGL_INVALID_INDEX )
			{
				l_attribute->Unbind();
			}
		}
	}
}
