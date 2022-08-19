#include "DrawEdgesPostEffect/ObjectIDEdgeDetection.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

namespace draw_edges
{
	namespace oied
	{
		enum Idx : uint32_t
		{
			eMaterials,
			eModels,
			eDepthObj,
		};

		static std::unique_ptr< ast::Shader > getVertexShader( VkExtent3D const & size )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
					vtx_texture = uv;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPixelShader( VkExtent3D const & extent
			, int contourMethod )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			castor3d::shader::Materials materials{ writer, eMaterials, 0u };
			C3D_ModelsData( writer, eModels, 0u );
			auto c3d_depthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObj", eDepthObj, 0u );

			// Shader outputs
			auto fragColour = writer.declOutput< Float >( "pxl_fragColour", 0u );

			auto computeContour = writer.implementFunction< sdw::Float >( "c3d_computeContour"
				, [&]( sdw::IVec2 const & texelCoord
					, sdw::Int const & X
					, sdw::Float const & edgeWidth )
				{
					auto w = 1_i;
					auto h = 1_i;

					auto A = writer.declLocale( "A", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   -w,   +h ), 0_i ).z() ) );  //  +---+---+---+
					auto B = writer.declLocale( "B", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2( +0_i,   +h ), 0_i ).z() ) );  //  | A | B | C |
					auto C = writer.declLocale( "C", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   +w,   +h ), 0_i ).z() ) );  //  +---+---+---+
					auto D = writer.declLocale( "D", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   -w, +0_i ), 0_i ).z() ) );  //  | D | X | E |
					auto E = writer.declLocale( "E", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   +w, +0_i ), 0_i ).z() ) );  //  +---+---+---+
					auto F = writer.declLocale( "F", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   -w,   -h ), 0_i ).z() ) );  //  | F | G | H |
					auto G = writer.declLocale( "G", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2( +0_i,   -h ), 0_i ).z() ) );  //  +---+---+---+
					auto H = writer.declLocale( "H", writer.cast< sdw::Int >( c3d_depthObj.fetch( texelCoord + ivec2(   +w,   -h ), 0_i ).z() ) );

					switch ( contourMethod )
					{
					case 0:  // smaller
						IF( writer, X < A || X < B || X < C || X < D || X < E || X < F || X < G || X < H )
						{
							writer.returnStmt( 1.0_f );
						}
						FI;
						break;
					case 1:  // bigger
						IF( writer, X > A || X > B || X > C || X > D || X > E || X > F || X > G || X > H )
						{
							writer.returnStmt( 1.0_f );
						}
						FI;
						break;
					case 2:  // thicker
						IF( writer, X != A || X != B || X != C || X != D || X != E || X != F || X != G || X != H )
						{
							writer.returnStmt( 1.0_f );
						}
						FI;
						break;
					case 3:  // different
						writer.returnStmt( writer.cast< sdw::Float >( writer.cast< sdw::Int >( X != A )
								+ writer.cast< sdw::Int >( X != C )
								+ writer.cast< sdw::Int >( X != F )
								+ writer.cast< sdw::Int >( X != H ) ) * sdw::Float{ 1.0f / 6.0f }
							+ writer.cast< sdw::Float >( writer.cast< sdw::Int >( X != B )
								+ writer.cast< sdw::Int >( X != D )
								+ writer.cast< sdw::Int >( X != E )
								+ writer.cast< sdw::Int >( X != G ) ) * sdw::Float{ 1.0f / 3.0f } );

						break;
					}

					writer.returnStmt( 0.0_f );
				}
				, sdw::InIVec2{ writer, "texelCoord" }
				, sdw::InInt{ writer, "X" }
				, sdw::InFloat{ writer, "edgeWidth" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					auto size = writer.declLocale( "size"
						, ivec2( sdw::Int{ int( extent.width ) }, sdw::Int{ int( extent.height ) } ) );
					auto texelCoord = writer.declLocale( "texelCoord"
						, ivec2( vec2( size ) * vtx_texture ) );
					auto X = writer.declLocale( "X"
						, c3d_depthObj.fetch( texelCoord, 0_i ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( X.z() ) );

					IF( writer, nodeId == 0_u )
					{
						writer.demote();
					}
					FI;

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );

					IF( writer, material.edgeColour().w() == 0.0_f )
					{
						writer.demote();
					}
					FI;

					fragColour = material.objectFactor() * computeContour( texelCoord
						, writer.cast< sdw::Int >( X.z() )
						, material.edgeWidth() );

				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	ObjectIDEdgeDetection::ObjectIDEdgeDetection( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderTarget & renderTarget
		, castor3d::RenderDevice const & device
		, castor3d::PassBuffer const & passBuffer
		, crg::ImageViewId const & depthObj
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ graph }
		, m_extent{ castor3d::getSafeBandedExtent3D( renderTarget.getSize() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "DEObjDetection", oied::getVertexShader( m_extent ) }
	, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "DEObjDetection", oied::getPixelShader( m_extent, 1 ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_result{ m_device
			, m_graph.getHandler()
			, "DEObjDet"
			, 0u
			, m_extent
			, 1u
			, 1u
			, VK_FORMAT_R16_SFLOAT
			, ( VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT ) }
		, m_pass{ m_graph.createPass( "ObjectIDDetection"
			, [this, &device, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} ) }
	{
		auto & modelBuffer = renderTarget.getScene()->getModelBuffer().getBuffer();
		m_pass.addDependency( previousPass );
		passBuffer.createPassBinding( m_pass, oied::eMaterials );
		m_pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( oied::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		m_pass.addSampledView( depthObj, oied::eDepthObj );
		m_pass.addOutputColourView( m_result.targetViewId
			, castor3d::transparentBlackClearColor );
	}

	void ObjectIDEdgeDetection::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( "Object ID Edge Detection"
			, m_result
			, m_graph.getFinalLayoutState( m_result.sampledViewId ).layout
			, castor3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}
