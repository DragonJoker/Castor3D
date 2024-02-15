#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/QueueRenderNodes.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshlet.hpp"
#include "Castor3D/Shader/Shaders/GlslMeshVertex.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"
#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace visres
	{
		static constexpr bool useCompute{ false };

		enum Sets : uint32_t
		{
			eInOuts,
			eVtx,
			eTex,
		};

		enum InOutBindings : uint32_t
		{
			eCamera,
			eScene,
			eModels,
			eBillboards,
			eMaterials,
			eSssProfiles,
			eTexConfigs,
			eTexAnims,
			eInData,
			eInOutDiffuse,
			eMapBrdf,
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
			eOutResult,
			eOutScattering,
			eCount,
		};

		enum VtxBindings : uint32_t
		{
			eInMeshlets,
			eInIndices,
			eInPosition,
			eInNormal,
			eInTangent,
			eInBitangent,
			eInTexcoord0,
			eInTexcoord1,
			eInTexcoord2,
			eInTexcoord3,
			eInColour,
			eInPassMasks,
			eInVelocity,
		};

		enum TexBindings : uint32_t
		{
			eTextures,
		};

		template< bool ComputeT >
		struct ShaderWriter;

		static uint32_t constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
		static uint32_t constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;
		static uint32_t constexpr maxPrimitiveIDSize = uint32_t( castor::getBitSize( MaxMeshletTriangleCount ) );
		static uint32_t constexpr maxPrimitiveIDMask = ( 0x000000001u << maxPrimitiveIDSize ) - 1u;
		using ShadeFunc = sdw::Function< sdw::Boolean
			, sdw::InIVec2
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InUInt
			, sdw::OutVec4
			, sdw::InOutVec4
			, sdw::OutVec4 >;

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::TraditionalGraphicsWriter;

			static void implementMain( Type & writer
				, PipelineFlags const & flags
				, bool isMeshShading
				, bool isDeferredLighting
				, bool outputScattering
				, uint32_t stride
				, sdw::RUImage2DRg32 const & c3d_imgData
				, sdw::RWImage2DRgba32 const & c3d_imgDiffuse
				, sdw::UInt const &
				, sdw::UInt const & billboardNodeId
				, ShadeFunc const & shade )
			{
				auto c3d_imgOutResult = writer.declOutput< sdw::Vec4 >( "c3d_imgOutResult", sdw::EntryPoint::eFragment, 0u );
				auto c3d_imgOutScattering = writer.declOutput< sdw::Vec4 >( "c3d_imgOutScattering", sdw::EntryPoint::eFragment, 1u, outputScattering );

				writer.implementEntryPoint( [&writer]( sdw::VertexIn const & in
					, sdw::VertexOut out )
					{
						auto position = writer.declLocale( "position"
							, vec2( ( in.vertexIndex << 1 ) & 2, in.vertexIndex & 2 ) );
						out.vtx.position = vec4( position * 2.0_f - 1.0_f, 0.0_f, 1.0_f );
					} );

				writer.implementEntryPoint( [&]( sdw::FragmentIn const & in
					, sdw::FragmentOut const & )
					{
						auto pos = ivec2( in.fragCoord.xy() );
						auto indata = writer.declLocale( "indata"
							, c3d_imgData.load( pos ) );
						auto nodePipelineId = writer.declLocale( "nodePipelineId"
							, indata.x() );
						auto curNodeId = writer.declLocale( "curNodeId"
							, nodePipelineId >> maxPipelinesSize );
						auto curPipelineId = writer.declLocale( "curPipelineId"
							, nodePipelineId & maxPipelinesMask );
						auto meshletId = writer.declLocale( "meshletId"
							, isMeshShading ? indata.y() >> maxPrimitiveIDSize : 0_u );
						auto primitiveId = writer.declLocale( "primitiveId"
							, isMeshShading ? indata.y() & maxPrimitiveIDMask : indata.y() );
						auto result = writer.declLocale( "result", vec4( 0.0_f ) );
						auto scattering = writer.declLocale( "scattering", vec4( 0.0_f ) );
						auto diffuse = writer.declLocale( "diffuse"
							, ( ( isDeferredLighting && flags.pass.hasDeferredDiffuseLightingFlag )
								? c3d_imgDiffuse.load( pos )
								: vec4( 0.0_f ) ) );

						IF( writer, ( stride != 0u ? ( curNodeId != billboardNodeId ) : curNodeId == 0_u )
							|| !shade( pos, curNodeId, curPipelineId, primitiveId, meshletId, result, diffuse, scattering ) )
						{
							writer.demote();
						}
						FI

						c3d_imgOutResult = result;
						c3d_imgOutScattering = scattering;

						if ( flags.pass.hasDeferredDiffuseLightingFlag
							&& !isDeferredLighting )
						{
							c3d_imgDiffuse.store( pos, diffuse );
						}
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Type = sdw::ComputeWriter;

			static void implementMain( Type & writer
				, PipelineFlags const & flags
				, bool isMeshShading
				, bool isDeferredLighting
				, bool outputScattering
				, uint32_t stride
				, sdw::RUImage2DRg32 const & c3d_imgData
				, sdw::RWImage2DRgba32 const & c3d_imgDiffuse
				, sdw::UInt const & pipelineId
				, sdw::UInt const & billboardNodeId
				, ShadeFunc const & shade )
			{
				auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", InOutBindings::eMaterialsCounts, Sets::eInOuts );
				auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
				MaterialsCounts.end();

				auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", InOutBindings::eMaterialsStarts, Sets::eInOuts );
				auto materialsStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
				MaterialsStarts.end();

				auto PixelsXY = writer.declStorageBuffer<>( "PixelsXY", InOutBindings::ePixelsXY, Sets::eInOuts );
				auto pixelsXY = PixelsXY.declMemberArray< sdw::UVec2 >( "pixelsXY" );
				PixelsXY.end();

				auto c3d_imgOutResult = writer.declStorageImg< sdw::WImage2DRgba32 >( "c3d_imgOutResult", uint32_t( InOutBindings::eOutResult ), Sets::eInOuts );
				auto c3d_imgOutScattering = writer.declStorageImg< sdw::WImage2DRgba32 >( "c3d_imgOutScattering", uint32_t( InOutBindings::eOutScattering ), Sets::eInOuts, outputScattering );

				writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 4u, 4u, 1u }
					, [&]( sdw::ComputeIn const & in )
					{
						auto pos = in.globalInvocationID.xy();
						auto index = pos.y() * ( in.numWorkGroups.x() * in.workGroupSize.x() ) + pos.x();
						auto materialsStart = writer.declLocale( "materialsStart"
							, materialsStarts[pipelineId] );

						IF( writer, index < materialsCounts[pipelineId] )
						{
							auto pixelIndex = writer.declLocale( "pixelIndex"
								, materialsStart + index );
							auto pixel = writer.declLocale( "pixel"
								, pixelsXY[pixelIndex] );
							auto ipixel = writer.declLocale( "ipixel"
								, ivec2( pixel ) );
							auto indata = writer.declLocale( "indata"
								, c3d_imgData.load( ipixel ) );
							auto nodePipelineId = writer.declLocale( "nodePipelineId"
								, indata.x() );
							auto nodeId = writer.declLocale( "nodeId"
								, nodePipelineId >> maxPipelinesSize );
							auto meshletId = writer.declLocale( "meshletId"
								, isMeshShading ? indata.y() >> maxPrimitiveIDSize : 0_u );
							auto primitiveId = writer.declLocale( "primitiveId"
								, isMeshShading ? indata.y() & maxPrimitiveIDMask : indata.y() );
							auto result = writer.declLocale( "result", vec4( 0.0_f ) );
							auto scattering = writer.declLocale( "scattering", vec4( 0.0_f ) );
							auto diffuse = writer.declLocale( "diffuse"
								, ( ( isDeferredLighting && flags.pass.hasDeferredDiffuseLightingFlag )
									? c3d_imgDiffuse.load( ipixel )
									: vec4( 0.0_f ) ) );

							IF( writer, ( stride != 0u ? ( nodeId == billboardNodeId ) : nodeId != 0_u )
								&& shade( ipixel, nodeId, pipelineId, primitiveId, meshletId, result, diffuse, scattering ) )
							{
								c3d_imgOutResult.store( ipixel, result );
								c3d_imgOutScattering.store( ipixel, scattering );

								if ( flags.pass.hasDeferredDiffuseLightingFlag
									&& !isDeferredLighting )
								{
									c3d_imgDiffuse.store( ipixel, diffuse );
								}
							}
							FI
						}
						FI
					} );
			}
		};

		struct BarycentricFullDerivatives;
		Writer_Parameter( BarycentricFullDerivatives );

		struct BarycentricFullDerivatives
			: public sdw::StructInstanceHelperT< "C3D_BarycentricFullDerivatives"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "lambda" >
			, sdw::Vec3Field< "dx" >
			, sdw::Vec3Field< "dy" > >
		{
			BarycentricFullDerivatives( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
			{
			}

			auto lambda()const { return getMember< "lambda" >(); }
			auto dx()const { return getMember< "dx" >(); }
			auto dy()const { return getMember< "dy" >(); }

			sdw::Float interpolate( sdw::Vec3 const & attributes )const
			{
				return dot( attributes, lambda() );
			}

			sdw::Vec3 interpolate( sdw::Vec3 const & v0
				, sdw::Vec3 const & v1
				, sdw::Vec3 const & v2 )const
			{
				return vec3( interpolate( vec3( v0.x(), v1.x(), v2.x() ) )
					, interpolate( vec3( v0.y(), v1.y(), v2.y() ) )
					, interpolate( vec3( v0.z(), v1.z(), v2.z() ) ) );
			}

			sdw::Vec4 interpolate( sdw::Vec4 const & v0
				, sdw::Vec4 const & v1
				, sdw::Vec4 const & v2 )const
			{
				return vec4( interpolate( vec3( v0.x(), v1.x(), v2.x() ) )
					, interpolate( vec3( v0.y(), v1.y(), v2.y() ) )
					, interpolate( vec3( v0.z(), v1.z(), v2.z() ) )
					, interpolate( vec3( v0.w(), v1.w(), v2.w() ) ) );
			}

			sdw::Vec3 computeGradient( sdw::Vec3 const & mergedV )const
			{
				return vec3( dot( mergedV, lambda() )
					, dot( mergedV, dx() )
					, dot( mergedV, dy() ) );
			}

			shader::DerivTex computeGradient( sdw::Vec3 const & pv0tex
				, sdw::Vec3 const & pv1tex
				, sdw::Vec3 const & pv2tex )
			{
				if ( !m_computeGradient )
				{
					auto & writer = *getWriter();
					m_computeGradient = writer.implementFunction< shader::DerivTex >( "computeGradient"
						, [&writer]( BarycentricFullDerivatives const & derivatives
							, sdw::Vec2 const & v0tex
							, sdw::Vec2 const & v1tex
							, sdw::Vec2 const & v2tex )
						{
							auto mergedX = writer.declLocale( "merged0X"
								, vec3( v0tex.x(), v1tex.x(), v2tex.x() ) );
							auto mergedY = writer.declLocale( "merged0Y"
								, vec3( v0tex.y(), v1tex.y(), v2tex.y() ) );
							auto resultsX = writer.declLocale( "results0X"
								, derivatives.computeGradient( mergedX ) );
							auto resultsY = writer.declLocale( "results0Y"
								, derivatives.computeGradient( mergedY ) );
							auto result = writer.declLocale< shader::DerivTex >( "result" );
							result.uv() = vec2( resultsX.x(), resultsY.x() );
							result.dPdx() = vec2( resultsX.y(), resultsY.y() );
							result.dPdy() = vec2( resultsX.z(), resultsY.z() );
							writer.returnStmt( result );
						}
						, InBarycentricFullDerivatives{ writer, "derivatives" }
						, sdw::InVec2{ writer, "v0tex" }
						, sdw::InVec2{ writer, "v1tex" }
						, sdw::InVec2{ writer, "v2tex" } );
				}

				return m_computeGradient( *this, pv0tex.xy(), pv1tex.xy(), pv2tex.xy() );
			}

		private:
			sdw::Function< shader::DerivTex
				, InBarycentricFullDerivatives
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2 > m_computeGradient;
		};

		struct Position
			: public sdw::StructInstance
		{
			Position( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, castor::move( expr ), enabled }
				, position{ getMember< sdw::Vec4 >( "position" ) }
				, fill{ getMemberArray< sdw::Vec4 >( "fill", true ) }
			{
			}

			SDW_DeclStructInstance( , Position );

			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
				, uint32_t stride )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "C3D_VisPosition" );

				if ( result->empty() )
				{
					result->declMember( "position"
						, sdw::type::Kind::eVec4F
						, sdw::type::NotArray );
					auto fillCount = stride == 0u
						? 0u
						: uint32_t( ( stride / sizeof( castor::Point4f ) ) - 1u );
					result->declMember( "fill"
						, sdw::type::Kind::eVec4F
						, fillCount
						, fillCount > 0u );
				}

				return result;
			}

		public:
			sdw::Vec4 position;
			sdw::Array< sdw::Vec4 > fill;
		};

		struct VisibilityHelpers
		{
			explicit VisibilityHelpers( sdw::ShaderWriter & writer
				, shader::PassShaders & passShaders
				, shader::SubmeshShaders & submeshShaders
				, PipelineFlags const & flags
				, uint32_t stride
				, bool meshlets )
				: m_writer{ writer }
				, m_passShaders{ passShaders }
				, m_submeshShaders{ submeshShaders }
				, m_flags{ flags }
				, m_stride{ stride }
				, m_meshlets{ meshlets }
				, m_meshBuffers{ writer
					, flags
					, uint32_t( VtxBindings::eInMeshlets )
					, uint32_t( Sets::eVtx )
					, m_stride
					, meshlets }
			{
			}

			BarycentricFullDerivatives calcFullBarycentric( sdw::Vec4 const & ppt0
				, sdw::Vec4 const & ppt1
				, sdw::Vec4 const & ppt2
				, sdw::Vec2 const & ppixelNdc
				, sdw::Vec2 const & pwinSize )
			{
				if ( !m_calcFullBarycentric )
				{
					m_calcFullBarycentric = m_writer.implementFunction< BarycentricFullDerivatives >( "calcFullBarycentric"
						, [this]( sdw::Vec4 const & pt0
							, sdw::Vec4 const & pt1
							, sdw::Vec4 const & pt2
							, sdw::Vec2 const & pixelNdc
							, sdw::Vec2 const & winSize )
						{
							auto result = m_writer.declLocale< BarycentricFullDerivatives >( "result" );
							result.dx() = vec3( 0.0_f );
							result.dy() = vec3( 0.0_f );
							result.lambda() = vec3( 0.0_f );
							auto w = m_writer.declLocale( "w"
								, vec3( pt0.w(), pt1.w(), pt2.w() ) );

							IF( m_writer, !any( w == vec3( 0.0_f ) ) )
							{
								auto invW = m_writer.declLocale( "invW"
									, vec3( 1.0_f ) / w );

								auto ndc0 = m_writer.declLocale( "ndc0"
									, pt0.xy() * invW.x() );
								auto ndc1 = m_writer.declLocale( "ndc1"
									, pt1.xy() * invW.y() );
								auto ndc2 = m_writer.declLocale( "ndc2"
									, pt2.xy() * invW.z() );

								auto det = m_writer.declLocale( "det"
									, determinant( mat2( ndc2 - ndc1, ndc0 - ndc1 ) ) );

								IF( m_writer, det != 0.0_f )
								{
									auto invDet = m_writer.declLocale( "invDet"
										, 1.0_f / det );
									result.dx() = vec3( ndc1.y() - ndc2.y(), ndc2.y() - ndc0.y(), ndc0.y() - ndc1.y() ) * invDet * invW;
									result.dy() = vec3( ndc2.x() - ndc1.x(), ndc0.x() - ndc2.x(), ndc1.x() - ndc0.x() ) * invDet * invW;
									auto ddxSum = m_writer.declLocale( "ddxSum"
										, dot( result.dx(), vec3( 1.0_f ) ) );
									auto ddySum = m_writer.declLocale( "ddySum"
										, dot( result.dy(), vec3( 1.0_f ) ) );

									auto deltaVec = m_writer.declLocale( "deltaVec"
										, pixelNdc - ndc0 );
									auto interpInvW = m_writer.declLocale( "interpInvW"
										, invW.x() + deltaVec.x() * ddxSum + deltaVec.y() * ddySum );
									auto interpW = m_writer.declLocale( "interpW"
										, 1.0_f / interpInvW );

									result.lambda() = vec3( interpW * ( invW[0] + deltaVec.x() * result.dx().x() + deltaVec.y() * result.dy().x() )
										, interpW * ( 0.0_f + deltaVec.x() * result.dx().y() + deltaVec.y() * result.dy().y() )
										, interpW * ( 0.0_f + deltaVec.x() * result.dx().z() + deltaVec.y() * result.dy().z() ) );

									result.dx() *= ( 2.0_f / winSize.x() );
									result.dy() *= ( 2.0_f / winSize.y() );
									ddxSum *= ( 2.0_f / winSize.x() );
									ddySum *= ( 2.0_f / winSize.y() );

									auto interpW_ddx = m_writer.declLocale( "interpW_ddx"
										, 1.0_f / ( interpInvW + ddxSum ) );
									auto interpW_ddy = m_writer.declLocale( "interpW_ddy"
										, 1.0_f / ( interpInvW + ddySum ) );

									result.dx() = interpW_ddx * ( result.lambda() * interpInvW + result.dx() ) - result.lambda();
									result.dy() = interpW_ddy * ( result.lambda() * interpInvW + result.dy() ) - result.lambda();
								}
								FI
							}
							FI

							m_writer.returnStmt( result );
						}
						, sdw::InVec4{ m_writer, "pt0" }
						, sdw::InVec4{ m_writer, "pt1" }
						, sdw::InVec4{ m_writer, "pt2" }
						, sdw::InVec2{ m_writer, "pixelNdc" }
						, sdw::InVec2{ m_writer, "winSize" } );
				}

				return m_calcFullBarycentric( ppt0, ppt1, ppt2, ppixelNdc, pwinSize );
			}

			void loadVertices( sdw::UInt const & pnodeId
				, sdw::UInt const & pprimitiveId
				, sdw::UInt const & pmeshletId
				, shader::ModelData const & pmodelData
				, shader::MeshVertex const & pv0
				, shader::MeshVertex const & pv1
				, shader::MeshVertex const & pv2 )
			{
				if ( !m_loadVertices )
				{
					m_loadVertices = m_writer.implementFunction< sdw::Void >( "loadVertices"
						, [&]( sdw::UInt const & /*nodeId*/
							, sdw::UInt const & primitiveId
							, sdw::UInt const & meshletId
							, shader::ModelData const & modelData
							, shader::MeshVertex const & v0
							, shader::MeshVertex const & v1
							, shader::MeshVertex const & v2 )
						{
							auto loadVertex = m_writer.implementFunction< sdw::Void >( "loadVertex"
								, [&]( sdw::UInt const & vertexId
									, shader::MeshVertex result )
								{
									result.position = m_meshBuffers.positions[vertexId].position;
									result.normal = m_meshBuffers.normals[vertexId].xyz();
									result.tangent = m_meshBuffers.tangents[vertexId];
									result.bitangent = m_meshBuffers.bitangents[vertexId].xyz();
									result.texture0 = m_meshBuffers.textures0[vertexId].xyz();
									result.texture1 = m_meshBuffers.textures1[vertexId].xyz();
									result.texture2 = m_meshBuffers.textures2[vertexId].xyz();
									result.texture3 = m_meshBuffers.textures3[vertexId].xyz();
									result.colour = m_meshBuffers.colours[vertexId].xyz();
									result.passMasks = m_meshBuffers.passMasks[vertexId];
									result.velocity = m_meshBuffers.velocities[vertexId].xyz();
								}
								, sdw::InUInt{ m_writer, "vertexId" }
								, shader::OutMeshVertex{ m_writer, "result", m_submeshShaders } );

							if ( m_meshlets )
							{
								auto meshlet = m_writer.declLocale( "meshlet"
									, m_meshBuffers.meshlets[meshletId] );
								auto baseIndex = m_writer.declLocale( "baseIndex"
									, primitiveId * 3u );
								auto indices = m_writer.declLocale( "indices"
									, uvec3( meshlet.vertices()[m_writer.cast< sdw::UInt >( meshlet.indices()[baseIndex + 0u] )]
										, meshlet.vertices()[m_writer.cast< sdw::UInt >( meshlet.indices()[baseIndex + 1u] )]
										, meshlet.vertices()[m_writer.cast< sdw::UInt >( meshlet.indices()[baseIndex + 2u] )] ) );
								auto baseVertex = m_writer.declLocale( "baseVertex"
									, modelData.getVertexOffset() );
								loadVertex( baseVertex + indices.x(), v0 );
								loadVertex( baseVertex + indices.y(), v1 );
								loadVertex( baseVertex + indices.z(), v2 );
							}
							else
							{
								auto baseIndex = m_writer.declLocale( "baseIndex"
									, modelData.getIndexOffset() + primitiveId * 3u );
								auto indices = m_writer.declLocale( "indices"
									, uvec3( m_meshBuffers.indices[baseIndex + 0u]
										, m_meshBuffers.indices[baseIndex + 1u]
										, m_meshBuffers.indices[baseIndex + 2u] ) );
								auto baseVertex = m_writer.declLocale( "baseVertex"
									, modelData.getVertexOffset() );
								loadVertex( baseVertex + indices.x(), v0 );
								loadVertex( baseVertex + indices.y(), v1 );
								loadVertex( baseVertex + indices.z(), v2 );
							}
						}
						, sdw::InUInt{ m_writer, "nodeId" }
						, sdw::InUInt{ m_writer, "primitiveId" }
						, sdw::InUInt{ m_writer, "meshletId" }
						, shader::InModelData{ m_writer, "modelData" }
						, shader::OutMeshVertex{ m_writer, "v0", m_submeshShaders }
						, shader::OutMeshVertex{ m_writer, "v1", m_submeshShaders }
						, shader::OutMeshVertex{ m_writer, "v2", m_submeshShaders } );
				}

				m_loadVertices( pnodeId, pprimitiveId, pmeshletId, pmodelData, pv0, pv1, pv2 );
			}

			void loadVertices( sdw::UInt const & pnodeId
				, sdw::UInt const & pprimitiveId
				, shader::ModelData const & pmodelData
				, shader::MeshVertex const & pv0
				, shader::MeshVertex const & pv1
				, shader::MeshVertex const & pv2
				, shader::CameraData const & c3d_cameraData
				, sdw::Array< shader::BillboardData > const & c3d_billboardData )
			{
				if ( !m_loadVertices )
				{
					m_loadVertices = m_writer.implementFunction< sdw::Void >( "loadVertices"
						, [&]( sdw::UInt const & nodeId
							, sdw::UInt const & primitiveId
							, sdw::UInt const &
							, shader::ModelData const & modelData
							, shader::MeshVertex v0
							, shader::MeshVertex v1
							, shader::MeshVertex v2 )
						{
							auto bbPositions = m_writer.declConstantArray( "bbPositions"
								, castor::Vector< sdw::Vec3 >{ vec3( -0.5_f, -0.5_f, 1.0_f )
								, vec3( -0.5_f, +0.5_f, 1.0_f )
								, vec3( +0.5_f, -0.5_f, 1.0_f )
								, vec3( +0.5_f, +0.5_f, 1.0_f ) } );
							auto bbTexcoords = m_writer.declConstantArray( "bbTexcoords"
								, castor::Vector< sdw::Vec2 >{ vec2( 0.0_f, 0.0_f )
								, vec2( 0.0_f, 1.0_f )
								, vec2( 1.0_f, 0.0_f )
								, vec2( 1.0_f, 1.0_f ) } );

							auto instanceId = m_writer.declLocale( "instanceId"
								, primitiveId / 2u );
							auto firstTriangle = m_writer.declLocale( "firstTriangle"
								, ( primitiveId % 2u ) == 0u );
							auto center = m_writer.declLocale( "center"
								, m_meshBuffers.positions[instanceId].position );
							auto bbcenter = m_writer.declLocale( "bbcenter"
								, modelData.modelToCurWorld( center ).xyz() );
							auto centerToCamera = m_writer.declLocale( "centerToCamera"
								, c3d_cameraData.getPosToCamera( bbcenter ) );
							centerToCamera.y() = 0.0_f;
							centerToCamera = normalize( centerToCamera );

							auto billboardData = m_writer.declLocale( "billboardData"
								, c3d_billboardData[nodeId - 1u] );
							auto right = m_writer.declLocale( "right"
								, billboardData.getCameraRight( c3d_cameraData ) );
							auto up = m_writer.declLocale( "up"
								, billboardData.getCameraUp( c3d_cameraData ) );
							auto width = m_writer.declLocale( "width"
								, billboardData.getWidth( c3d_cameraData ) );
							auto height = m_writer.declLocale( "height"
								, billboardData.getHeight( c3d_cameraData ) );

							auto vertexId = m_writer.declLocale( "vertexId"
								, m_writer.ternary( firstTriangle
									, uvec3( 0_u, 1_u, 2_u )
									, uvec3( 1_u, 3_u, 2_u ) ) );
							auto scaledRight = m_writer.declLocale( "scaledRight", vec3( 0.0_f ) );
							auto scaledUp = m_writer.declLocale( "scaledUp", vec3( 0.0_f ) );

							scaledRight = right * bbPositions[vertexId.x()].x() * width;
							scaledUp = up * bbPositions[vertexId.x()].y() * height;
							v0.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
							v0.texture0 = vec3( bbTexcoords[vertexId.x()], 0.0_f );
							v0.normal = centerToCamera;
							v0.tangent = vec4( up, 0.0_f );

							scaledRight = right * bbPositions[vertexId.y()].x() * width;
							scaledUp = up * bbPositions[vertexId.y()].y() * height;
							v1.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
							v1.texture0 = vec3( bbTexcoords[vertexId.y()], 0.0_f );
							v1.normal = centerToCamera;
							v1.tangent = vec4( up, 0.0_f );

							scaledRight = right * bbPositions[vertexId.z()].x() * width;
							scaledUp = up * bbPositions[vertexId.z()].y() * height;
							v2.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
							v2.texture0 = vec3( bbTexcoords[vertexId.z()], 0.0_f );
							v2.normal = centerToCamera;
							v2.tangent = vec4( up, 0.0_f );
						}
						, sdw::InUInt{ m_writer, "nodeId" }
						, sdw::InUInt{ m_writer, "primitiveId" }
						, sdw::InUInt{ m_writer, "meshletId" }
						, shader::InModelData{ m_writer, "modelData" }
						, shader::OutMeshVertex{ m_writer, "v0", m_submeshShaders }
						, shader::OutMeshVertex{ m_writer, "v1", m_submeshShaders }
						, shader::OutMeshVertex{ m_writer, "v2", m_submeshShaders } );
				}

				m_loadVertices( pnodeId, pprimitiveId, 0_u, pmodelData, pv0, pv1, pv2 );
			}

			void loadSurface( sdw::UInt const & pnodeId
				, sdw::UInt const & pprimitiveId
				, sdw::UInt const & pmeshletId
				, sdw::Vec2 const & ppixelCoord
				, shader::ModelData const & pmodelData
				, shader::Material const & pmaterial
				, sdw::Float const & pdepth
				, shader::CameraData const & c3d_cameraData
				, sdw::Array< shader::BillboardData > const & c3d_billboardData
				, shader::DerivFragmentSurface const & presult )
			{
				if ( !m_loadSurface )
				{
					m_loadSurface = m_writer.implementFunction< sdw::Void >( "loadSurface"
						, [&]( sdw::UInt const & nodeId
							, sdw::UInt const & primitiveId
							, sdw::UInt const & meshletId
							, sdw::Vec2 const & pixelCoord
							, shader::ModelData const & modelData
							, shader::Material const & material
							, sdw::Float depth
							, shader::DerivFragmentSurface result )
						{
							result.worldPosition = vec4( 0.0_f );
							result.viewPosition = vec4( 0.0_f );
							result.curPosition = vec4( 0.0_f );
							result.prvPosition = vec4( 0.0_f );
							result.tangentSpaceFragPosition = vec3( 0.0_f );
							result.tangentSpaceViewPosition = vec3( 0.0_f );
							result.normal = vec3( 0.0_f );
							result.tangent = vec4( 0.0_f );
							result.bitangent = vec3( 0.0_f );
							result.texture0.uv() = vec2( 0.0_f );
							result.texture1.uv() = vec2( 0.0_f );
							result.texture2.uv() = vec2( 0.0_f );
							result.texture3.uv() = vec2( 0.0_f );
							result.texture0.dPdx() = vec2( 0.0_f );
							result.texture1.dPdx() = vec2( 0.0_f );
							result.texture2.dPdx() = vec2( 0.0_f );
							result.texture3.dPdx() = vec2( 0.0_f );
							result.texture0.dPdy() = vec2( 0.0_f );
							result.texture1.dPdy() = vec2( 0.0_f );
							result.texture2.dPdy() = vec2( 0.0_f );
							result.texture3.dPdy() = vec2( 0.0_f );
							result.colour = vec3( 1.0_f );

							auto hdrCoords = m_writer.declLocale( "hdrCoords"
								, pixelCoord / vec2( c3d_cameraData.renderSize() ) );
							auto screenCoords = m_writer.declLocale( "screenCoords"
								, fma( hdrCoords, vec2( 2.0_f ), vec2( -1.0_f ) ) );

							auto v0 = m_writer.declLocale< shader::MeshVertex >( "v0", true, m_submeshShaders );
							auto v1 = m_writer.declLocale< shader::MeshVertex >( "v1", true, m_submeshShaders );
							auto v2 = m_writer.declLocale< shader::MeshVertex >( "v2", true, m_submeshShaders );

							if ( m_stride == 0u )
							{
								loadVertices( nodeId, primitiveId, meshletId, modelData, v0, v1, v2 );
							}
							else
							{
								loadVertices( nodeId, primitiveId, modelData, v0, v1, v2
									, c3d_cameraData
									, c3d_billboardData );
							}

							bool isWorldPos = m_flags.hasWorldPosInputs()
								|| ( m_stride != 0u );

							// Transform positions to clip space
							auto p0 = m_writer.declLocale( "p0"
								, c3d_cameraData.worldToCurProj( isWorldPos
									? v0.position
									: modelData.modelToCurWorld( v0.position ) ) );
							auto p1 = m_writer.declLocale( "p1"
								, c3d_cameraData.worldToCurProj( isWorldPos
									? v1.position
									: modelData.modelToCurWorld( v1.position ) ) );
							auto p2 = m_writer.declLocale( "p2"
								, c3d_cameraData.worldToCurProj( isWorldPos
									? v2.position
									: modelData.modelToCurWorld( v2.position ) ) );

							auto derivatives = m_writer.declLocale( "derivatives"
								, calcFullBarycentric( p0, p1, p2, screenCoords, vec2( c3d_cameraData.renderSize() ) ) );

							// Interpolate texture coordinates and calculate the gradients for texture sampling with mipmapping support
							if ( m_flags.enableTexcoord0() )
							{
								result.texture0 = derivatives.computeGradient( v0.texture0
									, v1.texture0
									, v2.texture0 );
							}

							if ( m_flags.enableTexcoord1() )
							{
								result.texture1 = derivatives.computeGradient( v0.texture1
									, v1.texture1
									, v2.texture1 );
							}

							if ( m_flags.enableTexcoord2() )
							{
								result.texture2 = derivatives.computeGradient( v0.texture2
									, v1.texture2
									, v2.texture2 );
							}

							if ( m_flags.enableTexcoord3() )
							{
								result.texture3 = derivatives.computeGradient( v0.texture3
									, v1.texture3
									, v2.texture3 );
							}

							if ( m_flags.enableColours() )
							{
								result.colour = derivatives.interpolate( v0.colour.xyz()
									, v1.colour.xyz()
									, v2.colour.xyz() );
							}

							auto normal = m_writer.declLocale( "normal"
								, vec3( 0.0_f )
								, m_flags.enableNormal() );

							if ( m_flags.enableNormal() )
							{
								normal = normalize( derivatives.interpolate( v0.normal.xyz()
									, v1.normal.xyz()
									, v2.normal.xyz() ) );
							}

							auto tangent = m_writer.declLocale( "tangent"
								, vec4( 0.0_f )
								, m_flags.enableTangentSpace() );

							if ( m_flags.enableTangentSpace() )
							{
								tangent = normalize( derivatives.interpolate( v0.tangent
									, v1.tangent
									, v2.tangent ) );
							}

							auto bitangent = m_writer.declLocale( "bitangent"
								, vec3( 0.0_f )
								, m_flags.enableBitangent() );

							if ( m_flags.enableBitangent() )
							{
								bitangent = normalize( derivatives.interpolate( v0.bitangent
									, v1.bitangent
									, v2.bitangent ) );
							}

							if ( m_flags.enablePassMasks() )
							{
								auto passMultipliers0 = m_writer.declLocaleArray< sdw::Vec4 >( "passMultipliers0", 4u );
								auto passMultipliers1 = m_writer.declLocaleArray< sdw::Vec4 >( "passMultipliers1", 4u );
								auto passMultipliers2 = m_writer.declLocaleArray< sdw::Vec4 >( "passMultipliers2", 4u );
								material.getPassMultipliers( m_flags
									, v0.passMasks
									, passMultipliers0 );
								material.getPassMultipliers( m_flags
									, v1.passMasks
									, passMultipliers1 );
								material.getPassMultipliers( m_flags
									, v2.passMasks
									, passMultipliers2 );

								for ( uint32_t i = 0u; i < 4u; ++i )
								{
									result.passMultipliers[i] = derivatives.interpolate( passMultipliers0[i]
										, passMultipliers1[i]
										, passMultipliers2[i] );
								}
							}

							auto curProjPosition = m_writer.declLocale( "curProjPosition"
								, derivatives.interpolate( p0, p1, p2 ) );

							IF( m_writer, curProjPosition.w() == 0.0_f )
							{
								curProjPosition.w() = 1.0_f;
							}
							FI

							depth = ( curProjPosition.z() / curProjPosition.w() );
							auto curPosition = m_writer.declLocale( "curPosition"
								, c3d_cameraData.projToView( curProjPosition ) );
							result.viewPosition = curPosition;
							curPosition = c3d_cameraData.curViewToWorld( curPosition );
							result.worldPosition = curPosition;

							auto prvPosition = m_writer.declLocale( "prvPosition"
								, curPosition );
							curPosition = modelData.worldToModel( curPosition );

							if ( m_stride == 0u )
							{
								if ( m_flags.hasWorldPosInputs() )
								{
									auto velocity = m_writer.declLocale( "velocity"
										, derivatives.interpolate( v0.velocity.xyz()
											, v1.velocity.xyz()
											, v2.velocity.xyz() ) );
									prvPosition.xyz() += velocity;
								}
								else if ( m_flags.enableNormal() )
								{
									auto curMtxModel = m_writer.declLocale( "curMtxModel"
										, modelData.getModelMtx() );
									auto mtxNormal = m_writer.declLocale( "mtxNormal"
										, modelData.getNormalMtx( m_flags, curMtxModel ) );
									normal = normalize( mtxNormal * normal );

									if ( m_flags.enableTangentSpace() )
									{
										auto prvMtxModel = m_writer.declLocale( "prvMtxModel"
											, modelData.getPrvModelMtx( m_flags, curMtxModel ) );
										prvPosition = prvMtxModel * curPosition;
										tangent = vec4( normalize( mtxNormal * tangent.xyz() ), tangent.w() );
										bitangent = normalize( mtxNormal * bitangent );
									}
								}
							}

							prvPosition = c3d_cameraData.worldToPrvProj( prvPosition );
							result.computeVelocity( c3d_cameraData
								, curProjPosition
								, prvPosition );

							if ( m_flags.enableNormal() )
							{
								result.computeTangentSpace( m_flags
									, c3d_cameraData.position()
									, result.worldPosition.xyz()
									, normal
									, tangent
									, bitangent );
							}
						}
						, sdw::InUInt{ m_writer, "nodeId" }
						, sdw::InUInt{ m_writer, "primitiveId" }
						, sdw::InUInt{ m_writer, "meshletId" }
						, sdw::InVec2{ m_writer, "pixelCoord" }
						, shader::InModelData{ m_writer, "modelData" }
						, shader::InMaterial{ m_writer, "material", m_passShaders }
						, sdw::OutFloat{ m_writer, "depth" }
						, shader::OutDerivFragmentSurface{ m_writer, "result", m_submeshShaders } );
				}

				m_loadSurface( pnodeId, pprimitiveId, pmeshletId, ppixelCoord, pmodelData, pmaterial, pdepth, presult );
			}

		private:
			sdw::ShaderWriter & m_writer;
			shader::PassShaders & m_passShaders;
			shader::SubmeshShaders & m_submeshShaders;
			PipelineFlags const & m_flags;
			uint32_t m_stride;
			bool m_meshlets;
			shader::MeshBuffers m_meshBuffers;
			sdw::Function< BarycentricFullDerivatives
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InVec2
				, sdw::InVec2 > m_calcFullBarycentric;
			sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InUInt
				, shader::InModelData
				, shader::OutMeshVertex
				, shader::OutMeshVertex
				, shader::OutMeshVertex > m_loadVertices;
			sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InVec2
				, shader::InModelData
				, shader::InMaterial
				, sdw::OutFloat
				, shader::OutDerivFragmentSurface > m_loadSurface;
		};

		static ShaderPtr getProgram( RenderDevice const & device
			, Scene const & scene
			, RenderTechnique const & technique
			, VkExtent3D const & imageSize
			, PipelineFlags const & flags
			, IndirectLightingData const * indirectLighting
			, DebugConfig & debugConfig
			, uint32_t stride
			, bool hasSsao
			, ClustersConfig const & clustersConfig
			, bool outputScattering
			, DeferredLightingFilter deferredLighting
			, bool isMeshShading
			, bool areDebugTargetsEnabled )
		{
			bool isDeferredLighting = ( deferredLighting == DeferredLightingFilter::eDeferredOnly );
			auto & engine = *device.renderSystem.getEngine();
			ShaderWriter< useCompute >::Type writer{ &engine.getShaderAllocator() };

			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, flags
				, ( ComponentModeFlag::eDerivTex
					| VisibilityResolvePass::getComponentsMask() )
				, utils };
			shader::SubmeshShaders submeshShaders{ engine.getSubmeshComponentsRegister()
				, flags };
			shader::CookTorranceBRDF cookTorrance{ writer, brdf };

			auto index = uint32_t( InOutBindings::eCount );
			C3D_Camera( writer
				, InOutBindings::eCamera
				, Sets::eInOuts );
			C3D_Scene( writer
				, InOutBindings::eScene
				, Sets::eInOuts );
			C3D_ModelsData( writer
				, InOutBindings::eModels
				, Sets::eInOuts );
			C3D_BillboardOpt( writer
				, InOutBindings::eBillboards
				, Sets::eInOuts
				, stride != 0u );
			shader::Materials materials{ *scene.getEngine()
				, writer
				, passShaders
				, InOutBindings::eMaterials
				, Sets::eInOuts
				, index };
			shader::SssProfiles sssProfiles{ writer
				, InOutBindings::eSssProfiles
				, Sets::eInOuts
				, !C3D_DisableSSSTransmittance };
			shader::TextureConfigurations textureConfigs{ writer
				, InOutBindings::eTexConfigs
				, Sets::eInOuts };
			shader::TextureAnimations textureAnims{ writer
				, InOutBindings::eTexAnims
				, Sets::eInOuts };
			auto c3d_imgData = writer.declStorageImg< sdw::RUImage2DRg32 >( "c3d_imgData"
				, InOutBindings::eInData
				, Sets::eInOuts );
			auto c3d_imgDiffuse = writer.declStorageImg< sdw::RWImage2DRgba32 >( "c3d_imgDiffuse"
				, InOutBindings::eInOutDiffuse
				, Sets::eInOuts
				, flags.pass.hasDeferredDiffuseLightingFlag );
			auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
				, InOutBindings::eMapBrdf
				, Sets::eInOuts );
			auto lightsIndex = index;
			++index;
			auto c3d_mapOcclusion = writer.declCombinedImg< FImg2DR32 >( "c3d_mapOcclusion"
				, ( hasSsao ? index++ : 0u )
				, Sets::eInOuts
				, hasSsao );
			shader::Lights lights{ *scene.getEngine()
				, flags.lightingModelId
				, flags.backgroundModelId
				, materials
				, brdf
				, utils
				, shader::ShadowOptions{ flags.getShadowFlags(), true /* vsm */, false /* rsm */, technique.hasShadowBuffer() /* reserveIds */ }
				, &sssProfiles
				, lightsIndex /* lightBinding */
				, Sets::eInOuts /* lightSet */
				, index /* shadowMapBinding */
				, Sets::eInOuts /* shadowMapSet */
				, true /* enableVolumetric */ };
			shader::ReflectionModel reflections{ writer
				, utils
				, index
				, uint32_t( Sets::eInOuts )
				, lights.hasIblSupport() };
			auto backgroundModel = shader::BackgroundModel::createModel( scene
				, writer
				, utils
				, makeExtent2D( imageSize )
				, true
				, index
				, Sets::eInOuts );
			shader::GlobalIllumination indirect{ writer
				, utils
				, index
				, Sets::eInOuts
				, flags.getGlobalIlluminationFlags()
				, *indirectLighting };
			shader::ClusteredLights clusteredLights{ writer
				, index
				, Sets::eInOuts
				, &clustersConfig };

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps", TexBindings::eTextures, Sets::eTex ) );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
			auto pipelineId = pcb.declMember< sdw::UInt >( "pipelineId" );
			auto billboardNodeId = pcb.declMember< sdw::UInt >( "billboardNodeId", stride != 0u );
			pcb.end();

			shader::Fog fog{ writer };
			VisibilityHelpers visHelpers{ writer, passShaders, submeshShaders, flags, stride, isMeshShading };

			auto shade = writer.implementFunction< sdw::Boolean >( "shade"
				, [&]( sdw::IVec2 const & ipixel
					, sdw::UInt const & curNodeId
					, sdw::UInt const & curPipelineId
					, sdw::UInt const & primitiveId
					, sdw::UInt const & meshletId
					, sdw::Vec4 outResult
					, sdw::Vec4 inoutDiffuse
					, sdw::Vec4 outScattering )
				{
					{
						shader::DebugOutput output{ debugConfig
							, cuT( "Opaque" )
							, c3d_cameraData.debugIndex()
							, outResult
							, areDebugTargetsEnabled };

						if ( !VisibilityResolvePass::useCompute() )
						{
							IF( writer, pipelineId != curPipelineId )
							{
								writer.returnStmt( 0_b );
							}
							FI
						}

						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[curNodeId - 1u] );
						auto material = writer.declLocale( "material"
							, materials.getMaterial( modelData.getMaterialId() ) );
						auto depth = writer.declLocale( "depth"
							, 0.0_f );
						auto occlusion = writer.declLocale( "occlusion"
							, ( hasSsao
								? c3d_mapOcclusion.fetch( ipixel, 0_i )
								: 1.0_f ) );
						auto baseSurface = writer.declLocale< shader::DerivFragmentSurface >( "baseSurface"
							, true
							, submeshShaders );
						visHelpers.loadSurface( curNodeId
							, primitiveId
							, meshletId
							, vec2( ipixel )
							, modelData
							, material
							, depth
							, c3d_cameraData
							, c3d_billboardData
							, baseSurface );
						auto components = writer.declLocale( "components"
							, shader::BlendComponents{ materials
								, material
								, baseSurface } );
						materials.blendMaterials( output
							, VK_COMPARE_OP_ALWAYS
							, flags
							, textureConfigs
							, textureAnims
							, c3d_maps
							, material
							, modelData.getMaterialId()
							, baseSurface.passMultipliers
							, components );
						output.registerOutput( cuT( "Surface" ), cuT( "Normal" ), fma( components.normal, vec3( 0.5_f ), vec3( 0.5_f ) ) );
						output.registerOutput( cuT( "Surface" ), cuT( "Tangent" ), fma( baseSurface.tangent.xyz(), vec3( 0.5_f ), vec3( 0.5_f ) ) );
						output.registerOutput( cuT( "Surface" ), cuT( "Bitangent" ), fma( baseSurface.bitangent, vec3( 0.5_f ), vec3( 0.5_f ) ) );
						output.registerOutput( cuT( "Surface" ), cuT( "World Position" ), baseSurface.worldPosition );
						output.registerOutput( cuT( "Surface" ), cuT( "View Position" ), baseSurface.viewPosition );

						if ( components.occlusion )
						{
							occlusion *= components.occlusion;
						}

						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( baseSurface.worldPosition.xyz(), c3d_cameraData.position() ) );

						if ( components.transmission )
						{
							IF( writer, components.transmission >= 0.1_f )
							{
								writer.returnStmt( 0_b );
							}
							FI
						}

						if ( auto lightingModel = lights.getLightingModel() )
						{
							// Direct Lighting
							IF( writer, material.lighting )
							{
								auto surface = writer.declLocale( "surface"
									, shader::Surface{ vec3( vec2( ipixel ), depth )
										, baseSurface.viewPosition
										, baseSurface.worldPosition
										, normalize( components.normal ) } );

								lightingModel->finish( passShaders
									, baseSurface
									, surface
									, utils
									, c3d_cameraData.position()
									, components );
								auto lightSurface = shader::LightSurface::create( writer
									, "lightSurface"
									, c3d_cameraData.position()
									, surface.worldPosition
									, surface.viewPosition.xyz()
									, surface.clipPosition
									, surface.normal );

								if ( flags.pass.hasDeferredDiffuseLightingFlag
									&& !isDeferredLighting )
								{
									auto diffuse = writer.declLocale( "diffuse", vec3( 0.0_f ) );
									lights.computeCombinedDif( clusteredLights
										, components
										, *backgroundModel
										, lightSurface
										, modelData.isShadowReceiver()
										, lightSurface.clipPosition().xy()
										, lightSurface.viewPosition().z()
										, output
										, diffuse );
									inoutDiffuse = vec4( diffuse, components.transmittance );
									outScattering = vec4( 0.0_f );
								}
								else
								{
									auto directLighting = writer.declLocale( "directLighting"
										, shader::DirectLighting{ writer } );

									if ( isDeferredLighting )
									{
										lights.computeCombinedAllButDif( clusteredLights
											, components
											, *backgroundModel
											, lightSurface
											, modelData.isShadowReceiver()
											, lightSurface.clipPosition().xy()
											, lightSurface.viewPosition().z()
											, inoutDiffuse.rgb()
											, output
											, directLighting );
									}
									else
									{
										lights.computeCombinedDifSpec( clusteredLights
											, components
											, *backgroundModel
											, lightSurface
											, modelData.isShadowReceiver()
											, lightSurface.clipPosition().xy()
											, lightSurface.viewPosition().z()
											, output
											, directLighting );
									}

									directLighting.ambient() = components.ambientColour * c3d_sceneData.ambientLight() * components.ambientFactor;
									output.registerOutput( cuT( "Lighting" ), cuT( "Ambient" ), directLighting.ambient() );
									output.registerOutput( cuT( "Lighting" ), cuT( "Occlusion" ), occlusion );
									output.registerOutput( cuT( "Lighting" ), cuT( "Emissive" ), components.emissiveColour * components.emissiveFactor );

									// Indirect Lighting
									lightSurface.updateL( utils
										, components.normal
										, components.f0
										, components );
									auto indirectLighting = writer.declLocale( "indirectLighting"
										, shader::IndirectLighting{ writer } );
									indirect.computeCombinedDifSpec( flags.getGlobalIlluminationFlags()
										, flags.hasDiffuseGI()
										, cookTorrance
										, lightSurface
										, components.roughness
										, c3d_mapBrdf
										, indirectLighting
										, output );

									// Reflections/Refraction
									auto reflectedDiffuse = writer.declLocale( "reflectedDiffuse"
										, vec3( 0.0_f ) );
									auto reflectedSpecular = writer.declLocale( "reflectedSpecular"
										, vec3( 0.0_f ) );
									auto refracted = writer.declLocale( "refracted"
										, vec3( 0.0_f ) );
									auto coatReflected = writer.declLocale( "coatReflected"
										, vec3( 0.0_f ) );
									auto sheenReflected = writer.declLocale( "sheenReflected"
										, vec3( 0.0_f ) );

									if ( components.hasMember( "thicknessFactor" ) )
									{
										components.thicknessFactor *= length( modelData.getScale() );
									}

									lightSurface.updateN( utils
										, components.normal
										, components.f0
										, components );
									passShaders.computeReflRefr( reflections
										, components
										, lightSurface
										, *backgroundModel
										, c3d_cameraData
										, directLighting
										, indirectLighting
										, vec2( ipixel )
										, modelData.getEnvMapIndex()
										, incident
										, components.hasReflection
										, components.hasRefraction
										, components.refractionRatio
										, reflectedDiffuse
										, reflectedSpecular
										, refracted
										, coatReflected
										, sheenReflected
										, output );
									output.registerOutput( cuT( "Reflections" ), cuT( "Incident" ), sdw::fma( incident, vec3( 0.5_f ), vec3( 0.5_f ) ) );

									// Combine
									outResult = vec4( lightingModel->combine( output
											, components
											, incident
											, directLighting
											, indirectLighting
											, occlusion
											, components.emissiveColour * components.emissiveFactor
											, reflectedDiffuse
											, reflectedSpecular
											, refracted
											, coatReflected
											, sheenReflected )
										, components.opacity );
									outScattering = vec4( directLighting.scattering(), 1.0_f);
								}
							}
							ELSE
							{
								outResult = vec4( components.colour, components.opacity );
								outScattering = vec4( 0.0_f );

								if ( flags.pass.hasDeferredDiffuseLightingFlag
									&& !isDeferredLighting )
								{
									inoutDiffuse = vec4( 0.0_f );
								}
							}
							FI
						}
						else
						{
							outResult = vec4( components.colour, components.opacity );
							outScattering = vec4( 0.0_f );

							if ( flags.pass.hasDeferredDiffuseLightingFlag
								&& !isDeferredLighting )
							{
								inoutDiffuse = vec4( 0.0_f );
							}
						}

						if ( !flags.pass.hasDeferredDiffuseLightingFlag
							|| deferredLighting != DeferredLightingFilter::eDeferLighting )
						{
							if ( flags.hasFog() )
							{
								outResult = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
									, outResult
									, baseSurface.worldPosition.xyz()
									, c3d_cameraData.position()
									, c3d_sceneData );

								if ( outputScattering )
								{
									outScattering = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
										, outScattering
										, baseSurface.worldPosition.xyz()
										, c3d_cameraData.position()
										, c3d_sceneData );
								}
							}

							auto linearDepth = writer.declLocale( "linearDepth"
								, utils.lineariseDepth( depth, c3d_cameraData.nearPlane(), c3d_cameraData.farPlane() ) );
							backgroundModel->applyVolume( vec2( ipixel )
								, linearDepth
								, vec2( c3d_cameraData.renderSize() )
								, c3d_cameraData.depthPlanes()
								, outResult );

							if ( outputScattering )
							{
								backgroundModel->applyVolume( vec2( ipixel )
									, linearDepth
									, vec2( c3d_cameraData.renderSize() )
									, c3d_cameraData.depthPlanes()
									, outScattering );
							}
						}

						outResult.a() = 1.0_f;
					}
					writer.returnStmt( 1_b );
				}
				, sdw::InIVec2{ writer, "ipixel" }
				, sdw::InUInt{ writer, "curNodeId" }
				, sdw::InUInt{ writer, "curPipelineId" }
				, sdw::InUInt{ writer, "primitiveId" }
				, sdw::InUInt{ writer, "meshletId" }
				, sdw::OutVec4{ writer, "outResult" }
				, sdw::InOutVec4{ writer, "inoutDiffuse" }
				, sdw::OutVec4{ writer, "outScattering" } );

			ShaderWriter< useCompute >::implementMain( writer
				, flags
				, isMeshShading
				, isDeferredLighting
				, outputScattering
				, stride
				, c3d_imgData
				, c3d_imgDiffuse
				, pipelineId
				, billboardNodeId
				, shade );

			return writer.getBuilder().releaseShader();
		}

		static ashes::DescriptorSetLayoutPtr createInDescriptorLayout( RenderDevice const & device
			, castor::String const & name
			, MaterialCache const & matCache
			, Scene const & scene
			, RenderTechnique const & technique
			, bool allowClusteredLighting
			, Texture const * ssao
			, IndirectLightingData const * indirectLighting )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute()
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eCamera
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eScene
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eModels
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( InOutBindings::eBillboards )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( matCache.getPassBuffer().createLayoutBinding( InOutBindings::eMaterials
				, stages ) );
			bindings.emplace_back( matCache.getSssProfileBuffer().createLayoutBinding( InOutBindings::eSssProfiles
				, stages ) );
			bindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( InOutBindings::eTexConfigs
				, stages ) );
			bindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( InOutBindings::eTexAnims
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eInData
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eInOutDiffuse
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMapBrdf
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, stages ) );

			if ( VisibilityResolvePass::useCompute() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMaterialsCounts
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMaterialsStarts
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::ePixelsXY
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutScattering
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
			}

			auto const & engine = *device.renderSystem.getEngine();
			auto index = uint32_t( InOutBindings::eCount );
			engine.addSpecificsBuffersBindings( bindings
				, stages
				, index );
			bindings.emplace_back( scene.getLightCache().createLayoutBinding( stages
				, index ) );
			++index;

			if ( ssao )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, stages ) ); // c3d_mapOcclusion
				++index;
			}

			if ( technique.hasShadowBuffer() )
			{
				RenderNodesPass::addShadowBindings( bindings
					, stages
					, index );
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( index // c3d_mapEnvironment
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, stages ) );
			++index;

			if ( auto background = scene.getBackground() )
			{
				RenderNodesPass::addBackgroundBindings( *background
					, bindings
					, stages
					, index );
			}

			RenderNodesPass::addGIBindings( scene.getFlags()
				, *indirectLighting
				, bindings
				, stages
				, index );

			if ( allowClusteredLighting
				&& technique.getRenderTarget().getFrustumClusters() )
			{
				RenderNodesPass::addClusteredLightingBindings( *technique.getRenderTarget().getFrustumClusters()
					, bindings
					, stages
					, index );
			}

			return device->createDescriptorSetLayout( castor::toUtf8( name ) + "InOut"
				, castor::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createInDescriptorSet( castor::String const & name
			, ashes::DescriptorSetPool const & pool
			, crg::RunnableGraph & graph
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, RenderTechnique const & technique
			, Scene const & scene
			, bool allowClusteredLighting
			, crg::ImageViewIdArray const & targetImage
			, Texture const * ssao
			, IndirectLightingData const * indirectLighting
			, DeferredLightingFilter deferredLighting )
		{
			auto const & engine = *scene.getOwner();
			auto const & matCache = engine.getMaterialCache();
			ashes::WriteDescriptorSetArray writes;
			writes.push_back( cameraUbo.getDescriptorWrite( InOutBindings::eCamera ) );
			writes.push_back( sceneUbo.getDescriptorWrite( InOutBindings::eScene ) );
			writes.push_back( makeDescriptorWrite( scene.getModelBuffer()
				, InOutBindings::eModels
				, 0u
				, scene.getModelBuffer().getCount() ) );
			writes.push_back( makeDescriptorWrite( scene.getBillboardsBuffer()
				, InOutBindings::eBillboards
				, 0u
				, scene.getBillboardsBuffer().getCount() ) );
			writes.push_back( matCache.getPassBuffer().getBinding( InOutBindings::eMaterials ) );
			writes.push_back( matCache.getSssProfileBuffer().getBinding( InOutBindings::eSssProfiles ) );
			writes.push_back( matCache.getTexConfigBuffer().getBinding( InOutBindings::eTexConfigs ) );
			writes.push_back( matCache.getTexAnimBuffer().getBinding( InOutBindings::eTexAnims ) );
			auto & visibilityPassResult = technique.getVisibilityResult();
			writes.push_back( makeImageViewDescriptorWrite( visibilityPassResult.targetView
				, InOutBindings::eInData ) );

			if ( deferredLighting == DeferredLightingFilter::eDeferredOnly )
			{
				writes.push_back( makeImageViewDescriptorWrite( technique.getSssDiffuse().targetView
					, InOutBindings::eInOutDiffuse ) );
			}
			else
			{
				writes.push_back( makeImageViewDescriptorWrite( technique.getDiffuse().targetView
					, InOutBindings::eInOutDiffuse ) );
			}

			writes.push_back( makeImageViewDescriptorWrite( engine.getRenderSystem()->getPrefilteredBrdfTexture().wholeView
				, *engine.getRenderSystem()->getPrefilteredBrdfTexture().sampler
				, InOutBindings::eMapBrdf ) );

			if ( VisibilityResolvePass::useCompute() )
			{
				writes.push_back( makeDescriptorWrite( technique.getMaterialsCounts()
					, InOutBindings::eMaterialsCounts
					, 0u
					, technique.getMaterialsCounts().getCount() ) );
				writes.push_back( makeDescriptorWrite( technique.getMaterialsStarts()
					, InOutBindings::eMaterialsStarts
					, 0u
					, technique.getMaterialsStarts().getCount() ) );
				writes.push_back( makeDescriptorWrite( technique.getPixelXY()
					, InOutBindings::ePixelsXY
					, 0u
					, technique.getPixelXY().getCount() ) );
				writes.push_back( makeImageViewDescriptorWrite( graph.createImageView( targetImage.front() )
					, InOutBindings::eOutResult ) );
				writes.push_back( makeImageViewDescriptorWrite( technique.getScattering().sampledView
					, InOutBindings::eOutScattering ) );
			}

			auto index = uint32_t( InOutBindings::eCount );
			engine.addSpecificsBuffersDescriptors( writes, index );
			writes.push_back( scene.getLightCache().getBinding( index ) );
			++index;

			if ( ssao )
			{
				bindTexture( ssao->wholeView
					, *ssao->sampler
					, writes
					, index );
			}

			if ( technique.hasShadowBuffer() )
			{
				RenderNodesPass::addShadowDescriptor( *engine.getRenderSystem()
					, graph
					, writes
					, technique.getShadowMaps()
					, technique.getShadowBuffer()
					, index );
			}

			bindTexture( scene.getEnvironmentMap().getColourId().sampledView
				, *scene.getEnvironmentMap().getColourId().sampler
				, writes
				, index );

			if ( auto background = scene.getBackground() )
			{
				RenderNodesPass::addBackgroundDescriptor( *background
					, writes
					, targetImage
					, index );
			}

			RenderNodesPass::addGIDescriptor( scene.getFlags()
				, *indirectLighting
				, writes
				, index );

			if ( allowClusteredLighting
				&& technique.getRenderTarget().getFrustumClusters() )
			{
				RenderNodesPass::addClusteredLightingDescriptor( *technique.getRenderTarget().getFrustumClusters()
					, writes
					, index );
			}

			auto result = pool.createDescriptorSet( castor::toUtf8( name ) + "InOut"
				, Sets::eInOuts );
			result->setBindings( castor::move( writes ) );
			result->update();
			return result;
		}

		static ashes::DescriptorSetLayoutPtr createVtxDescriptorLayout( RenderDevice const & device
			, castor::String const & name )
		{
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute()
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPosition
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			return device->createDescriptorSetLayout( castor::toUtf8( name ) + "Vtx"
				, castor::move( bindings ) );
		}

		static ashes::DescriptorSetLayoutPtr createVtxDescriptorLayout( RenderDevice const & device
			, castor::String const & name
			, PipelineFlags const & flags
			, bool isMeshShading )
		{
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute()
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;

			if ( isMeshShading )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInMeshlets
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableIndices() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInIndices
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enablePosition() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPosition
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableNormal() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInNormal
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableTangentSpace() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableBitangent() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInBitangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableTexcoord0() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord0
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableTexcoord1() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord1
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableTexcoord2() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord2
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableTexcoord3() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord3
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableColours() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInColour
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enablePassMasks() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPassMasks
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( flags.enableVelocity() )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInVelocity
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			return device->createDescriptorSetLayout( castor::toUtf8( name ) + "Vtx"
				, castor::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createVtxDescriptorSet( castor::String const & name
			, ashes::DescriptorSetPool const & pool
			, ashes::BufferBase const & positionsBuffer
			, VkDeviceSize offset
			, VkDeviceSize range )
		{
			ashes::WriteDescriptorSetArray writes;
			writes.emplace_back( makeDescriptorWrite( positionsBuffer, VtxBindings::eInPosition, offset, range ) );
			auto result = pool.createDescriptorSet( castor::toUtf8( name ) + "Vtx"
				, Sets::eVtx );
			result->setBindings( castor::move( writes ) );
			result->update();
			return result;
		}

		static ashes::DescriptorSetPtr createVtxDescriptorSet( castor::String const & name
			, PipelineFlags const & flags
			, bool isMeshShading
			, ashes::DescriptorSetPool const & pool
			, ObjectBufferPool::ModelBuffers const & modelBuffers
			, ashes::BufferBase const * indexBuffer )
		{
			ashes::WriteDescriptorSetArray writes;

			if ( isMeshShading )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eMeshlets )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInMeshlets, 0u, buffer.getSize() ) );
			}

			if ( flags.enableIndices() )
			{
				CU_Require( indexBuffer );
				writes.emplace_back( makeDescriptorWrite( *indexBuffer, VtxBindings::eInIndices, 0u, indexBuffer->getSize() ) );
			}

			if ( flags.enablePosition() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::ePositions )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPosition, 0u, buffer.getSize() ) );
			}

			if ( flags.enableNormal() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eNormals )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInNormal, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTangentSpace() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eTangents )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTangent, 0u, buffer.getSize() ) );
			}

			if ( flags.enableBitangent() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eBitangents )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInBitangent, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord0() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords0 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord0, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord1() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords1 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord1, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord2() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords2 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord2, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord3() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords3 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord3, 0u, buffer.getSize() ) );
			}

			if ( flags.enableColours() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eColours )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInColour, 0u, buffer.getSize() ) );
			}

			if ( flags.enablePassMasks() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::ePassMasks )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPassMasks, 0u, buffer.getSize() ) );
			}

			if ( flags.enableVelocity() )
			{
				auto const & buffer = modelBuffers.buffers[size_t( SubmeshData::eVelocity )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInVelocity, 0u, buffer.getSize() ) );
			}

			auto result = pool.createDescriptorSet( castor::toUtf8( name ) + "Vtx"
				, Sets::eVtx );
			result->setBindings( castor::move( writes ) );
			result->update();
			return result;
		}

		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, castor::String const & name
			, crg::ImageViewIdArray const & targetImage
			, Texture const * scattering
			, bool first
			, DeferredLightingFilter deferredLighting )
		{
			auto srcLayout = ( ( first && deferredLighting == DeferredLightingFilter::eDeferLighting )
				? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
			auto srcStage = VkPipelineStageFlags( first
				? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				: VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
			auto dstStage = VkPipelineStageFlags( first
				? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				: VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT );
			auto srcAccess = VkAccessFlags( first
				? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				: VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT );
			auto dstAccess = VkAccessFlags( first
				? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				: VK_ACCESS_SHADER_READ_BIT );
			ashes::VkAttachmentDescriptionArray attaches;
			attaches.emplace_back( VkAttachmentDescription{ 0u
				, targetImage.front().data->info.format
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, srcLayout
				, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );

			if ( scattering )
			{
				attaches.emplace_back( VkAttachmentDescription{ 0u
					, scattering->getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, ( first ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD )
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, ( first ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL )
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			}

			ashes::SubpassDescription subpassesDesc{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, ashes::nullopt
				, {} };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( castor::move( subpassesDesc ) );
			ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
					, 0u
					, srcStage
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, srcAccess
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, dstStage
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, dstAccess
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, castor::move( attaches )
				, castor::move( subpasses )
				, castor::move( dependencies ) };
			return device->createRenderPass( castor::toUtf8( name )
				, castor::move( createInfo ) );
		}

		static ashes::FrameBufferPtr createFrameBuffer( ashes::RenderPass const & renderPass
			, castor::String const & name
			, crg::RunnableGraph & graph
			, crg::ImageViewIdArray const & targetImage
			, Texture const * scattering )
		{
			ashes::VkImageViewArray fbAttaches;
			auto extent = getExtent( targetImage.front() );
			fbAttaches.emplace_back( graph.createImageView( targetImage.front() ) );

			if ( scattering )
			{
				fbAttaches.emplace_back( scattering->targetView );
			}

			return renderPass.createFrameBuffer( castor::toUtf8( name )
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, uint32_t( fbAttaches.size() )
					, fbAttaches.data()
					, extent.width
					, extent.height
					, 1u ) );
		}

		static ashes::PipelinePtr createPipeline( RenderDevice const & device
			, VkExtent3D const & extent
			, ashes::PipelineShaderStageCreateInfoArray stages
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::RenderPass const & renderPass
			, Texture const * scattering
			, bool blend )
		{
			auto blendState = blend
				? ashes::PipelineColorBlendStateCreateInfo{ 0u
					, VK_FALSE
					, VK_LOGIC_OP_COPY
					, { { VK_TRUE
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_FACTOR_ZERO
						, VK_BLEND_OP_ADD
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_FACTOR_ZERO
						, VK_BLEND_OP_ADD
						, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT } }}
				: ashes::PipelineColorBlendStateCreateInfo{};

			if ( scattering )
			{
				blendState.attachments.push_back( blendState.attachments.back() );
			}

			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u, {}, {} };
			ashes::PipelineViewportStateCreateInfo viewportState{ 0u
				, { makeViewport( castor::Point2ui{ extent.width, extent.height } ) }
				, { makeScissor( castor::Point2ui{ extent.width, extent.height } ) } };
			return device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
				, castor::move( stages )
				, castor::move( vertexState )
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP }
				, ashes::nullopt
				, castor::move( viewportState )
				, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
				, ashes::PipelineMultisampleStateCreateInfo{}
				, std::nullopt
				, castor::move( blendState )
				, ashes::nullopt
				, pipelineLayout
				, static_cast< VkRenderPass const & >( renderPass ) } );
		}

		struct PushData
		{
			uint32_t pipelineId;
			uint32_t billboardNodeId;
		};
	}

	VisibilityResolvePass::VisibilityResolvePass( RenderTechnique * parent
		, crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, RenderNodesPass const & nodesPass
		, crg::ImageViewIdArray targetImage
		, crg::ImageViewIdArray targetDepth
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: castor::Named{ category + cuT( "/" ) + name }
		, RenderTechniquePass{ parent, *parent->getRenderTarget().getScene(), techniquePassDesc.m_outputScattering }
		, crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( useCompute() ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ); } )
				, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return doIsEnabled(); } )
				, IsComputePassCallback( [](){ return useCompute(); } ) }
			, renderPassDesc.m_ruConfig }
		, m_device{ device }
		, m_nodesPass{ nodesPass }
		, m_cameraUbo{ renderPassDesc.m_cameraUbo }
		, m_sceneUbo{ *renderPassDesc.m_sceneUbo }
		, m_targetImage{ castor::move( targetImage ) }
		, m_targetDepth{ castor::move( targetDepth ) }
		, m_ssaoConfig{ techniquePassDesc.m_ssaoConfig }
		, m_ssao{ techniquePassDesc.m_ssao }
		, m_deferredLightingFilter{ renderPassDesc.m_deferredLightingFilter }
		, m_parallaxOcclusionFilter{ renderPassDesc.m_parallaxOcclusionFilter }
		, m_onNodesPassSort( m_nodesPass.onSortNodes.connect( [this]( RenderNodesPass const & ){ m_commandsChanged = true; } ) )
		, m_renderPass{ ( useCompute()
			? nullptr
			: visres::createRenderPass( m_device, getName(), m_targetImage, m_outputScattering ? &parent->getScattering() : nullptr, true, m_deferredLightingFilter ) ) }
		, m_framebuffer{ ( useCompute()
			? nullptr
			: visres::createFrameBuffer( *m_renderPass, getName(), graph, m_targetImage, m_outputScattering ? &parent->getScattering() : nullptr ) ) }
		, m_clustersConfig{ techniquePassDesc.m_clustersConfig }
	{
	}

	void VisibilityResolvePass::accept( RenderTechniqueVisitor & visitor )
	{
		doAccept( visitor );
	}

	void VisibilityResolvePass::update( CpuUpdater & updater )
	{
		m_maxPipelineId = m_nodesPass.getMaxPipelineId();

		if ( m_commandsChanged || m_maxPipelineId.isDirty() )
		{
			m_maxPipelineId.reset();
			m_activePipelines.clear();
			m_activeBillboardPipelines.clear();

			for ( auto const & [pipelineHash, buffer] : m_nodesPass.getPassPipelineNodes() )
			{
				auto pipelineId = m_nodesPass.getPipelineNodesIndex( pipelineHash, *buffer );
				PipelineFlags pipelineFlags{ getPipelineHiHashDetails( *this
					, pipelineHash
					, getShaderFlags() ) };

				if ( pipelineFlags.isBillboard() )
				{
					continue;
				}

				getEngine()->getPassComponentsRegister().registerPassComponentCombine( pipelineFlags.pass );
				pipelineFlags.m_sceneFlags = getScene().getFlags();
				pipelineFlags.backgroundModelId = getScene().getBackground()->getModelID();

				if ( pipelineFlags.pass.hasParallaxOcclusionMappingOneFlag
					|| pipelineFlags.pass.hasParallaxOcclusionMappingRepeatFlag
					|| ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly
						&& !pipelineFlags.pass.hasDeferredDiffuseLightingFlag ) )
				{
					continue;
				}

				auto & pipeline = doCreatePipeline( pipelineFlags );
				auto it = m_activePipelines.try_emplace( &pipeline ).first;

				auto hash = std::hash< ashes::BufferBase const * >{}( buffer );
				auto [pit, res] = pipeline.vtxDescriptorSets.try_emplace( hash );

				if ( res )
				{
					auto & modelBuffers = m_device.geometryPools->getBuffers( *buffer );
					ashes::BufferBase const * indexBuffer{};

					if ( pipelineFlags.enableIndices() )
					{
						indexBuffer = &m_device.geometryPools->getIndexBuffer( *buffer );
					}

					pit->second = visres::createVtxDescriptorSet( getName()
						, pipelineFlags
						, m_nodesPass.isMeshShading()
						, *pipeline.vtxDescriptorPool
						, modelBuffers
						, indexBuffer );
				}

				auto & pipelines = it->second.try_emplace( pit->second.get() ).first->second;
				pipelines.push_back( pipelineId );
			}

			for ( auto & [_, pipelinesNodes] : m_nodesPass.getBillboardNodes() )
			{
				auto & origPipeline = pipelinesNodes.pipeline;
				if ( !origPipeline.pipeline )
				{
					continue;
				}

				auto & pipelineFlags = origPipeline.pipeline->getFlags();

				if ( pipelineFlags.pass.hasParallaxOcclusionMappingOneFlag
					|| pipelineFlags.pass.hasParallaxOcclusionMappingRepeatFlag
					|| ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly
						&& !pipelineFlags.pass.hasDeferredDiffuseLightingFlag ) )
				{
					continue;
				}

				auto pipelineHash = origPipeline.pipeline->getFlagsHash();
				auto & buffers = pipelinesNodes.nodes;

				for ( auto & [buffer, nodes] : buffers )
				{
					for ( auto & node : nodes )
					{
						auto & positionsBuffer = node.node->data.getVertexBuffer();
						auto & pipeline = doCreatePipeline( pipelineFlags
							, node.node->data.getVertexStride() );
						auto it = m_activeBillboardPipelines.try_emplace( &pipeline ).first;
						auto hash = size_t( positionsBuffer.getOffset() );
						hash = castor::hashCombinePtr( hash, positionsBuffer.getBuffer().getBuffer() );
						auto [pit, res] = pipeline.vtxDescriptorSets.try_emplace( hash );
						auto pipelineId = m_nodesPass.getPipelineNodesIndex( pipelineHash, *buffer );

						if ( res )
						{
							pit->second = visres::createVtxDescriptorSet( getName()
								, *pipeline.vtxDescriptorPool
								, positionsBuffer.getBuffer().getBuffer()
								, positionsBuffer.getOffset()
								, positionsBuffer.getSize() );
						}

						it->second.try_emplace( node.node->getId()
							, PipelineNodesDescriptors{ pipelineId, pit->second.get() } );
					}
				}
			}

			reRecordCurrent();
		}
	}

	PipelineFlags VisibilityResolvePass::createPipelineFlags( PassComponentCombine const & passComponents
		, SubmeshComponentCombine const & submeshComponents
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets
		, SubmeshRenderData * submeshData )const
	{
		auto result = m_nodesPass.createPipelineFlags( passComponents
			, submeshComponents
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, lightingModelId
			, backgroundModelId
			, alphaFunc
			, blendAlphaFunc
			, textures
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, passLayerIndex
			, morphTargets
			, submeshData );
		result.m_shaderFlags = getShaderFlags();
		return result;
	}

	bool VisibilityResolvePass::areValidPassFlags( PassComponentCombine const & passFlags )const noexcept
	{
		return m_nodesPass.areValidPassFlags( passFlags );
	}

	ShaderFlags VisibilityResolvePass::getShaderFlags()const noexcept
	{
		return ShaderFlag::eNormal
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eWorldSpace
			| ShaderFlag::eViewSpace
			| ShaderFlag::eVelocity
			| ShaderFlag::eOpacity
			| ShaderFlag::eColour;
	}

	void VisibilityResolvePass::countNodes( RenderInfo & info )const noexcept
	{
		if ( m_deferredLightingFilter != DeferredLightingFilter::eDeferredOnly )
		{
			m_nodesPass.countNodes( info );
		}

		info.drawCalls += m_drawCalls;
	}

	bool VisibilityResolvePass::isPassEnabled()const noexcept
	{
		return m_nodesPass.isPassEnabled()
			&& doIsEnabled();
	}

	ComponentModeFlags VisibilityResolvePass::getComponentsMask()noexcept
	{
		return ( ComponentModeFlag::eColour
			| ComponentModeFlag::eOpacity
			| ComponentModeFlag::eDiffuseLighting
			| ComponentModeFlag::eSpecularLighting
			| ComponentModeFlag::eNormals
			| ComponentModeFlag::eGeometry
			| ComponentModeFlag::eOcclusion );
	}

	bool VisibilityResolvePass::useCompute()noexcept
	{
		return visres::useCompute;
	}

	void VisibilityResolvePass::doAccept( castor3d::RenderTechniqueVisitor & visitor )
	{
		if ( visitor.getFlags().renderPassType == m_nodesPass.getTypeID()
			&& visitor.config.allowProgramsVisit )
		{
			auto pipelineFlags = visitor.getFlags();
			m_nodesPass.forceAdjustFlags( pipelineFlags );

			if ( pipelineFlags.pass.hasParallaxOcclusionMappingOneFlag
				|| pipelineFlags.pass.hasParallaxOcclusionMappingRepeatFlag
				|| ( m_deferredLightingFilter == DeferredLightingFilter::eDeferredOnly
					&& !pipelineFlags.pass.hasDeferredDiffuseLightingFlag ) )
			{
				return;
			}

			ShaderModule shaderModule{ ( useCompute() ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_FRAGMENT_BIT )
				, getName()
				, visres::getProgram( m_device
					, getScene()
					, *m_parent
					, m_parent->getNormal().getExtent()
					, visitor.getFlags()
					, &getIndirectLighting()
					, getDebugConfig()
					, 0u
					, hasSsao()
					, *getClustersConfig()
					, m_outputScattering
					, m_deferredLightingFilter
					, m_nodesPass.isMeshShading()
					, areDebugTargetsEnabled() ) };
			makeShaderState( m_device, shaderModule );
			visitor.visit( shaderModule );
		}
	}

	bool VisibilityResolvePass::doIsEnabled()const noexcept
	{
		return !m_activePipelines.empty()
			|| !m_activeBillboardPipelines.empty();
	}

	void VisibilityResolvePass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t )
	{
		if ( useCompute() )
		{
			doRecordCompute( context, commandBuffer );
		}
		else
		{
			doRecordGraphics( context, commandBuffer );
		}

		m_commandsChanged = false;
	}

	void VisibilityResolvePass::doRecordCompute( crg::RecordContext const & context
		, VkCommandBuffer commandBuffer )
	{
		m_drawCalls = {};
		castor::Array< VkDescriptorSet, 3u > descriptorSets{ VkDescriptorSet{}
			, VkDescriptorSet{}
			, *getScene().getBindlessTexDescriptorSet() };
		visres::PushData pushData{ 0u, 0u };

		for ( auto const & [pipeline, descriptors] : m_activePipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline->pipeline );
			descriptorSets[0] = *pipeline->ioDescriptorSet;

			for ( auto const & [descriptor, ids] : descriptors )
			{
				descriptorSets[1] = *descriptor;
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, *pipeline->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );

				for ( auto pipelineId : ids )
				{
					pushData.pipelineId = pipelineId;
					context.getContext().vkCmdPushConstants( commandBuffer
						, *pipeline->pipelineLayout
						, VK_SHADER_STAGE_COMPUTE_BIT
						, 0u
						, sizeof( visres::PushData )
						, &pushData );
					context.getContext().vkCmdDispatchIndirect( commandBuffer, getTechnique().getMaterialsIndirectCounts(), pushData.pipelineId * sizeof( castor::Point3ui ) );
					++m_drawCalls;
				}
			}
		}

		for ( auto const & [pipeline, descriptors] : m_activeBillboardPipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipeline->pipeline );
			descriptorSets[0] = *pipeline->ioDescriptorSet;

			for ( auto & [id, descriptor] : descriptors )
			{
				descriptorSets[1] = *descriptor.vtxDescriptorSet;
				pushData.pipelineId = descriptor.pipelineId;
				pushData.billboardNodeId = id;
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipeline->pipelineLayout
					, VK_SHADER_STAGE_COMPUTE_BIT
					, 0u
					, sizeof( visres::PushData )
					, &pushData );
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, *pipeline->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDispatchIndirect( commandBuffer, getTechnique().getMaterialsIndirectCounts(), pushData.pipelineId * sizeof( castor::Point3ui ) );
				++m_drawCalls;
			}
		}
	}

	void VisibilityResolvePass::doRecordGraphics( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		m_drawCalls = {};
		castor::Array< VkDescriptorSet, 3u > descriptorSets{ VkDescriptorSet{}
			, VkDescriptorSet{}
			, *getScene().getBindlessTexDescriptorSet() };
		visres::PushData pushData{ 0u, 0u };
		castor::Vector< VkClearValue > clearValues;
		clearValues.push_back( transparentBlackClearColor );
		clearValues.push_back( transparentBlackClearColor );
		auto & extent = m_parent->getNormal().getExtent();
		VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
			, nullptr
			, VkRenderPass{}
			, VkFramebuffer{}
			, VkRect2D{ { 0, 0 }, { extent.width, extent.height } }
			, uint32_t( clearValues.size() )
			, clearValues.data() };
		beginInfo.renderPass = *m_renderPass;
		beginInfo.framebuffer = *m_framebuffer;
		context.getContext().vkCmdBeginRenderPass( commandBuffer
			, &beginInfo
			, VK_SUBPASS_CONTENTS_INLINE );

		for ( auto const & [pipeline, descriptors] : m_activePipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, *pipeline->pipeline );
			descriptorSets[0] = *pipeline->ioDescriptorSet;

			for ( auto & [descriptor, ids] : descriptors )
			{
				descriptorSets[1] = *descriptor;
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipeline->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );

				for ( auto pipelineId : ids )
				{
					pushData.pipelineId = pipelineId;
					context.getContext().vkCmdPushConstants( commandBuffer
						, *pipeline->pipelineLayout
						, VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( visres::PushData )
						, &pushData );
					context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );
					++m_drawCalls;
				}
			}
		}

		for ( auto const & [pipeline, descriptors] : m_activeBillboardPipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, *pipeline->pipeline );
			descriptorSets[0] = *pipeline->ioDescriptorSet;

			for ( auto & [id, descriptor] : descriptors )
			{
				descriptorSets[1] = *descriptor.vtxDescriptorSet;
				pushData.pipelineId = descriptor.pipelineId;
				pushData.billboardNodeId = id;
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipeline->pipelineLayout
					, VK_SHADER_STAGE_FRAGMENT_BIT
					, 0u
					, sizeof( visres::PushData )
					, &pushData );
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipeline->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );
				++m_drawCalls;
			}
		}

		context.getContext().vkCmdEndRenderPass( commandBuffer );
		context.setLayoutState( m_targetImage.front()
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineFlags const & flags
		, uint32_t stride )
	{
		return doCreatePipeline( flags
			, stride
			, m_billboardPipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineFlags const & flags )
	{
		return doCreatePipeline( flags
			, 0u
			, m_pipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineFlags const & flags
		, uint32_t stride
		, PipelineContainer & pipelines )
	{
		auto it = std::find_if( pipelines.begin()
			, pipelines.end()
			, [&flags]( PipelinePtr const & lookup )
			{
				return lookup->flags == flags;
			} );

		if ( it == pipelines.end() )
		{
			auto stageBit = useCompute()
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT;
			auto stageFlags = VkShaderStageFlags( stageBit );
			auto extent = m_parent->getNormal().getExtent();
			auto result = castor::make_unique< Pipeline >( flags );
			result->flags = flags;
			result->vtxDescriptorLayout = stride == 0u
				? visres::createVtxDescriptorLayout( m_device, getName(), flags, m_nodesPass.isMeshShading() )
				: visres::createVtxDescriptorLayout( m_device, getName() );
			result->ioDescriptorLayout = visres::createInDescriptorLayout( m_device, getName(), getScene().getOwner()->getMaterialCache()
				, getScene(), *m_parent, getClustersConfig()->enabled, hasSsao() ? m_ssao : nullptr, &getIndirectLighting() );
			result->pipelineLayout = m_device->createPipelineLayout( castor::toUtf8( getName() )
				, { *result->ioDescriptorLayout, *result->vtxDescriptorLayout, *getScene().getBindlessTexDescriptorLayout() }
				, { { stageFlags, 0u, sizeof( visres::PushData ) } } );

			result->shader = ProgramModule{ getName()
				, visres::getProgram( m_device
					, getScene()
					, *m_parent
					, extent
					, flags
					, &getIndirectLighting()
					, getDebugConfig()
					, stride
					, hasSsao()
					, *getClustersConfig()
					, m_outputScattering
					, m_deferredLightingFilter
					, m_nodesPass.isMeshShading()
					, areDebugTargetsEnabled() ) };
			auto stages = makeProgramStates( m_device, result->shader );

			if ( useCompute() )
			{
				result->pipeline = m_device->createPipeline( ashes::ComputePipelineCreateInfo{ 0u
					, castor::move( stages.front() )
					, *result->pipelineLayout } );
			}
			else
			{
				result->pipeline = visres::createPipeline( m_device
					, extent
					, castor::move( stages )
					, *result->pipelineLayout
					, *m_renderPass
					, m_outputScattering ? &getTechnique().getScattering() : nullptr
					, false );
			}

			result->vtxDescriptorPool = result->vtxDescriptorLayout->createPool( MaxPipelines );
			result->ioDescriptorPool = result->ioDescriptorLayout->createPool( 1u );
			result->ioDescriptorSet = visres::createInDescriptorSet( getName(), *result->ioDescriptorPool, m_graph, m_cameraUbo, m_sceneUbo, *m_parent, getScene()
				, getClustersConfig()->enabled, m_targetImage, hasSsao() ? m_ssao : nullptr, &getIndirectLighting(), m_deferredLightingFilter );
			pipelines.push_back( castor::move( result ) );
			it = std::next( pipelines.begin(), ptrdiff_t( pipelines.size() - 1u ) );
		}

		return **it;
	}
}
