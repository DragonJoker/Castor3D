#include "Castor3D/Render/Technique/Visibility/VisibilityResolvePass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/Technique/Visibility/VisibilityPassResult.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/BillboardUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	//*********************************************************************************************

#define DeclareSsbo( Name, Type, Binding, Enable )\
	sdw::StorageBuffer Name##Buffer{ writer\
		, #Name + std::string{ "Buffer" }\
		, Binding\
		, Sets::eVtx\
		, ast::type::MemoryLayout::eStd430\
		, Enable };\
	auto Name = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
	Name##Buffer.end()

	//*********************************************************************************************

	namespace visres
	{
		enum Sets : uint32_t
		{
			eInOuts,
			eVtx,
			eTex,
		};

		enum InOutBindings : uint32_t
		{
			eMatrix,
			eScene,
			eModels,
			eBillboards,
			eMaterials,
			eTexConfigs,
			eTexAnims,
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
			eSsao,
			eInData,
			eOutData0,
			eOutData1,
			eOutData2,
			eOutData3,
			eOutData4,
			eOutData5,
		};

		enum VtxBindings : uint32_t
		{
			eInIndices,
			eInPosition,
			eInNormal,
			eInTangent,
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

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::FragmentWriter;

			static castor3d::ShaderPtr getVertexProgram()
			{
				sdw::VertexWriter writer;

				writer.implementMain( [&]( sdw::VertexIn in
					, sdw::VertexOut out )
					{
						auto position = writer.declLocale( "position"
							, vec2( ( in.vertexIndex << 1 ) & 2, in.vertexIndex & 2 ) );
						out.vtx.position = vec4( position * 2.0_f - 1.0_f, 0.0_f, 1.0_f );
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMain( [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
					{
						func( ivec2( in.fragCoord.xy() ) );
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Type = sdw::ComputeWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 64u, 1u, 1u }
					, [&]( sdw::ComputeIn in )
					{
						func( in.globalInvocationID.xy() );
					} );
			}
		};

		struct BarycentricFullDerivatives;
		Writer_Parameter( BarycentricFullDerivatives );

		struct BarycentricFullDerivatives
			: public sdw::StructInstanceHelperT< "BarycentricFullDerivatives"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "lambda" >
			, sdw::Vec3Field< "dx" >
			, sdw::Vec3Field< "dy" > >
		{
			BarycentricFullDerivatives( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
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
				, sdw::Vec3 const & v2 )
			{
				return vec3( interpolate( vec3( v0.x(), v1.x(), v2.x() ) )
					, interpolate( vec3( v0.y(), v1.y(), v2.y() ) )
					, interpolate( vec3( v0.z(), v1.z(), v2.z() ) ) );
			}

			sdw::Vec4 interpolate( sdw::Vec4 const & v0
				, sdw::Vec4 const & v1
				, sdw::Vec4 const & v2 )
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
						, [&]( BarycentricFullDerivatives const & derivatives
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
							result.dx() = vec2( resultsX.y(), resultsY.y() );
							result.dy() = vec2( resultsX.z(), resultsY.z() );
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
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
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

		static ShaderPtr getProgram( RenderDevice const & device
			, VkExtent3D const & imageSize
			, PassTypeID passType
			, TextureFlags textureFlags
			, SubmeshFlags submeshFlags
			, PassFlags passFlags
			, uint32_t stride )
		{
			auto & engine = *device.renderSystem.getEngine();
			ShaderWriter< VisibilityResolvePass::useCompute >::Type writer;

			DeclareSsbo( c3d_inIndices
				, sdw::UInt
				, VtxBindings::eInIndices
				, checkFlag( submeshFlags, SubmeshFlag::eIndex ) );
			sdw::StorageBuffer c3d_inPositionBuffer{ writer
				, std::string{ "c3d_inPositionBuffer" }
				, VtxBindings::eInPosition
				, Sets::eVtx
				, ast::type::MemoryLayout::eStd430
				, checkFlag( submeshFlags, SubmeshFlag::ePositions ) };
			auto c3d_inPosition = c3d_inPositionBuffer.declMemberArray< Position >( "c3d_inPosition"
				, checkFlag( submeshFlags, SubmeshFlag::ePositions )
				, stride );
			c3d_inPositionBuffer.end();
			DeclareSsbo( c3d_inNormal
				, sdw::Vec4
				, VtxBindings::eInNormal
				, checkFlag( submeshFlags, SubmeshFlag::eNormals ) );
			DeclareSsbo( c3d_inTangent
				, sdw::Vec4
				, VtxBindings::eInTangent
				, checkFlag( submeshFlags, SubmeshFlag::eTangents ) );
			DeclareSsbo( c3d_inTexcoord0
				, sdw::Vec4
				, VtxBindings::eInTexcoord0
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) && ( stride == 0u ) );
			DeclareSsbo( c3d_inTexcoord1
				, sdw::Vec4
				, VtxBindings::eInTexcoord1
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) );
			DeclareSsbo( c3d_inTexcoord2
				, sdw::Vec4
				, VtxBindings::eInTexcoord2
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) );
			DeclareSsbo( c3d_inTexcoord3
				, sdw::Vec4
				, VtxBindings::eInTexcoord3
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) );
			DeclareSsbo( c3d_inColour
				, sdw::Vec4
				, VtxBindings::eInColour
				, checkFlag( submeshFlags, SubmeshFlag::eColours ) );
			DeclareSsbo( c3d_inVelocity
				, sdw::Vec4
				, VtxBindings::eInVelocity
				, checkFlag( submeshFlags, SubmeshFlag::eVelocity ) );
			DeclareSsbo( c3d_inPassMasks
				, sdw::UVec4
				, VtxBindings::eInPassMasks
				, checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) );

			C3D_Matrix( writer
				, InOutBindings::eMatrix
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
			shader::Materials materials{ writer
				, InOutBindings::eMaterials
				, Sets::eInOuts };
			shader::TextureConfigurations textureConfigs{ writer
				, InOutBindings::eTexConfigs
				, Sets::eInOuts };
			shader::TextureAnimations textureAnims{ writer
				, InOutBindings::eTexAnims
				, Sets::eInOuts };

			auto MaterialsCounts = writer.declStorageBuffer<>( "MaterialsCounts", InOutBindings::eMaterialsCounts, Sets::eInOuts );
			auto materialsCounts = MaterialsCounts.declMemberArray< sdw::UInt >( "materialsCounts" );
			MaterialsCounts.end();

			auto MaterialsStarts = writer.declStorageBuffer<>( "MaterialsStarts", InOutBindings::eMaterialsStarts, Sets::eInOuts );
			auto materialsStarts = MaterialsStarts.declMemberArray< sdw::UInt >( "materialsStarts" );
			MaterialsStarts.end();

			auto PixelsXY = writer.declStorageBuffer<>( "PixelsXY", InOutBindings::ePixelsXY, Sets::eInOuts );
			auto pixelsXY = PixelsXY.declMemberArray< sdw::UVec2 >( "pixelsXY" );
			PixelsXY.end();

			auto imgData = writer.declStorageImg< sdw::RUImage2DRgba32 >( "imgData", InOutBindings::eInData, Sets::eInOuts );

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps", TexBindings::eTextures, Sets::eTex ) );

			sdw::PushConstantBuffer pcb{ writer, "DrawData" };
			auto passTypeIndex = pcb.declMember< sdw::UInt >( "passTypeIndex" );
			auto billboardNodeId = pcb.declMember< sdw::UInt >( "billboardNodeId", stride != 0u );
			pcb.end();

			shader::Utils utils{ writer };
			auto lightingModel = utils.createLightingModel( engine
				, shader::getLightingModelName( engine, passType )
				, {}
				, nullptr
				, true );

			auto depthLinearization = []( sdw::Float const & d
				, sdw::Float const & n
				, sdw::Float const & f )
			{
				return ( 2.0_f * n ) / ( f + n - d * ( f - n ) );
			};

			auto calcFullBarycentric = writer.implementFunction< BarycentricFullDerivatives >( "calcFullBarycentric"
				, [&]( sdw::Vec4 const & pt0
					, sdw::Vec4 const & pt1
					, sdw::Vec4 const & pt2
					, sdw::Vec2 const & pixelNdc
					, sdw::Vec2 const & winSize )
				{
					auto result = writer.declLocale< BarycentricFullDerivatives >( "result" );

					auto invW = writer.declLocale( "invW"
						, vec3( 1.0_f ) / vec3( pt0.w(), pt1.w(), pt2.w() ) );

					auto ndc0 = writer.declLocale( "ndc0"
						, pt0.xy() * invW.x() );
					auto ndc1 = writer.declLocale( "ndc1"
						, pt1.xy() * invW.y() );
					auto ndc2 = writer.declLocale( "ndc2"
						, pt2.xy() * invW.z() );

					auto invDet = writer.declLocale( "invDet"
						, 1.0_f / determinant( mat2( ndc2 - ndc1, ndc0 - ndc1 ) ) );
					result.dx() = vec3( ndc1.y() - ndc2.y(), ndc2.y() - ndc0.y(), ndc0.y() - ndc1.y() ) * invDet * invW;
					result.dy() = vec3( ndc2.x() - ndc1.x(), ndc0.x() - ndc2.x(), ndc1.x() - ndc0.x() ) * invDet * invW;
					auto ddxSum = writer.declLocale( "ddxSum"
						, dot( result.dx(), vec3( 1.0_f ) ) );
					auto ddySum = writer.declLocale( "ddySum"
						, dot( result.dy(), vec3( 1.0_f ) ) );

					auto deltaVec = writer.declLocale( "deltaVec"
						, pixelNdc - ndc0 );
					auto interpInvW = writer.declLocale( "interpInvW"
						, invW.x() + deltaVec.x() * ddxSum + deltaVec.y() * ddySum );
					auto interpW = writer.declLocale( "interpW"
						, 1.0_f / interpInvW );

					result.lambda() = vec3( interpW * ( invW[0] + deltaVec.x() * result.dx().x() + deltaVec.y() * result.dy().x() )
						, interpW * ( 0.0_f + deltaVec.x() * result.dx().y() + deltaVec.y() * result.dy().y() )
						, interpW * ( 0.0_f + deltaVec.x() * result.dx().z() + deltaVec.y() * result.dy().z() ) );

					result.dx() *= ( 2.0_f / winSize.x() );
					result.dy() *= ( 2.0_f / winSize.y() );
					ddxSum *= ( 2.0_f / winSize.x() );
					ddySum *= ( 2.0_f / winSize.y() );

					result.dy() *= -1.0_f;
					ddySum *= -1.0_f;

					auto interpW_ddx = writer.declLocale( "interpW_ddx"
						, 1.0_f / ( interpInvW + ddxSum ) );
					auto interpW_ddy = writer.declLocale( "interpW_ddy"
						, 1.0_f / ( interpInvW + ddySum ) );

					result.dx() = interpW_ddx * ( result.lambda() * interpInvW + result.dx() ) - result.lambda();
					result.dy() = interpW_ddy * ( result.lambda() * interpInvW + result.dy() ) - result.lambda();

					writer.returnStmt( result );
				}
				, sdw::InVec4{ writer, "pt0" }
				, sdw::InVec4{ writer, "pt1" }
				, sdw::InVec4{ writer, "pt2" }
				, sdw::InVec2{ writer, "pixelNdc" }
				, sdw::InVec2{ writer, "winSize" } );

			auto loadVertices = writer.implementFunction< sdw::Void >( "loadVertices"
				, [&]( sdw::UInt const & nodeId
					, sdw::UInt const & primitiveId
					, shader::ModelIndices const & modelData
					, shader::VertexSurface v0
					, shader::VertexSurface v1
					, shader::VertexSurface v2 )
				{
					if ( stride == 0u )
					{
						auto loadVertex = writer.implementFunction< shader::VertexSurface >( "loadVertex"
							, [&]( sdw::UInt const & vertexId )
							{
								auto result = writer.declLocale< shader::VertexSurface >( std::string( "result" ) );
								result.position = c3d_inPosition[vertexId].position;
								result.normal = c3d_inNormal[vertexId].xyz();
								result.tangent = c3d_inTangent[vertexId].xyz();
								result.texture0 = c3d_inTexcoord0[vertexId].xyz();
								result.texture1 = c3d_inTexcoord1[vertexId].xyz();
								result.texture2 = c3d_inTexcoord2[vertexId].xyz();
								result.texture3 = c3d_inTexcoord3[vertexId].xyz();
								result.colour = c3d_inColour[vertexId].xyz();
								result.passMasks = c3d_inPassMasks[vertexId];
								result.velocity = c3d_inVelocity[vertexId].xyz();

								writer.returnStmt( result );
							}
							, sdw::InUInt{ writer, "vertexId" } );

						auto baseIndex = writer.declLocale( "baseIndex"
							, modelData.getIndexOffset() + primitiveId * 3u );
						auto indices = writer.declLocale( "indices"
							, uvec3( c3d_inIndices[baseIndex + 0u]
								, c3d_inIndices[baseIndex + 1u]
								, c3d_inIndices[baseIndex + 2u] ) );
						auto baseVertex = writer.declLocale( "baseVertex"
							, modelData.getVertexOffset() );
						v0 = loadVertex( baseVertex + indices.x() );
						v1 = loadVertex( baseVertex + indices.y() );
						v2 = loadVertex( baseVertex + indices.z() );
					}
					else
					{
						auto bbPositions = writer.declConstantArray( "bbPositions"
							, std::vector< sdw::Vec3 >{ vec3( -0.5_f, -0.5_f, 1.0_f )
							  , vec3( -0.5_f, +0.5_f, 1.0_f )
							  , vec3( +0.5_f, -0.5_f, 1.0_f )
							  , vec3( +0.5_f, +0.5_f, 1.0_f ) } );
						auto bbTexcoords = writer.declConstantArray( "bbTexcoords"
							, std::vector< sdw::Vec2 >{ vec2( 0.0_f, 0.0_f )
							  , vec2( 0.0_f, 1.0_f )
							  , vec2( 1.0_f, 0.0_f )
							  , vec2( 1.0_f, 1.0_f ) } );

						auto instanceId = writer.declLocale( "instanceId"
							, primitiveId / 2u );
						auto firstTriangle = writer.declLocale( "firstTriangle"
							, ( primitiveId % 2u ) == 0u );
						auto center = writer.declLocale( "center"
							, c3d_inPosition[instanceId].position );
						auto bbcenter = writer.declLocale( "bbcenter"
							, modelData.modelToCurWorld( center ).xyz() );
						auto centerToCamera = writer.declLocale( "centerToCamera"
							, c3d_sceneData.getPosToCamera( bbcenter ) );
						centerToCamera.y() = 0.0_f;
						centerToCamera = normalize( centerToCamera );

						auto billboardData = writer.declLocale( "billboardData"
							, c3d_billboardData[nodeId - 1u] );
						auto right = writer.declLocale( "right"
							, billboardData.getCameraRight( c3d_matrixData ) );
						auto up = writer.declLocale( "up"
							, billboardData.getCameraUp( c3d_matrixData ) );
						auto width = writer.declLocale( "width"
							, billboardData.getWidth( c3d_sceneData ) );
						auto height = writer.declLocale( "height"
							, billboardData.getHeight( c3d_sceneData ) );

						auto vertexId = writer.declLocale( "vertexId"
							, writer.ternary( firstTriangle
								, uvec3( 0_u, 1_u, 2_u )
								, uvec3( 1_u, 3_u, 2_u ) ) );
						auto scaledRight = writer.declLocale( "scaledRight", vec3( 0.0_f ) );
						auto scaledUp = writer.declLocale( "scaledUp", vec3( 0.0_f ) );

						scaledRight = right * bbPositions[vertexId.x()].x() * width;
						scaledUp = up * bbPositions[vertexId.x()].y() * height;
						v0.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
						v0.texture0 = vec3( bbTexcoords[vertexId.x()], 0.0_f );

						scaledRight = right * bbPositions[vertexId.y()].x() * width;
						scaledUp = up * bbPositions[vertexId.y()].y() * height;
						v1.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
						v1.texture0 = vec3( bbTexcoords[vertexId.y()], 0.0_f );

						scaledRight = right * bbPositions[vertexId.z()].x() * width;
						scaledUp = up * bbPositions[vertexId.z()].y() * height;
						v2.position = vec4( ( bbcenter + scaledRight + scaledUp ), 1.0_f );
						v2.texture0 = vec3( bbTexcoords[vertexId.z()], 0.0_f );
					}
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "primitiveId" }
				, shader::InModelIndices{ writer, "modelData" }
				, shader::OutVertexSurface{ writer, "v0" }
				, shader::OutVertexSurface{ writer, "v1" }
				, shader::OutVertexSurface{ writer, "v2" } );

			auto loadSurface = writer.implementFunction< shader::DerivFragmentSurface >( "loadSurface"
				, [&]( sdw::UInt const & nodeId
					, sdw::UInt const & primitiveId
					, sdw::Vec2 const & pixelCoord
					, shader::ModelIndices const & modelData
					, shader::Material const & material )
				{
					auto result = writer.declLocale< shader::DerivFragmentSurface >( "result" );
					result.worldPosition = vec4( 0.0_f );
					result.viewPosition = vec4( 0.0_f );
					result.curPosition = vec4( 0.0_f );
					result.prvPosition = vec4( 0.0_f );
					result.tangentSpaceFragPosition = vec3( 0.0_f );
					result.tangentSpaceViewPosition = vec3( 0.0_f );
					result.normal = vec3( 0.0_f );
					result.tangent = vec3( 0.0_f );
					result.bitangent = vec3( 0.0_f );
					result.texture0.uv() = vec2( 0.0_f );
					result.texture0.dx() = vec2( 0.0_f );
					result.texture0.dy() = vec2( 0.0_f );
					result.texture1.uv() = vec2( 0.0_f );
					result.texture1.dx() = vec2( 0.0_f );
					result.texture1.dy() = vec2( 0.0_f );
					result.texture2.uv() = vec2( 0.0_f );
					result.texture2.dx() = vec2( 0.0_f );
					result.texture2.dy() = vec2( 0.0_f );
					result.texture3.uv() = vec2( 0.0_f );
					result.texture3.dx() = vec2( 0.0_f );
					result.texture3.dy() = vec2( 0.0_f );
					result.colour = vec3( 1.0_f );

					auto hdrCoords = writer.declLocale( "hdrCoords"
						, pixelCoord / c3d_sceneData.renderSize );
					auto screenCoords = writer.declLocale( "screenCoords"
						, fma( hdrCoords, vec2( 2.0_f ), vec2( -1.0_f ) ) );

					auto v0 = writer.declLocale< shader::VertexSurface >( "v0" );
					auto v1 = writer.declLocale< shader::VertexSurface >( "v1" );
					auto v2 = writer.declLocale< shader::VertexSurface >( "v2" );
					loadVertices( nodeId, primitiveId, modelData, v0, v1, v2 );

					bool isWorldPos = checkFlag( submeshFlags, SubmeshFlag::eVelocity )
						|| ( stride != 0u );

					// Transform positions to clip space
					auto p0 = writer.declLocale( "p0"
						, c3d_matrixData.worldToCurProj( isWorldPos
							? v0.position
							: modelData.modelToCurWorld( v0.position ) ) );
					auto p1 = writer.declLocale( "p1"
						, c3d_matrixData.worldToCurProj( isWorldPos
							? v1.position
							: modelData.modelToCurWorld( v1.position ) ) );
					auto p2 = writer.declLocale( "p2"
						, c3d_matrixData.worldToCurProj( isWorldPos
							? v2.position
							: modelData.modelToCurWorld( v2.position ) ) );

					auto derivatives = writer.declLocale( "derivatives"
						, calcFullBarycentric( p0, p1, p2, screenCoords, c3d_sceneData.renderSize ) );

					// Interpolate texture coordinates and calculate the gradients for texture sampling with mipmapping support
					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
					{
						result.texture0 = derivatives.computeGradient( v0.texture0
							, v1.texture0
							, v2.texture0 );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
					{
						result.texture1 = derivatives.computeGradient( v0.texture1
							, v1.texture1
							, v2.texture1 );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
					{
						result.texture2 = derivatives.computeGradient( v0.texture2
							, v1.texture2
							, v2.texture2 );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
					{
						result.texture3 = derivatives.computeGradient( v0.texture3
							, v1.texture3
							, v2.texture3 );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
					{
						result.colour = derivatives.interpolate( v0.colour.xyz()
							, v1.colour.xyz()
							, v2.colour.xyz() );
					}

					auto normal = writer.declLocale< sdw::Vec3 >( "normal"
						, vec3( 0.0_f )
						, checkFlag( submeshFlags, SubmeshFlag::eNormals ) );

					if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
					{
						normal = derivatives.interpolate( v0.normal.xyz()
							, v1.normal.xyz()
							, v2.normal.xyz() );
					}

					auto tangent = writer.declLocale< sdw::Vec3 >( "tangent"
						, vec3( 0.0_f )
						, checkFlag( submeshFlags, SubmeshFlag::eTangents ) );

					if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
					{
						tangent = derivatives.interpolate( v0.tangent.xyz()
							, v1.tangent.xyz()
							, v2.tangent.xyz() );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
					{
						auto passMultipliers0 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers0", 4u );
						auto passMultipliers1 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers1", 4u );
						auto passMultipliers2 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers2", 4u );
						material.getPassMultipliers( submeshFlags
							, v0.passMasks
							, passMultipliers0 );
						material.getPassMultipliers( submeshFlags
							, v1.passMasks
							, passMultipliers1 );
						material.getPassMultipliers( submeshFlags
							, v2.passMasks
							, passMultipliers2 );

						for ( uint32_t i = 0u; i < 4u; ++i )
						{
							result.passMultipliers[i] = derivatives.interpolate( passMultipliers0[i]
								, passMultipliers1[i]
								, passMultipliers2[i] );
						}
					}

					auto curPosition = writer.declLocale( "curPosition"
						, vec4( derivatives.interpolate( v0.position.xyz()
								, v1.position.xyz()
								, v2.position.xyz() )
							, 1.0_f ) );
					auto prvPosition = writer.declLocale( "prvPosition"
						, curPosition );

					if ( stride > 0u )
					{
						if ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) )
						{
							auto velocity = writer.declLocale( "velocity"
								, derivatives.interpolate( v0.velocity.xyz()
									, v1.velocity.xyz()
									, v2.velocity.xyz() ) );
							prvPosition.xyz() += velocity;
							prvPosition = c3d_matrixData.worldToPrvProj( prvPosition );

							if ( checkFlag( submeshFlags, SubmeshFlag::eNormals )
								&& checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
							{
								auto curMtxModel = writer.declLocale( "curMtxModel"
									, modelData.getModelMtx() );
								auto mtxNormal = writer.declLocale( "mtxNormal"
									, modelData.getNormalMtx( ProgramFlag::eNone, curMtxModel ) );
								normal = normalize( mtxNormal * normal );
								tangent = normalize( mtxNormal * tangent );
							}
						}
						else
						{
							curPosition = modelData.modelToCurWorld( curPosition );
							prvPosition = modelData.modelToPrvWorld( prvPosition );
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
					{
						result.computeTangentSpace( submeshFlags
							, ProgramFlag::eNone
							, c3d_sceneData.cameraPosition
							, curPosition.xyz()
							, normal
							, tangent );
					}

					result.worldPosition = curPosition;
					curPosition = c3d_matrixData.worldToCurProj( curPosition );
					prvPosition = c3d_matrixData.worldToPrvProj( prvPosition );
					result.viewPosition = utils.clipToScreen( curPosition );

					result.computeVelocity( c3d_matrixData
						, curPosition
						, prvPosition );

					writer.returnStmt( result );
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "primitiveId" }
				, sdw::InVec2{ writer, "pixelCoord" }
				, shader::InModelIndices{ writer, "modelData" }
				, shader::InMaterial{ writer, "material" } );

			auto shade = writer.implementFunction< sdw::Boolean >( "shade"
				, [&]( sdw::IVec2 const & ipixel
					, sdw::Vec4 outData0
					, sdw::Vec4 outData1
					, sdw::Vec4 outData2
					, sdw::Vec4 outData3
					, sdw::Vec4 outData4
					, sdw::Vec4 outData5 )
				{
					auto data = writer.declLocale( "data"
						, imgData.load( ipixel ) );
					auto nodeId = writer.declLocale( "nodeId"
						, data.x() );

					IF( writer, nodeId == 0_u )
					{
						writer.returnStmt( 0_b );
					}
					FI;

					if ( stride != 0u )
					{
						IF( writer, nodeId != billboardNodeId )
						{
							writer.returnStmt( 0_b );
						}
						FI;
					}

					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );

					IF( writer, material.passCount() > 1u )
					{
						auto i = writer.declLocale( "i", 1_u );

						WHILE( writer, material.passTypeIndex() != passTypeIndex
							&& i < material.passCount() )
						{
							material = materials.getMaterial( modelData.getMaterialId() + i );
							++i;
						}
						ELIHW;
					}
					FI;

					IF( writer, material.passTypeIndex() != passTypeIndex )
					{
						writer.returnStmt( 0_b );
					}
					FI;

					auto primitiveId = writer.declLocale( "primitiveId"
						, data.z() );
					auto surface = writer.declLocale( "surface"
						, loadSurface( nodeId
							, primitiveId
							, vec2( ipixel )
							, modelData
							, material ) );
					shader::VisibilityBlendComponents components{ writer
						, "out"
						, { surface.texture0, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) }
						, { surface.texture1, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) }
						, { surface.texture2, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) }
						, { surface.texture3, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) }
						, 1.0_f
						, { normalize( surface.normal ), checkFlag( submeshFlags, SubmeshFlag::eNormals ) }
						, { normalize( surface.tangent ), checkFlag( submeshFlags, SubmeshFlag::eTangents ) }
						, { normalize( surface.bitangent ), checkFlag( submeshFlags, SubmeshFlag::eTangents ) }
						, { surface.tangentSpaceViewPosition, checkFlag( submeshFlags, SubmeshFlag::eTangents ) }
						, { surface.tangentSpaceFragPosition, checkFlag( submeshFlags, SubmeshFlag::eTangents ) }
						, 1.0_f
						, 1.0_f
						, vec3( 0.0_f ) };
					auto lightMat = materials.blendMaterials( utils
						, passFlags
						, submeshFlags
						, textureFlags
						, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 )
						, textureConfigs
						, textureAnims
						, *lightingModel
						, c3d_maps
						, material
						, modelData.getMaterialId()
						, surface.passMultipliers
						, surface.colour
						, components );
					outData0 = vec4( surface.viewPosition.z()
						, length( surface.worldPosition.xyz() - c3d_sceneData.cameraPosition )
						, writer.cast< sdw::Float >( nodeId )
						, 0.0_f );
					outData1 = vec4( components.normal(), 0.0_f );
					auto data2 = writer.declLocale< sdw::Vec4 >( "data2" );
					auto data3 = writer.declLocale< sdw::Vec4 >( "data3" );
					lightMat->output( data2, data3 );
					outData2 = data2;
					outData3 = data3;
					outData4 = vec4( components.emissive(), components.transmittance() );
					outData5 = vec4( surface.getVelocity(), 0.0_f, components.occlusion() );
					writer.returnStmt( 1_b );
				}
				, sdw::InIVec2{ writer, "ipixel" }
				, sdw::OutVec4{ writer, "outData0" }
				, sdw::OutVec4{ writer, "outData1" }
				, sdw::OutVec4{ writer, "outData2" }
				, sdw::OutVec4{ writer, "outData3" }
				, sdw::OutVec4{ writer, "outData4" }
				, sdw::OutVec4{ writer, "outData5" } );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
				auto imgData0 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData0", InOutBindings::eOutData0, Sets::eInOuts );
				auto imgData1 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData1", InOutBindings::eOutData1, Sets::eInOuts );
				auto imgData2 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData2", InOutBindings::eOutData2, Sets::eInOuts );
				auto imgData3 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData3", InOutBindings::eOutData3, Sets::eInOuts );
				auto imgData4 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData4", InOutBindings::eOutData4, Sets::eInOuts );
				auto imgData5 = writer.declStorageImg< sdw::WImage2DRgba32 >( "imgData5", InOutBindings::eOutData5, Sets::eInOuts );

				ShaderWriter< VisibilityResolvePass::useCompute >::implementMain( writer
					, [&]( sdw::UVec2 const & pos )
					{
						auto pixelID = writer.declLocale( "pixelID"
							, materialsStarts[passTypeIndex] + pos.x() );
						auto pixel = writer.declLocale( "pixel"
							, pixelsXY[pixelID] );
						auto ipixel = writer.declLocale( "ipixel"
							, ivec2( pixel ) );
						auto data0 = writer.declLocale( "data0", vec4( 0.0_f ) );
						auto data1 = writer.declLocale( "data1", vec4( 0.0_f ) );
						auto data2 = writer.declLocale( "data2", vec4( 0.0_f ) );
						auto data3 = writer.declLocale( "data3", vec4( 0.0_f ) );
						auto data4 = writer.declLocale( "data4", vec4( 0.0_f ) );
						auto data5 = writer.declLocale( "data5", vec4( 0.0_f ) );

						IF( writer, shade( ipixel, data0, data1, data2, data3, data4, data5 ) )
						{
							imgData0.store( ipixel, data0 );
							imgData1.store( ipixel, data1 );
							imgData2.store( ipixel, data2 );
							imgData3.store( ipixel, data3 );
							imgData4.store( ipixel, data4 );
							imgData5.store( ipixel, data5 );
						}
						FI;
					} );
			}
			else
			{
				uint32_t idx = 0u;
				auto imgData0 = writer.declOutput< sdw::Vec4 >( "imgData0", idx++ );
				auto imgData1 = writer.declOutput< sdw::Vec4 >( "imgData1", idx++ );
				auto imgData2 = writer.declOutput< sdw::Vec4 >( "imgData2", idx++ );
				auto imgData3 = writer.declOutput< sdw::Vec4 >( "imgData3", idx++ );
				auto imgData4 = writer.declOutput< sdw::Vec4 >( "imgData4", idx++ );
				auto imgData5 = writer.declOutput< sdw::Vec4 >( "imgData5", idx++ );

				ShaderWriter< VisibilityResolvePass::useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & pos )
					{
						auto data0 = writer.declLocale( "data0", vec4( 0.0_f ) );
						auto data1 = writer.declLocale( "data1", vec4( 0.0_f ) );
						auto data2 = writer.declLocale( "data2", vec4( 0.0_f ) );
						auto data3 = writer.declLocale( "data3", vec4( 0.0_f ) );
						auto data4 = writer.declLocale( "data4", vec4( 0.0_f ) );
						auto data5 = writer.declLocale( "data5", vec4( 0.0_f ) );

						IF( writer, shade( pos, data0, data1, data2, data3, data4, data5 ) )
						{
							imgData0 = data0;
							imgData1 = data1;
							imgData2 = data2;
							imgData3 = data3;
							imgData4 = data4;
							imgData5 = data5;
						}
						ELSE
						{
							imgData0 = vec4( 0.0_f );
							imgData1 = vec4( 0.0_f );
							imgData2 = vec4( 0.0_f );
							imgData3 = vec4( 0.0_f );
							imgData4 = vec4( 0.0_f );
							imgData5 = vec4( 0.0_f );
						}
						FI;
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ashes::DescriptorSetLayoutPtr createInDescriptorLayout( RenderDevice const & device
			, std::string const & name
			, MaterialCache const & matCache )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMatrix
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
			bindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( InOutBindings::eTexConfigs
				, stages ) );
			bindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( InOutBindings::eTexAnims
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMaterialsCounts
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMaterialsStarts
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::ePixelsXY
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eInData
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, stages ) );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData0
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData1
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData2
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData3
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData4
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eOutData5
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
			}

			return device->createDescriptorSetLayout( name + "InOut"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createInDescriptorSet( std::string const & name
			, ashes::DescriptorSetPool const & pool
			, MatrixUbo const & matrixUbo
			, SceneUbo const & sceneUbo
			, RenderTechnique const & technique
			, Scene const & scene )
		{
			auto & matCache = scene.getOwner()->getMaterialCache();
			ashes::WriteDescriptorSetArray writes;
			writes.push_back( matrixUbo.getDescriptorWrite( InOutBindings::eMatrix ) );
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
			writes.push_back( matCache.getTexConfigBuffer().getBinding( InOutBindings::eTexConfigs ) );
			writes.push_back( matCache.getTexAnimBuffer().getBinding( InOutBindings::eTexAnims ) );
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
			auto & visibilityPassResult = technique.getVisibilityResult();
			writes.push_back( makeDescriptorWrite( visibilityPassResult[VbTexture::eData].targetView
				, InOutBindings::eInData ) );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
				auto & opaquePassResult = technique.getOpaqueResult();
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData0].targetView
					, InOutBindings::eOutData0 ) );
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData1].targetView
					, InOutBindings::eOutData1 ) );
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData2].targetView
					, InOutBindings::eOutData2 ) );
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData3].targetView
					, InOutBindings::eOutData3 ) );
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData4].targetView
					, InOutBindings::eOutData4 ) );
				writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData5].targetView
					, InOutBindings::eOutData5 ) );
			}

			auto result = pool.createDescriptorSet( name + "InOut"
				, Sets::eInOuts );
			result->setBindings( std::move( writes ) );
			result->update();
			return result;
		}

		static ashes::DescriptorSetLayoutPtr createVtxDescriptorLayout( RenderDevice const & device
			, std::string const & name )
		{
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPosition
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			return device->createDescriptorSetLayout( name + "Vtx"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetLayoutPtr createVtxDescriptorLayout( RenderDevice const & device
			, std::string const & name
			, SubmeshFlags const & submeshFlags )
		{
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;

			if ( checkFlag( submeshFlags, SubmeshFlag::eIndex ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInIndices
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPosition
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInNormal
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTangent
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord0
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord1
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord2
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInTexcoord3
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInColour
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInPassMasks
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( VtxBindings::eInVelocity
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, stages ) );
			}

			return device->createDescriptorSetLayout( name + "Vtx"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createVtxDescriptorSet( std::string const & name
			, ashes::DescriptorSetPool const & pool
			, ashes::BufferBase const & positionsBuffer
			, VkDeviceSize offset
			, VkDeviceSize range )
		{
			ashes::WriteDescriptorSetArray writes;
			writes.emplace_back( makeDescriptorWrite( positionsBuffer, VtxBindings::eInPosition, offset, range ) );
			auto result = pool.createDescriptorSet( name + "Vtx"
				, Sets::eVtx );
			result->setBindings( std::move( writes ) );
			result->update();
			return result;
		}

		static ashes::DescriptorSetPtr createVtxDescriptorSet( std::string const & name
			, ashes::DescriptorSetPool const & pool
			, ObjectBufferPool::ModelBuffers const & modelBuffers
			, ashes::BufferBase const * indexBuffer )
		{
			ashes::WriteDescriptorSetArray writes;

			if ( indexBuffer )
			{
				writes.emplace_back( makeDescriptorWrite( *indexBuffer, VtxBindings::eInIndices, 0u, indexBuffer->getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::ePositions )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::ePositions )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPosition, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eNormals )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eNormals )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInNormal, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eTangents )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTangents )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTangent, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eTexcoords0 )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords0 )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord0, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eTexcoords1 )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords1 )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord1, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eTexcoords2 )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords2 )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord2, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eTexcoords3 )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords3 )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord3, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eColours )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eColours )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInColour, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::ePassMasks )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::ePassMasks )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPassMasks, 0u, buffer.getSize() ) );
			}

			if ( modelBuffers.buffers[size_t( SubmeshData::eVelocity )] )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eVelocity )]->getBuffer().getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInVelocity, 0u, buffer.getSize() ) );
			}

			auto result = pool.createDescriptorSet( name + "Vtx"
				, Sets::eVtx );
			result->setBindings( std::move( writes ) );
			result->update();
			return result;
		}

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, std::string const & name
			, bool blend )
		{
			auto loadOp = ( blend
				? VK_ATTACHMENT_LOAD_OP_LOAD
				: VK_ATTACHMENT_LOAD_OP_CLEAR );
			auto srcLayout = ( blend
				? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				: VK_IMAGE_LAYOUT_UNDEFINED );
			ashes::VkAttachmentDescriptionArray attaches{ { 0u
					, getFormat( device, DsTexture::eData0 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				, { 0u
					, getFormat( device, DsTexture::eData1 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				, { 0u
					, getFormat( device, DsTexture::eData2 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				, { 0u
					, getFormat( device, DsTexture::eData3 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				, { 0u
					, getFormat( device, DsTexture::eData4 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
				, { 0u
					, getFormat( device, DsTexture::eData5 )
					, VK_SAMPLE_COUNT_1_BIT
					, loadOp
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, srcLayout
					, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 4u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
					, { 5u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, ashes::nullopt
				, {} } );
			ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo{ 0u
				, std::move( attaches )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( name
				, std::move( createInfo ) );
		}

		ashes::FrameBufferPtr createFrameBuffer( ashes::RenderPass const & renderPass
			, std::string const & name
			, RenderTechnique const & technique )
		{
			auto & textures = technique.getOpaqueResult();
			ashes::VkImageViewArray fbAttaches;
			fbAttaches.emplace_back( textures[DsTexture::eData0].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData1].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData2].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData3].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData4].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData5].targetView );
			return renderPass.createFrameBuffer( name
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, uint32_t( fbAttaches.size() )
					, fbAttaches.data()
					, textures[DsTexture::eData0].getExtent().width
					, textures[DsTexture::eData0].getExtent().height
					, 1u ) );
		}

		ashes::PipelinePtr createPipeline( RenderDevice const & device
			, VkExtent3D const & extent
			, ashes::PipelineShaderStageCreateInfoArray const & stages
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::RenderPass const & renderPass
			, bool blend )
		{
			auto blendState = blend
				? ashes::PipelineColorBlendStateCreateInfo{ 0u
					, VK_FALSE
					, VK_LOGIC_OP_COPY
					, { { VK_TRUE
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_OP_ADD
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_FACTOR_ONE
						, VK_BLEND_OP_ADD
						, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT } }}
				: ashes::PipelineColorBlendStateCreateInfo{};
			blendState.attachments.push_back( blendState.attachments.front() );
			blendState.attachments.push_back( blendState.attachments.front() );
			blendState.attachments.push_back( blendState.attachments.front() );
			blendState.attachments.push_back( blendState.attachments.front() );
			blendState.attachments.push_back( blendState.attachments.front() );
			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u, {}, {} };
			ashes::PipelineViewportStateCreateInfo viewportState{ 0u
				, { makeViewport( castor::Point2ui{ extent.width, extent.height } ) }
				, { makeScissor( castor::Point2ui{ extent.width, extent.height } ) } };
			return device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
				, stages
				, std::move( vertexState )
				, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP }
				, ashes::nullopt
				, std::move( viewportState )
				, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
				, ashes::PipelineMultisampleStateCreateInfo{}
				, std::nullopt
				, std::move( blendState )
				, ashes::nullopt
				, pipelineLayout
				, static_cast< VkRenderPass const & >( renderPass ) } );
		}

		struct PushData
		{
			uint32_t passTypeIndex;
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
		, RenderNodesPass const & depthPass
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: castor::Named{ category + cuT( "/" ) + name }
		, RenderTechniquePass{ parent, *parent->getRenderTarget().getScene() }
		, crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [](){ return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; } )
				, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return doIsEnabled(); } )
				, IsComputePassCallback( [](){ return VisibilityResolvePass::useCompute; } ) }
			, renderPassDesc.m_ruConfig }
		, m_device{ device }
		, m_depthPass{ depthPass }
		, m_matrixUbo{ renderPassDesc.m_matrixUbo }
		, m_sceneUbo{ *renderPassDesc.m_sceneUbo }
		, m_culler{ renderPassDesc.m_culler }
		, m_onCullerCompute( m_culler.onCompute.connect( [this]( SceneCuller const & culler ){ doOnCullerCompute( culler ); } ) )
		, m_inOutsDescriptorLayout{ visres::createInDescriptorLayout( m_device, getName(), getScene().getOwner()->getMaterialCache() ) }
		, m_inOutsDescriptorPool{ m_inOutsDescriptorLayout->createPool( 1u ) }
		, m_inOutsDescriptorSet{ visres::createInDescriptorSet( getName(), *m_inOutsDescriptorPool, m_matrixUbo, m_sceneUbo, *parent, getScene() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, getName()
			, ( useCompute
				? nullptr
				: visres::ShaderWriter< false >::getVertexProgram() ) }
		, m_firstRenderPass{ ( useCompute
			? nullptr
			: visres::createRenderPass( m_device, getName(), false ) ) }
		, m_firstFramebuffer{ ( useCompute
			? nullptr
			: visres::createFrameBuffer( *m_firstRenderPass, getName(), *m_parent ) ) }
		, m_blendRenderPass{ ( useCompute
			? nullptr
			: visres::createRenderPass( m_device, getName(), true ) ) }
		, m_blendFramebuffer{ ( useCompute
			? nullptr
			: visres::createFrameBuffer( *m_blendRenderPass, getName(), *m_parent ) ) }
	{
	}

	void VisibilityResolvePass::accept( RenderTechniqueVisitor & visitor )
	{
	}

	void VisibilityResolvePass::update( CpuUpdater & updater )
	{
		if ( m_commandsChanged )
		{
			m_activePipelines.clear();

			for ( auto & itPipeline : m_culler.getSubmeshNodes( m_depthPass ) )
			{
				PipelineBaseHash const & pipelineHash = itPipeline.first;
				auto [submeshFlags, programFlags, passType, passFlags, maxTexcoordSet, texturesCount, textureFlags] = getPipelineHashDetails( pipelineHash );
				auto & pipeline = doCreatePipeline( passType
					, textureFlags
					, submeshFlags
					, passFlags & PassFlag::eAllVisibility );
				auto it = m_activePipelines.emplace( &pipeline
					, std::set< ashes::DescriptorSet const * >{} ).first;

				for ( auto & itBuffer : itPipeline.second )
				{
					uint64_t hash = 0u;
					hash = castor::hashCombinePtr( hash, *itBuffer.first );
					auto ires = pipeline.descriptorSets.emplace( hash, ashes::DescriptorSetPtr{} );

					if ( ires.second )
					{
						auto & modelBuffers = m_device.geometryPools->getBuffers( *itBuffer.first );
						ashes::BufferBase const * indexBuffer{};

						if ( checkFlag( submeshFlags, SubmeshFlag::eIndex ) )
						{
							indexBuffer = &m_device.geometryPools->getIndexBuffer( *itBuffer.first );
						}

						ires.first->second = visres::createVtxDescriptorSet( getName(), *pipeline.descriptorPool
							, modelBuffers
							, indexBuffer );
					}

					it->second.emplace( ires.first->second.get() );
				}
			}

			for ( auto & itPipeline : m_culler.getInstancedSubmeshNodes( m_depthPass ) )
			{
				PipelineBaseHash const & pipelineHash = itPipeline.first;
				auto [submeshFlags, programFlags, passType, passFlags, maxTexcoordSet, texturesCount, textureFlags] = getPipelineHashDetails( pipelineHash );
				auto & pipeline = doCreatePipeline( passType
					, textureFlags
					, submeshFlags
					, passFlags & PassFlag::eAllVisibility );
				auto it = m_activePipelines.emplace( &pipeline
					, std::set< ashes::DescriptorSet const * >{} ).first;

				for ( auto & itBuffer : itPipeline.second )
				{
					uint64_t hash = 0u;
					hash = castor::hashCombinePtr( hash, *itBuffer.first );
					auto ires = pipeline.descriptorSets.emplace( hash, ashes::DescriptorSetPtr{} );

					if ( ires.second )
					{
						auto & modelBuffers = m_device.geometryPools->getBuffers( *itBuffer.first );
						ashes::BufferBase const * indexBuffer{};

						if ( checkFlag( submeshFlags, SubmeshFlag::eIndex ) )
						{
							indexBuffer = &m_device.geometryPools->getIndexBuffer( *itBuffer.first );
						}

						ires.first->second = visres::createVtxDescriptorSet( getName(), *pipeline.descriptorPool
							, modelBuffers
							, indexBuffer );
					}

					it->second.emplace( ires.first->second.get() );
				}
			}

			for ( auto & itPipeline : m_culler.getBillboardNodes( m_depthPass ) )
			{
				PipelineBaseHash const & pipelineHash = itPipeline.first;
				auto [submeshFlags, programFlags, passType, passFlags, maxTexcoordSet, texturesCount, textureFlags] = getPipelineHashDetails( pipelineHash );

				for ( auto & itBuffer : itPipeline.second )
				{
					for ( auto & sidedCulled : itBuffer.second )
					{
						auto & culled = sidedCulled.first;
						auto & positionsBuffer = culled.node->data.getVertexBuffer();
						auto & pipeline = doCreatePipeline( passType
							, textureFlags
							, passFlags & PassFlag::eAllVisibility
							, culled.node->data.getVertexStride() );
						auto it = m_activeBillboardPipelines.emplace( &pipeline
							, std::map< uint32_t, ashes::DescriptorSet const * >{} ).first;
						uint64_t hash = positionsBuffer.getOffset();
						hash = castor::hashCombinePtr( hash, positionsBuffer.getBuffer().getBuffer() );
						auto ires = pipeline.descriptorSets.emplace( hash, ashes::DescriptorSetPtr{} );

						if ( ires.second )
						{
							ires.first->second = visres::createVtxDescriptorSet( getName()
								, *pipeline.descriptorPool
								, positionsBuffer.getBuffer().getBuffer()
								, positionsBuffer.getOffset()
								, positionsBuffer.getSize() );
						}

						it->second.emplace( culled.node->getId(), ires.first->second.get() );
					}
				}
			}
		}
	}

	void VisibilityResolvePass::doInitialise()
	{
	}

	bool VisibilityResolvePass::doIsEnabled()const
	{
		return !m_activePipelines.empty()
			|| !m_activeBillboardPipelines.empty();
	}

	void VisibilityResolvePass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		if constexpr ( useCompute )
		{
			doRecordCompute( context, commandBuffer );
		}
		else
		{
			doRecordGraphics( context, commandBuffer );
		}
	}

	void VisibilityResolvePass::doRecordCompute( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		auto & opaqueResult = m_parent->getOpaqueResult();
		auto size = uint32_t( m_parent->getMaterialsStarts().getCount() );
		std::array< VkDescriptorSet, 3u > descriptorSets{ *m_inOutsDescriptorSet
			, nullptr
			, *getScene().getBindlessTexDescriptorSet() };
		visres::PushData pushData{ 0u, 0u };

		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData0].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData0].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData1].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData1].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData2].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData2].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData2].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData2].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData3].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData3].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData4].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData4].targetViewId.data->info.subresourceRange );
		context.getContext().vkCmdClearColorImage( commandBuffer
			, opaqueResult[DsTexture::eData5].image
			, VK_IMAGE_LAYOUT_GENERAL
			, &transparentBlackClearColor.color
			, 1u
			, &opaqueResult[DsTexture::eData5].targetViewId.data->info.subresourceRange );

		for ( auto & pipelineIt : m_activePipelines )
		{
			pushData.passTypeIndex = pipelineIt.first->passTypeIndex;
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipelineIt.first->firstPipeline );
			context.getContext().vkCmdPushConstants( commandBuffer
				, *pipelineIt.first->pipelineLayout
				, VK_SHADER_STAGE_COMPUTE_BIT
				, 0u
				, sizeof( visres::PushData )
				, &pushData );

			for ( auto & descriptorSet : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSet;
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDispatch( commandBuffer, size / 64u, 1u, 1u );
			}
		}

		for ( auto & pipelineIt : m_activeBillboardPipelines )
		{
			pushData.passTypeIndex = pipelineIt.first->passTypeIndex;
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *pipelineIt.first->firstPipeline );

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSetIt.second;
				pushData.billboardNodeId = descriptorSetIt.first;
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipelineIt.first->pipelineLayout
					, VK_SHADER_STAGE_COMPUTE_BIT
					, 0u
					, sizeof( visres::PushData )
					, &pushData );
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDispatch( commandBuffer, size / 64u, 1u, 1u );
			}
		}

		context.setLayoutState( opaqueResult[DsTexture::eData0].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData1].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData2].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData3].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData4].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData5].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_GENERAL ) );
	}

	void VisibilityResolvePass::doRecordGraphics( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		auto & opaqueResult = m_parent->getOpaqueResult();
		std::array< VkDescriptorSet, 3u > descriptorSets{ *m_inOutsDescriptorSet
			, nullptr
			, *getScene().getBindlessTexDescriptorSet() };
		bool first = true;
		bool renderPassBound = false;
		bool pipelineBound = false;
		visres::PushData pushData{ 0u, 0u };

		auto bind = [&]( Pipeline const & pipeline, bool billboards )
		{
			if ( !renderPassBound )
			{
				static std::array< VkClearValue, 6u > clearValues{ getClearValue( DsTexture::eData0 )
					, getClearValue( DsTexture::eData1 )
					, getClearValue( DsTexture::eData2 )
					, getClearValue( DsTexture::eData3 )
					, getClearValue( DsTexture::eData4 )
					, getClearValue( DsTexture::eData5 ) };
				auto & extent = m_parent->getNormalTexture().getExtent();
				VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
					, nullptr
					, nullptr
					, nullptr
					, { { 0, 0 }, { extent.width, extent.height } }
					, uint32_t( clearValues.size() )
					, clearValues.data() };

				if ( first )
				{
					beginInfo.renderPass = *m_firstRenderPass;
					beginInfo.framebuffer = *m_firstFramebuffer;
				}
				else
				{
					beginInfo.renderPass = *m_blendRenderPass;
					beginInfo.framebuffer = *m_blendFramebuffer;
				}

				context.getContext().vkCmdBeginRenderPass( commandBuffer
					, &beginInfo
					, VK_SUBPASS_CONTENTS_INLINE );
				renderPassBound = true;
				pipelineBound = false;
			}

			if ( !pipelineBound )
			{
				context.getContext().vkCmdBindPipeline( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, ( first
						? *pipeline.firstPipeline
						: *pipeline.blendPipeline ) );

				if ( !billboards )
				{
					context.getContext().vkCmdPushConstants( commandBuffer
						, *pipeline.pipelineLayout
						, VK_SHADER_STAGE_FRAGMENT_BIT
						, 0u
						, sizeof( visres::PushData )
						, &pushData );
				}

				pipelineBound = true;
			}

			if ( billboards )
			{
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipeline.pipelineLayout
					, VK_SHADER_STAGE_FRAGMENT_BIT
					, 0u
					, sizeof( visres::PushData )
					, &pushData );
			}
		};

		for ( auto & pipelineIt : m_activePipelines )
		{
			pushData.passTypeIndex = pipelineIt.first->passTypeIndex;

			for ( auto & descriptorSet : pipelineIt.second )
			{
				bind( *pipelineIt.first, false );

				descriptorSets[1] = *descriptorSet;
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );

				if ( first )
				{
					context.getContext().vkCmdEndRenderPass( commandBuffer );
					first = false;
					renderPassBound = false;
					pipelineBound = false;
				}
			}

			pipelineBound = false;
		}

		for ( auto & pipelineIt : m_activeBillboardPipelines )
		{
			pushData.passTypeIndex = pipelineIt.first->passTypeIndex;

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				pushData.billboardNodeId = descriptorSetIt.first;
				bind( *pipelineIt.first, true );

				descriptorSets[1] = *descriptorSetIt.second;
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );

				if ( first )
				{
					context.getContext().vkCmdEndRenderPass( commandBuffer );
					first = false;
					renderPassBound = false;
					pipelineBound = false;
				}
			}

			pipelineBound = false;
		}

		if ( renderPassBound )
		{
			context.getContext().vkCmdEndRenderPass( commandBuffer );
		}

		context.setLayoutState( opaqueResult[DsTexture::eData0].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData1].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData2].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData3].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData4].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
		context.setLayoutState( opaqueResult[DsTexture::eData5].targetViewId
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PassTypeID passType
		, TextureFlags textureFlags
		, PassFlags passFlags
		, uint32_t stride )
	{
		return doCreatePipeline( passType
			, textureFlags
			, SubmeshFlag::ePositions | SubmeshFlag::eTexcoords0
			, passFlags
			, stride
			, m_billboardPipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PassTypeID passType
		, TextureFlags textureFlags
		, SubmeshFlags submeshFlags
		, PassFlags passFlags )
	{
		return doCreatePipeline( passType
			, textureFlags
			, submeshFlags
			, passFlags
			, 0u
			, m_pipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PassTypeID passType
		, TextureFlags textureFlags
		, SubmeshFlags submeshFlags
		, PassFlags passFlags
		, uint32_t stride
		, std::unordered_map< uint64_t, PipelinePtr > & pipelines )
	{
		auto hash = getHash( submeshFlags
			, ProgramFlag::eNone
			, passType
			, passFlags
			, 0u
			, 0u
			, textureFlags );

		if ( stride )
		{
			hash = castor::hashCombine( hash, stride );
		}

		auto it = pipelines.find( hash );

		if ( it == pipelines.end() )
		{
			auto stage = VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT;
			auto stages = VkShaderStageFlags( stage );
			auto extent = m_parent->getNormalTexture().getExtent();
			auto & matCache = getScene().getEngine()->getMaterialCache();
			auto result = std::make_unique< Pipeline >( ShaderModule{ stage
				, getName()
				, visres::getProgram( m_device, extent, passType, textureFlags, submeshFlags, passFlags, stride ) } );
			result->passTypeIndex = matCache.getPassTypeIndex( passType, passFlags, textureFlags );

			if constexpr ( !useCompute )
			{
				result->stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, m_vertexShader ) };
			}

			result->stages.push_back( makeShaderState( m_device, result->shader ) );
			result->descriptorLayout = stride == 0u
				? visres::createVtxDescriptorLayout( m_device, getName(), submeshFlags )
				: visres::createVtxDescriptorLayout( m_device, getName() );
			result->pipelineLayout = m_device->createPipelineLayout( getName()
				, { *m_inOutsDescriptorLayout, *result->descriptorLayout, *getScene().getBindlessTexDescriptorLayout() }
				, { { stages, 0u, sizeof( visres::PushData ) } } );

			if constexpr ( useCompute )
			{
				result->firstPipeline = m_device->createPipeline( ashes::ComputePipelineCreateInfo{ 0u
					, result->stages.front()
					, *result->pipelineLayout } );
			}
			else
			{
				result->firstPipeline = visres::createPipeline( m_device
					, extent
					, result->stages
					, *result->pipelineLayout
					, *m_firstRenderPass
					, false );
				result->blendPipeline = visres::createPipeline( m_device
					, extent
					, result->stages
					, *result->pipelineLayout
					, *m_blendRenderPass
					, true );
			}

			result->descriptorPool = result->descriptorLayout->createPool( MaxPipelines );
			it = pipelines.emplace( hash, std::move( result ) ).first;
		}

		return *it->second;
	}

	void VisibilityResolvePass::doOnCullerCompute( SceneCuller const & culler )
	{
		m_commandsChanged = m_commandsChanged || culler.areCulledChanged();
	}
}
