#include "Castor3D/Render/Transform/VertexTransforming.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Transform/VertexTransformPass.hpp"
#include "Castor3D/Render/Transform/VertexTransformingPass.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/ComputeWriter.hpp>

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, VertexTransforming )

namespace castor3d
{
	//*********************************************************************************************

	namespace vtxtrsg
	{
		struct Skin
			: public sdw::StructInstance
		{
			Skin( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstance{ writer, std::move( expr ), enabled }
				, boneIds0{ this->getMember< sdw::UVec4 >( "boneIds0", true ) }
				, boneIds1{ this->getMember< sdw::UVec4 >( "boneIds1", true ) }
				, boneWeights0{ this->getMember< sdw::Vec4 >( "boneWeights0", true ) }
				, boneWeights1{ this->getMember< sdw::Vec4 >( "boneWeights1", true ) }
			{
			}

			SDW_DeclStructInstance( , Skin );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, SubmeshFlags const & submeshFlags )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
					, "C3D_Skin" );

				if ( result->empty() )
				{
					result->declMember( "boneIds0", ast::type::Kind::eVec4U
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eBones ) );
					result->declMember( "boneIds1", ast::type::Kind::eVec4U
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eBones ) );
					result->declMember( "boneWeights0", ast::type::Kind::eVec4F
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eBones ) );
					result->declMember( "boneWeights1", ast::type::Kind::eVec4F
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eBones ) );
				}

				return result;
			}

			sdw::UVec4 boneIds0;
			sdw::UVec4 boneIds1;
			sdw::Vec4 boneWeights0;
			sdw::Vec4 boneWeights1;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;

			bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eMorphTargets
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eMorphingWeights
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::ePositions ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInPosition
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutPosition
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eNormals ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInNormal
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutNormal
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eTangents ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInTangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutTangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords0 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInTexcoord0
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutTexcoord0
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords1 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInTexcoord1
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutTexcoord1
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords2 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInTexcoord2
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutTexcoord2
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords3 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInTexcoord3
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutTexcoord3
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eColours ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInColour
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutColour
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eBones ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInBones
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutBones
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			return device->createDescriptorSetLayout( pipeline.getName()
				, std::move( bindings ) );
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			return device->createPipelineLayout( pipeline.getName()
				, *pipeline.descriptorSetLayout
				, VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( castor::Point4ui ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			// Initialise the pipeline.
			return device->createPipeline( pipeline.getName()
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, pipeline.shader )
					, *pipeline.pipelineLayout ) );
		}

		ShaderPtr getShaderSource( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			using namespace sdw;

			ComputeWriter writer;

			// Common
			auto objectIDs = writer.declPushConstantsBuffer<>( "ObjectIDs" );
			auto c3d_objectIDs = objectIDs.declMember< shader::ObjectIds >( "c3d_objectIDs" );
			objectIDs.end();

			// Morphing
			auto c3d_morphTargets = writer.declArrayStorageBuffer< shader::MorphTargetsData >( "c3d_morphTargets"
				, { uint32_t( VertexTransformPass::eMorphTargets ), 0u }
				, pipeline.morphFlags != MorphFlag::eNone
				, pipeline.morphFlags );
			auto c3d_morphingWeights = writer.declArrayStorageBuffer< shader::MorphingWeightsData >( "c3d_morphingWeights"
				, { uint32_t( VertexTransformPass::eMorphingWeights ), 0u }
				, pipeline.morphFlags != MorphFlag::eNone );

#define DeclareSsbo( Name, Type, Binding, Enable )\
	sdw::Ssbo Name##Buffer{ writer\
		, #Name + std::string{ "Buffer" }\
		, uint32_t( Binding )\
		, 0u\
		, ast::type::MemoryLayout::eStd430\
		, Enable };\
	auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
	Name##Buffer.end()

