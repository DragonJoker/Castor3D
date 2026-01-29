#include "Castor3D/Render/Opaque/VisibilityReorderPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ashespp/Buffer/Buffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, VisibilityReorderPass )

namespace c3d
{
	//*********************************************************************************************

	namespace matcount
	{
		enum Bindings : uint32_t
		{
			eData,
			eMaterialsCounts,
		};

		static ShaderPtr getProgram( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto MaterialsCounts = writer.declStorageBuffer<>( "C3D_MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "counts" );
			MaterialsCounts.end();

			writer.implementMain( 16u, 16u
				, [&]( sdw::ComputeIn const & in )
				{
					auto pixel = writer.declLocale( "pixel"
						, in.globalInvocationID.xy() );
					auto data = writer.declLocale( "data"
						, dataMap.load( ivec2( pixel ) ) );
					auto nodePipelineId = writer.declLocale( "nodePipelineId"
						, data.x() );
					auto nodeId = writer.declLocale( "nodeId"
						, nodePipelineId >> maxPipelinesSize );

					sdwIF( writer, nodeId > 0_u )
					{
						auto pipelineId = writer.declLocale( "pipelineId"
							, nodePipelineId & maxPipelinesMask );
						sdw::atomicAdd( materialsCounts[pipelineId], 1_u );
					}
					sdwFI
				} );

			return writer.getBuilder().releaseShader();
		}

		static void createPass( String const & name
			, crg::FramePassGroup & graph
			, RenderDevice const & device
			, crg::RunnablePass::IsEnabledCallback isEnabled
			, Texture const & data
			, Buffer & materialsCounts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto renderSize = data.getExtent();
			auto & pass = graph.createPass( toUtf8( name ) + "/MaterialsCount"
				, [&stages, &device, enable = c3d::move( isEnabled ), renderSize]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = makeRawUnique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.isEnabled( enable )
							.groupCountX( divRoundUp( renderSize.width, 16u ) )
							.groupCountY( divRoundUp( renderSize.height, 16u ) )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addInputStorage( *data.getLastAttach(), Bindings::eData );
			materialsCounts.setLastAttach( pass.addClearableOutputStorageBuffer( materialsCounts.bufferViewId, uint32_t( Bindings::eMaterialsCounts ) ) );
		}
	}

	//*********************************************************************************************

	namespace matstart
	{
		enum Bindings : uint32_t
		{
			eMaterialsCounts,
			eIndirectCounts,
			eMaterialsStarts,
		};

		static ShaderPtr getProgram( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto IndirectCounts = writer.declStorageBuffer<>( "IndirectCounts", Bindings::eIndirectCounts, 0u );
			auto indirectCounts = IndirectCounts.declMemberArray< sdw::UInt >( "indirectCounts" );
			IndirectCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			writer.implementMain( 64u
				, [&]( sdw::ComputeIn const & in )
				{
					auto pipelineId = writer.declLocale( "pipelineId"
						, in.globalInvocationID.x() );

					sdwIF( writer, materialsCounts[pipelineId] > 0_u )
					{
						auto result = writer.declLocale( "result", 0_u );

						sdwFOR( writer, sdw::UInt, i, 0_u, i < pipelineId, ++i )
						{
							result += materialsCounts[i];
						}
						sdwROF

						indirectCounts[pipelineId * 3u + 0u] = writer.cast< sdw::UInt >( ceil( writer.cast< sdw::Float >( materialsCounts[pipelineId] ) / 64.0_f ) );
						indirectCounts[pipelineId * 3u + 1u] = 4u;
						indirectCounts[pipelineId * 3u + 2u] = 1u;
						materialStarts[pipelineId] = result;
					}
					sdwFI
				} );

			return writer.getBuilder().releaseShader();
		}

		static void createPass( String const & name
			, crg::FramePassGroup & graph
			, RenderDevice const & device
			, crg::RunnablePass::IsEnabledCallback isEnabled
			, Buffer const & materialsCounts
			, Buffer & indirectCounts
			, Buffer & starts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto & pass = graph.createPass( toUtf8( name ) + "/MaterialsStart"
				, [&stages, &device, enable = c3d::move( isEnabled )]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = makeRawUnique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.isEnabled( enable )
							.groupCountX( c3d::getEngine( device ).getMaxPassTypeCount() / 64u )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addInputStorage( *materialsCounts.getLastAttach(), uint32_t( Bindings::eMaterialsCounts ) );
			indirectCounts.setLastAttach( pass.addClearableOutputStorageBuffer( indirectCounts.bufferViewId, uint32_t( Bindings::eIndirectCounts ) ) );
			starts.setLastAttach( pass.addClearableOutputStorageBuffer( starts.bufferViewId, uint32_t( Bindings::eMaterialsStarts ) ) );
		}
	}

	//*********************************************************************************************

	namespace pixelxy
	{
		enum Bindings : uint32_t
		{
			eData,
			eMaterialsStarts,
			eMaterialsCounts,
			ePixelsXY,
		};

		static ShaderPtr getProgram( RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialsStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto PixelsXY = writer.declStorageBuffer<>( "PixelsXY", Bindings::ePixelsXY, 0u );
			auto pixelsXY = PixelsXY.declMemberArray< sdw::UVec2 >( "pixelsXY" );
			PixelsXY.end();

			writer.implementMain( 16u, 16u
				, [&]( sdw::ComputeIn const & in )
				{
					auto pixel = writer.declLocale( "pixel"
						, in.globalInvocationID.xy() );
					auto data = writer.declLocale( "data"
						, dataMap.load( ivec2( pixel ) ) );
					auto nodePipelineId = writer.declLocale( "nodePipelineId"
						, data.x() );
					auto nodeId = writer.declLocale( "nodeId"
						, nodePipelineId >> maxPipelinesSize );

					sdwIF( writer, nodeId > 0_u )
					{
						auto pipelineId = writer.declLocale( "pipelineId"
							, nodePipelineId & maxPipelinesMask );
						auto pixelIndex = writer.declLocale( "pixelIndex"
							, materialsStarts[pipelineId] + sdw::atomicAdd( materialsCounts[pipelineId], 1_u ) );
						pixelsXY[pixelIndex] = pixel;
					}
					sdwFI
				} );

			return writer.getBuilder().releaseShader();
		}

		static void createPass( String const & name
			, crg::FramePassGroup & graph
			, RenderDevice const & device
			, crg::RunnablePass::IsEnabledCallback isEnabled
			, Texture const & data
			, Buffer & materialsCounts
			, Buffer const & materialsStarts
			, Buffer & pixels
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto renderSize = data.getExtent();
			auto & pass = graph.createPass( toUtf8( name ) + "/PixelsXY"
				, [&stages, &device, enable = c3d::move( isEnabled ), renderSize]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = makeRawUnique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.isEnabled( enable )
							.groupCountX( divRoundUp( renderSize.width, 16u ) )
							.groupCountY( divRoundUp( renderSize.height, 16u ) )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addInputStorage( *data.getLastAttach(), Bindings::eData );
			pass.addInputStorage( *materialsStarts.getLastAttach(), uint32_t( Bindings::eMaterialsStarts ) );
			materialsCounts.setLastAttach( pass.addClearableOutputStorageBuffer( materialsCounts.bufferViewId, uint32_t( Bindings::eMaterialsCounts ) ) );
			pixels.setLastAttach( pass.addClearableOutputStorageBuffer( pixels.bufferViewId, uint32_t( Bindings::ePixelsXY ) ) );
		}
	}

	//*********************************************************************************************

	VisibilityReorderPass::VisibilityReorderPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, Texture const & data
		, Buffer & materialsCounts
		, Buffer & indirectCounts
		, Buffer & materialsStarts
		, Buffer & pixels
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: Named{ cuT( "VisibilityReorder" ) }
		, m_computeCountsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName()
			, matcount::getProgram( device ) }
		, m_countsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeCountsShader ) } }
		, m_computeStartsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "Starts" )
			, matstart::getProgram( device ) }
		, m_startsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeStartsShader ) } }
		, m_computePixelsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "PixelsXY" )
			, pixelxy::getProgram( device ) }
		, m_pixelsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computePixelsShader ) } }
	{
		matcount::createPass( getName() + cuT( "/Counts" )
			, graph
			, device
			, isEnabled
			, data
			, materialsCounts
			, m_countsStages );
		matstart::createPass( getName() + cuT( "/Starts" )
			, graph
			, device
			, isEnabled
			, materialsCounts
			, indirectCounts
			, materialsStarts
			, m_startsStages );
		pixelxy::createPass( getName() + cuT( "/Pixels" )
			, graph
			, device
			, c3d::move( isEnabled )
			, data
			, materialsCounts
			, materialsStarts
			, pixels
			, m_pixelsStages );
	}

	void VisibilityReorderPass::accept( ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_computeCountsShader );
		visitor.visit( m_computeStartsShader );
		visitor.visit( m_computePixelsShader );
	}

	//************************************************************************************************
}
