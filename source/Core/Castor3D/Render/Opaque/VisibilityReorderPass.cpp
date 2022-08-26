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

CU_ImplementCUSmartPtr( castor3d, VisibilityReorderPass )

namespace castor3d
{
	//*********************************************************************************************

	namespace vissort
	{
		class NodesPipelines
			: public shader::BufferT< sdw::UVec4 >
		{
		public:
			NodesPipelines( sdw::ShaderWriter & writer
				, uint32_t binding
				, uint32_t set )
				: BufferT{ "NodesPipelines", "nodesPipelines", writer, binding, set }
			{
			}

			sdw::UInt getPipelinesCount()const
			{
				return this->getSecondaryCount();
			}

			sdw::UInt operator[]( sdw::UInt const & index )const
			{
				return getData( index / 4u )[index % 4u];
			}
		};
	}

	namespace matcount
	{
		enum Bindings : uint32_t
		{
			eData,
			eMaterialsCounts,
		};

		static ShaderPtr getProgram( VkExtent3D sourceSize )
		{
			sdw::ComputeWriter writer;

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
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
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, crg::ImageViewId const & data
			, ashes::Buffer< uint32_t > const & counts
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
							.groupCountX( uint32_t( std::ceil( float( renderSize.width ) / 16.0f ) ) )
							.groupCountY( uint32_t( std::ceil( float( renderSize.height ) / 16.0f ) ) )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( *previousPass );
			pass.addInputStorageView( data, Bindings::eData );
			pass.addOutputStorageBuffer( { counts, "MaterialsCount" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( counts.getBuffer().getSize() ) );
			previousPass = &pass;
			return pass;
		}
	}

	//*********************************************************************************************

	namespace matstart
	{
		enum Bindings : uint32_t
		{
			eNodesPipelines,
			eMaterialsCounts,
			eMaterialsStarts,
		};

		static ShaderPtr getProgram()
		{
			sdw::ComputeWriter writer;

			vissort::NodesPipelines nodesPipelines{ writer, Bindings::eNodesPipelines, 0u };

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			writer.implementMain( 64u
				, [&]( sdw::ComputeIn in )
				{
					auto pipelineId = writer.declLocale( "pipelineId"
						, in.globalInvocationID.x() );

					IF( writer, pipelineId < nodesPipelines.getPipelinesCount() )
					{
						auto result = writer.declLocale( "result", 0_u );

						FOR( writer, sdw::UInt, i, 0_u, i < pipelineId, ++i )
						{
							result += materialsCounts[i];
						}
						ROF;

						materialStarts[pipelineId] = result;
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, ShaderBuffer const & pipelinesIds
			, ashes::Buffer< uint32_t > const & counts
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
			pass.addDependency( *previousPass );
			pipelinesIds.createPassBinding( pass, "NodesPipelines", Bindings::eNodesPipelines );
			pass.addInputStorageBuffer( { counts, "MaterialsCounts" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( counts.getBuffer().getSize() ) );
			pass.addOutputStorageBuffer( { starts, "MaterialsStart" }
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
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
		};

		static ShaderPtr getProgram( VkExtent3D sourceSize )
		{
			sdw::ComputeWriter writer;

			auto dataMap = writer.declStorageImg< sdw::RUImage2DRg32 >( "dataMap", Bindings::eData, 0u );
			auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialsStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

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
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, crg::ImageViewId const & data
			, ashes::Buffer< uint32_t > const & counts
			, ashes::Buffer< uint32_t > const & starts
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
							.groupCountX( uint32_t( std::ceil( float( renderSize.width ) / 16.0f ) ) )
							.groupCountY( uint32_t( std::ceil( float( renderSize.height ) / 16.0f ) ) )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) ) );
					device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( *previousPass );
			pass.addInputStorageView( data, Bindings::eData );
			pass.addOutputStorageBuffer( { counts, "MaterialsCounts" }
				, uint32_t( Bindings::eMaterialsCounts )
				, 0u
				, uint32_t( counts.getBuffer().getSize() ) );
			pass.addInputStorageBuffer( { starts, "MaterialsStart" }
				, uint32_t( Bindings::eMaterialsStarts )
				, 0u
				, uint32_t( starts.getBuffer().getSize() ) );
			pass.addOutputStorageBuffer( { pixels, "PixelsXY" }
				, uint32_t( Bindings::ePixelsXY )
				, 0u
				, uint32_t( pixels.getBuffer().getSize() ) );
			previousPass = &pass;
			return pass;
		}
	}

	//*********************************************************************************************

	class BuffersClearPass
		: public crg::RunnablePass
	{
	public:
		BuffersClearPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph )
			: crg::RunnablePass{ pass
			, context
			, graph
			, { [](){}
				, GetSemaphoreWaitFlagsCallback( [](){ return VK_PIPELINE_STAGE_TRANSFER_BIT; } )
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); } } }
		{
		}

	private:
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index )
		{
			for ( auto & attach : m_pass.buffers )
			{
				auto buffer = attach.buffer.buffer;
				m_context.vkCmdFillBuffer( commandBuffer
					, buffer.buffer
					, 0u
					, ashes::WholeSize
					, 0u );
			}
		}
	};

	//*********************************************************************************************

	VisibilityReorderPass::VisibilityReorderPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, crg::ImageViewId const & data
		, ShaderBuffer const & pipelinesIds
		, ashes::Buffer< uint32_t > const & counts1
		, ashes::Buffer< uint32_t > const & counts2
		, ashes::Buffer< uint32_t > const & starts
		, ashes::Buffer< castor::Point2ui > const & pixels )
		: castor::Named{ "VisibilityPass" }
		, m_computeCountsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName()
			, matcount::getProgram( getExtent( data ) ) }
		, m_countsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeCountsShader ) } }
		, m_computeStartsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "Starts" )
			, matstart::getProgram() }
		, m_startsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeStartsShader ) } }
		, m_computePixelsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "PixelsXY" )
			, pixelxy::getProgram( getExtent( data ) ) }
		, m_pixelsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computePixelsShader ) } }
	{
		auto & clear = graph.createPass( getName() + "/Clear"
			, [&device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< BuffersClearPass >( framePass
					, context
					, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		clear.addDependencies( previousPasses );
		clear.addOutputStorageBuffer( { counts1, "MaterialsCounts1" }
			, 0u
			, 0u
			, uint32_t( counts1.getBuffer().getSize() ) );
		clear.addOutputStorageBuffer( { counts2, "MaterialsCounts2" }
			, 0u
			, 0u
			, uint32_t( counts2.getBuffer().getSize() ) );
		clear.addOutputStorageBuffer( { starts, "MaterialsStarts" }
			, 0u
			, 0u
			, uint32_t( starts.getBuffer().getSize() ) );
		crg::FramePass const * ppreviousPass = &clear;

		m_lastPass = &matcount::createPass( getName()
			, graph
			, ppreviousPass
			, device
			, data
			, counts1
			, m_countsStages );
		m_lastPass = &matstart::createPass( getName()
			, graph
			, ppreviousPass
			, device
			, pipelinesIds
			, counts1
			, starts
			, m_startsStages );
		m_lastPass = &pixelxy::createPass( getName()
			, graph
			, ppreviousPass
			, device
			, data
			, counts2
			, starts
			, pixels
			, m_pixelsStages );
	}

	void VisibilityReorderPass::accept( PipelineVisitor & visitor )
	{
		visitor.visit( m_computeCountsShader );
		visitor.visit( m_computeStartsShader );
		visitor.visit( m_computePixelsShader );
	}

	//************************************************************************************************
}
