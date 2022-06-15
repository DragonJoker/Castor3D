#include "AtmosphereScattering/AtmosphereVolumePass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace skyview
	{
		enum Bindings : uint32_t
		{
			eMatrix,
			eScene,
			eAtmosphere,
			eTransmittance,
		};

		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, sliceId{ getMember< sdw::Int >( "sliceId" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "AtmosphereSurface"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "sliceId"
						, sdw::type::Kind::eInt
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			sdw::Int sliceId;
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			C3D_Matrix( writer
				, Bindings::eMatrix
				, 0u );
			C3D_Scene( writer
				, Bindings::eScene
				, 0u );
			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphere
				, 0u );

			auto inPosition = writer.declInput< sdw::Vec4 >( "inPosition", 0u );

			writer.implementMainT< sdw::VoidT, SurfaceT >( sdw::VertexIn{ writer }
				, sdw::VertexOutT< SurfaceT >{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOutT< SurfaceT > out )
				{
					out.vtx.position = vec4( inPosition.xy(), 0.9999999, 1.0 );
					out.sliceId = in.instanceIndex;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		castor3d::ShaderPtr getGeometryProgram()
		{
			sdw::GeometryWriter writer;
			writer.implementMainT< sdw::TriangleListT< SurfaceT >, sdw::TriangleStreamT< SurfaceT > >( sdw::TriangleListT< SurfaceT >{ writer }
			, sdw::TriangleStreamT< SurfaceT >{ writer, 3u }
			, [&]( sdw::GeometryIn in
				, sdw::TriangleListT< SurfaceT > list
				, sdw::TriangleStreamT< SurfaceT > out )
			{
				FOR( writer, sdw::UInt, i, 0_u, i < 3_u, ++i )
				{
					out.sliceId = list[0].sliceId;
					out.vtx.position = list[i].vtx.position;
					out.layer = list[0].sliceId;
					out.append();
				}
				ROF;

				out.restartStrip();
			} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( VkExtent3D const & renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			sdw::FragmentWriter writer;

			C3D_Matrix( writer
				, Bindings::eMatrix
				, 0u );
			C3D_Scene( writer
				, Bindings::eScene
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );

			auto depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );
			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );
			auto apSliceCount = writer.declConstant( "apSliceCount"
				, 32.0_f );
			auto apKmPerSlice = writer.declConstant( "apKmPerSlice"
				, 4.0_f );

			auto inViewDir = writer.declInput< sdw::Vec3 >( "inViewDir", 0u );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0 ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer
				, c3d_atmosphereData
				, c3d_matrixData
				, { false, false, false, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			auto aerialPerspectiveSliceToDepth = writer.implementFunction< sdw::Float >( "aerialPerspectiveSliceToDepth"
				, [&]( sdw::Float const & slice )
				{
					writer.returnStmt( slice * apKmPerSlice );
				}
				, sdw::InFloat{ writer, "slice" } );

			auto process = writer.implementFunction< sdw::Vec4 >( "process"
				, [&]( sdw::Vec2 pixPos
					, sdw::Int sliceId )
				{
					auto sampleCountIni = writer.declLocale( "sampleCountIni"
						, max( 1.0_f, writer.cast< sdw::Float >( sliceId + 1_i ) * 2.0_f ) );

					auto clipSpace = writer.declLocale( "clipSpace"
						, vec3( ( pixPos / vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) ) * vec2( 2.0_f, -2.0_f ) - vec2( 1.0_f, -1.0_f ), 0.5_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_matrixData.curProjToWorld( vec4( clipSpace, 1.0_f ) ) );
					auto worldDir = writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - c3d_sceneData.cameraPosition ) );
					auto earthR = writer.declLocale( "earthR"
						, c3d_atmosphereData.bottomRadius );
					auto earthO = writer.declLocale( "earthO"
						, vec3( 0.0_f, 0.0_f, -earthR ) );
					auto camPos = writer.declLocale( "camPos"
						, c3d_sceneData.cameraPosition + vec3( 0.0_f, 0.0_f, earthR ) );
					auto sunDir = writer.declLocale( "sunDir"
						, c3d_atmosphereData.sunDirection );
					auto sunLuminance = writer.declLocale( "sunLuminance"
						, 0.0_f );

					auto slice = writer.declLocale( "slice"
						, ( ( writer.cast< sdw::Float >( sliceId ) + 0.5_f ) / apSliceCount ) );
					slice *= slice;	// squared distribution
					slice *= apSliceCount;

					auto worldPos = writer.declLocale( "worldPos"
						, camPos );
					auto viewHeight = writer.declLocale< sdw::Float >( "viewHeight" );

					// Compute position from froxel information
					auto tMax = writer.declLocale( "tMax"
						, aerialPerspectiveSliceToDepth( slice ) );
					auto newWorldPos = writer.declLocale( "newWorldPos"
						, worldPos + tMax * worldDir );

					// If the voxel is under the ground, make sure to offset it out on the ground.
					viewHeight = length( newWorldPos );

					IF( writer, viewHeight <= ( c3d_atmosphereData.bottomRadius + planetRadiusOffset ) )
					{
						// Apply a position offset to make sure no artefact are visible close to the earth boundaries for large voxel.
						newWorldPos = normalize( newWorldPos ) * ( c3d_atmosphereData.bottomRadius + planetRadiusOffset + 0.001_f );
						worldDir = normalize( newWorldPos - camPos );
						tMax = length( newWorldPos - camPos );
					}
					FI;

					auto tMaxMax = writer.declLocale( "tMaxMax"
						, tMax );

					// Move ray marching start up to top atmosphere.
					viewHeight = length( worldPos );

					IF( writer, viewHeight >= c3d_atmosphereData.topRadius )
					{
						auto prevWorlPos = writer.declLocale( "prevWorlPos"
							, worldPos );

						IF( writer, !atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
						{
							// Ray is not intersecting the atmosphere
							writer.returnStmt( vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
						}
						FI;

						auto lengthToAtmosphere = writer.declLocale( "lengthToAtmosphere"
							, length( prevWorlPos - worldPos ) );

						IF( writer, tMaxMax < lengthToAtmosphere )
						{
							// tMaxMax for this voxel is not within earth atmosphere
							writer.returnStmt( vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
						}
						FI;

						// Now world position has been moved to the atmosphere boundary: we need to reduce tMaxMax accordingly. 
						tMaxMax = max( 0.0_f, tMaxMax - lengthToAtmosphere );
					}
					FI;

					SingleScatteringResult ss = writer.declLocale( "ss"
						, atmosphereConfig.integrateScatteredLuminance( pixPos
							, worldPos
							, worldDir
							, sunDir
							, sampleCountIni
							, depthBufferValue
							, tMaxMax ) );

					auto transmittance = writer.declLocale( "transmittance"
						, dot( ss.transmittance, vec3( 1.0_f / 3.0_f, 1.0_f / 3.0_f, 1.0_f / 3.0_f ) ) );
					writer.returnStmt( vec4( ss.luminance, 1.0_f - transmittance ) );
				}
				, sdw::InVec2{ writer, "pixPos" }
				, sdw::InInt{ writer, "sliceId" } );

			writer.implementMainT< SurfaceT, sdw::VoidT >( sdw::FragmentInT< SurfaceT >{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentInT< SurfaceT > in
					, sdw::FragmentOut out )
				{
					pxl_colour = process( in.fragCoord.xy(), in.sliceId );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereVolumePass::AtmosphereVolumePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::SceneUbo const & sceneUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & transmittanceView
		, crg::ImageViewId const & multiScatterView
		, crg::ImageViewId const & resultView )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "VolumePass", skyview::getVertexProgram() }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, "VolumePass", skyview::getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "VolumePass", skyview::getPixelProgram( getExtent( resultView ), getExtent( transmittanceView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( "VolumePass"
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.instances( renderSize.depth )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		matrixUbo.createPassBinding( pass
			, skyview::eMatrix );
		sceneUbo.createPassBinding( pass
			, skyview::eScene );
		atmosphereUbo.createPassBinding( pass
			, skyview::eAtmosphere );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( transmittanceView
			, skyview::eTransmittance
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void AtmosphereVolumePass::accept( castor3d::BackgroundVisitor & visitor )
	{
		//visitor.visit( m_vertexShader );
		//visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
