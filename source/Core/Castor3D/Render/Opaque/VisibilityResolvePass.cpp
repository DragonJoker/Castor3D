#include "Castor3D/Render/Opaque/VisibilityResolvePass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Culling/PipelineNodes.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
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
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslReflection.hpp"
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

#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

namespace castor3d
{
	//*********************************************************************************************

#define DeclareSsbo( Name, Type, Binding, Enable )\
	[this]()\
	{\
		sdw::StorageBuffer Name##Buffer{ m_writer\
			, #Name + std::string{ "Buffer" }\
			, Binding\
			, Sets::eVtx\
			, ast::type::MemoryLayout::eStd430\
			, Enable };\
		auto result = Name##Buffer.declMemberArray< Type >( #Name, Enable );\
		Name##Buffer.end();\
		return result;\
	}()

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
			eCamera,
			eScene,
			eModels,
			eBillboards,
			eMaterials,
			eTexConfigs,
			eTexAnims,
			eInData,
			eMapBrdf,
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
			eOutResult,
			eCount,
		};

		enum VtxBindings : uint32_t
		{
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
			: public sdw::StructInstanceHelperT< "C3D_BarycentricFullDerivatives"
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

		struct VisibilityHelpers
		{
			explicit VisibilityHelpers( sdw::ShaderWriter & writer
				, shader::PassShaders & passShaders
				, PipelineFlags const & flags
				, uint32_t stride )
				: m_writer{ writer }
				, m_passShaders{ passShaders }
				, m_flags{ flags }
				, m_stride{ stride }
				, m_inIndices{ DeclareSsbo( c3d_inIndices
					, sdw::UInt
					, VtxBindings::eInIndices
					, m_flags.enableIndices() ) }
				, m_inPosition{ [this]()
					{
						sdw::StorageBuffer c3d_inPositionBuffer{ m_writer
							, std::string{ "c3d_inPositionBuffer" }
						, VtxBindings::eInPosition
							, Sets::eVtx
							, ast::type::MemoryLayout::eStd430
							, m_flags.enablePosition() };
						auto result = c3d_inPositionBuffer.declMemberArray< Position >( "c3d_inPosition"
							, m_flags.enablePosition()
							, m_stride );
						c3d_inPositionBuffer.end();
						return result;
					}() }
				, m_inNormal{ DeclareSsbo( c3d_inNormal
					, sdw::Vec4
					, VtxBindings::eInNormal
					, m_flags.enableNormal() ) }
				, m_inTangent{ DeclareSsbo( c3d_inTangent
					, sdw::Vec4
					, VtxBindings::eInTangent
					, m_flags.enableTangentSpace() ) }
				, m_inBitangent{ DeclareSsbo( c3d_inBitangent
					, sdw::Vec4
					, VtxBindings::eInBitangent
					, m_flags.enableBitangent() ) }
				, m_inTexcoord0{ DeclareSsbo( c3d_inTexcoord0
					, sdw::Vec4
					, VtxBindings::eInTexcoord0
					, m_flags.enableTexcoord0() && ( m_stride == 0u ) ) }
				, m_inTexcoord1{ DeclareSsbo( c3d_inTexcoord1
					, sdw::Vec4
					, VtxBindings::eInTexcoord1
					, m_flags.enableTexcoord1() ) }
				, m_inTexcoord2{ DeclareSsbo( c3d_inTexcoord2
					, sdw::Vec4
					, VtxBindings::eInTexcoord2
					, m_flags.enableTexcoord2() ) }
				, m_inTexcoord3{ DeclareSsbo( c3d_inTexcoord3
					, sdw::Vec4
					, VtxBindings::eInTexcoord3
					, m_flags.enableTexcoord3() ) }
				, m_inColour{ DeclareSsbo( c3d_inColour
					, sdw::Vec4
					, VtxBindings::eInColour
					, m_flags.enableColours() ) }
				, m_inPassMasks{ DeclareSsbo( c3d_inPassMasks
					, sdw::UVec4
					, VtxBindings::eInPassMasks
					, m_flags.enablePassMasks() ) }
				, m_inVelocity{ DeclareSsbo( c3d_inVelocity
					, sdw::Vec4
					, VtxBindings::eInVelocity
					, m_flags.enableVelocity() ) }
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
						, [&]( sdw::Vec4 const & pt0
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
								FI;
							}
							FI;

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
				, shader::ModelData const & pmodelData
				, shader::VertexSurface & pv0
				, shader::VertexSurface & pv1
				, shader::VertexSurface & pv2 )
			{
				if ( !m_loadVertices )
				{
					m_loadVertices = m_writer.implementFunction< sdw::Void >( "loadVertices"
						, [&]( sdw::UInt const & nodeId
							, sdw::UInt const & primitiveId
							, shader::ModelData const & modelData
							, shader::VertexSurface v0
							, shader::VertexSurface v1
							, shader::VertexSurface v2 )
						{
							auto loadVertex = m_writer.implementFunction< shader::VertexSurface >( "loadVertex"
								, [&]( sdw::UInt const & vertexId )
								{
									auto result = m_writer.declLocale< shader::VertexSurface >( std::string( "result" ) );
									result.position = m_inPosition[vertexId].position;
									result.normal = m_inNormal[vertexId].xyz();
									result.tangent = m_inTangent[vertexId];
									result.bitangent = m_inBitangent[vertexId].xyz();
									result.texture0 = m_inTexcoord0[vertexId].xyz();
									result.texture1 = m_inTexcoord1[vertexId].xyz();
									result.texture2 = m_inTexcoord2[vertexId].xyz();
									result.texture3 = m_inTexcoord3[vertexId].xyz();
									result.colour = m_inColour[vertexId].xyz();
									result.passMasks = m_inPassMasks[vertexId];
									result.velocity = m_inVelocity[vertexId].xyz();

									m_writer.returnStmt( result );
								}
							, sdw::InUInt{ m_writer, "vertexId" } );

							auto baseIndex = m_writer.declLocale( "baseIndex"
								, modelData.getIndexOffset() + primitiveId * 3u );
							auto indices = m_writer.declLocale( "indices"
								, uvec3( m_inIndices[baseIndex + 0u]
									, m_inIndices[baseIndex + 1u]
									, m_inIndices[baseIndex + 2u] ) );
							auto baseVertex = m_writer.declLocale( "baseVertex"
								, modelData.getVertexOffset() );
							v0 = loadVertex( baseVertex + indices.x() );
							v1 = loadVertex( baseVertex + indices.y() );
							v2 = loadVertex( baseVertex + indices.z() );
						}
						, sdw::InUInt{ m_writer, "nodeId" }
						, sdw::InUInt{ m_writer, "primitiveId" }
						, shader::InModelData{ m_writer, "modelData" }
						, shader::OutVertexSurface{ m_writer, "v0" }
						, shader::OutVertexSurface{ m_writer, "v1" }
						, shader::OutVertexSurface{ m_writer, "v2" } );
				}

				m_loadVertices( pnodeId, pprimitiveId, pmodelData, pv0, pv1, pv2 );
			}

			void loadVertices( sdw::UInt const & pnodeId
				, sdw::UInt const & pprimitiveId
				, shader::ModelData const & pmodelData
				, shader::VertexSurface & pv0
				, shader::VertexSurface & pv1
				, shader::VertexSurface & pv2
				, shader::CameraData const & c3d_cameraData
				, sdw::Array< shader::BillboardData > const & c3d_billboardData )
			{
				if ( !m_loadVertices )
				{
					m_loadVertices = m_writer.implementFunction< sdw::Void >( "loadVertices"
						, [&]( sdw::UInt const & nodeId
							, sdw::UInt const & primitiveId
							, shader::ModelData const & modelData
							, shader::VertexSurface v0
							, shader::VertexSurface v1
							, shader::VertexSurface v2 )
						{
							auto bbPositions = m_writer.declConstantArray( "bbPositions"
								, std::vector< sdw::Vec3 >{ vec3( -0.5_f, -0.5_f, 1.0_f )
								, vec3( -0.5_f, +0.5_f, 1.0_f )
								, vec3( +0.5_f, -0.5_f, 1.0_f )
								, vec3( +0.5_f, +0.5_f, 1.0_f ) } );
							auto bbTexcoords = m_writer.declConstantArray( "bbTexcoords"
								, std::vector< sdw::Vec2 >{ vec2( 0.0_f, 0.0_f )
								, vec2( 0.0_f, 1.0_f )
								, vec2( 1.0_f, 0.0_f )
								, vec2( 1.0_f, 1.0_f ) } );

							auto instanceId = m_writer.declLocale( "instanceId"
								, primitiveId / 2u );
							auto firstTriangle = m_writer.declLocale( "firstTriangle"
								, ( primitiveId % 2u ) == 0u );
							auto center = m_writer.declLocale( "center"
								, m_inPosition[instanceId].position );
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
						, shader::InModelData{ m_writer, "modelData" }
						, shader::OutVertexSurface{ m_writer, "v0" }
						, shader::OutVertexSurface{ m_writer, "v1" }
						, shader::OutVertexSurface{ m_writer, "v2" } );
				}

				m_loadVertices( pnodeId, pprimitiveId, pmodelData, pv0, pv1, pv2 );
			}

			shader::DerivFragmentSurface loadSurface( sdw::UInt const & pnodeId
				, sdw::UInt const & pprimitiveId
				, sdw::Vec2 const & ppixelCoord
				, shader::ModelData const & pmodelData
				, shader::Material const & pmaterial
				, sdw::Float & pdepth
				, shader::CameraData const & c3d_cameraData
				, sdw::Array< shader::BillboardData > const & c3d_billboardData )
			{
				if ( !m_loadSurface )
				{
					m_loadSurface = m_writer.implementFunction< shader::DerivFragmentSurface >( "loadSurface"
						, [&]( sdw::UInt const & nodeId
							, sdw::UInt const & primitiveId
							, sdw::Vec2 const & pixelCoord
							, shader::ModelData const & modelData
							, shader::Material const & material
							, sdw::Float depth )
						{
							auto result = m_writer.declLocale< shader::DerivFragmentSurface >( "result" );
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

							auto v0 = m_writer.declLocale< shader::VertexSurface >( "v0" );
							auto v1 = m_writer.declLocale< shader::VertexSurface >( "v1" );
							auto v2 = m_writer.declLocale< shader::VertexSurface >( "v2" );

							if ( m_stride == 0u )
							{
								loadVertices( nodeId, primitiveId, modelData, v0, v1, v2 );
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
							FI;

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

							m_writer.returnStmt( result );
						}
						, sdw::InUInt{ m_writer, "nodeId" }
						, sdw::InUInt{ m_writer, "primitiveId" }
						, sdw::InVec2{ m_writer, "pixelCoord" }
						, shader::InModelData{ m_writer, "modelData" }
						, shader::InMaterial{ m_writer, "material", m_passShaders }
						, sdw::OutFloat{ m_writer, "depth" } );
				}

				return m_loadSurface( pnodeId, pprimitiveId, ppixelCoord, pmodelData, pmaterial, pdepth );
			}

		private:
			sdw::ShaderWriter & m_writer;
			shader::PassShaders & m_passShaders;
			PipelineFlags const & m_flags;
			uint32_t m_stride;
			sdw::UInt32Array m_inIndices;
			sdw::Array< Position > m_inPosition;
			sdw::Vec4Array m_inNormal;
			sdw::Vec4Array m_inTangent;
			sdw::Vec4Array m_inBitangent;
			sdw::Vec4Array m_inTexcoord0;
			sdw::Vec4Array m_inTexcoord1;
			sdw::Vec4Array m_inTexcoord2;
			sdw::Vec4Array m_inTexcoord3;
			sdw::Vec4Array m_inColour;
			sdw::U32Vec4Array m_inPassMasks;
			sdw::Vec4Array m_inVelocity;
			sdw::Function< BarycentricFullDerivatives
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InVec2
				, sdw::InVec2 > m_calcFullBarycentric;
			sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InUInt
				, shader::InModelData
				, shader::OutVertexSurface
				, shader::OutVertexSurface
				, shader::OutVertexSurface > m_loadVertices;
			sdw::Function< shader::DerivFragmentSurface
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InVec2
				, shader::InModelData
				, shader::InMaterial
				, sdw::OutFloat > m_loadSurface;
		};

		static ShaderPtr getProgram( RenderDevice const & device
			, Scene const & scene
			, RenderTechnique const & technique
			, VkExtent3D const & imageSize
			, PipelineFlags const & flags
			, IndirectLightingData const * indirectLighting
			, DebugConfig & debugConfig
			, uint32_t stride
			, bool blend
			, bool hasSsao
			, bool clusteredLighting )
		{
			auto & engine = *device.renderSystem.getEngine();
			ShaderWriter< VisibilityResolvePass::useCompute >::Type writer;

			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ engine.getPassComponentsRegister()
				, flags
				, ( ComponentModeFlag::eDerivTex
					| VisibilityResolvePass::getComponentsMask() )
				, utils };
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
			shader::TextureConfigurations textureConfigs{ writer
				, InOutBindings::eTexConfigs
				, Sets::eInOuts };
			shader::TextureAnimations textureAnims{ writer
				, InOutBindings::eTexAnims
				, Sets::eInOuts };
			auto c3d_imgData = writer.declStorageImg< sdw::RUImage2DRg32 >( "c3d_imgData"
				, InOutBindings::eInData
				, Sets::eInOuts );
			auto c3d_mapBrdf = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapBrdf"
				, InOutBindings::eMapBrdf
				, Sets::eInOuts );
			auto lightsIndex = index++;
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
				, nullptr
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
				, indirectLighting };
			shader::ClusteredLights clusteredLights{ writer
				, index
				, Sets::eInOuts
				, clusteredLighting };

			auto constexpr maxPipelinesSize = uint32_t( castor::getBitSize( MaxPipelines ) );
			auto constexpr maxPipelinesMask = ( 0x000000001u << maxPipelinesSize ) - 1u;

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps", TexBindings::eTextures, Sets::eTex ) );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
			auto pipelineId = pcb.declMember< sdw::UInt >( "pipelineId" );
			auto billboardNodeId = pcb.declMember< sdw::UInt >( "billboardNodeId", stride != 0u );
			pcb.end();

			shader::Fog fog{ writer };
			VisibilityHelpers visHelpers{ writer, passShaders, flags, stride };

			auto shade = writer.implementFunction< sdw::Boolean >( "shade"
				, [&]( sdw::IVec2 const & ipixel
					, sdw::UInt nodeId
					, sdw::UInt pipeline
					, sdw::UInt primitiveId
					, sdw::Vec4 outResult )
				{
					{
						shader::DebugOutput output{ debugConfig
							, cuT( "Opaque" )
							, c3d_cameraData.debugIndex()
							, outResult
							, true };

						IF( writer, pipelineId != pipeline )
						{
							writer.returnStmt( 0_b );
						}
						FI;

						auto modelData = writer.declLocale( "modelData"
							, c3d_modelsData[nodeId - 1u] );
						auto material = writer.declLocale( "material"
							, materials.getMaterial( modelData.getMaterialId() ) );
						auto depth = writer.declLocale( "depth"
							, 0.0_f );
						auto occlusion = writer.declLocale( "occlusion"
							, ( hasSsao
								? c3d_mapOcclusion.fetch( ipixel, 0_i )
								: 1.0_f ) );
						auto baseSurface = writer.declLocale( "baseSurface"
							, visHelpers.loadSurface( nodeId
								, primitiveId
								, vec2( ipixel )
								, modelData
								, material
								, depth
								, c3d_cameraData
								, c3d_billboardData ) );
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

						if ( components.occlusion )
						{
							occlusion *= components.occlusion;
						}

						auto incident = writer.declLocale( "incident"
							, reflections.computeIncident( baseSurface.worldPosition.xyz(), c3d_cameraData.position() ) );

						if ( components.transmission )
						{
							IF( writer, lights.getFinalTransmission( components, incident ) >= 0.1_f )
							{
								writer.demote();
							}
							FI;
						}

						if ( auto lightingModel = lights.getLightingModel() )
						{
							IF( writer, material.lighting )
							{
								shader::OutputComponents lighting{ writer, false };
								auto surface = writer.declLocale( "surface"
									, shader::Surface{ vec3( vec2( ipixel ), depth )
										, baseSurface.viewPosition.xyz()
										, baseSurface.worldPosition.xyz()
										, normalize( components.normal ) } );

								lightingModel->finish( passShaders
									, surface
									, utils
									, c3d_cameraData.position()
									, components );
								auto lightSurface = shader::LightSurface::create( writer
									, "lightSurface"
									, c3d_cameraData.position()
									, surface.worldPosition.xyz()
									, surface.viewPosition.xyz()
									, surface.clipPosition
									, surface.normal );
								lights.computeCombinedDifSpec( clusteredLights
									, components
									, *backgroundModel
									, lightSurface
									, modelData.isShadowReceiver()
									, lightSurface.clipPosition().xy()
									, lightSurface.viewPosition().z()
									, output
									, lighting );
								auto directAmbient = writer.declLocale( "directAmbient"
									, components.ambientColour * c3d_sceneData.ambientLight() * components.ambientFactor );
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
								reflections.computeCombined( components
									, lightSurface
									, *backgroundModel
									, modelData.getEnvMapIndex()
									, components.hasReflection
									, components.hasRefraction
									, components.refractionRatio
									, reflectedDiffuse
									, reflectedSpecular
									, refracted
									, coatReflected
									, sheenReflected
									, output );
								lightSurface.updateL( utils
									, components.normal
									, components.f0
									, components );
								auto indirectOcclusion = indirect.computeOcclusion( flags.getGlobalIlluminationFlags()
									, lightSurface
									, output );
								auto lightIndirectDiffuse = indirect.computeDiffuse( flags.getGlobalIlluminationFlags()
									, lightSurface
									, indirectOcclusion
									, output );
								auto lightIndirectSpecular = indirect.computeSpecular( flags.getGlobalIlluminationFlags()
									, lightSurface
									, components.roughness
									, indirectOcclusion
									, lightIndirectDiffuse.w()
									, c3d_mapBrdf
									, output );
								auto indirectAmbient = indirect.computeAmbient( flags.getGlobalIlluminationFlags()
									, lightIndirectDiffuse.xyz()
									, output );
								auto indirectDiffuse = writer.declLocale( "indirectDiffuse"
									, ( flags.hasDiffuseGI()
										? cookTorrance.computeDiffuse( normalize( lightIndirectDiffuse.xyz() )
											, length( lightIndirectDiffuse.xyz() )
											, lightSurface.difF() )
										: vec3( 0.0_f ) ) );

								output.registerOutput( "Lighting", "Ambient", directAmbient );
								output.registerOutput( "Indirect", "Diffuse", indirectDiffuse );
								output.registerOutput( "Incident", sdw::fma( incident, vec3( 0.5_f ), vec3( 0.5_f ) ) );
								output.registerOutput( "Occlusion", occlusion );
								output.registerOutput( "Emissive", components.emissiveColour * components.emissiveFactor );

								outResult = vec4( lightingModel->combine( output
										, components
										, incident
										, lighting.diffuse
										, indirectDiffuse
										, lighting.specular
										, lighting.scattering
										, lighting.coatingSpecular
										, lighting.sheen
										, lightIndirectSpecular
										, directAmbient
										, indirectAmbient
										, occlusion
										, components.emissiveColour * components.emissiveFactor
										, reflectedDiffuse
										, reflectedSpecular
										, refracted
										, coatReflected
										, sheenReflected )
									, components.opacity );
							}
							ELSE
							{
								outResult = vec4( components.colour, components.opacity );
							}
							FI;
						}
						else
						{
							outResult = vec4( components.colour, components.opacity );
						}

						if ( flags.hasFog() )
						{
							outResult = fog.apply( c3d_sceneData.getBackgroundColour( utils, c3d_cameraData.gamma() )
								, outResult
								, baseSurface.worldPosition.xyz()
								, c3d_cameraData.position()
								, c3d_sceneData );
						}

						backgroundModel->applyVolume( vec2( ipixel )
							, utils.lineariseDepth( depth, c3d_cameraData.nearPlane(), c3d_cameraData.farPlane() )
							, vec2( c3d_cameraData.renderSize() )
							, c3d_cameraData.depthPlanes()
							, outResult );
						outResult.a() = 1.0_f;
					}
					writer.returnStmt( 1_b );
				}
				, sdw::InIVec2{ writer, "ipixel" }
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "pipeline" }
				, sdw::InUInt{ writer, "primitiveId" }
				, sdw::OutVec4{ writer, "outResult" } );

			if constexpr ( VisibilityResolvePass::useCompute )
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

				ShaderWriter< VisibilityResolvePass::useCompute >::implementMain( writer
					, [&]( sdw::UVec2 const & pos )
					{
						auto pixelID = writer.declLocale( "pixelID"
							, materialsStarts[pipelineId] + pos.x() );
						auto pixel = writer.declLocale( "pixel"
							, pixelsXY[pixelID] );
						auto ipixel = writer.declLocale( "ipixel"
							, ivec2( pixel ) );
						auto indata = writer.declLocale( "indata"
							, c3d_imgData.load( ipixel ) );
						auto nodePipelineId = writer.declLocale( "nodePipelineId"
							, indata.x() );
						auto nodeId = writer.declLocale( "nodeId"
							, nodePipelineId >> maxPipelinesSize );
						auto pipeline = writer.declLocale( "pipeline"
							, nodePipelineId & maxPipelinesMask );
						auto primitiveId = writer.declLocale( "primitiveId"
							, indata.y() );
						auto result = writer.declLocale( "result", vec4( 0.0_f ) );

						IF( writer, ( stride != 0u ? ( nodeId == billboardNodeId ) : nodeId != 0_u )
							&& shade( ipixel, nodeId, pipeline, primitiveId, result ) )
						{
							c3d_imgOutResult.store( ipixel, result );
						}
						FI;
					} );
			}
			else
			{
				uint32_t idx = 0u;
				auto c3d_imgOutResult = writer.declOutput< sdw::Vec4 >( "c3d_imgOutResult", idx++ );

				ShaderWriter< VisibilityResolvePass::useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & pos )
					{
						auto indata = writer.declLocale( "indata"
							, c3d_imgData.load( pos ) );
						auto nodePipelineId = writer.declLocale( "nodePipelineId"
							, indata.x() );
						auto nodeId = writer.declLocale( "nodeId"
							, nodePipelineId >> maxPipelinesSize );
						auto pipeline = writer.declLocale( "pipeline"
							, nodePipelineId & maxPipelinesMask );
						auto primitiveId = writer.declLocale( "primitiveId"
							, indata.y() );
						auto result = writer.declLocale( "result", vec4( 0.0_f ) );

						IF( writer, ( stride != 0u ? ( nodeId != billboardNodeId ) : nodeId == 0_u )
							|| !shade( pos, nodeId, pipeline, primitiveId, result ) )
						{
							writer.demote();
						}
						FI;

						c3d_imgOutResult = result;
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ashes::DescriptorSetLayoutPtr createInDescriptorLayout( RenderDevice const & device
			, std::string const & name
			, MaterialCache const & matCache
			, crg::ImageViewIdArray const & targetImage
			, Scene const & scene
			, RenderTechnique const & technique
			, bool allowClusteredLighting
			, Texture const * ssao
			, IndirectLightingData const * indirectLighting )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings;
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute
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
			bindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( InOutBindings::eTexConfigs
				, stages ) );
			bindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( InOutBindings::eTexAnims
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eInData
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( InOutBindings::eMapBrdf
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );

			if constexpr ( VisibilityResolvePass::useCompute )
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
			}

			auto & engine = *device.renderSystem.getEngine();
			auto index = uint32_t( InOutBindings::eCount );
			engine.addSpecificsBuffersBindings( bindings
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, index );
			bindings.emplace_back( scene.getLightCache().createLayoutBinding( index++ ) );

			if ( ssao )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_FRAGMENT_BIT ) ); // c3d_mapOcclusion
			}

			if ( technique.hasShadowBuffer() )
			{
				RenderNodesPass::addShadowBindings( bindings
					, index );
			}

			bindings.emplace_back( makeDescriptorSetLayoutBinding( index++ // c3d_mapEnvironment
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );

			if ( auto background = scene.getBackground() )
			{
				RenderNodesPass::addBackgroundBindings( *background
					, bindings
					, index );
			}

			RenderNodesPass::addGIBindings( *indirectLighting
				, bindings
				, index );

			if ( allowClusteredLighting )
			{
				RenderNodesPass::addClusteredLightingBindings( technique.getRenderTarget().getFrustumClusters()
					, bindings
					, index );
			}

			return device->createDescriptorSetLayout( name + "InOut"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createInDescriptorSet( std::string const & name
			, ashes::DescriptorSetPool const & pool
			, crg::RunnableGraph & graph
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, RenderTechnique const & technique
			, Scene const & scene
			, bool allowClusteredLighting
			, crg::ImageViewIdArray const & targetImage
			, Texture const * ssao
			, IndirectLightingData const * indirectLighting )
		{
			auto & engine = *scene.getOwner();
			auto & matCache = engine.getMaterialCache();
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
			writes.push_back( matCache.getTexConfigBuffer().getBinding( InOutBindings::eTexConfigs ) );
			writes.push_back( matCache.getTexAnimBuffer().getBinding( InOutBindings::eTexAnims ) );
			auto & visibilityPassResult = technique.getVisibilityResult();
			writes.push_back( makeImageViewDescriptorWrite( visibilityPassResult.targetView
				, InOutBindings::eInData ) );
			writes.push_back( makeImageViewDescriptorWrite( engine.getRenderSystem()->getPrefilteredBrdfTexture().wholeView
				, *engine.getRenderSystem()->getPrefilteredBrdfTexture().sampler
				, InOutBindings::eMapBrdf ) );

			if constexpr ( VisibilityResolvePass::useCompute )
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
			}

			auto index = uint32_t( InOutBindings::eCount );
			engine.addSpecificsBuffersDescriptors( writes, index );
			writes.push_back( scene.getLightCache().getBinding( index++ ) );

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

			RenderNodesPass::addGIDescriptor( *indirectLighting
				, writes
				, index );

			if ( allowClusteredLighting )
			{
				RenderNodesPass::addClusteredLightingDescriptor( technique.getRenderTarget().getFrustumClusters()
					, writes
					, index );
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
			, PipelineFlags const & flags )
		{
			auto stages = VkShaderStageFlags( VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT );
			ashes::VkDescriptorSetLayoutBindingArray bindings;

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
			, PipelineFlags const & flags
			, ashes::DescriptorSetPool const & pool
			, ObjectBufferPool::ModelBuffers const & modelBuffers
			, ashes::BufferBase const * indexBuffer )
		{
			ashes::WriteDescriptorSetArray writes;

			if ( flags.enableIndices() )
			{
				CU_Require( indexBuffer );
				writes.emplace_back( makeDescriptorWrite( *indexBuffer, VtxBindings::eInIndices, 0u, indexBuffer->getSize() ) );
			}

			if ( flags.enablePosition() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::ePositions )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPosition, 0u, buffer.getSize() ) );
			}

			if ( flags.enableNormal() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eNormals )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInNormal, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTangentSpace() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTangents )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTangent, 0u, buffer.getSize() ) );
			}

			if ( flags.enableBitangent() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eBitangents )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInBitangent, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord0() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords0 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord0, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord1() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords1 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord1, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord2() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords2 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord2, 0u, buffer.getSize() ) );
			}

			if ( flags.enableTexcoord3() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eTexcoords3 )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInTexcoord3, 0u, buffer.getSize() ) );
			}

			if ( flags.enableColours() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eColours )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInColour, 0u, buffer.getSize() ) );
			}

			if ( flags.enablePassMasks() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::ePassMasks )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInPassMasks, 0u, buffer.getSize() ) );
			}

			if ( flags.enableVelocity() )
			{
				auto & buffer = modelBuffers.buffers[size_t( SubmeshData::eVelocity )]->getBuffer();
				writes.emplace_back( makeDescriptorWrite( buffer, VtxBindings::eInVelocity, 0u, buffer.getSize() ) );
			}

			auto result = pool.createDescriptorSet( name + "Vtx"
				, Sets::eVtx );
			result->setBindings( std::move( writes ) );
			result->update();
			return result;
		}

		static ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, std::string const & name
			, crg::ImageViewIdArray const & targetImage
			, bool first )
		{
			auto srcLayout = ( first
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
			attaches.push_back( { 0u
				, targetImage.front().data->info.format
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, srcLayout
				, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } );
			ashes::SubpassDescription subpassesDesc{ 0u
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, {}
				, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
				, {}
				, ashes::nullopt
				, {} };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.push_back( std::move( subpassesDesc ) );
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
				, std::move( attaches )
				, std::move( subpasses )
				, std::move( dependencies ) };
			return device->createRenderPass( name
				, std::move( createInfo ) );
		}

		static ashes::FrameBufferPtr createFrameBuffer( ashes::RenderPass const & renderPass
			, std::string const & name
			, RenderTechnique const & technique
			, crg::RunnableGraph & graph
			, crg::ImageViewIdArray const & targetImage )
		{
			ashes::VkImageViewArray fbAttaches;
			auto extent = getExtent( targetImage.front() );
			fbAttaches.emplace_back( graph.createImageView( targetImage.front() ) );
			return renderPass.createFrameBuffer( name
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
			, crg::ImageViewIdArray const & targetImage
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
			ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u, {}, {} };
			ashes::PipelineViewportStateCreateInfo viewportState{ 0u
				, { makeViewport( castor::Point2ui{ extent.width, extent.height } ) }
				, { makeScissor( castor::Point2ui{ extent.width, extent.height } ) } };
			return device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
				, std::move( stages )
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
		, ShaderBuffer * pipelinesIds
		, RenderNodesPassDesc const & renderPassDesc
		, RenderTechniquePassDesc const & techniquePassDesc )
		: castor::Named{ category + cuT( "/" ) + name }
		, RenderTechniquePass{ parent, *parent->getRenderTarget().getScene() }
		, crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( useCompute ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ); } )
				, [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); }
				, GetPassIndexCallback( [](){ return 0u; } )
				, IsEnabledCallback( [this](){ return doIsEnabled(); } )
				, IsComputePassCallback( [](){ return VisibilityResolvePass::useCompute; } ) }
			, renderPassDesc.m_ruConfig }
		, m_device{ device }
		, m_nodesPass{ nodesPass }
		, m_pipelinesIds{ pipelinesIds }
		, m_cameraUbo{ renderPassDesc.m_cameraUbo }
		, m_sceneUbo{ *renderPassDesc.m_sceneUbo }
		, m_targetImage{ std::move( targetImage ) }
		, m_targetDepth{ std::move( targetDepth ) }
		, m_ssao{ techniquePassDesc.m_ssao }
		, m_onNodesPassSort( m_nodesPass.onSortNodes.connect( [this]( RenderNodesPass const & pass ){ m_commandsChanged = true; } ) )
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, getName()
			, ( useCompute
				? nullptr
				: visres::ShaderWriter< false >::getVertexProgram() ) }
		, m_firstRenderPass{ ( useCompute
			? nullptr
			: visres::createRenderPass( m_device, getName(), m_targetImage, true ) ) }
		, m_firstFramebuffer{ ( useCompute
			? nullptr
			: visres::createFrameBuffer( *m_firstRenderPass, getName(), *m_parent, graph, m_targetImage ) ) }
		, m_blendRenderPass{ ( useCompute
			? nullptr
			: visres::createRenderPass( m_device, getName(), m_targetImage, false ) ) }
		, m_blendFramebuffer{ ( useCompute
			? nullptr
			: visres::createFrameBuffer( *m_blendRenderPass, getName(), *m_parent, graph, m_targetImage ) ) }
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
			m_activeBillboardPipelines.clear();

			if ( m_pipelinesIds )
			{
				auto [count, maxPipelineId] = m_nodesPass.fillPipelinesIds( castor::makeArrayView( reinterpret_cast< uint32_t * >( m_pipelinesIds->getPtr() )
					, MaxObjectNodesCount ) );
				m_pipelinesIds->setCount( count );
				m_pipelinesIds->setSecondCount( maxPipelineId );
			}

			uint32_t index = 0u;

			for ( auto & bufferIt : m_nodesPass.getPassPipelineNodes() )
			{
				auto buffer = bufferIt.second;
				auto pipelineHash = bufferIt.first;
				auto pipelineId = index++;
				PipelineFlags pipelineFlags{ getPipelineHiHashDetails( *this
					, pipelineHash
					, getShaderFlags() ) };
				pipelineFlags.m_sceneFlags = getScene().getFlags();
				pipelineFlags.backgroundModelId = getScene().getBackground()->getModelID();
				//pipelineHash = getPipelineBaseHash( getEngine()->getPassComponentsRegister(), pipelineFlags );

				if ( !pipelineFlags.isBillboard() )
				{
					auto & pipeline = doCreatePipeline( pipelineHash, pipelineFlags );
					auto it = m_activePipelines.emplace( &pipeline
						, SubmeshPipelinesNodesDescriptors{} ).first;

					size_t hash = 0u;
					hash = castor::hashCombinePtr( hash, *buffer );
					auto ires = pipeline.vtxDescriptorSets.emplace( hash, ashes::DescriptorSetPtr{} );

					if ( ires.second )
					{
						auto & modelBuffers = m_device.geometryPools->getBuffers( *buffer );
						ashes::BufferBase const * indexBuffer{};

						if ( pipelineFlags.enableIndices() )
						{
							indexBuffer = &m_device.geometryPools->getIndexBuffer( *buffer );
						}

						ires.first->second = visres::createVtxDescriptorSet( getName()
							, pipelineFlags
							, *pipeline.vtxDescriptorPool
							, modelBuffers
							, indexBuffer );
					}

					it->second.emplace( ires.first->second.get(), pipelineId );
				}
			}

			for ( auto & itPipeline : m_nodesPass.getBillboardNodes() )
			{
				for ( auto & itBuffer : itPipeline.second )
				{
					for ( auto & culled : itBuffer.second )
					{
						auto & positionsBuffer = culled.node->data.getVertexBuffer();
						auto & pipelineFlags = itPipeline.first->getFlags();
						auto pipelineHash = itPipeline.first->getFlagsHash();
						auto & pipeline = doCreatePipeline( pipelineHash
							, pipelineFlags
							, culled.node->data.getVertexStride() );
						auto it = m_activeBillboardPipelines.emplace( &pipeline
							, BillboardPipelinesNodesDescriptors{} ).first;
						auto hash = size_t( positionsBuffer.getOffset() );
						hash = castor::hashCombinePtr( hash, positionsBuffer.getBuffer().getBuffer() );
						auto ires = pipeline.vtxDescriptorSets.emplace( hash, ashes::DescriptorSetPtr{} );
						auto pipelineId = m_nodesPass.getPipelineNodesIndex( pipelineHash
							, *itBuffer.first );

						if ( ires.second )
						{
							ires.first->second = visres::createVtxDescriptorSet( getName()
								, *pipeline.vtxDescriptorPool
								, positionsBuffer.getBuffer().getBuffer()
								, positionsBuffer.getOffset()
								, positionsBuffer.getSize() );
						}

						it->second.emplace( culled.node->getId()
							, PipelineNodesDescriptors{ pipelineId, ires.first->second.get() } );
					}
				}
			}

			reRecordCurrent();
			m_commandsChanged = false;
		}
	}

	PipelineFlags VisibilityResolvePass::createPipelineFlags( PassComponentCombine components
		, BlendMode colourBlendMode
		, BlendMode alphaBlendMode
		, RenderPassTypeID renderPassTypeId
		, LightingModelID lightingModelId
		, BackgroundModelID backgroundModelId
		, VkCompareOp alphaFunc
		, VkCompareOp blendAlphaFunc
		, TextureCombine const & textures
		, SubmeshFlags const & submeshFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, VkPrimitiveTopology topology
		, bool isFrontCulled
		, uint32_t passLayerIndex
		, GpuBufferOffsetT< castor::Point4f > const & morphTargets )const
	{
		auto result = m_nodesPass.createPipelineFlags( std::move( components )
			, colourBlendMode
			, alphaBlendMode
			, renderPassTypeId
			, lightingModelId
			, backgroundModelId
			, alphaFunc
			, blendAlphaFunc
			, textures
			, submeshFlags
			, programFlags
			, sceneFlags
			, topology
			, isFrontCulled
			, passLayerIndex
			, morphTargets );
		result.m_shaderFlags = getShaderFlags();
		return result;
	}

	bool VisibilityResolvePass::areValidPassFlags( PassComponentCombine const & passFlags )const
	{
		return m_nodesPass.areValidPassFlags( passFlags );
	}

	ShaderFlags VisibilityResolvePass::getShaderFlags()const
	{
		return ShaderFlag::eNormal
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eWorldSpace
			| ShaderFlag::eViewSpace
			| ShaderFlag::eVelocity
			| ShaderFlag::eOpacity
			| ShaderFlag::eColour;
	}

	void VisibilityResolvePass::countNodes( RenderInfo & info )const
	{
		info.drawCalls += m_drawCalls;
	}

	bool VisibilityResolvePass::isPassEnabled()const
	{
		return m_nodesPass.isPassEnabled()
			&& doIsEnabled();
	}

	ComponentModeFlags VisibilityResolvePass::getComponentsMask()
	{
		return ( ComponentModeFlag::eColour
			| ComponentModeFlag::eOpacity
			| ComponentModeFlag::eDiffuseLighting
			| ComponentModeFlag::eSpecularLighting
			| ComponentModeFlag::eNormals
			| ComponentModeFlag::eGeometry
			| ComponentModeFlag::eOcclusion );
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
		m_drawCalls = {};
		auto size = uint32_t( m_parent->getMaterialsStarts().getCount() );
		std::array< VkDescriptorSet, 3u > descriptorSets{ VkDescriptorSet{}
			, VkDescriptorSet{}
			, *getScene().getBindlessTexDescriptorSet() };
		visres::PushData pushData{ 0u, 0u };
		std::vector< VkClearValue > clearValues;
		clearValues.push_back( transparentBlackClearColor );
		bool first = true;

		for ( auto & pipelineIt : m_activePipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, ( first
					? *pipelineIt.first->shaders[0].pipeline
					: *pipelineIt.first->shaders[1].pipeline ) );
			descriptorSets[0] = *pipelineIt.first->ioDescriptorSet;

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSetIt.first;
				pushData.pipelineId = descriptorSetIt.second;
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
				++m_drawCalls;
				first = false;
			}
		}

		for ( auto & pipelineIt : m_activeBillboardPipelines )
		{
			context.getContext().vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, ( first
					? *pipelineIt.first->shaders[0].pipeline
					: *pipelineIt.first->shaders[1].pipeline ) );
			descriptorSets[0] = *pipelineIt.first->ioDescriptorSet;

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSetIt.second.vtxDescriptorSet;
				pushData.pipelineId = descriptorSetIt.second.pipelineId;
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
				++m_drawCalls;
				first = false;
			}
		}

		context.setLayoutState( m_targetImage.front()
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
	}

	void VisibilityResolvePass::doRecordGraphics( crg::RecordContext & context
		, VkCommandBuffer commandBuffer )
	{
		m_drawCalls = {};
		std::array< VkDescriptorSet, 3u > descriptorSets{ VkDescriptorSet{}
			, VkDescriptorSet{}
			, *getScene().getBindlessTexDescriptorSet() };
		bool first = true;
		bool renderPassBound = false;
		bool pipelineBound = false;
		visres::PushData pushData{ 0u, 0u };

		auto bind = [&]( Pipeline const & pipeline, bool billboards )
		{
			if ( !renderPassBound )
			{
				std::vector< VkClearValue > clearValues;
				clearValues.push_back( transparentBlackClearColor );
				auto & extent = m_parent->getNormal().getExtent();
				VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
					, nullptr
					, VkRenderPass{}
					, VkFramebuffer{}
					, VkRect2D{ { 0, 0 }, { extent.width, extent.height } }
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
						? *pipeline.shaders[0].pipeline
						: *pipeline.shaders[1].pipeline ) );
				pipelineBound = true;
			}

			context.getContext().vkCmdPushConstants( commandBuffer
				, *pipeline.pipelineLayout
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, 0u
				, sizeof( visres::PushData )
				, &pushData );
		};

		for ( auto & pipelineIt : m_activePipelines )
		{
			descriptorSets[0] = *pipelineIt.first->ioDescriptorSet;

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSetIt.first;
				pushData.pipelineId = descriptorSetIt.second;
				bind( *pipelineIt.first, false );
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );
				++m_drawCalls;

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
			descriptorSets[0] = *pipelineIt.first->ioDescriptorSet;

			for ( auto & descriptorSetIt : pipelineIt.second )
			{
				descriptorSets[1] = *descriptorSetIt.second.vtxDescriptorSet;
				pushData.pipelineId = descriptorSetIt.second.pipelineId;
				pushData.billboardNodeId = descriptorSetIt.first;
				bind( *pipelineIt.first, true );
				context.getContext().vkCmdBindDescriptorSets( commandBuffer
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, *pipelineIt.first->pipelineLayout
					, 0u
					, uint32_t( descriptorSets.size() )
					, descriptorSets.data()
					, 0u
					, nullptr );
				context.getContext().vkCmdDraw( commandBuffer, 3u, 1u, 0u, 0u );
				++m_drawCalls;

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

		context.setLayoutState( m_targetImage.front()
			, crg::makeLayoutState( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineBaseHash const & hash
		, PipelineFlags const & flags
		, uint32_t stride )
	{
		return doCreatePipeline( hash
			, flags
			, stride
			, m_billboardPipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineBaseHash const & hash
		, PipelineFlags const & flags )
	{
		return doCreatePipeline( hash
			, flags
			, 0u
			, m_pipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PipelineBaseHash const & hash
		, PipelineFlags const & flags
		, uint32_t stride
		, PipelineContainer & pipelines )
	{
		auto it = pipelines.find( hash );

		if ( it == pipelines.end() )
		{
			auto stageBit = VisibilityResolvePass::useCompute
				? VK_SHADER_STAGE_COMPUTE_BIT
				: VK_SHADER_STAGE_FRAGMENT_BIT;
			auto stageFlags = VkShaderStageFlags( stageBit );
			auto extent = m_parent->getNormal().getExtent();
			auto result = std::make_unique< Pipeline >();
			result->vtxDescriptorLayout = stride == 0u
				? visres::createVtxDescriptorLayout( m_device, getName(), flags )
				: visres::createVtxDescriptorLayout( m_device, getName() );
			result->ioDescriptorLayout = visres::createInDescriptorLayout( m_device, getName(), getScene().getOwner()->getMaterialCache()
				, m_targetImage, getScene(), *m_parent, m_allowClusteredLighting, m_ssao, &m_parent->getIndirectLighting() );
			result->pipelineLayout = m_device->createPipelineLayout( getName()
				, { *result->ioDescriptorLayout, *result->vtxDescriptorLayout, *getScene().getBindlessTexDescriptorLayout() }
				, { { stageFlags, 0u, sizeof( visres::PushData ) } } );

			result->shaders[0].shader = ShaderModule{ stageBit
				, getName()
				, visres::getProgram( m_device, getScene(), *m_parent, extent, flags, &m_parent->getIndirectLighting(), getDebugConfig(), stride, false, m_ssao != nullptr, m_allowClusteredLighting ) };
			result->shaders[1].shader = ShaderModule{ stageBit
				, getName()
				, visres::getProgram( m_device, getScene(), *m_parent, extent, flags, &m_parent->getIndirectLighting(), getDebugConfig(), stride, true, m_ssao != nullptr, m_allowClusteredLighting ) };

			if constexpr ( useCompute )
			{
				ashes::PipelineShaderStageCreateInfo stage{ makeShaderState( m_device, result->shaders[0].shader ) };
				result->shaders[0].pipeline = m_device->createPipeline( ashes::ComputePipelineCreateInfo{ 0u
					, std::move( stage )
					, *result->pipelineLayout } );
				stage = makeShaderState( m_device, result->shaders[1].shader );
				result->shaders[1].pipeline = m_device->createPipeline( ashes::ComputePipelineCreateInfo{ 0u
					, std::move( stage )
					, *result->pipelineLayout } );
			}
			else
			{
				ashes::PipelineShaderStageCreateInfoArray stages{ makeShaderState( m_device, m_vertexShader ) };
				stages.push_back( makeShaderState( m_device, result->shaders[0].shader ) );
				result->shaders[0].pipeline = visres::createPipeline( m_device
					, extent
					, std::move( stages )
					, *result->pipelineLayout
					, *m_firstRenderPass
					, m_targetImage
					, false );
				stages.push_back( makeShaderState( m_device, m_vertexShader ) );
				stages.push_back( makeShaderState( m_device, result->shaders[1].shader ) );
				result->shaders[1].pipeline = visres::createPipeline( m_device
					, extent
					, std::move( stages )
					, *result->pipelineLayout
					, *m_blendRenderPass
					, m_targetImage
					, true );
			}

			result->vtxDescriptorPool = result->vtxDescriptorLayout->createPool( MaxPipelines );
			result->ioDescriptorPool = result->ioDescriptorLayout->createPool( 1u );
			result->ioDescriptorSet = visres::createInDescriptorSet( getName(), *result->ioDescriptorPool, m_graph, m_cameraUbo, m_sceneUbo, *m_parent, getScene()
				, m_allowClusteredLighting, m_targetImage, m_ssao, &m_parent->getIndirectLighting() );
			it = pipelines.emplace( hash, std::move( result ) ).first;
		}

		return *it->second;
	}
}
