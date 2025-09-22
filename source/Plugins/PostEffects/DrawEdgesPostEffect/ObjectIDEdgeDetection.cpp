#include "DrawEdgesPostEffect/ObjectIDEdgeDetection.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/ShaderBuffers/PassBuffer.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslPassShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <Shaders/GlslToonProfile.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <numeric>

namespace draw_edges
{
	namespace oied
	{
		namespace c3ds = c3d::shader;

		enum Idx : uint32_t
		{
			eMaterials,
			eModels,
			eDepthObj,
			eSpecifics,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D const & extent
			, int contourMethod )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			c3d::shader::Utils utils{ writer };
			c3d::shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, c3d::TextureCombine{}
				, c3d::ComponentModeFlag::eNone
				, utils };

			auto specifics = uint32_t( eSpecifics );
			c3d::shader::Materials materials{ engine, writer, passShaders, eMaterials, 0u, specifics };
			C3D_ModelsData( writer, eModels, 0u );
			auto c3d_depthObj = writer.declCombinedImg< FImg2DRgba32 >( "c3d_depthObj", eDepthObj, 0u );

			auto outColour = writer.declOutput< sdw::Float >( "outColour", sdw::EntryPoint::eFragment, 0u );

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
						sdwIF( writer, X < A || X < B || X < C || X < D || X < E || X < F || X < G || X < H )
						{
							writer.returnStmt( 1.0_f );
						}
						sdwFI
						break;
					case 1:  // bigger
						sdwIF( writer, X > A || X > B || X > C || X > D || X > E || X > F || X > G || X > H )
						{
							writer.returnStmt( 1.0_f );
						}
						sdwFI
						break;
					case 2:  // thicker
						sdwIF( writer, X != A || X != B || X != C || X != D || X != E || X != F || X != G || X != H )
						{
							writer.returnStmt( 1.0_f );
						}
						sdwFI
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

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::PosUv2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					out.uv() = in.uv();
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, sdw::VoidT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOut out )
				{
					auto size = writer.declLocale( "size"
						, ivec2( sdw::Int{ int( extent.width ) }, sdw::Int{ int( extent.height ) } ) );
					auto texelCoord = writer.declLocale( "texelCoord"
						, ivec2( vec2( size ) * in.uv() ) );
					auto X = writer.declLocale( "X"
						, c3d_depthObj.fetch( texelCoord, 0_i ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( X.z() ) );

					sdwIF( writer, nodeId == 0_u )
					{
						writer.demote();
					}
					sdwFI

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
					auto & toonProfiles = materials.getSpecificsBuffer< toon::shader::ToonProfile >();
					auto toonProfile = writer.declLocale( "toonProfile"
						, toonProfiles.getData( modelData.getMaterialId() - 1u ) );

					sdwIF( writer, toonProfile.edgeColour().w() == 0.0_f )
					{
						writer.demote();
					}
					sdwFI

					outColour = toonProfile.objectFactor() * computeContour( texelCoord
						, writer.cast< sdw::Int >( X.z() )
						, toonProfile.edgeWidth() );

				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	ObjectIDEdgeDetection::ObjectIDEdgeDetection( crg::FramePassGroup & graph
		, c3d::RenderTarget & renderTarget
		, c3d::RenderDevice const & device
		, c3d::PassBuffer const & passBuffer
		, c3d::Texture const & depthObj
		, bool const * enabled )
		: m_device{ device }
		, m_graph{ graph }
		, m_extent{ c3d::getSafeBandedExtent3D( renderTarget.getDisplaySize() ) }
		, m_shader{ cuT( "DEObjDetection" ), oied::getProgram( *renderTarget.getEngine(), m_extent, 1 ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_result{ m_device
			, renderTarget.getResources()
			, cuT( "DEObjDet" )
			, { c3d::ImageCreateFlags::eNone
				, m_extent, 1u, 1u
				, c3d::PixelFormat::eR16_SFLOAT
				, ( c3d::ImageUsageFlags::eSampled
					| c3d::ImageUsageFlags::eColorAttachment
					| c3d::ImageUsageFlags::eTransferSrc
					| c3d::ImageUsageFlags::eTransferDst ) }
			, {} }
	{
		auto & pass = m_graph.createPass( "ObjectIDDetection"
			, [this, &device, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( c3d::makeExtent2D( m_extent ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		auto & modelBuffer = renderTarget.getScene()->getModelBuffer();
		passBuffer.createPassBinding( pass, oied::eMaterials );
		pass.addInputStorage( *modelBuffer.getLastAttach(), oied::eModels );
		pass.addInputSampled( *depthObj.getSampledLastAttach(), oied::eDepthObj );
		auto index = uint32_t( oied::eSpecifics );
		device.renderSystem.getEngine()->createSpecificsBuffersPassBindings( pass, index );
		m_result.setLastAttach( pass.addOutputColourTarget( m_result.getTargetViewId(), c3d::transparentBlackClearColor ) );
		m_result.create();
	}

	ObjectIDEdgeDetection::~ObjectIDEdgeDetection()
	{
		m_result.destroy();
	}

	void ObjectIDEdgeDetection::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( cuT( "Object ID Edge Detection" )
			, m_result
			, m_graph.getFinalLayoutState( m_result.getSampledViewId() ).layout
			, c3d::TextureFactors{}.invert( true ) );
	}

	//*********************************************************************************************
}
