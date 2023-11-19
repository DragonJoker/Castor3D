#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslCullData.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshVertex.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslTaskPayload.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/ObjectIdsUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#define SDW_PreferredMeshShadingExtension SDW_MeshShadingNV

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/ModernGraphicsWriter.hpp>

CU_ImplementSmartPtr( castor3d, DefaultRenderComponent )

namespace castor3d
{
	//*********************************************************************************************

	void DefaultRenderComponent::RenderShader::getShaderSource( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		if ( flags.isBillboard() )
		{
			doGetBillboardShaderSource( engine, flags, componentsMask, builder );
		}
		else if ( engine.hasMeshShaders() && flags.usesMesh() )
		{
			doGetModernShaderSource( engine, flags, componentsMask, builder );
		}
		else
		{
			doGetTraditionalShaderSource( engine, flags, componentsMask, builder );
		}
	}

	void DefaultRenderComponent::RenderShader::doGetBillboardShaderSource( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		sdw::VertexWriter writer{ builder };
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ engine.getPassComponentsRegister()
			, flags
			, componentsMask
			, utils };
		shader::SubmeshShaders submeshShaders{ engine.getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		C3D_Billboard( writer
			, GlobalBuffersIdx::eBillboardsData
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::BillboardSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::BillboardSurfaceT >{ writer, flags }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, passShaders, flags }
			, [&]( sdw::VertexInT< shader::BillboardSurfaceT > in
				, sdw::VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					,  shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID ) ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto passMultipliers = std::vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f ) };
				out.passMultipliers[0] = passMultipliers[0];
				out.passMultipliers[1] = passMultipliers[1];
				out.passMultipliers[2] = passMultipliers[2];
				out.passMultipliers[3] = passMultipliers[3];
				out.nodeId = nodeId;

				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( in.center, 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( in.center, 1.0_f ) ).xyz() );
				auto curToCamera = writer.declLocale( "curToCamera"
					, c3d_cameraData.getPosToCamera( curBbcenter ) );
				curToCamera.y() = 0.0_f;
				curToCamera = normalize( curToCamera );

				auto billboardData = writer.declLocale( "billboardData"
					, c3d_billboardData[nodeId - 1u] );
				auto right = writer.declLocale( "right"
					, billboardData.getCameraRight( c3d_cameraData ) );
				auto up = writer.declLocale( "up"
					, billboardData.getCameraUp( c3d_cameraData ) );
				auto width = writer.declLocale( "width"
					, billboardData.getWidth( c3d_cameraData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_cameraData ) );
				auto scaledRight = writer.declLocale( "scaledRight"
					, right * in.position.x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * in.position.y() * height );
				auto worldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				out.worldPosition = worldPos;
				out.texture0 = vec3( in.texture0, 0.0_f );

				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_cameraData.worldToPrvProj( vec4( prvBbcenter + scaledRight + scaledUp, 1.0_f ) ) );
				auto curPosition = writer.declLocale( "curPosition"
					, c3d_cameraData.worldToCurProj( worldPos ) );
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;
				out.vertexId = in.instanceIndex - in.baseInstance;
				out.computeTangentSpace( flags
					, c3d_cameraData.position()
					, worldPos.xyz()
					, curToCamera
					, vec4( up, 0.0_f )
					, right );
			} );
	}

	void DefaultRenderComponent::RenderShader::doGetTraditionalShaderSource( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		sdw::VertexWriter writer{ builder };
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ engine.getPassComponentsRegister()
			, flags
			, componentsMask
			, utils };
		shader::SubmeshShaders submeshShaders{ engine.getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		writer.implementMainT< shader::MeshVertexT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::MeshVertexT >{ writer, submeshShaders }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, passShaders, flags }
			, [&]( sdw::VertexInT< shader::MeshVertexT > in
				, sdw::VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, in
						, pipelineID
						, writer.cast< sdw::UInt >( in.drawID )
						, flags ) );
				auto curPosition = writer.declLocale( "curPosition"
					, in.position );
				auto curNormal = writer.declLocale( "curNormal"
					, in.normal );
				auto curTangent = writer.declLocale( "curTangent"
					, in.tangent );
				auto curBitangent = writer.declLocale( "curBitangent"
					, in.bitangent );
				out.texture0 = in.texture0;
				out.texture1 = in.texture1;
				out.texture2 = in.texture2;
				out.texture3 = in.texture3;
				out.colour = in.colour;
				out.nodeId = nodeId;
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				material.getPassMultipliers( flags
					, in.passMasks
					, out.passMultipliers );

				auto curMtxModel = writer.declLocale< sdw::Mat4 >( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += in.velocity;

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = c3d_cameraData.worldToPrvProj( prvMtxModel * prvPosition );
					auto worldPos = writer.declLocale( "worldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				out.worldPosition = worldPos;
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				out.vertexId = in.vertexIndex - in.baseVertex;
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;

			} );
	}

	void DefaultRenderComponent::RenderShader::doGetModernShaderSource( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		sdw::ModernGraphicsWriter writer{ builder };
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ engine.getPassComponentsRegister()
			, flags
			, componentsMask
			, utils };
		shader::SubmeshShaders submeshShaders{ engine.getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawOffset = pcb.declMember< sdw::UInt >( "drawOffset" );
		auto instanceCount = pcb.declMember< sdw::UInt >( "instanceCount" );
		pcb.end();

#define DeclareSsbo( Name, Type, Binding, Enable )\
		sdw::StorageBuffer Name##Buffer{ writer\
			, #Name + std::string{ "Buffer" }\
			, uint32_t( Binding )\
			, RenderPipeline::eMeshBuffers\
			, ast::type::MemoryLayout::eStd430\
			, Enable };\
		auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
		Name##Buffer.end()

		// Inputs
		auto c3d_meshlets = writer.declArrayStorageBuffer< shader::Meshlet >( "c3d_meshlets"
			, uint32_t( MeshBuffersIdx::eMeshlets )
			, RenderPipeline::eMeshBuffers );
		auto c3d_cullData = writer.declArrayStorageBuffer< shader::CullData >( "c3d_cullBuffer"
			, uint32_t( MeshBuffersIdx::eCullData )
			, RenderPipeline::eMeshBuffers
			, flags.usesTask() );
		DeclareSsbo( c3d_position
			, sdw::Vec4
			, MeshBuffersIdx::ePosition
			, flags.enablePosition() );
		DeclareSsbo( c3d_normal
			, sdw::Vec4
			, MeshBuffersIdx::eNormal
			, flags.enableNormal() );
		DeclareSsbo( c3d_tangent
			, sdw::Vec4
			, MeshBuffersIdx::eTangent
			, flags.enableTangentSpace() );
		DeclareSsbo( c3d_bitangent
			, sdw::Vec4
			, MeshBuffersIdx::eBitangent
			, flags.enableTangentSpace() );
		DeclareSsbo( c3d_texcoord0
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord0
			, flags.enableTexcoord0() );
		DeclareSsbo( c3d_texcoord1
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord1
			, flags.enableTexcoord1() );
		DeclareSsbo( c3d_texcoord2
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord2
			, flags.enableTexcoord2() );
		DeclareSsbo( c3d_texcoord3
			, sdw::Vec4
			, MeshBuffersIdx::eTexcoord3
			, flags.enableTexcoord3() );
		DeclareSsbo( c3d_colour
			, sdw::Vec4
			, MeshBuffersIdx::eColour
			, flags.enableColours() );
		DeclareSsbo( c3d_passMasks
			, sdw::UVec4
			, MeshBuffersIdx::ePassMasks
			, flags.enablePassMasks() );
		DeclareSsbo( c3d_velocity
			, sdw::Vec4
			, MeshBuffersIdx::eVelocity
			, flags.enableVelocity() );
		DeclareSsbo( c3d_instances
			, sdw::UVec4
			, MeshBuffersIdx::eInstances
			, flags.enableInstantiation() );

