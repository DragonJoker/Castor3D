#include "Castor3D/Render/Technique/Visibility/VisibilityReorderPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
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

	namespace matcount
	{
		enum Bindings : uint32_t
		{
			eData0,
			eMaterials,
			eModelsData,
			eMaterialsCounts,
		};

		static ShaderPtr getProgram( VkExtent3D sourceSize )
		{
			sdw::ComputeWriter writer;

			auto data0Map = writer.declStorageImg< sdw::RImage2DRgba32 >( "data0Map", Bindings::eData0, 0u );
			shader::Materials materials{ writer, Bindings::eMaterials, 0u };
			C3D_ModelsData( writer, Bindings::eModelsData, 0u );

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			writer.implementMain( 16u, 16u
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, in.globalInvocationID.xy() );
					auto data0 = data0Map.load( ivec2( pixel ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( data0.z() ) );

					IF( writer, nodeId > 0_u )
					{
						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[nodeId - 1_u] );
						auto passTypeIndex = writer.declLocale( "passTypeIndex"
							, materials.getMaterial( modelData.getMaterialId() - 1_u ).passTypeIndex() );
						sdw::atomicAdd( materialsCounts[passTypeIndex], 1_u );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, Scene const & scene
			, crg::ImageViewId const & data0
			, ashes::Buffer< uint32_t > const & counts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto & modelBuffer = scene.getModelBuffer().getBuffer();
			auto & matCache = scene.getOwner()->getMaterialCache();
			auto renderSize = getExtent( data0 );
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
			pass.addInputStorageView( data0, Bindings::eData0 );
			matCache.getPassBuffer().createPassBinding( pass, Bindings::eMaterials );
			pass.addInputStorageBuffer( { modelBuffer, "Models" }
				, uint32_t( Bindings::eModelsData )
				, 0u
				, uint32_t( modelBuffer.getSize() ) );
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
			eMaterials,
			eMaterialsCounts,
			eMaterialsStarts,
		};

		static ShaderPtr getProgram()
		{
			sdw::ComputeWriter writer;

			shader::Materials materials{ writer, Bindings::eMaterials, 0u };

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", Bindings::eMaterialsCounts, 0u );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", Bindings::eMaterialsStarts, 0u );
			auto materialStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			writer.implementMain( 64u
				, [&]( sdw::ComputeIn in )
				{
					auto passTypeIndex = writer.declLocale( "passTypeIndex"
						, in.globalInvocationID.x() );

					IF( writer, passTypeIndex < materials.getPassTypesCount() )
					{
						auto result = writer.declLocale( "result", 0_u );

						FOR( writer, sdw::UInt, i, 0_u, i < passTypeIndex, ++i )
						{
							result += materialsCounts[i];
						}
						ROF;

						materialStarts[passTypeIndex] = result;
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static crg::FramePass const & createPass( castor::String const & name
			, crg::FramePassGroup & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, Scene const & scene
			, ashes::Buffer< uint32_t > const & counts
			, ashes::Buffer< uint32_t > const & starts
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto & matCache = scene.getOwner()->getMaterialCache();
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
			matCache.getPassBuffer().createPassBinding( pass, Bindings::eMaterials );
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
			eData0,
			eMaterials,
			eModelsData,
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
		};

		static ShaderPtr getProgram( VkExtent3D sourceSize )
		{
			sdw::ComputeWriter writer;

			auto data0Map = writer.declStorageImg< sdw::RImage2DRgba32 >( "data0Map", Bindings::eData0, 0u );
			shader::Materials materials{ writer, Bindings::eMaterials, 0u };
			C3D_ModelsData( writer, Bindings::eModelsData, 0u );

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
					auto data0 = data0Map.load( ivec2( pixel ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( data0.z() ) );

					IF( writer, nodeId > 0_u )
					{
						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[nodeId - 1_u] );
						auto passTypeIndex = writer.declLocale( "passTypeIndex"
							, materials.getMaterial( modelData.getMaterialId() - 1_u ).passTypeIndex() );
						auto pixelIndex = writer.declLocale( "pixelIndex"
							, materialsStarts[passTypeIndex] + sdw::atomicAdd( materialsCounts[passTypeIndex], 1_u ) );
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
			, Scene const & scene
			, crg::ImageViewId const & data0
			, ashes::Buffer< uint32_t > const & counts
			, ashes::Buffer< uint32_t > const & starts
			, ashes::Buffer< castor::Point2ui > const & pixels
			, ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			auto & modelBuffer = scene.getModelBuffer().getBuffer();
			auto & matCache = scene.getOwner()->getMaterialCache();
			auto renderSize = getExtent( data0 );
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
			pass.addInputStorageView( data0, Bindings::eData0 );
			matCache.getPassBuffer().createPassBinding( pass, Bindings::eMaterials );
			pass.addInputStorageBuffer( { modelBuffer, "Models" }
				, uint32_t( Bindings::eModelsData )
				, 0u
				, uint32_t( modelBuffer.getSize() ) );
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
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, Scene const & scene
		, crg::ImageViewId const & data0
		, ashes::Buffer< uint32_t > const & counts1
		, ashes::Buffer< uint32_t > const & counts2
		, ashes::Buffer< uint32_t > const & starts
		, ashes::Buffer< castor::Point2ui > const & pixels )
		: castor::Named{ "VisibilityPass" }
		, m_computeCountsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName()
			, matcount::getProgram( getExtent( data0 ) ) }
		, m_countsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeCountsShader ) } }
		, m_computeStartsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "Starts" )
			, matstart::getProgram() }
		, m_startsStages{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeStartsShader ) } }
		, m_computePixelsShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName() + cuT( "PixelsXY" )
			, pixelxy::getProgram( getExtent( data0 ) ) }
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
		clear.addDependency( previousPass );
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
			, scene
			, data0
			, counts1
			, m_countsStages );
		m_lastPass = &matstart::createPass( getName()
			, graph
			, ppreviousPass
			, device
			, scene
			, counts1
			, starts
			, m_startsStages );
		m_lastPass = &pixelxy::createPass( getName()
			, graph
			, ppreviousPass
			, device
			, scene
			, data0
			, counts2
			, starts
			, pixels
			, m_pixelsStages );

		//auto & matCache = scene.getOwner()->getMaterialCache();

		//for ( uint32_t passTypeIndex = 0u; passTypeIndex < matCache.getCurrentPassTypeCount(); ++passTypeIndex )
		//{
		//	auto [passTypeID, passFlags, textureFlags, textureCount] = matCache.getPassTypeDetails( passTypeIndex );
		//}
	}

	void VisibilityReorderPass::accept( PipelineVisitor & visitor )
	{
		visitor.visit( m_computeCountsShader );
		visitor.visit( m_computeStartsShader );
		visitor.visit( m_computePixelsShader );
	}

	//************************************************************************************************
}
