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
			eMaterials,
			eTexConfigs,
			eTexAnims,
			eMaterialsCounts,
			eMaterialsStarts,
			ePixelsXY,
			eSsao,
			eInData0,
			eOutData2,
			eOutData3,
			eOutData4,
			eOutData5,
		};

		enum VtxBindings : uint32_t
		{
			eInIndices,
			eInPosition,
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

		struct BarycentricDerivatives
			: public sdw::StructInstanceHelperT< "BarycentricDerivatives"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec2Field< "interp" >
			, sdw::Vec2Field< "dx" >
			, sdw::Vec2Field< "dy" > >
		{
			BarycentricDerivatives( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto interp()const { return getMember< "interp" >(); }
			auto dx()const { return getMember< "dx" >(); }
			auto dy()const { return getMember< "dy" >(); }

			void computeGradient( shader::DerivTex & result
				, sdw::Float const & w
				, sdw::Float const & mip )
			{
				result.dx() = dx() * w * mip;
				result.dy() = dy() * w * mip;
				result.uv() = interp() * w;
			}
		};

		Writer_Parameter( BarycentricDerivatives );

		struct PartialDerivatives
			: public sdw::StructInstanceHelperT< "PartialDerivatives"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "db_dx" >
			, sdw::Vec3Field< "db_dy" > >
		{
			PartialDerivatives( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto db_dx()const { return getMember< "db_dx" >(); }
			auto db_dy()const { return getMember< "db_dy" >(); }

			static PartialDerivatives create( sdw::ShaderWriter & writer
				, std::string name
				, sdw::Vec2 const & p0
				, sdw::Vec2 const & p1
				, sdw::Vec2 const & p2 )
			{
				auto output = writer.declLocale< PartialDerivatives >( std::move( name ) );
				auto det = writer.declLocale( "det"
					, 1.0_f / determinant( mat2( p2 - p1, p0 - p1 ) ) );
				output.db_dx() = vec3( p1.y() - p2.y(), p2.y() - p0.y(), p0.y() - p1.y() ) * det;
				output.db_dy() = vec3( p2.x() - p1.x(), p0.x() - p2.x(), p1.x() - p0.x() ) * det;

				return output;
			}

			// Helper functions to interpolate vertex attributes at point 'd' using the partial derivatives
			sdw::Vec3 interpolate( sdw::Mat3 const & attributes
				, sdw::Vec2 const & d )
			{
				return ( vec3( attributes[0][0], attributes[1][0], attributes[2][0] )
					+ d.x() * ( attributes * db_dx() )
					+ d.y() * ( attributes * db_dy() ) );
			}

			sdw::Float interpolate( sdw::Vec3 const & attributes
				, sdw::Vec2 const & d )
			{
				return ( attributes[0]
					+ d.x() * dot( attributes, db_dx() )
					+ d.y() * dot( attributes, db_dy() ) );
			}

			// Interpolate 2D attributes using the partial derivatives and generates dx and dy for texture sampling.
			BarycentricDerivatives calcBarycentric( sdw::Mat3x2 const & pattributes
				, sdw::Vec2 const & pd
				, sdw::Vec2 const & ptwoOverRes )
			{
				if ( !m_calcBarycentric )
				{
					auto & writer = *getWriter();
					m_calcBarycentric = writer.implementFunction< BarycentricDerivatives >( "calcBarycentric"
						, [&]( sdw::Mat3x2 const & attributes
							, sdw::Vec3 const & db_dx
							, sdw::Vec3 const & db_dy
							, sdw::Vec2 const & d
							, sdw::Vec2 const & twoOverRes )
						{
							auto attr0 = writer.declLocale( "attr0"
								, vec3( attributes[0][0], attributes[1][0], attributes[2][0] ) );
							auto attr1 = writer.declLocale( "attr1"
								, vec3( attributes[0][1], attributes[1][1], attributes[2][1] ) );
							auto attribute_x = writer.declLocale( "attribute_x"
								, vec2( dot( db_dx, attr0 ), dot( db_dx, attr1 ) ) );
							auto attribute_y = writer.declLocale( "attribute_y"
								, vec2( dot( db_dy, attr0 ), dot( db_dy, attr1 ) ) );
							auto attribute_s = writer.declLocale( "attribute_s"
								, attributes[0] );

							auto result = writer.declLocale< BarycentricDerivatives >( "result" );
							result.dx() = attribute_x * twoOverRes.x();
							result.dy() = attribute_y * twoOverRes.y();
							result.interp() = ( attribute_s + d.x() * attribute_x + d.y() * attribute_y );
							writer.returnStmt( result );
						}
						, sdw::InMat3x2{ writer, "attributes" }
						, sdw::InVec3{ writer, "db_dx" }
						, sdw::InVec3{ writer, "db_dy" }
						, sdw::InVec2{ writer, "d" }
						, sdw::InVec2{ writer, "twoOverRes" } );
				}

				return m_calcBarycentric( pattributes, db_dx(), db_dy(), pd, ptwoOverRes );
			}

		private:
			sdw::Function< BarycentricDerivatives
				, sdw::InMat3x2
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec2
				, sdw::InVec2 > m_calcBarycentric;
		};

		Writer_Parameter( PartialDerivatives );

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
			DeclareSsbo( c3d_inTexcoord0
				, sdw::Vec4
				, VtxBindings::eInTexcoord0
				, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) );
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

			auto imgData0 = writer.declStorageImg< sdw::RImage2DRgba32 >( "imgData0", InOutBindings::eInData0, Sets::eInOuts );

			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps", TexBindings::eTextures, Sets::eTex ) );

			sdw::PushConstantBuffer pcb{ writer, "DrawData" };
			auto passTypeIndex = pcb.declMember< sdw::UInt >( "passTypeIndex" );
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

			auto loadIndices = writer.implementFunction< sdw::UVec3 >( "loadIndices"
				, [&]( sdw::UInt const & primitiveId
					, shader::ModelIndices const & modelData )
				{
					if ( stride == 0u )
					{
						auto baseIndex = writer.declLocale( "baseIndex"
							, modelData.getIndexOffset() + primitiveId * 3u );
						writer.returnStmt( uvec3( c3d_inIndices[baseIndex + 0u]
							, c3d_inIndices[baseIndex + 1u]
							, c3d_inIndices[baseIndex + 2u] ) );
					}
					else
					{
						auto instanceId = writer.declLocale( "instanceId"
							, primitiveId / 2u );
						auto firstTriangle = writer.declLocale( "firstTriangle"
							, ( primitiveId % 2u ) == 0u );
						writer.returnStmt( writer.ternary( firstTriangle
							, uvec3( instanceId + 0_u, instanceId + 1_u, instanceId + 2_u )
							, uvec3( instanceId + 1_u, instanceId + 3_u, instanceId + 2_u ) ) );
					}
				}
				, sdw::InUInt{ writer, "primitiveId" }
				, shader::InModelIndices{ writer, "modelData" } );

			auto loadVertex = writer.implementFunction< shader::VertexSurface >( "loadVertex"
				, [&]( sdw::UInt const & vertexId )
				{
					auto result = writer.declLocale< shader::VertexSurface >( std::string( "result" ) );
					result.position = c3d_inPosition[vertexId].position;
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

			auto loadSurface = writer.implementFunction< shader::DerivFragmentSurface >( "loadSurface"
				, [&]( sdw::UInt const & nodeId
					, sdw::UInt const & primitiveId
					, sdw::Vec2 const & pixelCoord
					, sdw::Float const & depth
					, shader::ModelIndices const & modelData )
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
					auto indices = writer.declLocale( "indices"
						, loadIndices( primitiveId, modelData ) );
					auto baseVertex = writer.declLocale( "baseVertex"
						, modelData.getVertexOffset() );

					auto v0 = writer.declLocale( "v0"
						, loadVertex( baseVertex + indices.x() ) );
					auto v1 = writer.declLocale( "v1"
						, loadVertex( baseVertex + indices.y() ) );
					auto v2 = writer.declLocale( "v2"
						, loadVertex( baseVertex + indices.z() ) );

					auto twoOverRes = writer.declLocale( "twoOverRes"
						, vec2( 2.0_f ) / c3d_sceneData.renderSize );

					// Transform positions to clip space
					auto p0 = writer.declLocale( "p0"
						, c3d_matrixData.worldToCurProj( checkFlag( submeshFlags, SubmeshFlag::eVelocity )
							? v0.position
							: modelData.modelToCurWorld( v0.position ) ) );
					auto p1 = writer.declLocale( "p1"
						, c3d_matrixData.worldToCurProj( checkFlag( submeshFlags, SubmeshFlag::eVelocity )
							? v1.position
							: modelData.modelToCurWorld( v1.position ) ) );
					auto p2 = writer.declLocale( "p2"
						, c3d_matrixData.worldToCurProj( checkFlag( submeshFlags, SubmeshFlag::eVelocity )
							? v2.position
							: modelData.modelToCurWorld( v2.position ) ) );

					// Calculate the inverse of w, since it's going to be used several times
					auto oneOverW = writer.declLocale( "oneOverW"
						,vec3( 1.0_f ) / vec3( p0.w(), p1.w(), p2.w() ) );

					// Project vertex positions to calculate 2D post-perspective positions
					p0.xyz() *= oneOverW.x();
					p1.xyz() *= oneOverW.y();
					p2.xyz() *= oneOverW.z();

					// Compute partial derivatives. This is necessary to interpolate triangle attributes per pixel.
					auto derivativesOut = PartialDerivatives::create( writer
						, "derivativesOut"
						, p0.xy(), p1.xy(), p2.xy() );

					// Calculate delta vector (d) that points from the projected vertex 0 to the current screen point
					auto d = writer.declLocale( "d"
						, screenCoords - p0.xy() );

					// Interpolate the 1/w (oneOverW) for all three vertices of the triangle
					// using the barycentric coordinates and the delta vector
					auto w = writer.declLocale( "w"
						, 1.0_f / derivativesOut.interpolate( oneOverW, d ) );

					// Reconstruct the Z value at this screen point performing only the necessary matrix * vector multiplication
					// operations that involve computing Z
					auto z = writer.declLocale( "z"
						, w * c3d_matrixData.getProjMtx()[2][2] + c3d_matrixData.getProjMtx()[3][2] );

					// TEXTURE COORD INTERPOLATION;
					auto linearZ = writer.declLocale( "linearZ"
						, utils.rescaleDepth( z / w, c3d_sceneData.nearPlane, c3d_sceneData.farPlane ) );
					auto mip = writer.declLocale( "mip"
						, pow( pow( linearZ, 0.9_f ) * 5.0_f, 1.5_f ) );

					// Interpolate texture coordinates and calculate the gradients for texture sampling with mipmapping support
					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
					{
						// Apply perspective correction to texture coordinates
						auto texCoords = writer.declLocale( "texCoords0"
							, mat3x2( v0.texture0.xy() * oneOverW.x()
								, v1.texture0.xy() * oneOverW.y()
								, v2.texture0.xy() * oneOverW.z() ) );
						auto results0 = writer.declLocale( "results0"
							, derivativesOut.calcBarycentric( texCoords, d, twoOverRes ) );
						results0.computeGradient( result.texture0, w, mip );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
					{
						// Apply perspective correction to texture coordinates
						auto texCoords = writer.declLocale( "texCoords1"
							, mat3x2( v0.texture0.xy() * oneOverW.x()
								, v1.texture0.xy() * oneOverW.y()
								, v2.texture0.xy() * oneOverW.z() ) );
						auto results1 = writer.declLocale( "results1"
							, derivativesOut.calcBarycentric( texCoords, d, twoOverRes ) );
						results1.computeGradient( result.texture1, w, mip );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
					{
						// Apply perspective correction to texture coordinates
						auto texCoords = writer.declLocale( "texCoords2"
							, mat3x2( v0.texture0.xy() * oneOverW.x()
								, v1.texture0.xy() * oneOverW.y()
								, v2.texture0.xy() * oneOverW.z() ) );
						auto results2 = writer.declLocale( "results2"
							, derivativesOut.calcBarycentric( texCoords, d, twoOverRes ) );
						results2.computeGradient( result.texture2, w, mip );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
					{
						// Apply perspective correction to texture coordinates
						auto texCoords = writer.declLocale( "texCoords3"
							, mat3x2( v0.texture0.xy() * oneOverW.x()
								, v1.texture0.xy() * oneOverW.y()
								, v2.texture0.xy() * oneOverW.z() ) );
						auto results3 = writer.declLocale( "results3"
							, derivativesOut.calcBarycentric( texCoords, d, twoOverRes ) );
						results3.computeGradient( result.texture3, w, mip );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
					{
						auto colours = writer.declLocale( "colours"
							, mat3( v0.colour.xyz(), v1.colour.xyz(), v2.colour.xyz() ) );
						result.colour = derivativesOut.interpolate( colours, d );
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
					{
						auto passMultipliers0 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers0", 4u );
						auto passMultipliers1 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers1", 4u );
						auto passMultipliers2 = writer.declLocaleArray< sdw::Vec4 >( "passMultipliers2", 4u );
						auto material = writer.declLocale( "material"
							, materials.getMaterial( modelData.getMaterialId() ) );
						material.getPassMultipliers( submeshFlags
							, v0.passMasks
							, passMultipliers0 );
						material.getPassMultipliers( submeshFlags
							, v1.passMasks
							, passMultipliers1 );
						material.getPassMultipliers( submeshFlags
							, v2.passMasks
							, passMultipliers2 );
						auto passMultipliersXYZ = writer.declLocale< sdw::Mat3 >( "passMultipliersXYZ" );
						auto passMultipliersW = writer.declLocale< sdw::Vec3 >( "passMultipliersW" );

						for ( uint32_t i = 0u; i < 4u; ++i )
						{
							passMultipliersXYZ = mat3( passMultipliers0[i].xyz(), passMultipliers1[i].xyz(), passMultipliers2[i].xyz() );
							passMultipliersW = vec3( passMultipliers0[i].w(), passMultipliers1[i].w(), passMultipliers2[i].w() );
							result.passMultipliers[i] = vec4( derivativesOut.interpolate( passMultipliersXYZ, d )
								, derivativesOut.interpolate( passMultipliersW, d ) );
						}
					}

					auto positions = writer.declLocale( "positions"
						, mat3( v0.position.xyz(), v1.position.xyz(), v2.position.xyz() ) );
					result.curPosition = vec4( derivativesOut.interpolate( positions, d ), 1.0_f );
					result.prvPosition = result.curPosition;

					if ( checkFlag( submeshFlags, SubmeshFlag::eVelocity ) )
					{
						auto velocities = writer.declLocale( "velocities"
							, mat3( v0.velocity.xyz(), v1.velocity.xyz(), v2.velocity.xyz() ) );
						auto velocity = writer.declLocale( "velocity"
							, derivativesOut.interpolate( velocities, d ) );
						result.prvPosition.xyz() += velocity;
						result.prvPosition = c3d_matrixData.worldToPrvProj( result.prvPosition );
					}
					else
					{
						result.curPosition = modelData.modelToCurWorld( result.curPosition );
						result.prvPosition = modelData.modelToPrvWorld( result.prvPosition );
					}

					result.prvPosition = c3d_matrixData.worldToPrvProj( result.prvPosition );
					result.curPosition = c3d_matrixData.worldToCurProj( result.curPosition );
					result.computeVelocity( c3d_matrixData
						, result.curPosition
						, result.prvPosition );

					writer.returnStmt( result );
				}
				, sdw::InUInt{ writer, "nodeId" }
				, sdw::InUInt{ writer, "primitiveId" }
				, sdw::InVec2{ writer, "pixelCoord" }
				, sdw::InFloat{ writer, "depth" }
				, shader::InModelIndices{ writer, "modelData" } );

			auto shade = writer.implementFunction< sdw::Boolean >( "shade"
				, [&]( sdw::IVec2 const & ipixel
					, sdw::Vec4 outData2
					, sdw::Vec4 outData3
					, sdw::Vec4 outData4
					, sdw::Vec4 outData5 )
				{
					auto data0 = writer.declLocale( "data0"
						, imgData0.load( ipixel ) );
					auto nodeId = writer.declLocale( "nodeId"
						, writer.cast< sdw::UInt >( data0.z() ) );

					IF( writer, nodeId == 0_u )
					{
						writer.returnStmt( 0_b );
					}
					FI;

					auto primitiveId = writer.declLocale( "primitiveId"
						, writer.cast< sdw::UInt >( data0.w() ) );
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					auto surface = writer.declLocale( "surface"
						, loadSurface( nodeId
							, primitiveId
							, vec2( ipixel )
							, data0.x()
							, modelData ) );
					shader::VisibilityBlendComponents components{ writer
						, "out"
						, { surface.texture0, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) }
						, { surface.texture1, checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) }
						, { surface.texture2, checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) }
						, { surface.texture3, checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) }
						, 1.0_f
						, 1.0_f
						, 1.0_f
						, vec3( 0.0_f ) };
					auto lightMat = materials.blendMaterials( utils
						, passFlags
						, submeshFlags
						, TextureFlag::eAllButGeometry
						, checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 )
						, textureConfigs
						, textureAnims
						, *lightingModel
						, c3d_maps
						, modelData.getMaterialId()
						, surface.passMultipliers
						, surface.colour
						, components );
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
				, sdw::OutVec4{ writer, "outData2" }
				, sdw::OutVec4{ writer, "outData3" }
				, sdw::OutVec4{ writer, "outData4" }
				, sdw::OutVec4{ writer, "outData5" } );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
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
						auto data2 = writer.declLocale( "data2", vec4( 0.0_f ) );
						auto data3 = writer.declLocale( "data3", vec4( 0.0_f ) );
						auto data4 = writer.declLocale( "data4", vec4( 0.0_f ) );
						auto data5 = writer.declLocale( "data5", vec4( 0.0_f ) );

						IF( writer, shade( ipixel, data2, data3, data4, data5 ) )
						{
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
				auto imgData2 = writer.declOutput< sdw::Vec4 >( "imgData2", idx++ );
				auto imgData3 = writer.declOutput< sdw::Vec4 >( "imgData3", idx++ );
				auto imgData4 = writer.declOutput< sdw::Vec4 >( "imgData4", idx++ );
				auto imgData5 = writer.declOutput< sdw::Vec4 >( "imgData5", idx++ );

				ShaderWriter< VisibilityResolvePass::useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & pos )
					{
						auto data2 = writer.declLocale( "data2", vec4( 0.0_f ) );
						auto data3 = writer.declLocale( "data3", vec4( 0.0_f ) );
						auto data4 = writer.declLocale( "data4", vec4( 0.0_f ) );
						auto data5 = writer.declLocale( "data5", vec4( 0.0_f ) );

						IF( writer, shade( pos, data2, data3, data4, data5 ) )
						{
							imgData2 = data2;
							imgData3 = data3;
							imgData4 = data4;
							imgData5 = data5;
						}
						ELSE
						{
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
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eMatrix
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eScene
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eModels
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( matCache.getPassBuffer().createLayoutBinding( visres::InOutBindings::eMaterials
				, stages ) );
			bindings.emplace_back( matCache.getTexConfigBuffer().createLayoutBinding( visres::InOutBindings::eTexConfigs
				, stages ) );
			bindings.emplace_back( matCache.getTexAnimBuffer().createLayoutBinding( visres::InOutBindings::eTexAnims
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eMaterialsCounts
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eMaterialsStarts
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::ePixelsXY
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages ) );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eInData0
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, stages ) );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
				bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eOutData2
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eOutData3
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eOutData4
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, stages ) );
				bindings.emplace_back( makeDescriptorSetLayoutBinding( visres::InOutBindings::eOutData5
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
			auto & opaquePassResult = technique.getOpaqueResult();
			writes.push_back( makeDescriptorWrite( opaquePassResult[DsTexture::eData0].targetView
				, InOutBindings::eInData0 ) );

			if constexpr ( VisibilityResolvePass::useCompute )
			{
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
					, { 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
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
			fbAttaches.emplace_back( textures[DsTexture::eData2].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData3].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData4].targetView );
			fbAttaches.emplace_back( textures[DsTexture::eData5].targetView );
			return renderPass.createFrameBuffer( name
				, makeVkStruct< VkFramebufferCreateInfo >( 0u
					, renderPass
					, uint32_t( fbAttaches.size() )
					, fbAttaches.data()
					, textures[DsTexture::eData2].getExtent().width
					, textures[DsTexture::eData2].getExtent().height
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
				PipelineBaseHash const & hash = itPipeline.first;
				auto [submeshFlags, programFlags, passType, passFlags, maxTexcoordSet, texturesCount, textureFlags] = getPipelineHashDetails( hash );
				auto & pipeline = doCreatePipeline( passType
					, submeshFlags & SubmeshFlag::eAllButNmlTan
					, passFlags & PassFlag::eAllVisibility );
				auto it = m_activePipelines.emplace( &pipeline
					, std::set< ashes::DescriptorSet const * >{} ).first;

				for ( auto & itBuffer : itPipeline.second )
				{
					auto ires = pipeline.descriptorSets.emplace( itBuffer.first, ashes::DescriptorSetPtr{} );

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
				PipelineBaseHash const & hash = itPipeline.first;
				auto [submeshFlags, programFlags, passType, passFlags, maxTexcoordSet, texturesCount, textureFlags] = getPipelineHashDetails( hash );

				for ( auto & itBuffer : itPipeline.second )
				{
					for ( auto & sidedCulled : itBuffer.second )
					{
						auto & culled = sidedCulled.first;
						auto & positionsBuffer = culled.node->data.getVertexBuffer();
						auto & pipeline = doCreatePipeline( passType
							, passFlags & PassFlag::eAllVisibility
							, culled.node->data.getVertexStride() );
						auto it = m_activeBillboardPipelines.emplace( &pipeline
							, std::set< ashes::DescriptorSet const * >{} ).first;
						auto ires = pipeline.descriptorSets.emplace( &positionsBuffer.getBuffer().getBuffer(), ashes::DescriptorSetPtr{} );

						if ( ires.second )
						{
							ires.first->second = visres::createVtxDescriptorSet( getName()
								, *pipeline.descriptorPool
								, positionsBuffer.getBuffer().getBuffer()
								, positionsBuffer.getOffset()
								, positionsBuffer.getSize() );
						}

						it->second.emplace( ires.first->second.get() );
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

		auto loopPipelines = [&]( std::map< Pipeline const *, std::set< ashes::DescriptorSet const * > > const & pipelines )
		{
			for ( auto & pipelineIt : pipelines )
			{
				context.getContext().vkCmdBindPipeline( commandBuffer
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, *pipelineIt.first->firstPipeline );
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipelineIt.first->pipelineLayout
					, VK_SHADER_STAGE_COMPUTE_BIT
					, 0u
					, sizeof( uint32_t )
					, &pipelineIt.first->passTypeIndex );

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
		};

		loopPipelines( m_activePipelines );
		loopPipelines( m_activeBillboardPipelines );

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

		auto bind = [&]( Pipeline const & pipeline )
		{
			if ( !renderPassBound )
			{
				static std::array< VkClearValue, 4u > clearValues{ getClearValue( DsTexture::eData2 )
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
				context.getContext().vkCmdPushConstants( commandBuffer
					, *pipeline.pipelineLayout
					, VK_SHADER_STAGE_FRAGMENT_BIT
					, 0u
					, sizeof( uint32_t )
					, &pipeline.passTypeIndex );
				pipelineBound = true;
			}
		};

		auto loopPipelines = [&]( std::map< Pipeline const *, std::set< ashes::DescriptorSet const * > > const & pipelines )
		{
			for ( auto & pipelineIt : pipelines )
			{
				for ( auto & descriptorSet : pipelineIt.second )
				{
					bind( *pipelineIt.first );

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
		};

		loopPipelines( m_activePipelines );
		loopPipelines( m_activeBillboardPipelines );

		if ( renderPassBound )
		{
			context.getContext().vkCmdEndRenderPass( commandBuffer );
		}

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
		, PassFlags passFlags
		, uint32_t stride )
	{
		return doCreatePipeline( passType
			, SubmeshFlag::ePositions
			, passFlags
			, stride
			, m_billboardPipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PassTypeID passType
		, SubmeshFlags submeshFlags
		, PassFlags passFlags )
	{
		return doCreatePipeline( passType
			, submeshFlags
			, passFlags
			, 0u
			, m_pipelines );
	}

	VisibilityResolvePass::Pipeline & VisibilityResolvePass::doCreatePipeline( PassTypeID passType
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
			, TextureFlag::eNone );

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
				, visres::getProgram( m_device, extent, passType, submeshFlags, passFlags, stride ) } );
			result->passTypeIndex = matCache.getPassTypeIndex( passType, passFlags );

			if constexpr ( !useCompute )
			{
				result->stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, m_vertexShader ) };
			}

			result->stages.push_back( makeShaderState( m_device, result->shader ) );
			result->descriptorLayout = visres::createVtxDescriptorLayout( m_device, getName(), submeshFlags );
			result->pipelineLayout = m_device->createPipelineLayout( getName()
				, { *m_inOutsDescriptorLayout, *result->descriptorLayout, *getScene().getBindlessTexDescriptorLayout() }
				, { { stages, 0u, sizeof( uint32_t ) } } );

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
