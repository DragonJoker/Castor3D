#include "GlGeometryBuffers.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlVertexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	//************************************************************************************************

	GlGeometryBuffersImpl::GlGeometryBuffersImpl( GlGeometryBuffers & p_buffers )
		: OwnedBy< GlGeometryBuffers >( p_buffers )
	{
	}

	GlGeometryBuffersImpl::~GlGeometryBuffersImpl()
	{
	}

	//************************************************************************************************

	class GlGeometryBuffersStd
		: public GlGeometryBuffersImpl
		, public Holder
	{
	public:
		GlGeometryBuffersStd( GlGeometryBuffers & p_buffers, OpenGl & p_gl )
			: GlGeometryBuffersImpl( p_buffers )
			, Holder( p_gl )
		{
		}

		~GlGeometryBuffersStd()
		{
		}

		virtual bool Bind()
		{
			return static_cast< GeometryBuffers * >( GetOwner() )->Bind();
		}

		virtual void Unbind()
		{
			static_cast< GeometryBuffers * >( GetOwner() )->Unbind();
		}
	};

	//************************************************************************************************

	class GlGeometryBuffersVao
		: public GlGeometryBuffersImpl
		, public Bindable<
			std::function< bool( int, uint32_t * ) >,
			std::function< bool( int, uint32_t const * ) >,
			std::function< bool( uint32_t ) >
			>
	{
		using ObjectType = Bindable<
			std::function< bool( int, uint32_t * ) >,
			std::function< bool( int, uint32_t const * ) >,
			std::function< bool( uint32_t ) >
			>;

	public:
		GlGeometryBuffersVao( GlGeometryBuffers & p_buffers, OpenGl & p_gl )
			: GlGeometryBuffersImpl( p_buffers )
			, ObjectType( p_gl,
						  "GlVertexArrayObjects",
						  std::bind( &OpenGl::GenVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						  std::bind( &OpenGl::DeleteVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						  std::bind( &OpenGl::IsVertexArray, std::ref( p_gl ), std::placeholders::_1 ),
						  std::bind( &OpenGl::BindVertexArray, std::ref( p_gl ), std::placeholders::_1 )
						  )
		{
			bool l_return = ObjectType::Create();

			if ( l_return )
			{
				l_return = GetOpenGl().BindVertexArray( GetGlName() );
			}

			if ( l_return )
			{
				p_buffers.GetVertexBuffer().Bind();

				if ( p_buffers.HasIndexBuffer() )
				{
					p_buffers.GetIndexBuffer().Bind();
				}

				if ( p_buffers.HasMatrixBuffer() )
				{
					p_buffers.GetMatrixBuffer().Bind( 2 );
				}

				GetOpenGl().BindVertexArray( 0 );
			}
		}

		~GlGeometryBuffersVao()
		{
			ObjectType::Destroy();
		}

		virtual bool Bind()
		{
			return ObjectType::Bind();
		}

		virtual void Unbind()
		{
			ObjectType::Unbind();
		}
	};

	//************************************************************************************************

	GlGeometryBuffers::GlGeometryBuffers( OpenGl & p_gl, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer, eTOPOLOGY p_topology )
		: GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ), p_topology )
		, Holder( p_gl )
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::Draw( ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index )
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( p_program && p_program->HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( Bind() )
		{
			if ( m_pIndexBuffer )
			{
				GetOpenGl().DrawElements( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ) );
			}
			else
			{
				GetOpenGl().DrawArrays( l_eMode, int( p_index ), int( p_uiSize ) );
			}

			Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::DrawInstanced( ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count )
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( p_program->HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( m_pMatrixBuffer )
		{
			m_pMatrixBuffer->GetGpuBuffer()->Fill( m_pMatrixBuffer->data(), m_pMatrixBuffer->GetSize(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( Bind() )
		{
			if ( m_pIndexBuffer )
			{
				GetOpenGl().DrawElementsInstanced( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ), int( p_count ) );
			}
			else
			{
				GetOpenGl().DrawArraysInstanced( l_eMode, int( p_index ), int( p_uiSize ), int( p_count ) );
			}

			Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::Bind()
	{
		REQUIRE( m_impl );
		return m_impl->Bind();
	}

	void GlGeometryBuffers::Unbind()
	{
		REQUIRE( m_impl );
		m_impl->Unbind();
	}

	bool GlGeometryBuffers::DoCreate()
	{
		return true;
	}

	void GlGeometryBuffers::DoDestroy()
	{
	}

	bool GlGeometryBuffers::DoInitialise()
	{
		bool l_return = m_program.expired();

		if ( !l_return )
		{
			if ( GetOpenGl().HasVao() )
			{
				m_impl = std::make_unique< GlGeometryBuffersVao >( *this, GetOpenGl() );
			}
			else
			{
				m_impl = std::make_unique< GlGeometryBuffersStd >( *this, GetOpenGl() );
			}

			l_return = true;
		}

		return l_return;
	}

	void GlGeometryBuffers::DoCleanup()
	{
		m_impl.reset();
	}
}
