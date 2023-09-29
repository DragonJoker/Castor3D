#include "Castor3D/Render/Opaque/VisibilityReorderPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

CU_ImplementSmartPtr( castor3d, VisibilityReorderPass )

namespace castor3d
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
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto MaterialsCounts = writer.declStorageBuffer<>( "C3D_MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "counts" );
			MaterialsCounts.end();

			writer.implementMain( 16u, 16u
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, in.globalInvocationID.xy() );
					auto data = writer.declLocale( "data"
						, dataMap.load( ivec2( pixel ) ) );
					auto nodePipelineId = writer.declLocale( "nodePipelineId"
						, data.x() );
					auto nodeId = writer.declLocale( "nodeId"
						, nodePipelineId >> maxPipelinesSize );

					IF( writer, nodeId > 0_u )
					{
						auto pipelineId = writer.declLocale( "pipelineId"
							, nodePipelineId & maxPipelinesMask );
						sdw::atomicAdd( materialsCounts[pipelineId], 1_u );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, crg::ImageViewId const & data
			, ashes::Buffer< uint32_t > const & materialsCounts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto renderSize = getExtent( data );
			auto & pass = graph.createPass( name + "/MaterialsCount"
				, [&stages, &device, renderSize]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( castor::divRoundUp( renderSize.width, 16u ) )
							.groupCountY( castor::divRoundUp( renderSize.height, 16u ) )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependencies( previousPasses );

			if ( previousPass )
			{
				pass.addDependency( *previousPass );
			}

			pass.addInputStorageView( data, Bindings::eData );
			pass.addClearableOutputStorageBuffer( { materialsCounts, "MaterialsCount" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( materialsCounts.getBuffer().getSize() ) );
			previousPass = &pass;
			return pass;
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
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto IndirectCounts = writer.declStorageBuffer<>( "IndirectCounts", Bindings::eIndirectCounts, 0u );
			auto indirectCounts = IndirectCounts.declMemberArray< sdw::UVec3 >( "indirectCounts" );
			IndirectCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			writer.implementMain( 64u
				, [&]( sdw::ComputeIn in )
				{
					auto pipelineId = writer.declLocale( "pipelineId"
						, in.globalInvocationID.x() );

					IF( writer, materialsCounts[pipelineId] > 0_u )
					{
						auto result = writer.declLocale( "result", 0_u );

						FOR( writer, sdw::UInt, i, 0_u, i < pipelineId, ++i )
						{
							result += materialsCounts[i];
						}
						ROF;

						indirectCounts[pipelineId] = uvec3( writer.cast< sdw::UInt >( ceil( writer.cast< sdw::Float >( materialsCounts[pipelineId] ) / 64.0_f ) ), 1u, 1u );
						materialStarts[pipelineId] = result;
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, ashes::Buffer< uint32_t > const & materialsCounts
			, ashes::Buffer< castor::Point3ui > const & indirectCounts
			, ashes::Buffer< uint32_t > const & starts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto & pass = graph.createPass( name + "/MaterialsStart"
				, [&stages, &device]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( device.renderSystem.getEngine()->getMaxPassTypeCount() / 64u )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependencies( previousPasses );

			if ( previousPass )
			{
				pass.addDependency( *previousPass );
			}

			pass.addInputStorageBuffer( { materialsCounts, "MaterialsCounts" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( materialsCounts.getBuffer().getSize() ) );
			pass.addClearableOutputStorageBuffer( { indirectCounts, "IndirectCounts" }
				, uint32_t( Bindings::eIndirectCounts )
				, 0u
				, uint32_t( indirectCounts.getBuffer().getSize() ) );
			pass.addClearableOutputStorageBuffer( { starts, "MaterialsStart" }
				, uint32_t( Bindings::eMaterialsStarts )
				, 0u
				, uint32_t( starts.getBuffer().getSize() ) );
			previousPass = &pass;
			return pass;
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
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
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
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, in.globalInvocationID.xy() );
					auto data = writer.declLocale( "data"
						, dataMap.load( ivec2( pixel ) ) );
					auto nodePipelineId = writer.declLocale( "nodePipelineId"
						, data.x() );
					auto nodeId = writer.declLocale( "nodeId"
						, nodePipelineId >> maxPipelinesSize );

					IF( writer, nodeId > 0_u )
					{
						auto pipelineId = writer.declLocale( "pipelineId"
							, nodePipelineId & maxPipelinesMask );
						auto pixelIndex = writer.declLocale( "pixelIndex"
							, materialsStarts[pipelineId] + sdw::atomicAdd( materialsCounts[pipelineId], 1_u ) );
						pixelsXY[pixelIndex] = pixel;
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, crg::ImageViewId const & data
			, ashes::Buffer< uint32_t > const & materialsCounts
			, ashes::Buffer< uint32_t > const & materialsStarts
			, ashes::Buffer< castor::Point2ui > const & pixels
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto renderSize = getExtent( data );
			auto & pass = graph.createPass( name + "/PixelsXY"
				, [&stages, &device, renderSize]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = std::make_unique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( castor::divRoundUp( renderSize.width, 16u ) )
							.groupCountY( castor::divRoundUp( renderSize.height, 16u ) )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependencies( previousPasses );

			if ( previousPass )
			{
				pass.addDependency( *previousPass );
			}

			pass.addInputStorageView( data, Bindings::eData );
			pass.addInputStorageBuffer( { materialsStarts, "MaterialsStart" }
				, uint32_t( Bindings::eMaterialsStarts )
				, 0u
				, uint32_t( materialsStarts.getBuffer().getSize() ) );
			pass.addClearableOutputStorageBuffer( { materialsCounts, "MaterialsCounts" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( materialsCounts.getBuffer().getSize() ) );
			pass.addClearableOutputStorageBuffer( { pixels, "PixelsXY" }
				, uint32_t( Bindings::ePixelsXY )
				, 0u
				, uint32_t( pixels.getBuffer().getSize() ) );
			previousPass = &pass;
			return pass;
		}
	}

	//*********************************************************************************************

	VisibilityReorderPass::VisibilityReorderPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, crg::ImageViewId const & data
		, ashes::Buffer< uint32_t > const & materialsCounts
		, ashes::Buffer< castor::Point3ui > const & indirectCounts
		, ashes::Buffer< uint32_t > const & materialsStarts
		, ashes::Buffer< castor::Point2ui > const & pixels )
		: castor::Named{ "VisibilityReorder" }
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
		crg::FramePass const * previousPass{};
		m_lastPass = &matcount::createPass( getName() + cuT( "/Counts" )
			, graph
			, previousPasses
			, previousPass
			, device
			, data
			, materialsCounts
			, m_countsStages );
		m_lastPass = &matstart::createPass( getName() + cuT( "/Starts" )
			, graph
			, previousPasses
			, previousPass
			, device
			, materialsCounts
			, indirectCounts
			, materialsStarts
			, m_startsStages );
		m_lastPass = &pixelxy::createPass( getName() + cuT( "/Pixels" )
			, graph
			, previousPasses
			, previousPass
			, device
			, data
			, materialsCounts
			, materialsStarts
			, pixels
			, m_pixelsStages );
	}

	void VisibilityReorderPass::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_computeCountsShader );
		visitor.visit( m_computeStartsShader );
		visitor.visit( m_computePixelsShader );
	}

	//************************************************************************************************
}
