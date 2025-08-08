#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
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
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/ModernGraphicsWriterEXT.hpp>
#include <ShaderWriter/ModernGraphicsWriterNV.hpp>

CU_ImplementSmartPtr( c3d, DefaultRenderComponent )

namespace c3d
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
			if ( engine.getRenderDevice()->prefersMeshShaderEXT() )
			{
				doGetModernShaderSourceEXT( engine, flags, componentsMask, builder );
			}
			else
			{
				doGetModernShaderSourceNV( engine, flags, componentsMask, builder );
			}
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
		C3D_Render( writer
			, GlobalBuffersIdx::eRender
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
		shader::InstantiatedMeshBuffers meshBuffers{ writer
			, flags
			, uint32_t( MeshBuffersIdx::ePosition )
			, uint32_t( RenderPipeline::eMeshBuffers )
			, flags.vertexStride };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawID = pcb.declMember< sdw::Int >( "drawID", !engine.getRenderDevice()->hasDrawId() );
		pcb.end();

		writer.implementMainT< sdw::VoidT, shader::FragmentSurfaceT >( sdw::VertexIn{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, submeshShaders, passShaders, flags }
			, [&engine, &writer, &meshBuffers, &drawID, &pipelineID, &c3d_billboardData, &c3d_cameraData, &c3d_renderData, &c3d_modelsData, &c3d_objectIdsData, flags]( sdw::VertexIn const & in
				, sdw::VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto bbPositions = writer.declConstantArray( "bbPositions"
					, Vector< sdw::Vec3 >{ vec3( -0.5_f, -0.5_f, 1.0_f )
					, vec3( -0.5_f, +0.5_f, 1.0_f )
					, vec3( +0.5_f, -0.5_f, 1.0_f )
					, vec3( +0.5_f, +0.5_f, 1.0_f ) } );
				auto bbTexcoords = writer.declConstantArray( "bbTexcoords"
					, Vector< sdw::Vec2 >{ vec2( 0.0_f, 0.0_f )
					, vec2( 0.0_f, 1.0_f )
					, vec2( 1.0_f, 0.0_f )
					, vec2( 1.0_f, 1.0_f ) } );

				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, pipelineID
						, writer.cast< sdw::UInt >( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto passMultipliers = Vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f )
					, vec4( 0.0_f ) };
				out.passMultipliers[0] = passMultipliers[0];
				out.passMultipliers[1] = passMultipliers[1];
				out.passMultipliers[2] = passMultipliers[2];
				out.passMultipliers[3] = passMultipliers[3];
				out.nodeId = nodeId;
				out.texture0 = vec3( bbTexcoords[in.vertexIndex - in.baseVertex], 1.0_f );

				auto center = writer.declLocale( "center"
					, meshBuffers.positions[writer.cast< sdw::UInt >( in.instanceIndex )].position );
				auto curBbcenter = writer.declLocale( "curBbcenter"
					, modelData.modelToCurWorld( vec4( center.xyz(), 1.0_f ) ).xyz() );
				auto prvBbcenter = writer.declLocale( "prvBbcenter"
					, modelData.modelToPrvWorld( vec4( center.xyz(), 1.0_f ) ).xyz() );
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
					, billboardData.getWidth( c3d_renderData ) );
				auto height = writer.declLocale( "height"
					, billboardData.getHeight( c3d_renderData ) );

				auto scaledRight = writer.declLocale( "scaledRight"
					, right * bbPositions[in.vertexIndex - in.baseVertex].x() * width );
				auto scaledUp = writer.declLocale( "scaledUp"
					, up * bbPositions[in.vertexIndex - in.baseVertex].y() * height );
				auto curWorldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				auto prvWorldPos = writer.declLocale( "worldPos"
					, vec4( ( curBbcenter + scaledRight + scaledUp ), 1.0_f ) );
				auto curViewPosition = writer.declLocale( "curViewPosition"
					, c3d_cameraData.worldToCurView( curWorldPos ) );
				auto prvViewPosition = writer.declLocale( "prvViewPosition"
					, c3d_cameraData.worldToPrvView( prvWorldPos ) );
				auto curCSPosition = writer.declLocale( "curCSPosition"
					, c3d_cameraData.viewToProj( curViewPosition ) );
				auto prvCSPosition = writer.declLocale( "prvCSPosition"
					, c3d_cameraData.viewToProj( prvViewPosition ) );

				out.curPosition = curCSPosition.xyw();
				out.prvPosition = prvCSPosition.xyw();
				out.worldPosition = curWorldPos;
				out.viewPosition = curViewPosition;
				out.vtx.position = curCSPosition;
				out.vertexId = in.instanceIndex - in.baseInstance;
				out.computeTangentSpace( flags
					, c3d_cameraData.position()
					, curWorldPos.xyz()
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
		shader::InstantiatedMeshBuffers meshBuffers{ writer
			, flags
			, uint32_t( MeshBuffersIdx::ePosition )
			, uint32_t( RenderPipeline::eMeshBuffers )
			, flags.vertexStride };

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		auto drawID = pcb.declMember< sdw::Int >( "drawID", !engine.getRenderDevice()->hasDrawId() );
		pcb.end();

		writer.implementMainT< sdw::VoidT, shader::FragmentSurfaceT >( sdw::VertexIn{ writer }
			, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, submeshShaders, passShaders, flags }
			, [&writer, &engine, &meshBuffers, &materials, &c3d_cameraData, &c3d_modelsData, &c3d_objectIdsData, &drawID, &pipelineID, &flags]( sdw::VertexIn const & in
				, sdw::VertexOutT< shader::FragmentSurfaceT > out )
			{
				auto instanceId = writer.declLocale( "instanceId"
					, writer.cast< sdw::UInt >( in.instanceIndex )
						+ writer.cast< sdw::UInt >( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) );
				auto nodeId = writer.declLocale( "nodeId"
					, shader::getNodeId( c3d_objectIdsData
						, meshBuffers.instances
						, pipelineID
						, instanceId
						, flags ) );
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				auto vertexIndex = writer.declLocale( "vertexIndex"
					, writer.cast< sdw::UInt >( in.vertexIndex ) );

				auto curPosition = writer.declLocale( "curPosition"
					, meshBuffers.positions[vertexIndex].position );
				auto curNormal = writer.declLocale( "curNormal"
					, meshBuffers.normals[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, meshBuffers.tangents[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, meshBuffers.bitangents[vertexIndex].xyz() );
				out.texture0 = meshBuffers.textures0[vertexIndex].xyz();
				out.texture1 = meshBuffers.textures1[vertexIndex].xyz();
				out.texture2 = meshBuffers.textures2[vertexIndex].xyz();
				out.texture3 = meshBuffers.textures3[vertexIndex].xyz();
				out.colour = meshBuffers.colours[vertexIndex].xyz();

				out.nodeId = nodeId;
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() ) );
				material.getPassMultipliers( flags
					, meshBuffers.passMasks[vertexIndex]
					, out.passMultipliers );

				auto curMtxModel = writer.declLocale< sdw::Mat4 >( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += meshBuffers.velocities[vertexIndex].xyz();

				if ( flags.hasWorldPosInputs() )
				{
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curPosition );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = prvMtxModel * prvPosition;
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					out.computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto curWorldPos = writer.getVariable< sdw::Vec4 >( "curWorldPos" );
				auto curViewPosition = writer.declLocale( "curViewPosition"
					, c3d_cameraData.worldToCurView( curWorldPos ) );
				auto prvViewPosition = writer.declLocale( "prvViewPosition"
					, c3d_cameraData.worldToPrvView( prvPosition ) );
				auto curCSPosition = writer.declLocale( "curCSPosition"
					, c3d_cameraData.viewToProj( curViewPosition ) );
				auto prvCSPosition = writer.declLocale( "prvCSPosition"
					, c3d_cameraData.viewToProj( prvViewPosition ) );

				out.curPosition = curCSPosition.xyw();
				out.prvPosition = prvCSPosition.xyw();
				out.worldPosition = curWorldPos;
				out.viewPosition = curViewPosition;
				out.vtx.position = curCSPosition;
				out.vertexId = in.vertexIndex - in.baseVertex;
			} );
	}

	void DefaultRenderComponent::RenderShader::doGetModernShaderSourceEXT( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		sdw::ModernGraphicsWriterEXT writer{ builder };
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
		auto drawID = pcb.declMember< sdw::UInt >( "drawID", !engine.getRenderDevice()->hasDrawId() );
		auto drawOffset = pcb.declMember< sdw::UInt >( "drawOffset" );
		auto meshletOffset = pcb.declMember< sdw::UInt >( "meshletOffset" );
		pcb.end();

		// Inputs
		auto c3d_cullData = writer.declArrayStorageBuffer< shader::CullData >( "c3d_cullBuffer"
			, uint32_t( MeshBuffersIdx::eCullData )
			, RenderPipeline::eMeshBuffers
			, flags.usesTask() );
		shader::MeshletBuffers meshlets{ writer
			, flags
			, uint32_t( MeshBuffersIdx::eMeshlets )
			, uint32_t( RenderPipeline::eMeshBuffers ) };

		auto meshShader = [&]( sdw::UInt const & meshletIndex
			, sdw::MeshSubgroupInEXT const & in
			, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
			, sdw::TrianglesMeshEXTPrimitiveListOutT< sdw::VoidT > & primOut )
		{
			auto laneId = writer.declLocale( "laneId"
				, in.localInvocationID.x() );
			auto drawId = writer.declLocale( "drawId"
				, ( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) + drawOffset );
			auto nodeId = writer.declLocale( "nodeId"
				, shader::getNodeId( c3d_objectIdsData
					, meshlets.instances
					, pipelineID
					, drawId
					, flags ) );
			auto meshlet = writer.declLocale( "meshlet"
				, meshlets.meshlets[meshletIndex] );
			auto triangleCount = writer.declLocale( "triangleCount"
				, meshlet.triangleCount() );
			auto vertexCount = writer.declLocale( "vertexCount"
				, meshlet.vertexCount() );

			primOut.setMeshOutputCounts( vertexCount, triangleCount );
			auto indexCount = writer.declLocale( "indexCount"
				, triangleCount * 3u );

			sdwFOR( writer, sdw::UInt, i, laneId, i < indexCount, i += 32u )
			{
				primOut[i].primitiveID = i;
				primOut[i].primitiveIndex = uvec3( meshlet.indices()[i * 3u + 0u]
					, meshlet.indices()[i * 3u + 1u]
					, meshlet.indices()[i * 3u + 2u] );
			}
			sdwROF

			sdwFOR( writer, sdw::UInt, i, laneId, i < vertexCount, i += 32u )
			{
				auto vertexIndex = writer.declLocale( "vertexIndex", meshlet.vertices()[i] );

				auto curPosition = writer.declLocale( "curPosition"
					, meshlets.positions[vertexIndex].position );
				auto curNormal = writer.declLocale( "curNormal"
					, meshlets.normals[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, meshlets.tangents[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, meshlets.bitangents[vertexIndex].xyz() );
				vtxOut[i].texture0 = meshlets.textures0[vertexIndex].xyz();
				vtxOut[i].texture1 = meshlets.textures1[vertexIndex].xyz();
				vtxOut[i].texture2 = meshlets.textures2[vertexIndex].xyz();
				vtxOut[i].texture3 = meshlets.textures3[vertexIndex].xyz();
				vtxOut[i].colour = meshlets.colours[vertexIndex].xyz();
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				vtxOut[i].nodeId = nodeId;
				vtxOut[i].meshletId = meshletOffset + meshlet.meshletIndex();
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() )
					, flags.enablePassMasks() );
				auto passMultipliers = writer.declLocaleArray( "passMultipliers"
					, 4u
					, Vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f ) }
					, flags.enablePassMasks() );
				material.getPassMultipliers( flags
					, meshlets.passMasks[vertexIndex]
					, passMultipliers );
				vtxOut[i].passMultipliers[0] = passMultipliers[0];
				vtxOut[i].passMultipliers[1] = passMultipliers[1];
				vtxOut[i].passMultipliers[2] = passMultipliers[2];
				vtxOut[i].passMultipliers[3] = passMultipliers[3];

				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += meshlets.velocities[vertexIndex].xyz();

				if ( flags.hasWorldPosInputs() )
				{
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curPosition );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = prvMtxModel * prvPosition;
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto curWorldPos = writer.getVariable< sdw::Vec4 >( "curWorldPos" );
				auto curViewPosition = writer.declLocale( "curViewPosition"
					, c3d_cameraData.worldToCurView( curWorldPos ) );
				auto prvViewPosition = writer.declLocale( "prvViewPosition"
					, c3d_cameraData.worldToPrvView( prvPosition ) );
				auto curCSPosition = writer.declLocale( "curCSPosition"
					, c3d_cameraData.viewToProj( curViewPosition ) );
				auto prvCSPosition = writer.declLocale( "prvCSPosition"
					, c3d_cameraData.viewToProj( prvViewPosition ) );

				vtxOut[i].curPosition = curCSPosition.xyw();
				vtxOut[i].prvPosition = prvCSPosition.xyw();
				vtxOut[i].worldPosition = curWorldPos;
				vtxOut[i].viewPosition = curViewPosition;
				vtxOut[i].position = curCSPosition;
				vtxOut[i].vertexId = vertexIndex;
			}
			sdwROF
		};

		if ( flags.usesTask() )
		{
			bool checkCones = flags.isFrontCulled()
				&& flags.enableNormal()
				&& !flags.hasWorldPosInputs();

			auto checkVisible = writer.implementFunction< sdw::Boolean >( "checkVisible"
				, [&writer, &c3d_cullData, &c3d_cameraData, &c3d_modelsData, &checkCones, &flags]( sdw::UInt const & nodeId
					, sdw::UInt const & meshletId )
				{
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );

					sdwIF( writer, meshletId >= modelData.getMeshletCount() )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					sdwFI

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

					sdwFOR( writer, sdw::UInt, i, 0u, i < 6u, ++i )
					{
						sdwIF( writer, dot( c3d_cameraData.getFrustumPlane( i ).xyz(), sphereCenter ) + c3d_cameraData.getFrustumPlane( i ).w() <= -sphereRadius )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						sdwFI
					}
					sdwROF

					if ( checkCones )
					{
						auto coneNormal = writer.getVariable< sdw::Vec3 >( "coneNormal" );
						auto coneCutOff = writer.getVariable< sdw::Float >( "coneCutOff" );

						sdwIF( writer, coneCutOff == 1.0_f )
						{
							writer.returnStmt( sdw::Boolean{ true } );
						}
						sdwFI

						auto posToCamera = writer.declLocale( "posToCamera"
							, c3d_cameraData.position() - sphereCenter );

						sdwIF( writer, dot( posToCamera, coneNormal ) >= ( coneCutOff * length( posToCamera ) + sphereRadius ) )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						sdwFI
					}

					writer.returnStmt( sdw::Boolean{ true } );
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "meshletId" } );

			writer.implementEntryPointT< shader::PayloadT >( 32u, 1u, 1u
				, sdw::TaskPayloadOutEXTT< shader::PayloadT >{ writer }
				, [&engine, &writer, &drawOffset, &meshlets, &drawID, &pipelineID, &c3d_objectIdsData, &checkVisible, &flags]( sdw::TaskSubgroupInEXT const & in
					, sdw::TaskPayloadOutEXTT< shader::PayloadT > const & payload )
				{
					auto laneId = in.localInvocationID.x();
					auto baseId = in.workGroupID.x();
					auto meshletId = writer.declLocale( "meshletId"
						, ( baseId * 32u + laneId ) );
					auto drawId = writer.declLocale( "drawId"
						, ( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) + drawOffset );
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, meshlets.instances
							, pipelineID
							, drawId
							, flags ) );
					auto render = writer.declLocale( "render"
						, checkVisible( nodeId, meshletId ) );
					auto vote = writer.declLocale( "vote"
						, subgroupBallot( render ) );

					sdwIF( writer, render )
					{
						auto idxOffset = writer.declLocale( "idxOffset"
							, subgroupBallotExclusiveBitCount( vote ) );
						payload.meshletIndices()[idxOffset] = meshletId;
					}
					sdwFI

					sdwIF( writer, laneId == 0u )
					{
						auto tasks = writer.declLocale( "tasks"
							, subgroupBallotBitCount( vote ) );
						payload.dispatchMesh( tasks, 1_u, 1_u );
					}
					sdwFI
				} );
			writer.implementEntryPointT< shader::PayloadT, shader::FragmentSurfaceT, sdw::VoidT >( 32u, 1u, 1u
				, sdw::TaskPayloadInEXTT< shader::PayloadT >{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, submeshShaders, passShaders, flags }
				, sdw::TrianglesMeshEXTPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&writer, &meshShader]( sdw::MeshSubgroupInEXT const & in
					, sdw::TaskPayloadInEXTT< shader::PayloadT > const & payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
					, sdw::TrianglesMeshEXTPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID.x() );
					meshShader( payload.meshletIndices()[baseId], in, vtxOut, primOut );
				} );
		}
		else
		{
			writer.implementEntryPointT< sdw::VoidT, shader::FragmentSurfaceT, sdw::VoidT >( 32u, 1u, 1u
				, sdw::TaskPayloadInEXT{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, submeshShaders, passShaders, flags }
				, sdw::TrianglesMeshEXTPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&writer, &meshShader]( sdw::MeshSubgroupInEXT const & in
					, sdw::TaskPayloadInEXT const &
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
					, sdw::TrianglesMeshEXTPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID.x() );
					meshShader( baseId, in, vtxOut, primOut );
				} );
		}
	}

	void DefaultRenderComponent::RenderShader::doGetModernShaderSourceNV( Engine const & engine
		, PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		sdw::ModernGraphicsWriterNV writer{ builder };
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
		auto drawID = pcb.declMember< sdw::UInt >( "drawID", !engine.getRenderDevice()->hasDrawId() );
		auto drawOffset = pcb.declMember< sdw::UInt >( "drawOffset" );
		auto meshletOffset = pcb.declMember< sdw::UInt >( "meshletOffset" );
		pcb.end();

		// Inputs
		auto c3d_cullData = writer.declArrayStorageBuffer< shader::CullData >( "c3d_cullBuffer"
			, uint32_t( MeshBuffersIdx::eCullData )
			, RenderPipeline::eMeshBuffers
			, flags.usesTask() );
		shader::MeshletBuffers meshlets{ writer
			, flags
			, uint32_t( MeshBuffersIdx::eMeshlets )
			, uint32_t( RenderPipeline::eMeshBuffers ) };

		auto meshShader = [&]( sdw::UInt const & meshletIndex
			, sdw::MeshSubgroupInNV const & in
			, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
			, sdw::TrianglesMeshNVPrimitiveListOutT< sdw::VoidT > & primOut )
		{
			auto laneId = writer.declLocale( "laneId"
				, in.localInvocationID );
			auto drawId = writer.declLocale( "drawId"
				, ( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) + drawOffset );
			auto nodeId = writer.declLocale( "nodeId"
				, shader::getNodeId( c3d_objectIdsData
					, meshlets.instances
					, pipelineID
					, drawId
					, flags ) );
			auto meshlet = writer.declLocale( "meshlet"
				, meshlets.meshlets[meshletIndex] );
			auto triangleCount = writer.declLocale( "triangleCount"
				, meshlet.triangleCount() );
			auto vertexCount = writer.declLocale( "vertexCount"
				, meshlet.vertexCount() );

			primOut.setMeshOutputCounts( vertexCount, triangleCount );
			auto indexCount = writer.declLocale( "indexCount"
				, triangleCount * 3u );

			sdwFOR( writer, sdw::UInt, i, laneId, i < indexCount, i += 32u )
			{
				primOut[i].primitiveID = i;
				primOut[i].primitiveIndex = uvec3( meshlet.indices()[i * 3u + 0u]
					, meshlet.indices()[i * 3u + 1u]
					, meshlet.indices()[i * 3u + 2u] );
			}
			sdwROF

			sdwFOR( writer, sdw::UInt, i, laneId, i < vertexCount, i += 32u )
			{
				auto vertexIndex = writer.declLocale( "vertexIndex", meshlet.vertices()[i] );

				auto curPosition = writer.declLocale( "curPosition"
					, meshlets.positions[vertexIndex].position );
				auto curNormal = writer.declLocale( "curNormal"
					, meshlets.normals[vertexIndex].xyz() );
				auto curTangent = writer.declLocale( "curTangent"
					, meshlets.tangents[vertexIndex] );
				auto curBitangent = writer.declLocale( "curBitangent"
					, meshlets.bitangents[vertexIndex].xyz() );
				vtxOut[i].texture0 = meshlets.textures0[vertexIndex].xyz();
				vtxOut[i].texture1 = meshlets.textures1[vertexIndex].xyz();
				vtxOut[i].texture2 = meshlets.textures2[vertexIndex].xyz();
				vtxOut[i].texture3 = meshlets.textures3[vertexIndex].xyz();
				vtxOut[i].colour = meshlets.colours[vertexIndex].xyz();
				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[nodeId - 1u] );
				vtxOut[i].nodeId = nodeId;
				vtxOut[i].meshletId = meshletOffset + meshlet.meshletIndex();
				auto material = writer.declLocale( "material"
					, materials.getMaterial( modelData.getMaterialId() )
					, flags.enablePassMasks() );
				auto passMultipliers = writer.declLocaleArray( "passMultipliers"
					, 4u
					, Vector< sdw::Vec4 >{ vec4( 1.0_f, 0.0_f, 0.0_f, 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f )
						, vec4( 0.0_f ) }
					, flags.enablePassMasks() );
				material.getPassMultipliers( flags
					, meshlets.passMasks[vertexIndex]
					, passMultipliers );
				vtxOut[i].passMultipliers[0] = passMultipliers[0];
				vtxOut[i].passMultipliers[1] = passMultipliers[1];
				vtxOut[i].passMultipliers[2] = passMultipliers[2];
				vtxOut[i].passMultipliers[3] = passMultipliers[3];

				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvPosition = writer.declLocale( "prvPosition"
					, curPosition );
				prvPosition.xyz() += meshlets.velocities[vertexIndex].xyz();

				if ( flags.hasWorldPosInputs() )
				{
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curPosition );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, curNormal
						, curTangent
						, curBitangent );
				}
				else
				{
					auto prvMtxModel = writer.declLocale( "prvMtxModel"
						, modelData.getPrvModelMtx( flags, curMtxModel ) );
					prvPosition = prvMtxModel * prvPosition;
					auto curWorldPos = writer.declLocale( "curWorldPos"
						, curMtxModel * curPosition );
					auto mtxNormal = writer.declLocale( "mtxNormal"
						, modelData.getNormalMtx( flags, curMtxModel ) );
					vtxOut[i].computeTangentSpace( flags
						, c3d_cameraData.position()
						, curWorldPos.xyz()
						, mtxNormal
						, curNormal
						, curTangent
						, curBitangent );
				}

				auto curWorldPos = writer.getVariable< sdw::Vec4 >( "curWorldPos" );
				auto curViewPosition = writer.declLocale( "curViewPosition"
					, c3d_cameraData.worldToCurView( curWorldPos ) );
				auto prvViewPosition = writer.declLocale( "prvViewPosition"
					, c3d_cameraData.worldToPrvView( prvPosition ) );
				auto curCSPosition = writer.declLocale( "curCSPosition"
					, c3d_cameraData.viewToProj( curViewPosition ) );
				auto prvCSPosition = writer.declLocale( "prvCSPosition"
					, c3d_cameraData.viewToProj( prvViewPosition ) );

				vtxOut[i].curPosition = curCSPosition.xyw();
				vtxOut[i].prvPosition = prvCSPosition.xyw();
				vtxOut[i].worldPosition = curWorldPos;
				vtxOut[i].viewPosition = curViewPosition;
				vtxOut[i].position = curCSPosition;
				vtxOut[i].vertexId = vertexIndex;
			}
			sdwROF
		};

		if ( flags.usesTask() )
		{
			bool checkCones = flags.isFrontCulled()
				&& flags.enableNormal()
				&& !flags.hasWorldPosInputs();

			auto checkVisible = writer.implementFunction< sdw::Boolean >( "checkVisible"
				, [&writer, &c3d_cameraData, &c3d_cullData, &c3d_modelsData, &checkCones, &flags]( sdw::UInt const & nodeId
					, sdw::UInt const & meshletId )
				{
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );

					sdwIF( writer, meshletId >= modelData.getMeshletCount() )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					sdwFI

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

					sdwFOR( writer, sdw::UInt, i, 0u, i < 6u, ++i )
					{
						sdwIF( writer, dot( c3d_cameraData.getFrustumPlane( i ).xyz(), sphereCenter ) + c3d_cameraData.getFrustumPlane( i ).w() <= -sphereRadius )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						sdwFI
					}
					sdwROF

					if ( checkCones )
					{
						auto coneNormal = writer.getVariable< sdw::Vec3 >( "coneNormal" );
						auto coneCutOff = writer.getVariable< sdw::Float >( "coneCutOff" );

						sdwIF( writer, coneCutOff == 1.0_f )
						{
							writer.returnStmt( sdw::Boolean{ true } );
						}
						sdwFI

						auto posToCamera = writer.declLocale( "posToCamera"
							, c3d_cameraData.position() - sphereCenter );

						sdwIF( writer, dot( posToCamera, coneNormal ) >= ( coneCutOff * length( posToCamera ) + sphereRadius ) )
						{
							writer.returnStmt( sdw::Boolean{ false } );
						}
						sdwFI
					}

					writer.returnStmt( sdw::Boolean{ true } );
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "meshletId" } );

			writer.implementEntryPointT< shader::PayloadT >( 32u
				, sdw::TaskPayloadOutNVT< shader::PayloadT >{ writer }
				, [&engine, &writer, &meshlets, &drawOffset, &c3d_objectIdsData, &drawID, &pipelineID, &flags, &checkVisible]( sdw::TaskSubgroupInNV const & in
					, sdw::TaskPayloadOutNVT< shader::PayloadT > const & payload )
				{
					auto const & laneId = in.localInvocationID;
					auto const & baseId = in.workGroupID;
					auto meshletId = writer.declLocale( "meshletId"
						, ( baseId * 32u + laneId ) );
					auto drawId = writer.declLocale( "drawId"
						, ( engine.getRenderDevice()->hasDrawId() ? in.drawID : drawID ) + drawOffset );
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, meshlets.instances
							, pipelineID
							, drawId
							, flags ) );
					auto render = writer.declLocale( "render"
						, checkVisible( nodeId, meshletId ) );
					auto vote = writer.declLocale( "vote"
						, subgroupBallot( render ) );

					sdwIF( writer, render )
					{
						auto idxOffset = writer.declLocale( "idxOffset"
							, subgroupBallotExclusiveBitCount( vote ) );
						payload.meshletIndices()[idxOffset] = meshletId;
					}
					sdwFI

					sdwIF( writer, laneId == 0u )
					{
						auto tasks = writer.declLocale( "tasks"
							, subgroupBallotBitCount( vote ) );
						payload.dispatchMesh( tasks );
					}
					sdwFI
				} );
			writer.implementEntryPointT< shader::PayloadT, shader::FragmentSurfaceT, sdw::VoidT >( 32u
				, sdw::TaskPayloadInNVT< shader::PayloadT >{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, submeshShaders, passShaders, flags }
				, sdw::TrianglesMeshNVPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&writer, &meshShader]( sdw::MeshSubgroupInNV const & in
					, sdw::TaskPayloadInNVT< shader::PayloadT > const & payload
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
					, sdw::TrianglesMeshNVPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( payload.meshletIndices()[baseId], in, vtxOut, primOut );
				} );
		}
		else
		{
			writer.implementEntryPointT< sdw::VoidT, shader::FragmentSurfaceT, sdw::VoidT >( 32u
				, sdw::TaskPayloadInNV{ writer }
				, sdw::MeshVertexListOutT< shader::FragmentSurfaceT >{ writer, MaxMeshletVertexCount, submeshShaders, passShaders, flags }
				, sdw::TrianglesMeshNVPrimitiveListOutT< sdw::VoidT >{ writer, MaxMeshletTriangleCount }
				, [&writer, &meshShader]( sdw::MeshSubgroupInNV const & in
					, sdw::TaskPayloadInNV const &
					, sdw::MeshVertexListOutT< shader::FragmentSurfaceT > const & vtxOut
					, sdw::TrianglesMeshNVPrimitiveListOutT< sdw::VoidT > primOut )
				{
					auto baseId = writer.declLocale( "baseId"
						, in.workGroupID );
					meshShader( baseId, in, vtxOut, primOut );
				} );
		}
	}

	//*********************************************************************************************

	String const DefaultRenderComponent::TypeName = C3D_MakeSubmeshRenderComponentName( "default" );

	DefaultRenderComponent::DefaultRenderComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName }
	{
	}

	SubmeshComponentUPtr DefaultRenderComponent::clone( Submesh & submesh )const
	{
		auto result = makeUnique< DefaultRenderComponent >( submesh );
		return ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
