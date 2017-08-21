#include "ReflectionPass.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Skybox.hpp>
#include <Shader/Ubos/MatrixUbo.hpp>
#include <Shader/Ubos/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr c_environmentStart = 6u;
		static uint32_t constexpr c_environmentCount = 16u;

		VertexBufferSPtr doCreateVbo( Engine & engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & renderSystem = *engine.getRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & program
			, VertexBuffer & vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, program );
			result->initialise( { vbo }, nullptr );
			return result;
		}

		glsl::Shader doCreateVertexProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader doCreateLegacyPixelProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
			auto c3d_mapPostLight = writer.declUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapEnvironment = writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment, c_environmentCount );
			auto c3d_fresnelBias = writer.declUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.declUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.declUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.declFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.declFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			declareDecodeMaterial( writer );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );
				auto postLight = writer.declLocale( cuT( "postLight" )
					, texture( c3d_mapPostLight, vtx_texture ).xyz() );

				IF( writer, "envMapIndex < 1 || ( reflection + refraction == 0 )" )
				{
					writer.discard();
				}
				FI;

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto normal = writer.declLocale( cuT( "normal" )
					, normalize( data1.xyz() ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );
				envMapIndex = envMapIndex - 1_i;
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, texture( c3d_mapData2, vtx_texture ).xyz() );

				IF( writer, reflection != 0_i )
				{
					auto reflected = writer.declLocale( cuT( "reflected" )
						, reflect( incident, normal ) );
					pxl_reflection = texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();
				}
				FI;

				IF( writer, refraction != 0_i )
				{
					auto ratio = writer.declLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );
					auto subRatio = writer.declLocale( cuT( "subRatio" )
						, 1.0_f - ratio );
					auto addRatio = writer.declLocale( cuT( "addRatio" )
						, 1.0_f + ratio );
					auto reflectance = writer.declLocale( cuT( "reflectance" )
						, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
					auto product = writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -incident, normal ) ) );
					auto fresnel = writer.declLocale( cuT( "fresnel" )
						, reflectance + writer.paren( 1.0_f - reflectance ) * pow( 1.0_f - product, 5.0 ) );
					auto refracted = writer.declLocale( cuT( "refracted" )
						, refract( incident, normal, ratio ) );
					pxl_refraction.xyz() = texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz();
					pxl_refraction.xyz() *= diffuse / length( diffuse );
					pxl_refraction.w() = fresnel;
				}
				FI;

				IF( writer, cuT( "reflection != 0 && refraction == 0" ) )
				{
					pxl_reflection *= diffuse / length( diffuse );
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrMRPixelProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
			auto c3d_mapBrdf = writer.declUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto c3d_mapIrradiance = writer.declUniform< SamplerCube >( ShaderProgram::MapIrradiance );
			auto c3d_mapPrefiltered = writer.declUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
			auto c3d_mapEnvironment = writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment, c_environmentCount );
			auto c3d_fresnelBias = writer.declUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.declUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.declUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.declFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.declFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeMetallicIBL();
			declareDecodeMaterial( writer );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto albedo = writer.declLocale( cuT( "albedo" )
					, data2.xyz() );
				auto metalness = writer.declLocale( cuT( "metalness" )
					, data3.r() );
				auto roughness = writer.declLocale( cuT( "roughness" )
					, data3.g() );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						pxl_reflection = texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();

						IF( writer, refraction == 0_i )
						{
							pxl_reflection *= albedo / length( albedo );
						}
						FI;
					}
					ELSE
					{
						pxl_reflection = utils.computeMetallicIBL( normal
							, position
							, albedo
							, metalness
							, roughness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					}
					FI;

					auto ratio = writer.declLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );

					IF( writer, refraction != 0_i )
					{
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( 1.0_f - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						pxl_refraction.xyz() = texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz();
						pxl_refraction.xyz() *= albedo / length( albedo );
						pxl_refraction.w() = fresnel;
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						pxl_refraction.xyz() = texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
						pxl_refraction.xyz() *= albedo / length( albedo );
						pxl_refraction.w() = fresnel;
					}
					FI;
				}
				ELSE
				{
					pxl_reflection = utils.computeMetallicIBL( normal
						, position
						, albedo
						, metalness
						, roughness
						, c3d_cameraPosition
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );

					IF( writer, ratio != 0.0_f )
					{
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						pxl_refraction.xyz() = texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
						pxl_refraction.xyz() *= albedo / length( albedo );
						pxl_refraction.w() = fresnel;
					}
					FI;
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader doCreatePbrSGPixelProgram( RenderSystem & renderSystem )
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			UBO_HDR_CONFIG( writer );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eDepth ) );
			auto c3d_mapData1 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData1 ) );
			auto c3d_mapData2 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData2 ) );
			auto c3d_mapData3 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData3 ) );
			auto c3d_mapData4 = writer.declUniform< Sampler2D >( getTextureName( DsTexture::eData4 ) );
			auto c3d_mapBrdf = writer.declUniform< Sampler2D >( ShaderProgram::MapBrdf );
			auto c3d_mapIrradiance = writer.declUniform< SamplerCube >( ShaderProgram::MapIrradiance );
			auto c3d_mapPrefiltered = writer.declUniform< SamplerCube >( ShaderProgram::MapPrefiltered );
			auto c3d_mapEnvironment = writer.declUniform< SamplerCube >( ShaderProgram::MapEnvironment, c_environmentCount );
			auto c3d_fresnelBias = writer.declUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = writer.declUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = writer.declUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_reflection = writer.declFragData< Vec3 >( cuT( "pxl_reflection" ), 0 );
			auto pxl_refraction = writer.declFragData< Vec4 >( cuT( "pxl_refraction" ), 1 );

			glsl::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareFresnelSchlick();
			utils.declareComputeSpecularIBL();
			declareDecodeMaterial( writer );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto data1 = writer.declLocale( cuT( "data1" )
					, texture( c3d_mapData1, vtx_texture ) );
				auto flags = writer.declLocale( cuT( "flags" )
					, data1.w() );
				auto envMapIndex = writer.declLocale( cuT( "envMapIndex" )
					, 0_i );
				auto receiver = writer.declLocale( cuT( "receiver" )
					, 0_i );
				auto reflection = writer.declLocale( cuT( "reflection" )
					, 0_i );
				auto refraction = writer.declLocale( cuT( "refraction" )
					, 0_i );
				decodeMaterial( writer
					, flags
					, receiver
					, reflection
					, refraction
					, envMapIndex );

				pxl_reflection = vec3( 0.0_f );
				pxl_refraction = vec4( -1.0_f );
				auto data2 = writer.declLocale( cuT( "data2" )
					, texture( c3d_mapData2, vtx_texture ) );
				auto data3 = writer.declLocale( cuT( "data3" )
					, texture( c3d_mapData3, vtx_texture ) );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, data2.xyz() );
				auto specular = writer.declLocale( cuT( "specular" )
					, data3.xyz() );
				auto glossiness = writer.declLocale( cuT( "glossiness" )
					, data2.w() );
				auto normal = writer.declLocale( cuT( "normal" )
					, data1.xyz() );
				auto position = writer.declLocale( cuT( "position" )
					, utils.calcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto incident = writer.declLocale( cuT( "incident" )
					, normalize( position - c3d_cameraPosition ) );

				IF( writer, envMapIndex > 0_i )
				{
					envMapIndex = envMapIndex - 1_i;

					IF( writer, reflection != 0_i )
					{
						auto reflected = writer.declLocale( cuT( "reflected" )
							, reflect( incident, normal ) );
						pxl_reflection = texture( c3d_mapEnvironment[envMapIndex], reflected ).xyz();

						IF( writer, refraction == 0_i )
						{
							pxl_reflection *= diffuse / length( diffuse );
						}
						FI;
					}
					ELSE
					{
						pxl_reflection = utils.computeSpecularIBL( normal
							, position
							, diffuse
							, specular
							, glossiness
							, c3d_cameraPosition
							, c3d_mapIrradiance
							, c3d_mapPrefiltered
							, c3d_mapBrdf );
					}
					FI;

					auto ratio = writer.declLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );

					IF( writer, refraction != 0_i )
					{
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( 1.0_f - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						pxl_refraction.xyz() = texture( c3d_mapEnvironment[envMapIndex], refracted ).xyz();
						pxl_refraction.xyz() *= diffuse / length( diffuse );
						pxl_refraction.w() = fresnel;
					}
					ELSEIF( writer, ratio != 0.0_f )
					{
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refracted" )
							, refract( incident, normal, ratio ) );
						refracted.y() = -refracted.y();
						pxl_refraction.xyz() = texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
						pxl_refraction.xyz() *= diffuse / length( diffuse );
						pxl_refraction.w() = fresnel;
					}
					FI;
				}
				ELSE
				{
					pxl_reflection = utils.computeSpecularIBL( normal
					, position
						, diffuse
						, specular
						, glossiness
						, c3d_cameraPosition
						, c3d_mapIrradiance
						, c3d_mapPrefiltered
						, c3d_mapBrdf );
					auto ratio = writer.declLocale( cuT( "ratio" )
						, texture( c3d_mapData4, vtx_texture ).w() );

					IF( writer, ratio != 0.0_f )
					{
						auto roughness = writer.declLocale( cuT( "roughness" )
							, 1.0_f - glossiness );
						auto subRatio = writer.declLocale( cuT( "subRatio" )
							, 1.0_f - ratio );
						auto addRatio = writer.declLocale( cuT( "addRatio" )
							, 1.0_f + ratio );
						auto reflectance = writer.declLocale( cuT( "reflectance" )
							, writer.paren( subRatio * subRatio ) / writer.paren( addRatio * addRatio ) );
						auto product = writer.declLocale( cuT( "product" )
							, max( 0.0_f, dot( -incident, normal ) ) );
						auto fresnel = writer.declLocale( cuT( "fresnel" )
							, reflectance + writer.paren( max( 1.0_f - roughness, reflectance ) - reflectance ) * pow( 1.0_f - product, 5.0 ) );
						auto refracted = writer.declLocale( cuT( "refract" )
							, refract( incident, normal, ratio ) );
						refracted.y() = writer.ternary( envMapIndex != 0_i, refracted.y(), -refracted.y() );
						pxl_refraction.xyz() = texture( c3d_mapPrefiltered, refracted, roughness * Utils::MaxIblReflectionLod ).xyz();
						pxl_refraction.xyz() *= diffuse / length( diffuse );
						pxl_refraction.w() = fresnel;
					}
					FI;
				}
				FI;
			} );
			return writer.finalise();
		}

		ShaderProgramSPtr doCreateProgram( Engine & engine
			, bool isPbr
			, bool isMetallic )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto vtx = doCreateVertexProgram( renderSystem );
			auto pxl = isPbr
				? isMetallic
					? doCreatePbrMRPixelProgram( renderSystem )
					: doCreatePbrSGPixelProgram( renderSystem )
				: doCreateLegacyPixelProgram( renderSystem );
			auto result = engine.getShaderProgramCache().getNewProgram( false );
			result->createObject( ShaderType::eVertex );
			result->createObject( ShaderType::ePixel );
			result->setSource( ShaderType::eVertex, vtx );
			result->setSource( ShaderType::ePixel, pxl );
			result->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eDepth ), ShaderType::ePixel )->setValue( 0u );
			result->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData1 ), ShaderType::ePixel )->setValue( 1u );
			result->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData2 ), ShaderType::ePixel )->setValue( 2u );
			result->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData3 ), ShaderType::ePixel )->setValue( 3u );
			result->createUniform< UniformType::eSampler >( getTextureName( DsTexture::eData4 ), ShaderType::ePixel )->setValue( 4u );
			result->createUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->setValue( 5u );
			int c = int( c_environmentStart );

			if ( isPbr )
			{
				result->createUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel )->setValue( c++ );
				result->createUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel )->setValue( c++ );
				result->createUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel )->setValue( c++ );
			}

			result->createUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel, c_environmentCount )->setValues(
			{
				c + 0, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6, c + 7,
				c + 8, c + 9, c + 10, c + 11, c + 12, c + 13, c + 14, c + 15,
			} );

			result->initialise();
			return result;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo
			, HdrConfigUbo & configUbo )
		{
			RasteriserState rsState;
			rsState.setCulledFaces( Culling::eNone );
			DepthStencilState dsState;
			dsState.setDepthTest( false );
			dsState.setDepthMask( WritingMask::eZero );
			BlendState blState;
			auto result = engine.getRenderSystem()->createRenderPipeline( std::move( dsState )
				, std::move( rsState )
				, std::move( blState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			result->addUniformBuffer( matrixUbo.getUbo() );
			result->addUniformBuffer( sceneUbo.getUbo() );
			result->addUniformBuffer( gpInfoUbo.getUbo() );
			result->addUniformBuffer( configUbo.getUbo() );
			return result;
		}
	}

	ReflectionPass::ProgramPipeline::ProgramPipeline( Engine & engine
		, VertexBuffer & vbo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, HdrConfigUbo & configUbo
		, bool isPbr
		, bool isMetallic )
		: m_program{ doCreateProgram( engine, isPbr, isMetallic ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_program, vbo ) }
		, m_pipeline{ doCreateRenderPipeline( engine, *m_program, matrixUbo, sceneUbo, gpInfoUbo, configUbo ) }
	{
	}

	ReflectionPass::ProgramPipeline::~ProgramPipeline()
	{
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void ReflectionPass::ProgramPipeline::render()
	{
		m_pipeline->apply();
		m_geometryBuffers->draw( 6u, 0 );
	}

	ReflectionPass::ReflectionPass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo )
		: OwnedBy< Engine >{ engine }
		, m_size{ size }
		, m_reflection{ engine }
		, m_refraction{ engine }
		, m_frameBuffer{ engine.getRenderSystem()->createFrameBuffer() }
		, m_viewport{ engine }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_configUbo{ engine }
		, m_programs
		{
			{
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, m_configUbo, false, false },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, m_configUbo, true, false },
				ProgramPipeline{ engine, *m_vertexBuffer, m_matrixUbo, sceneUbo, m_gpInfoUbo, m_configUbo, true, true },
			}
		}
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Reflection" ) ) }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
		m_matrixUbo.update( m_viewport.getProjection() );

		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eTransparentBlack ) );
		bool result = m_frameBuffer->initialise();

		if ( result )
		{
			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGB32F
				, size );
			texture->getImage().initialiseSource();

			m_reflection.setIndex( 0u );
			m_reflection.setTexture( texture );
			m_reflection.setSampler( engine.getLightsSampler() );
			result = m_reflection.initialise();
			ENSURE( result );
			m_reflectAttach = m_frameBuffer->createAttachment( texture );

			texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA32F
				, size );
			texture->getImage().initialiseSource();

			m_refraction.setIndex( 1u );
			m_refraction.setTexture( texture );
			m_refraction.setSampler( engine.getLightsSampler() );
			result = m_refraction.initialise();
			ENSURE( result );
			m_refractAttach = m_frameBuffer->createAttachment( texture );

			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour
				, 0u
				, m_reflectAttach
				, texture->getType() );
			m_frameBuffer->attach( AttachmentPoint::eColour
				, 1u
				, m_refractAttach
				, texture->getType() );
			ENSURE( m_frameBuffer->isComplete() );
			m_frameBuffer->setDrawBuffers();
			m_frameBuffer->unbind();
		}
	}

	ReflectionPass::~ReflectionPass()
	{
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
		m_refractAttach.reset();
		m_reflectAttach.reset();
		m_reflection.cleanup();
		m_refraction.cleanup();

		m_configUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_vertexBuffer->cleanup();
	}

	void ReflectionPass::render( GeometryPassResult & gp
		, castor3d::TextureUnit const & lp
		, Scene const & p_scene
		, RenderInfo & info )
	{
		m_timer->start();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eColour );
		m_viewport.apply();
		m_configUbo.update( p_scene.getHdrConfig() );
		auto & maps = p_scene.getEnvironmentMaps();
		gp[size_t( DsTexture::eDepth )]->getTexture()->bind( 0u );
		gp[size_t( DsTexture::eDepth )]->getSampler()->bind( 0u );
		gp[size_t( DsTexture::eData1 )]->getTexture()->bind( 1u );
		gp[size_t( DsTexture::eData1 )]->getSampler()->bind( 1u );
		gp[size_t( DsTexture::eData2 )]->getTexture()->bind( 2u );
		gp[size_t( DsTexture::eData2 )]->getSampler()->bind( 2u );
		gp[size_t( DsTexture::eData3 )]->getTexture()->bind( 3u );
		gp[size_t( DsTexture::eData3 )]->getSampler()->bind( 3u );
		gp[size_t( DsTexture::eData4 )]->getTexture()->bind( 4u );
		gp[size_t( DsTexture::eData4 )]->getSampler()->bind( 4u );
		lp.getTexture()->bind( 5u );
		lp.getSampler()->bind( 5u );

		if ( p_scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
			|| p_scene.getMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			auto index = c_environmentStart;
			auto & skyboxIbl = p_scene.getSkybox().getIbl();
			skyboxIbl.getPrefilteredBrdf().getTexture()->bind( index );
			skyboxIbl.getPrefilteredBrdf().getSampler()->bind( index );
			++index;
			skyboxIbl.getIrradiance().getTexture()->bind( index );
			skyboxIbl.getIrradiance().getSampler()->bind( index );
			++index;
			skyboxIbl.getPrefilteredEnvironment().getTexture()->bind( index );
			skyboxIbl.getPrefilteredEnvironment().getSampler()->bind( index );
			++index;

			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->bind( index );
				map.get().getTexture().getSampler()->bind( index );
				++index;
			}

			auto program = p_scene.getMaterialsType() == MaterialType::ePbrMetallicRoughness
				? 2u
				: 1u;
			m_programs[program].render();
			index = c_environmentStart;
			skyboxIbl.getPrefilteredBrdf().getTexture()->unbind( index );
			skyboxIbl.getPrefilteredBrdf().getSampler()->unbind( index );
			++index;
			skyboxIbl.getIrradiance().getTexture()->unbind( index );
			skyboxIbl.getIrradiance().getSampler()->unbind( index );
			++index;
			skyboxIbl.getPrefilteredEnvironment().getTexture()->unbind( index );
			skyboxIbl.getPrefilteredEnvironment().getSampler()->unbind( index );
			++index;

			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->unbind( index );
				map.get().getTexture().getSampler()->unbind( index );
				++index;
			}
		}
		else
		{
			auto index = c_environmentStart;

			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->bind( index );
				map.get().getTexture().getSampler()->bind( index );
				++index;
			}

			m_programs[0].render();
			index = c_environmentStart;

			for ( auto & map : maps )
			{
				map.get().getTexture().getTexture()->unbind( index );
				map.get().getTexture().getSampler()->unbind( index );
				++index;
			}
		}

		lp.getTexture()->unbind( 5u );
		lp.getSampler()->unbind( 5u );
		gp[size_t( DsTexture::eData4 )]->getTexture()->unbind( 4u );
		gp[size_t( DsTexture::eData4 )]->getSampler()->unbind( 4u );
		gp[size_t( DsTexture::eData3 )]->getTexture()->unbind( 3u );
		gp[size_t( DsTexture::eData3 )]->getSampler()->unbind( 3u );
		gp[size_t( DsTexture::eData2 )]->getTexture()->unbind( 2u );
		gp[size_t( DsTexture::eData2 )]->getSampler()->unbind( 2u );
		gp[size_t( DsTexture::eData1 )]->getTexture()->unbind( 1u );
		gp[size_t( DsTexture::eData1 )]->getSampler()->unbind( 1u );
		gp[size_t( DsTexture::eDepth )]->getTexture()->unbind( 0u );
		gp[size_t( DsTexture::eDepth )]->getSampler()->unbind( 0u );
		m_frameBuffer->unbind();
		m_timer->stop();
	}
}