#undef DeclareSsbo

		auto meshShader = [&]( sdw::UInt const & meshletIndex
			, sdw::MeshSubgroupIn const & in
			, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > & vtxOut
			, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > & primOut )
		{
			auto laneId = writer.declLocale( "laneId"
				, in.localInvocationID );
			auto drawId = writer.declLocale( "drawId"
				, writer.cast< sdw::UInt >( in.drawID ) + drawOffset );
			auto nodeId = writer.declLocale( "nodeId"
				, shader::getNodeId( c3d_objectIdsData
					, c3d_instances
					, pipelineID
					, drawId
					, flags ) );
			auto meshlet = writer.declLocale( "meshlet"
				, c3d_meshlets[meshletIndex] );
			auto triangleCount = writer.declLocale( "triangleCount"
				, meshlet.triangleCount );
			auto vertexCount = writer.declLocale( "vertexCount"
				, meshlet.vertexCount );

			primOut.setMeshOutputCounts( vertexCount, triangleCount );
			auto indexCount = writer.declLocale( "indexCount"
				, triangleCount * 3u );

			FOR( writer, sdw::UInt, i, laneId, i < indexCount, i += 32u )
			{
				//primOut[i].primitiveID = i;
				primOut[i].primitiveIndex = uvec3( meshlet.indices[i * 3u + 0u]
					, meshlet.indices[i * 3u + 1u]
					, meshlet.indices[i * 3u + 2u] );
			}
			ROF;

			FOR( writer, sdw::UInt, i, laneId, i < vertexCount, i += 32u )
			{
				auto vertexIndex = writer.declLocale( "vertexIndex", meshlet.vertices[i] );

				auto curPosition = writer.declLocale( "curPosition"
					, c3d_position[vertexIndex] );
				auto curNormal = writer.declLocale( "curNormal"
					, c3d_normal[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, c3d_tangent[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, c3d_bitangent[vertexIndex].xyz() );
				vtxOut[i].texture0 = c3d_texcoord0[vertexIndex].xyz();
				vtxOut[i].texture1 = c3d_texcoord1[vertexIndex].xyz();
				vtxOut[i].texture2 = c3d_texcoord2[vertexIndex].xyz();
				vtxOut[i].texture3 = c3d_texcoord3[vertexIndex].xyz();
				vtxOut[i].colour = c3d_colour[vertexIndex].xyz();
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				vtxOut[i].nodeId = writer.cast< sdw::Int >( nodeId );
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				auto passMultipliers = writer.declLocaleArray( "passMultipliers"
					, 4u
					, std::vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f ) } );
				material.getPassMultipliers( flags
					, c3d_passMasks[vertexIndex]
					, passMultipliers );
				vtxOut[i].passMultipliers[0] = passMultipliers[0];
				vtxOut[i].passMultipliers[1] = passMultipliers[1];
				vtxOut[i].passMultipliers[2] = passMultipliers[2];
				vtxOut[i].passMultipliers[3] = passMultipliers[3];

				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += c3d_velocity[vertexIndex].xyz();

				if ( flags.hasWorldPosInputs() )
				{
					auto worldPos = writer.declLocale( "worldPos"
						, curPosition );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = c3d_cameraData.worldToPrvProj( prvMtxModel * prvPosition );
					auto worldPos = writer.declLocale( "worldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, worldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto worldPos = writer.getVariable< sdw::Vec4 >( "worldPos" );
				vtxOut[i].worldPosition = worldPos;
				vtxOut[i].viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				vtxOut[i].vertexId = vertexIndex;
				vtxOut[i].computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				vtxOut[i].position = curPosition;
			}
			ROF;
		};

		if ( flags.usesTask() )
		{
			bool checkCones = flags.isFrontCulled()
				&& flags.enableNormal()
				&& !flags.hasWorldPosInputs();

			auto checkVisible = writer.implementFunction< sdw::Boolean >( "checkVisible"
				, [&]( sdw::UInt nodeId
					, sdw::UInt meshletId )
				{
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );

					IF( writer, meshletId >= modelData.getMeshletCount() )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					FI;

					auto cullData = writer.declLocale( "cullData"
						, c3d_cullData[meshletId] );

					if ( flags.hasWorldPosInputs() )
					{
						auto sphereCenter = writer.declLocale( "sphereCenter"
							, cullData.sphere.xyz() );
						auto sphereRadius = writer.declLocale( "sphereRadius"
							, cullData.sphere.w() );
						auto coneNormal = writer.declLocale( "coneNormal"
							, flags.isFrontCulled() ? -cullData.cone.xyz() : cullData.cone.xyz()
							, checkCones );
						auto coneCutOff = writer.declLocale( "coneCutOff"
							, cullData.cone.w()
							, checkCones );
					}
					else
					{
						auto curMtxModel = writer.declLocale( "curMtxModel"
							, modelData.getModelMtx() );
						auto meanScale = writer.declLocale( "meanScale"
							, ( modelData.getScale().x() + modelData.getScale().y() + modelData.getScale().z() ) / 3.0f );

						auto sphereCenter = writer.declLocale( "sphereCenter"
							, ( curMtxModel * vec4( cullData.sphere.xyz(), 1.0 ) ).xyz() );
						auto sphereRadius = writer.declLocale( "sphereRadius"
							, cullData.sphere.w() * meanScale );

						auto coneNormal = writer.declLocale( "coneNormal"
							, normalize( ( curMtxModel * vec4( flags.isFrontCulled() ? -cullData.cone.xyz() : cullData.cone.xyz(), 0.0 ) ).xyz() )
							, checkCones );
						auto coneCutOff = writer.declLocale( "coneCutOff"
							, cullData.cone.w()
							, checkCones );
					}

					auto sphereCenter = writer.getVariable< sdw::Vec3 >( "sphereCenter" );
					auto sphereRadius = writer.getVariable< sdw::Float >( "sphereRadius" );

					FOR( writer, sdw::UInt, i, 0u, i < 6u, ++i )
					{
						IF( writer, dot( c3d_cameraData.getFrustumPlane( i ).xyz(), sphereCenter ) + c3d_cameraData.getFrustumPlane( i ).w() <= -sphereRadius )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						FI;
					}
					ROF;

					if ( checkCones )
					{
						auto coneNormal = writer.getVariable< sdw::Vec3 >( "coneNormal" );
						auto coneCutOff = writer.getVariable< sdw::Float >( "coneCutOff" );

						IF( writer, coneCutOff == 1.0_f )
						{
							writer.returnStmt( sdw::Boolean{ true } );
						}
						FI;

						auto posToCamera = writer.declLocale( "posToCamera"
							, c3d_cameraData.position() - sphereCenter );

						IF( writer, dot( posToCamera, coneNormal ) >= ( coneCutOff * length( posToCamera ) + sphereRadius ) )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						FI;
					}

					writer.returnStmt( sdw::Boolean{ true } );
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "meshletId" } );

			writer.implementEntryPointT< shader::PayloadT >( SDW_TaskLocalSize( 32u, 1u, 1u )
				, sdw::TaskPayloadOutT< shader::PayloadT >{ writer }
				, [&]( sdw::TaskSubgroupIn in
					, sdw::TaskPayloadOutT< shader::PayloadT > payload )
				{
					auto laneId = writer.declLocale( "laneId"
						, in.localInvocationID );
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					auto meshletId = writer.declLocale( "meshletId"
						, ( baseId * 32u + laneId ) );
					auto drawId = writer.declLocale( "drawId"
						, writer.cast< sdw::UInt >( in.drawID ) + drawOffset );
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, c3d_instances
							, pipelineID
							, drawId
							, flags ) );
					auto render = writer.declLocale( "render"
						, checkVisible( nodeId, meshletId ) );
					auto vote = writer.declLocale( "vote"
						, subgroupBallot( render ) );
					auto tasks = writer.declLocale( "tasks"
						, subgroupBallotBitCount( vote ) );
					auto idxOffset = writer.declLocale( "idxOffset"
						, subgroupBallotExclusiveBitCount( vote ) );

					IF( writer, render )
					{
						payload.meshletIndices()[idxOffset] = meshletId;
					}
					FI;

					IF( writer, laneId == 0u )
					{
						payload.dispatchMesh( SDW_TaskLocalSize( tasks, 1_u, 1_u ) );
					}
					FI;
				} );
			writer.implementEntryPointT< shader::PayloadT, shader::FragmentSurfaceT, sdw::VoidT >( SDW_MeshLocalSize( 32u, 1u, 1u )
				, sdw::TaskPayloadInT< shader::PayloadT >{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, passShaders, flags }
				, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&]( sdw::MeshSubgroupIn in
					, sdw::TaskPayloadInT< shader::PayloadT > payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > vtxOut
					, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( payload.meshletIndices()[baseId], in, vtxOut, primOut );
				} );
		}
		else
		{
			writer.implementEntryPointT< sdw::VoidT, shader::FragmentSurfaceT, sdw::VoidT >( SDW_MeshLocalSize( 32u, 1u, 1u )
				, sdw::TaskPayloadIn{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, passShaders, flags }
				, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&]( sdw::MeshSubgroupIn in
					, sdw::TaskPayloadIn payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > vtxOut
					, sdw::TrianglesMeshPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( baseId, in, vtxOut, primOut );
				} );
		}
	}

	//*********************************************************************************************

	castor::String const DefaultRenderComponent::TypeName = C3D_MakeSubmeshRenderComponentName( "default" );

	DefaultRenderComponent::DefaultRenderComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName }
	{
	}

	SubmeshComponentUPtr DefaultRenderComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< DefaultRenderComponent >( submesh );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
