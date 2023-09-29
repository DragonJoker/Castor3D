#include "Castor3D/Render/Transform/VertexTransforming.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/Transform/MeshletBoundsTransformPass.hpp"
#include "Castor3D/Render/Transform/MeshletBoundsTransformingPass.hpp"
#include "Castor3D/Render/Transform/VertexTransformPass.hpp"
#include "Castor3D/Render/Transform/VertexTransformingPass.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslCullData.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/ComputeWriter.hpp>

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementSmartPtr( castor3d, VertexTransforming )

namespace castor3d
{
	//*********************************************************************************************

#define DeclareSsbo( Name, Type, Binding, Enable )\
	sdw::StorageBuffer Name##Buffer{ writer\
		, #Name + std::string{ "Buffer" }\
		, uint32_t( Binding )\
		, 0u\
		, ast::type::MemoryLayout::eStd430\
		, Enable };\
	auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
	Name##Buffer.end()

#define DeclareSsboEx( Name, Type, Binding, Enable, Flags )\
	sdw::StorageBuffer Name##Buffer{ writer\
		, #Name + std::string{ "Buffer" }\
		, uint32_t( Binding )\
		, 0u\
		, ast::type::MemoryLayout::eStd430\
		, Enable };\
	auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable, Flags );\
	Name##Buffer.end()

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
						, checkFlag( submeshFlags, SubmeshFlag::eSkin ) );
					result->declMember( "boneIds1", ast::type::Kind::eVec4U
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eSkin ) );
					result->declMember( "boneWeights0", ast::type::Kind::eVec4F
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eSkin ) );
					result->declMember( "boneWeights1", ast::type::Kind::eVec4F
						, ast::type::NotArray
						, checkFlag( submeshFlags, SubmeshFlag::eSkin ) );
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

			bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eModelsData
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );

			if ( pipeline.morphFlags != MorphFlag::eNone )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eMorphTargets
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eMorphingWeights
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eSkin ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eSkinTransforms
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

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

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eBitangents ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInBitangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutBitangent
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

			if ( checkFlag( pipeline.submeshFlags, SubmeshFlag::eSkin ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eInSkin
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( VertexTransformPass::eOutVelocity
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );

			return device->createDescriptorSetLayout( pipeline.getName()
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device
			, BoundsTransformPipeline const & pipeline )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( MeshletBoundsTransformPass::ePositions
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );

			if ( pipeline.normals )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( MeshletBoundsTransformPass::eNormals
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) );
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( MeshletBoundsTransformPass::eMeshlets
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( MeshletBoundsTransformPass::eOutCullData
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_COMPUTE_BIT ) );
			return device->createDescriptorSetLayout( pipeline.getName()
				, std::move( bindings ) );
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			return device->createPipelineLayout( pipeline.getName() + "/PipelineLayout"
				, *pipeline.descriptorSetLayout
				, VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( castor::Point4ui ) } );
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, BoundsTransformPipeline const & pipeline )
		{
			return device->createPipelineLayout( pipeline.getName() + "/PipelineLayout"
				, *pipeline.descriptorSetLayout
				, VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, sizeof( uint32_t ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, TransformPipeline & pipeline )
		{
			// Initialise the pipeline.
			return device->createPipeline( pipeline.getName() + "/Pipeline"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, pipeline.shader )
					, *pipeline.pipelineLayout ) );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, BoundsTransformPipeline & pipeline )
		{
			// Initialise the pipeline.
			return device->createPipeline( pipeline.getName() + "/Pipeline"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, pipeline.shader )
					, *pipeline.pipelineLayout ) );
		}

		static ShaderPtr getShaderSource( RenderDevice const & device
			, TransformPipeline const & pipeline )
		{
			using namespace sdw;

			ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Common
			auto objectIDs = writer.declPushConstantsBuffer<>( "ObjectIDs" );
			auto c3d_objectIDs = objectIDs.declMember< shader::ObjectIds >( "c3d_objectIDs" );
			objectIDs.end();
			C3D_ModelsData( writer
				, VertexTransformPass::eModelsData
				, 0u );

			// Morphing
			auto c3d_morphTargets = writer.declArrayStorageBuffer< shader::MorphTargetsData >( "c3d_morphTargets"
				, { uint32_t( VertexTransformPass::eMorphTargets ), 0u }
				, pipeline.morphFlags != MorphFlag::eNone
				, pipeline.morphFlags );
			auto c3d_morphingWeights = writer.declArrayStorageBuffer< shader::MorphingWeightsData >( "c3d_morphingWeights"
				, { uint32_t( VertexTransformPass::eMorphingWeights ), 0u }
				, pipeline.morphFlags != MorphFlag::eNone && pipeline.hasMorphingWeights );

			// Skinning
			auto skinningData = SkinningUbo::declare( writer
				, uint32_t( VertexTransformPass::eSkinTransforms )
				, 0u
				, pipeline.submeshFlags );

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
			DeclareSsbo( c3d_inBitangent
				, sdw::Vec4
				, VertexTransformPass::eInBitangent
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eBitangents ) );
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
				, VertexTransformPass::eInSkin
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eSkin )
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
			DeclareSsbo( c3d_outBitangent
				, sdw::Vec4
				, VertexTransformPass::eOutBitangent
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eBitangents ) );
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
			DeclareSsbo( c3d_outVelocity
				, sdw::Vec4
				, VertexTransformPass::eOutVelocity
				, checkFlag( pipeline.submeshFlags, SubmeshFlag::eVelocity ) );

			auto size = uint32_t( device.properties.limits.nonCoherentAtomSize );
			writer.implementMainT< sdw::VoidT >( sdw::ComputeInT< sdw::VoidT >{ writer, size, 1u, 1u }
			, [&]( sdw::ComputeInT< sdw::VoidT > in )
			{
				auto index = writer.declLocale( "index"
					, in.globalInvocationID.x() );
				auto position = writer.declLocale( "position"
					, vec4( c3d_inPosition[index].xyz(), 1.0_f ) );
				auto oldPosition = writer.declLocale( "oldPosition"
					, c3d_outPosition[index] );
				auto normal = writer.declLocale( "normal"
					, c3d_inNormal[index] );
				auto tangent = writer.declLocale( "tangent"
					, c3d_inTangent[index] );
				auto bitangent = writer.declLocale( "bitangent"
					, c3d_inBitangent[index] );
				auto texcoord0 = writer.declLocale( "texture0"
					, c3d_inTexcoord0[index].xyz() );
				auto texcoord1 = writer.declLocale( "texture1"
					, c3d_inTexcoord1[index].xyz() );
				auto texcoord2 = writer.declLocale( "texture2"
					, c3d_inTexcoord2[index].xyz() );
				auto texcoord3 = writer.declLocale( "texture3"
					, c3d_inTexcoord3[index].xyz() );
				auto colour = writer.declLocale( "colour"
					, c3d_inColour[index].xyz() );

				if ( pipeline.hasMorphingWeights )
				{
					auto morphingWeights = writer.declLocale( "morphingWeights"
						, c3d_morphingWeights[c3d_objectIDs.morphingId] );
					morphingWeights.morph( c3d_morphTargets
						, index
						, position
						, normal
						, tangent
						, bitangent
						, texcoord0
						, texcoord1
						, texcoord2
						, texcoord3
						, colour );
				}
				else if ( pipeline.morphFlags != MorphFlag::eNone )
				{
					shader::MorphingWeightsData::morphNoAnim( c3d_morphTargets
						, index
						, position
						, normal
						, tangent
						, bitangent
						, texcoord0
						, texcoord1
						, texcoord2
						, texcoord3
						, colour );
				}

				c3d_outTexcoord0[index].xyz() = texcoord0;
				c3d_outTexcoord1[index].xyz() = texcoord1;
				c3d_outTexcoord2[index].xyz() = texcoord2;
				c3d_outTexcoord3[index].xyz() = texcoord3;
				c3d_outColour[index].xyz() = colour;

				auto skin = writer.declLocale( "skin"
					, c3d_inBones[index] );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[c3d_objectIDs.nodeId] );
				auto curMtxModel = writer.declLocale< Mat4 >( "curMtxModel"
					, modelData.getCurModelMtx( skinningData
						, c3d_objectIDs.skinningId
						, skin.boneIds0
						, skin.boneIds1
						, skin.boneWeights0
						, skin.boneWeights1 ) );
				position = curMtxModel * position;
				c3d_outPosition[index] = position;
				c3d_outVelocity[index].xyz() = oldPosition.xyz() - position.xyz();

				auto curMtxNormal = writer.declLocale< Mat3 >( "curMtxNormal"
					, modelData.getNormalMtx( checkFlag( pipeline.submeshFlags, SubmeshFlag::eSkin ), curMtxModel ) );
				c3d_outNormal[index].xyz() = normalize( curMtxNormal * normal.xyz() );
				c3d_outTangent[index] = vec4( normalize( curMtxNormal * tangent.xyz() ), tangent.w() );
				c3d_outBitangent[index].xyz() = normalize( curMtxNormal * bitangent.xyz() );
			} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getShaderSource( RenderDevice const & device
			, BoundsTransformPipeline const & pipeline )
		{
			using namespace sdw;
			ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };
			bool computeCones = pipeline.normals;

			// Inputs
			auto meshletsData = writer.declPushConstantsBuffer<>( "meshletsData" );
			auto c3d_meshletsCount = meshletsData.declMember< sdw::UInt >( "c3d_meshletsCount" );
			meshletsData.end();
			DeclareSsbo( c3d_positions
				, sdw::Vec4
				, MeshletBoundsTransformPass::ePositions
				, true );
			DeclareSsbo( c3d_normals
				, sdw::Vec4
				, MeshletBoundsTransformPass::eNormals
				, computeCones );
			auto c3d_meshlets = writer.declArrayStorageBuffer< shader::Meshlet >( "c3d_meshlets"
				, MeshletBoundsTransformPass::eMeshlets
				, 0u );

			// Inputs
			auto c3d_outCullData = writer.declArrayStorageBuffer< shader::CullData >( "c3d_outCullData"
				, MeshletBoundsTransformPass::eOutCullData
				, 0u );

			auto computeSphere = writer.implementFunction< sdw::Vec4 >( "computeSphere"
				, [&]( sdw::Array< sdw::Vec3 > const & points
					, sdw::UInt const & count )
				{
					// find extremum points along all 3 axes; for each axis we get a pair of points with min/max coordinates
					auto pmin = writer.declLocale( "pmin"
						, uvec3( 0_u, 0u, 0u ) );
					auto pmax = writer.declLocale( "pmax"
						, uvec3( 0_u, 0u, 0u ) );

					FOR( writer, sdw::UInt, i, 0u, i < count, ++i )
					{
						auto p = writer.declLocale( "p"
							, points[i] );

						for( uint32_t axis = 0u; axis < 3u; ++axis )
						{
							pmin[axis] = writer.ternary( p[axis] < points[pmin[axis]][axis], i, pmin[axis] );
							pmax[axis] = writer.ternary( p[axis] > points[pmax[axis]][axis], i, pmax[axis] );
						}
					}
					ROF;

					// find the pair of points with largest distance
					auto paxisd2 = writer.declLocale( "paxisd2"
						, 0.0_f );
					auto paxis = writer.declLocale( "paxis"
						, 0_i );
					auto d2 = writer.declLocale( "d2"
						, 0.0_f );

					for ( int axis = 0; axis < 3; ++axis )
					{
						auto p1 = points[pmin[axis]];
						auto p2 = points[pmax[axis]];

						d2 = ( p2[0] - p1[0] ) * ( p2[0] - p1[0] )
							+ ( p2[1] - p1[1] ) * ( p2[1] - p1[1] )
							+ ( p2[2] - p1[2] ) * ( p2[2] - p1[2] );

						IF( writer, d2 > paxisd2 )
						{
							paxisd2 = d2;
							paxis = axis;
						}
						FI;
					}

					// use the longest segment as the initial sphere diameter
					auto p1 = points[pmin[paxis]];
					auto p2 = points[pmax[paxis]];

					auto center = writer.declLocale( "center"
						, ( p1 + p2 ) / vec3( 2.0_f ) );
					auto radius = writer.declLocale( "radius"
						, sqrt( paxisd2 ) / 2.0f );

					// iteratively adjust the sphere up until all points fit
					FOR( writer, sdw::UInt, i, 0u, i < count, ++i )
					{
						auto p = points[i];
						d2 = ( p[0] - center[0] ) * ( p[0] - center[0] )
							+ ( p[1] - center[1] ) * ( p[1] - center[1] )
							+ ( p[2] - center[2] ) * ( p[2] - center[2] );

						IF( writer, d2 > radius * radius )
						{
							auto d = writer.declLocale( "d"
								, sqrt( d2 ) );

							auto k = writer.declLocale( "k"
								, vec3( 0.5f + ( radius / d ) / 2.0f ) );

							center = center * k + p * ( vec3( 1.0_f ) - k );
							radius = ( radius + d ) / 2.0f;
						}
						FI;
					}
					ROF;

					writer.returnStmt( vec4( center, radius ) );
				}
				, sdw::InVec3Array{ writer, "normals", MaxMeshletVertexCount }
				, sdw::InUInt{ writer, "count" } );

			auto size = uint32_t( device.properties.limits.nonCoherentAtomSize );
			writer.implementMainT< sdw::VoidT >( sdw::ComputeInT< sdw::VoidT >{ writer, size, 1u, 1u }
				, [&]( sdw::ComputeInT< sdw::VoidT > in )
				{
					auto meshletId = writer.declLocale( "meshletId"
						, in.globalInvocationID.x() );
					auto meshlet = writer.declLocale( "meshlet"
						, c3d_meshlets[meshletId] );
					auto minPos = writer.declLocale( "minPos"
						, c3d_positions[meshlet.vertices[0]].xyz() );
					auto maxPos = writer.declLocale( "maxPos"
						, minPos );
					auto normals = writer.declLocaleArray< sdw::Vec3 >( "normals"
						, MaxMeshletVertexCount
						, computeCones );
					normals[0] = c3d_normals[meshlet.vertices[0]].xyz();

					FOR( writer, sdw::UInt, i, 1u, i < meshlet.vertexCount, ++i )
					{
						auto point = writer.declLocale( "point"
							, c3d_positions[meshlet.vertices[i]].xyz() );
						minPos = min( minPos, point );
						maxPos = max( maxPos, point );
						normals[i] = c3d_normals[meshlet.vertices[i]].xyz();
					}
					ROF;

					auto center = writer.declLocale( "center"
						, minPos + ( ( maxPos - minPos ) / vec3( 2.0_f ) ) );
					auto radius = writer.declLocale( "radius"
						, sdw::distance( center, maxPos ) );
					c3d_outCullData[meshletId].sphere = vec4( center, radius );

					if ( computeCones )
					{
						auto sphere = writer.declLocale( "sphere"
							, computeSphere( normals, meshlet.vertexCount ) );
						auto axis = writer.declLocale( "axis"
							, normalize( sphere.xyz() ) );
						// compute a tight cone around all normals, mindp = cos(angle/2)
						auto mindp = writer.declLocale( "mindp"
							, 1.0_f );

						FOR( writer, sdw::UInt, i, 0u, i < meshlet.vertexCount, ++i )
						{
							auto dp = writer.declLocale( "dp"
								, dot( normals[i], axis ) );
							mindp = min( dp, mindp );
						}
						ROF;

						IF( writer, mindp <= 0.1_f )
						{
							// degenerate cluster, normal cone is larger than a hemisphere => trivial accept
							// note that if mindp is positive but close to 0, the triangle intersection code below gets less stable
							// we arbitrarily decide that if a normal cone is ~168 degrees wide or more, the cone isn't useful
							c3d_outCullData[meshletId].cone = vec4( axis, 1.0_f );
						}
						ELSE
						{
							// cos(a) for normal cone is mindp; we need to add 90 degrees on both sides and invert the cone
							// which gives us -cos(a+90) = -(-sin(a)) = sin(a) = sqrt(1 - cos^2(a))
							c3d_outCullData[meshletId].cone = vec4( axis
								, sqrt( 1.0_f - mindp * mindp ) );
						}
						FI;
					}
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	VertexTransforming::VertexTransforming( Scene const & scene
		, RenderDevice const & device )
		: castor::OwnedBy< Scene const >{ scene }
		, m_device{ device }
	{
	}

	crg::FramePass const & VertexTransforming::createPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass )
	{
		if ( m_boundsPass )
		{
			return m_boundsPass->getPass();
		}

		auto & pass = graph.createPass( "Transform/VertexTransforming"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto & modelsBuffer = getOwner()->getModelBuffer();
				auto res = std::make_unique< VertexTransformingPass >( framePass
					, context
					, runnableGraph
					, m_device
					, modelsBuffer );
				m_pass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );

				return res;
			} );

		if ( previousPass )
		{
			pass.addDependency( *previousPass );
		}

		auto & result = graph.createPass( "Transform/MeshletBoundsTransforming"
			, [this]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< MeshletBoundsTransformingPass >( framePass
					, context
					, runnableGraph
					, m_device );
				m_boundsPass = res.get();
				m_device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				doProcessPending();

				return res;
			} );
		result.addDependency( pass );
		return result;
	}

	void VertexTransforming::registerNode( SubmeshRenderNode const & node
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, GpuBufferOffsetT< castor3d::MorphingWeightsConfiguration > const & morphingWeights
		, GpuBufferOffsetT< SkinningTransformsConfiguration > const & skinTransforms )
	{
		if ( !m_pass || !node.data.isInitialised() )
		{
			m_pending.push_back( { &node
				, &morphTargets
				, &morphingWeights
				, &skinTransforms } );
		}
		else
		{
			static GpuBufferOffsetT< castor3d::MorphingWeightsConfiguration > const dummy{};
			auto & pipeline = doGetPipeline( TransformPipeline::getIndex( node.getSubmeshFlags()
				, node.getMorphFlags()
				, node.getProgramFlags()
				, bool( morphingWeights ) && node.data.isAnimated() ) );
			m_pass->registerNode( node
				, pipeline
				, morphTargets
				, *( node.data.isAnimated()
					? &morphingWeights
					: &dummy )
				, skinTransforms );

			if ( pipeline.meshletsBounds )
			{
				m_boundsPass->registerNode( node
					, *m_boundsPipelines[checkFlag( pipeline.submeshFlags, SubmeshFlag::eNormals ) ? 1u : 0u] );
			}
		}
	}

	TransformPipeline const & VertexTransforming::doGetPipeline( uint32_t index )
	{
		auto ires = m_pipelines.emplace( index, TransformPipeline{ index } );
		bool normals = checkFlag( ires.first->second.submeshFlags, SubmeshFlag::eNormals );

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

		if ( ires.first->second.meshletsBounds
			&& !m_boundsPipelines[normals ? 1u : 0u] )
		{
			m_boundsPipelines[normals ? 1u : 0u] = std::make_unique< BoundsTransformPipeline >( normals );
			auto & pipeline = *m_boundsPipelines[normals ? 1u : 0u];
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
				, *pending.morphingWeights
				, *pending.skinTransforms );
		}

		if ( !m_pending.empty() )
		{
			getOwner()->getEngine()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
				, [this]()
				{
					doProcessPending();
				} ) );
		}
	}

	//*********************************************************************************************
}
