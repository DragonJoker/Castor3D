#include "AtmosphereScattering/AtmosphereVolumePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
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

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace volume
	{
		enum Bindings : uint32_t
		{
			eCamera,
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

		static castor3d::ShaderPtr getGeometryProgram()
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

			C3D_Camera( writer
				, Bindings::eCamera
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

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0 ) );

			castor3d::shader::Utils utils{ writer };
			AtmosphereModel atmosphere{ writer
				, utils
				, c3d_atmosphereData
				, { false, nullptr, false, true }
				, { transmittanceExtent.width, transmittanceExtent.height } };
			atmosphere.setTransmittanceMap( transmittanceMap );

			auto aerialPerspectiveSliceToDepth = writer.implementFunction< sdw::Float >( "aerialPerspectiveSliceToDepth"
				, [&]( sdw::Float const & slice )
				{
					auto apKmPerSlice = writer.declConstant( "apKmPerSlice"
						, 4.0_f );
					writer.returnStmt( slice * apKmPerSlice );
				}
				, sdw::InFloat{ writer, "slice" } );

			auto process = writer.implementFunction< sdw::Vec4 >( "process"
				, [&]( sdw::Vec2 pixPos
					, sdw::Int sliceId )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					auto sampleCountIni = writer.declLocale( "sampleCountIni"
						, max( 1.0_f, writer.cast< sdw::Float >( sliceId + 1_i ) * 2.0_f ) );

					auto clipSpace = writer.declLocale( "clipSpace"
						, atmosphere.getClipSpace( pixPos, targetSize, 0.5_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );
					auto earthR = writer.declLocale( "earthR"
						, c3d_atmosphereData.bottomRadius );
					auto camPos = writer.declLocale( "camPos"
						, c3d_cameraData.position() + vec3( 0.0_f, earthR, 0.0_f ) );
					auto sunLuminance = writer.declLocale( "sunLuminance"
						, 0.0_f );
					auto ray = writer.declLocale< Ray >( "ray" );
					ray.origin = camPos;
					ray.direction = normalize( hPos.xyz() / hPos.w() - c3d_cameraData.position() );

					auto slice = writer.declLocale( "slice"
						, ( ( writer.cast< sdw::Float >( sliceId ) + 0.5_f ) / apSliceCount ) );
					slice *= slice;	// squared distribution
					slice *= apSliceCount;

					auto viewHeight = writer.declLocale< sdw::Float >( "viewHeight" );

					// Compute position from froxel information
					auto tMax = writer.declLocale( "tMax"
						, aerialPerspectiveSliceToDepth( slice ) );
					auto newWorldPos = writer.declLocale( "newWorldPos"
						, ray.step( tMax ) );

					// If the voxel is under the ground, make sure to offset it out on the ground.
					viewHeight = length( newWorldPos );

					IF( writer, viewHeight <= ( c3d_atmosphereData.bottomRadius + planetRadiusOffset ) )
					{
						// Apply a position offset to make sure no artefact are visible close to the earth boundaries for large voxel.
						newWorldPos = normalize( newWorldPos ) * ( c3d_atmosphereData.bottomRadius + planetRadiusOffset + 0.001_f );
						ray.direction = normalize( newWorldPos - camPos );
						tMax = length( newWorldPos - camPos );
					}
					FI;

					auto tMaxMax = writer.declLocale( "tMaxMax"
						, tMax );

					// Move ray marching start up to top atmosphere.
					viewHeight = length( ray.origin );

					IF( writer, viewHeight >= c3d_atmosphereData.topRadius )
					{
						auto prevWorlPos = writer.declLocale( "prevWorlPos"
							, ray.origin );

						IF( writer, !atmosphere.moveToTopAtmosphere( ray ) )
						{
							// Ray is not intersecting the atmosphere
							writer.returnStmt( vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f ) );
						}
						FI;

						auto lengthToAtmosphere = writer.declLocale( "lengthToAtmosphere"
							, length( prevWorlPos - ray.origin ) );

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
						, atmosphere.integrateScatteredLuminanceNoShadow( pixPos
							, ray
							, c3d_atmosphereData.sunDirection
							, sampleCountIni
							, depthBufferValue
							, tMaxMax ) );

					auto transmittance = writer.declLocale( "transmittance"
						, dot( ss.transmittance(), vec3( 1.0_f / 3.0_f ) ) );
					writer.returnStmt( vec4( ss.luminance(), 1.0_f - transmittance ) );
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
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & transmittanceView
		, crg::ImageViewId const & resultView
		, uint32_t index
		, bool const & enabled )
		: castor::Named{ "VolumePass" + castor::string::toString( index ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), volume::getVertexProgram() }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), volume::getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), volume::getPixelProgram( getExtent( resultView ), getExtent( transmittanceView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( getName()
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.instances( renderSize.depth )
					.enabled( &enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		cameraUbo.createPassBinding( pass
			, volume::eCamera );
		atmosphereUbo.createPassBinding( pass
			, volume::eAtmosphere );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( transmittanceView
			, volume::eTransmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void AtmosphereVolumePass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