#define DeclareSsboEx( Name, Type, Binding, Enable, Flags )\
	sdw::Ssbo Name##Buffer{ writer\
		, #Name + std::string{ "Buffer" }\
		, uint32_t( Binding )\
		, 0u\
		, ast::type::MemoryLayout::eStd430\
		, Enable };\
	auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable, Flags );\
	Name##Buffer.end()

			// Inputs
			DeclareSsbo( c3d_inPosition
				, sdw::Vec4
				, VertexTransformPass::eInPosition
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::ePositions ) );
			DeclareSsbo( c3d_inNormal
				, sdw::Vec4
				, VertexTransformPass::eInNormal
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eNormals ) );
			DeclareSsbo( c3d_inTangent
				, sdw::Vec4
				, VertexTransformPass::eInTangent
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTangents ) );
			DeclareSsbo( c3d_inTexcoord0
				, sdw::Vec4
				, VertexTransformPass::eInTexcoord0
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords0 ) );
			DeclareSsbo( c3d_inTexcoord1
				, sdw::Vec4
				, VertexTransformPass::eInTexcoord1
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords1 ) );
			DeclareSsbo( c3d_inTexcoord2
				, sdw::Vec4
				, VertexTransformPass::eInTexcoord2
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords2 ) );
			DeclareSsbo( c3d_inTexcoord3
				, sdw::Vec4
				, VertexTransformPass::eInTexcoord3
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords3 ) );
			DeclareSsbo( c3d_inColour
				, sdw::Vec4
				, VertexTransformPass::eInColour
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eColours ) );
			DeclareSsboEx( c3d_inBones
				, Skin
				, VertexTransformPass::eInBones
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eBones )
				, pipeline.submeshFlags );

			// Outputs
			DeclareSsbo( c3d_outPosition
				, sdw::Vec4
				, VertexTransformPass::eOutPosition
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::ePositions ) );
			DeclareSsbo( c3d_outNormal
				, sdw::Vec4
				, VertexTransformPass::eOutNormal
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eNormals ) );
			DeclareSsbo( c3d_outTangent
				, sdw::Vec4
				, VertexTransformPass::eOutTangent
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTangents ) );
			DeclareSsbo( c3d_outTexcoord0
				, sdw::Vec4
				, VertexTransformPass::eOutTexcoord0
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords0 ) );
			DeclareSsbo( c3d_outTexcoord1
				, sdw::Vec4
				, VertexTransformPass::eOutTexcoord1
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords1 ) );
			DeclareSsbo( c3d_outTexcoord2
				, sdw::Vec4
				, VertexTransformPass::eOutTexcoord2
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords2 ) );
			DeclareSsbo( c3d_outTexcoord3
				, sdw::Vec4
				, VertexTransformPass::eOutTexcoord3
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eTexcoords3 ) );
			DeclareSsbo( c3d_outColour
				, sdw::Vec4
				, VertexTransformPass::eOutColour
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eColours ) );
			DeclareSsboEx( c3d_outBones
				, Skin
				, VertexTransformPass::eOutBones
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eBones )
				, pipeline.submeshFlags );

