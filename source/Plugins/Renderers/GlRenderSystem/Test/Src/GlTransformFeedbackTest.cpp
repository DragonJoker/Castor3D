#include "GlTransformFeedbackTest.hpp"

#include <Engine.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Miscellaneous/TransformFeedback.hpp>
#include <Render/Pipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/RasteriserState.hpp>

#include <Buffer/GlBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

#define Glsl( x ) cuT( "#version 150 core\n" ) cuT( #x )

namespace Testing
{
	namespace
	{
		ShaderProgramSPtr DoCreateNoopProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				void main()
				{
					gl_Position = vec4( 0, 0, 0, 1 );
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateBasicComputeProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;
				out float outValue;

				void main()
				{
					outValue = sqrt( inValue );
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			return l_program;
		}

		ShaderProgramSPtr DoCreateGeometryShaderProgram( Engine & p_engine )
		{
			String l_vtx = Glsl(
				in float inValue;
				out float geoValue;

				void main()
				{
					geoValue = sqrt( inValue );
				}
			);
			String l_geo = Glsl(
				layout( points ) in;
				layout( triangle_strip, max_vertices = 3 ) out;

				in float[] geoValue;
				out float outValue;

				void main()
				{
					for ( int i = 0; i < 3; i++ )
					{
						outValue = geoValue[0] + i;
						EmitVertex();
					}

					EndPrimitive();
				}
			);
			auto l_program = p_engine.GetRenderSystem()->CreateShaderProgram();
			auto l_model = p_engine.GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			l_program->CreateObject( ShaderType::Geometry );
			l_program->SetSource( ShaderType::Vertex, l_model, l_vtx );
			l_program->SetSource( ShaderType::Geometry, l_model, l_geo );
			return l_program;
		}
	}

	GlTransformFeedbackTest::GlTransformFeedbackTest( Engine & p_engine )
		: GlTestCase{ "GlTransformFeedbackTest", p_engine }
	{
	}

	GlTransformFeedbackTest::~GlTransformFeedbackTest()
	{
	}

	void GlTransformFeedbackTest::DoRegisterTests()
	{
		DoRegisterTest( "GlTransformFeedbackTest::Creation", std::bind( &GlTransformFeedbackTest::Creation, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::BasicCompute", std::bind( &GlTransformFeedbackTest::BasicCompute, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::GeometryShader", std::bind( &GlTransformFeedbackTest::GeometryShader, this ) );
		DoRegisterTest( "GlTransformFeedbackTest::VariableFeedback", std::bind( &GlTransformFeedbackTest::VariableFeedback, this ) );
	}

	void GlTransformFeedbackTest::Creation()
	{
		auto l_program = DoCreateNoopProgram( m_engine );
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( {}, Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_transformFeedback->Initialise( {} ) );
		l_transformFeedback->Cleanup();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::BasicCompute()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateBasicComputeProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Points, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		CT_CHECK( l_transformFeedback->Bind() );
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		l_vboOut.Bind();
		auto l_buffer = reinterpret_cast< float * >( l_vboOut.Lock( 0, sizeof( l_data ), AccessType::Read ) );

		if ( l_buffer )
		{
			for ( int i = 0; i < 5; ++i )
			{
				CT_EQUAL( l_buffer[i], sqrt( l_data[i] ) );
			}

			l_vboOut.Unlock();
		}
		l_vboOut.Unbind();

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Destroy();
		l_vboIn.Destroy();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::GeometryShader()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Triangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		CT_CHECK( l_transformFeedback->Bind() );
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		l_vboOut.Bind();
		auto l_buffer = reinterpret_cast< float * >( l_vboOut.Lock( 0, sizeof( l_data ) * 3, AccessType::Read ) );

		if ( l_buffer )
		{
			for ( int j = 0; j < 5; ++j )
			{
				auto l_src = sqrt( l_data[j] );

				for ( int i = 0; i < 3; ++i )
				{
					CT_EQUAL( l_buffer[j * 3 + i], l_src + i );
				}
			}

			l_vboOut.Unlock();
		}

		l_vboOut.Unbind();

		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Destroy();
		l_vboIn.Destroy();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}

	void GlTransformFeedbackTest::VariableFeedback()
	{
		BufferDeclaration l_outputs
		{
			{
				BufferElementDeclaration( cuT( "outValue" ), 0, ElementType::Float, 0u )
			}
		};
		BufferDeclaration l_inputs
		{
			{
				BufferElementDeclaration( cuT( "inValue" ), 0, ElementType::Float, 0u )
			}
		};
		float l_data[] = { 1, 2, 3, 4, 5 };

		// Shader program
		auto l_program = DoCreateGeometryShaderProgram( m_engine );

		// Transform feedback
		auto l_transformFeedback = m_engine.GetRenderSystem()->CreateTransformFeedback( l_outputs, Topology::Triangles, *l_program );
		l_program->SetTransformLayout( l_outputs );

		// Pipeline
		RasteriserState l_rs;
		l_rs.SetDiscardPrimitives( true );
		auto l_pipeline = m_engine.GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *l_program, PipelineFlags{} );

		// Input VBO
		VertexBuffer l_vboIn{ m_engine, l_inputs };
		l_vboIn.Resize( sizeof( l_data ) );
		std::memcpy( l_vboIn.data(), l_data, sizeof( l_data ) );

		// Output VBO
		VertexBuffer l_vboOut{ m_engine, l_outputs };
		l_vboOut.Resize( sizeof( l_data ) * 3 );

		// VAO
		auto l_geometryBuffers = m_engine.GetRenderSystem()->CreateGeometryBuffers( Topology::Points, *l_program );

		m_engine.GetRenderSystem()->GetMainContext()->SetCurrent();
		CT_CHECK( l_program->Initialise() );
		CT_CHECK( l_vboIn.Create() );
		CT_CHECK( l_vboOut.Create() );
		CT_CHECK( l_vboIn.Upload( BufferAccessType::Static, BufferAccessNature::Draw ) );
		CT_CHECK( l_vboOut.Upload( BufferAccessType::Static, BufferAccessNature::Read ) );
		CT_CHECK( l_transformFeedback->Initialise( { l_vboOut } ) );
		CT_CHECK( l_geometryBuffers->Initialise( { l_vboIn }, nullptr ) );

		l_pipeline->Apply();
		CT_CHECK( l_transformFeedback->Bind() );
		CT_CHECK( l_geometryBuffers->Draw( 5, 0 ) );
		l_transformFeedback->Unbind();
		m_engine.GetRenderSystem()->GetMainContext()->SwapBuffers();

		CT_EQUAL( l_transformFeedback->GetWrittenPrimitives(), 5 );


		l_geometryBuffers->Cleanup();
		l_transformFeedback->Cleanup();
		l_vboOut.Destroy();
		l_vboIn.Destroy();
		l_program->Cleanup();
		m_engine.GetRenderSystem()->GetMainContext()->EndCurrent();
	}
}