#undef DeclareSsboEx
#undef DeclareSsbo

			auto size = uint32_t( device.properties.limits.nonCoherentAtomSize );
			writer.implementMainT< sdw::VoidT >( sdw::ComputeInT< sdw::VoidT >{ writer, size, 1u, 1u }
			, [&]( sdw::ComputeInT< sdw::VoidT > in )
			{
				auto index = writer.declLocale( "index"
					, in.globalInvocationID.x() );
				auto position = writer.declLocale( "position"
					, c3d_inPosition[index] );
				auto normal = writer.declLocale( "normal"
					, c3d_inNormal[index] );
				auto tangent = writer.declLocale( "tangent"
					, c3d_inTangent[index] );
				auto texcoord0 = writer.declLocale( "texcoord0"
					, c3d_inTexcoord0[index].xyz() );
				auto texcoord1 = writer.declLocale( "texcoord1"
					, c3d_inTexcoord1[index].xyz() );
				auto texcoord2 = writer.declLocale( "texcoord2"
					, c3d_inTexcoord2[index].xyz() );
				auto texcoord3 = writer.declLocale( "texcoord3"
					, c3d_inTexcoord3[index].xyz() );
				auto colour = writer.declLocale( "colour"
					, c3d_inColour[index].xyz() );
				auto morphingWeights = writer.declLocale( "morphingWeights"
					, c3d_morphingWeights[c3d_objectIDs.morphingId] );
				morphingWeights.morph( c3d_morphTargets
					, index
					, position
					, normal
					, tangent
					, texcoord0
					, texcoord1
					, texcoord2
					, texcoord3
					, colour );
				c3d_outTexcoord0[index].xyz() = texcoord0;
				c3d_outTexcoord1[index].xyz() = texcoord1;
				c3d_outTexcoord2[index].xyz() = texcoord2;
				c3d_outTexcoord3[index].xyz() = texcoord3;
				c3d_outColour[index].xyz() = colour;
				c3d_outPosition[index] = vec4( position.xyz(), 1.0_f );
				c3d_outNormal[index].xyz() = normalize( normal.xyz() );
				c3d_outTangent[index].xyz() = normalize( tangent.xyz() );
				c3d_outBones[index] = c3d_inBones[index];
			} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static uint32_t getIndex( SubmeshFlags const & submeshFlags
			, MorphFlags const & morphFlags
			, ProgramFlags const & programFlags )
		{
			remFlag( programFlags, ProgramFlag::eAllOptional );
			remFlag( programFlags, ProgramFlag::eInstantiation );
			remFlag( programFlags, ProgramFlag::eBillboards );
			remFlag( programFlags, ProgramFlag::ePicking );
			remFlag( programFlags, ProgramFlag::eSpherical );
			remFlag( programFlags, ProgramFlag::eFixedSize );
			remFlag( programFlags, ProgramFlag::eInvertNormals );
			remFlag( programFlags, ProgramFlag::eForceTexCoords );
			remFlag( programFlags, ProgramFlag::eLighting );
			remFlag( programFlags, ProgramFlag::eVsmShadowMap );
			remFlag( programFlags, ProgramFlag::eRsmShadowMap );
			constexpr auto maxSubmeshSize = castor::getBitSize( uint32_t( SubmeshFlag::eAllBase ) );
			constexpr auto maxMorphSize = castor::getBitSize( uint32_t( MorphFlag::eAllBase ) );
			constexpr auto maxProgramSize = castor::getBitSize( uint32_t( ProgramFlag::eAllBase ) );
			static_assert( maxSubmeshSize + maxMorphSize + maxProgramSize <= 32 );
			auto offset = 0u;
			uint32_t result{};
			result = uint64_t( submeshFlags ) << offset;
			offset += maxSubmeshSize;
			result |= uint64_t( morphFlags ) << offset;
			offset += maxMorphSize;
			result |= uint64_t( programFlags ) << offset;
			return result;
		}
	}

	//*********************************************************************************************

	VertexTransforming::VertexTransforming( Scene const & scene
		, RenderDevice const & device )
		: castor::OwnedBy< Scene const >{ scene }
		, m_device{ device }
	{
	}

	crg::FramePass const & VertexTransforming::createPass( crg::FrameGraph & graph
		, crg::FramePass const * previousPass )
	{
		if ( m_pass )
		{
			return m_pass->getPass();
		}

		auto & result = graph.createPass( "VertexTransforming"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< VertexTransformingPass >( framePass
					, context
					, runnableGraph
					, m_device );
				m_pass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				doProcessPending();

				return res;
			} );

		if ( previousPass )
		{
			result.addDependency( *previousPass );
		}

		return result;
	}

	void VertexTransforming::registerNode( SubmeshRenderNode const & node
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, GpuBufferOffsetT< castor3d::MorphingWeightsConfiguration > const & morphingWeights )
	{
		if ( !m_pass || !node.data.isInitialised() )
		{
			m_pending.push_back( { &node
				, &morphTargets
				, &morphingWeights } );
		}
		else
		{
			auto submeshFlags = node.getSubmeshFlags();
			auto morphFlags = node.getMorphFlags();
			auto programFlags = node.getProgramFlags();
			m_pass->registerNode( node
				, doGetPipeline( vtxtrsg::getIndex( submeshFlags, morphFlags, programFlags ) )
				, morphTargets
				, morphingWeights );
		}
	}

	TransformPipeline const & VertexTransforming::doGetPipeline( uint32_t index )
	{
		auto ires = m_pipelines.emplace( index, TransformPipeline{ index } );

		if ( ires.second )
		{
			auto & pipeline = ires.first->second;
			pipeline.shader = { VK_SHADER_STAGE_COMPUTE_BIT
				, pipeline.getName()
				, vtxtrsg::getShaderSource( m_device, pipeline ) };
			pipeline.descriptorSetLayout = vtxtrsg::createDescriptorLayout( m_device, pipeline );
			pipeline.pipelineLayout = vtxtrsg::createPipelineLayout( m_device, pipeline );
			pipeline.pipeline = vtxtrsg::createPipeline( m_device, pipeline );
			pipeline.descriptorSetPool = pipeline.descriptorSetLayout->createPool( MaxMorphingDataCount );
		}

		return ires.first->second;
	}

	void VertexTransforming::doProcessPending()
	{
		auto work = std::move( m_pending );

		for ( auto & pending : work )
		{
			registerNode( *pending.node
				, *pending.morphTargets
				, *pending.morphingWeights );
		}

		if ( !m_pending.empty() )
		{
			getOwner()->getEngine()->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					doProcessPending();
				} ) );
		}
	}

	//*********************************************************************************************
}
