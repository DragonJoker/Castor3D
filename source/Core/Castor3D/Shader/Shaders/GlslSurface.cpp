#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		Surface::Surface( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, clipPosition{ getMember< sdw::Vec2 >( "clipPosition" ) }
			, viewPosition{ getMember< sdw::Vec3 >( "viewPosition" ) }
			, worldPosition{ getMember< sdw::Vec3 >( "worldPosition" ) }
			, worldNormal{ getMember< sdw::Vec3 >( "worldNormal" ) }
			, texCoord{ getMember< sdw::Vec3 >( "texCoord" ) }
		{
		}

		Surface & Surface::operator=( Surface const & rhs )
		{
			StructInstance::operator=( rhs );
			return *this;
		}

		void Surface::create( sdw::Vec2 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal )
		{
			clipPosition = clip;
			viewPosition = view;
			worldPosition = world;
			worldNormal = normal;
		}

		void Surface::create( sdw::Vec2 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal
			, sdw::Vec3 coord )
		{
			clipPosition = clip;
			viewPosition = view;
			worldPosition = world;
			worldNormal = normal;
			texCoord = coord;
		}

		void Surface::create( sdw::Vec3 world
			, sdw::Vec3 normal )
		{
			create( sdw::vec2( 0.0_f )
				, sdw::vec3( 0.0_f )
				, world
				, normal
				, sdw::vec3( 0.0_f ) );
		}

		ast::type::StructPtr Surface::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd430
				, "Surface" );

			if ( result->empty() )
			{
				result->declMember( "clipPosition", ast::type::Kind::eVec2F );
				result->declMember( "viewPosition", ast::type::Kind::eVec3F );
				result->declMember( "worldPosition", ast::type::Kind::eVec3F );
				result->declMember( "worldNormal", ast::type::Kind::eVec3F );
				result->declMember( "texCoord", ast::type::Kind::eVec3F );
			}

			return result;
		}

		std::unique_ptr< sdw::Struct > Surface::declare( sdw::ShaderWriter & writer )
		{
			return std::make_unique< sdw::Struct >( writer
				, makeType( writer.getTypesCache() ) );
		}

		//*****************************************************************************************

		VertexSurface::VertexSurface( sdw::ShaderWriter & writer
			, ProgramFlags programFlags
			, ShaderFlags shaderFlags
			, bool hasTextures )
			: position{ writer.declInput< sdw::Vec4 >( "vtxPosition"
				, SceneRenderPass::VertexInputs::PositionLocation ) }
			, normal{ writer.declInput< sdw::Vec3 >( "vtxNormal"
				, SceneRenderPass::VertexInputs::NormalLocation
				, checkFlag( shaderFlags, ShaderFlag::eNormal ) ) }
			, tangent{ writer.declInput< sdw::Vec3 >( "vtxTangent"
				, SceneRenderPass::VertexInputs::TangentLocation
				, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) }
			, texture{ writer.declInput< sdw::Vec3 >( "vtxTexture"
				, SceneRenderPass::VertexInputs::TextureLocation
				, hasTextures ) }
			, boneIds0{ writer.declInput< sdw::IVec4 >( "vtxBoneIds0"
				, SceneRenderPass::VertexInputs::BoneIds0Location
				, checkFlag( programFlags, ProgramFlag::eSkinning ) ) }
			, boneIds1{ writer.declInput< sdw::IVec4 >( "vtxBoneIds1"
				, SceneRenderPass::VertexInputs::BoneIds1Location
				, checkFlag( programFlags, ProgramFlag::eSkinning ) ) }
			, boneWeights0{ writer.declInput< sdw::Vec4 >( "vtxWeights0"
				, SceneRenderPass::VertexInputs::Weights0Location
				, checkFlag( programFlags, ProgramFlag::eSkinning ) ) }
			, boneWeights1{ writer.declInput< sdw::Vec4 >( "vtxWeights1"
				, SceneRenderPass::VertexInputs::Weights1Location
				, checkFlag( programFlags, ProgramFlag::eSkinning ) ) }
			, transform{ writer.declInput< sdw::Mat4 >( "vtxTransform"
				, SceneRenderPass::VertexInputs::TransformLocation
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) ) }
			, material{ writer.declInput< sdw::Int >( "vtxMaterial"
				, SceneRenderPass::VertexInputs::MaterialLocation
				, checkFlag( programFlags, ProgramFlag::eInstantiation ) ) }
			, position2{ writer.declInput< sdw::Vec4 >( "vtxPosition2"
				, SceneRenderPass::VertexInputs::Position2Location
				, checkFlag( programFlags, ProgramFlag::eMorphing ) ) }
			, normal2{ writer.declInput< sdw::Vec3 >( "vtxNormal2"
				, SceneRenderPass::VertexInputs::Normal2Location
				, ( checkFlag( programFlags, ProgramFlag::eMorphing )
					&& checkFlag( shaderFlags, ShaderFlag::eNormal ) ) ) }
			, tangent2{ writer.declInput< sdw::Vec3 >( "vtxTangent2"
				, SceneRenderPass::VertexInputs::Tangent2Location
				, ( checkFlag( programFlags, ProgramFlag::eMorphing )
					&& checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) ) ) }
			, texture2{ writer.declInput< sdw::Vec3 >( "vtxTexture2"
				, SceneRenderPass::VertexInputs::Texture2Location
				, checkFlag( programFlags, ProgramFlag::eMorphing ) && hasTextures ) }
		{
		}

		void VertexSurface::morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec3 & uvw )const
		{
			morphing.morph( pos, position2
				, uvw, texture2 );
		}

		void VertexSurface::morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec3 & uvw )const
		{
			morphing.morph( pos, position2
				, nml, normal2
				, uvw, texture2 );
		}

		void VertexSurface::morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec3 & uvw )const
		{
			morphing.morph( pos, position2
				, nml, normal2
				, tan, tangent2
				, uvw, texture2 );
		}

		//*****************************************************************************************

		FragmentSurface::FragmentSurface( sdw::ShaderWriter & writer
			, sdw::Vec3 worldPosition
			, sdw::Vec3 viewPosition
			, sdw::Vec3 curPosition
			, sdw::Vec3 prvPosition
			, sdw::Vec3 tangentSpaceFragPosition
			, sdw::Vec3 tangentSpaceViewPosition
			, sdw::Vec3 normal
			, sdw::Vec3 tangent
			, sdw::Vec3 bitangent
			, sdw::Vec3 texture
			, sdw::UInt instance
			, sdw::UInt material )
			: m_writer{ writer }
			, worldPosition{ std::move( worldPosition ) }
			, viewPosition{ std::move( viewPosition ) }
			, curPosition{ std::move( curPosition ) }
			, prvPosition{ std::move( prvPosition ) }
			, tangentSpaceFragPosition{ std::move( tangentSpaceFragPosition ) }
			, tangentSpaceViewPosition{ std::move( tangentSpaceViewPosition ) }
			, normal{ std::move( normal ) }
			, tangent{ std::move( tangent ) }
			, bitangent{ std::move( bitangent ) }
			, texture{ std::move( texture ) }
			, instance{ std::move( instance ) }
			, material{ std::move( material ) }
		{
		}

		//*****************************************************************************************

		OutFragmentSurface::OutFragmentSurface( sdw::ShaderWriter & writer
			, ShaderFlags shaderFlags
			, bool hasTextures )
			: FragmentSurface{ writer
				, writer.declOutput< sdw::Vec3 >( "frgWorldPosition"
					, SceneRenderPass::VertexOutputs::WorldPositionLocation )
				, writer.declOutput< sdw::Vec3 >( "frgViewPosition"
					, SceneRenderPass::VertexOutputs::ViewPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eViewSpace ) )
				, writer.declOutput< sdw::Vec3 >( "frgCurPosition"
					, SceneRenderPass::VertexOutputs::CurPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eVelocity ) )
				, writer.declOutput< sdw::Vec3 >( "frgPrvPosition"
					, SceneRenderPass::VertexOutputs::PrvPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eVelocity ) )
				, writer.declOutput< sdw::Vec3 >( "frgTangentSpaceFragPosition"
					, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declOutput< sdw::Vec3 >( "frgTangentSpaceViewPosition"
					, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declOutput< sdw::Vec3 >( "frgNormal"
					, SceneRenderPass::VertexOutputs::NormalLocation
					, checkFlag( shaderFlags, ShaderFlag::eNormal ) )
				, writer.declOutput< sdw::Vec3 >( "frgTangent"
					, SceneRenderPass::VertexOutputs::TangentLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declOutput< sdw::Vec3 >( "frgBitangent"
					, SceneRenderPass::VertexOutputs::BitangentLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declOutput< sdw::Vec3 >( "frgTexture"
					, SceneRenderPass::VertexOutputs::TextureLocation
					, hasTextures )
				, writer.declOutput< sdw::UInt >( "frgInstance"
					, SceneRenderPass::VertexOutputs::InstanceLocation )
				, writer.declOutput< sdw::UInt >( "frgMaterial"
					, SceneRenderPass::VertexOutputs::MaterialLocation ) }
		{
		}

		void OutFragmentSurface::computeVelocity( MatrixData const & matrixData
			, sdw::Vec4 & curPos
			, sdw::Vec4 & prvPos )
		{
			prvPos = matrixData.worldToPrvProj( prvPos );
			curPos = matrixData.worldToCurProj( curPos );

			// Convert the jitter from non-homogeneous coordinates to homogeneous
			// coordinates and add it:
			// (note that for providing the jitter in non-homogeneous projection space,
			//  pixel coordinates (screen space) need to multiplied by two in the C++
			//  code)
			matrixData.jitter( curPos );
			matrixData.jitter( prvPos );

			curPosition = curPos.xyw();
			prvPosition = prvPos.xyw();
			// Positions in projection space are in [-1, 1] range, while texture
			// coordinates are in [0, 1] range. So, we divide by 2 to get velocities in
			// the scale (and flip the y axis):
			curPosition.xy() *= vec2( 0.5_f, -0.5_f );
			prvPosition.xy() *= vec2( 0.5_f, -0.5_f );
		}

		void OutFragmentSurface::computeTangentSpace( ProgramFlags programFlags
			, sdw::Vec3 const & cameraPosition
			, sdw::Mat3 const & mtx
			, sdw::Vec4 const & nml
			, sdw::Vec4 const & tan )
		{
			normal = normalize( mtx * nml.xyz() );
			tangent = normalize( mtx * tan.xyz() );
			tangent = normalize( sdw::fma( -normal, vec3( dot( tangent, normal ) ), tangent ) );
			bitangent = cross( normal, tangent );

			if ( checkFlag( programFlags, ProgramFlag::eInvertNormals ) )
			{
				normal = -normal;
				tangent = -tangent;
				bitangent = -bitangent;
			}

			auto tbn = m_writer.declLocale( "tbn"
				, transpose( mat3( tangent, bitangent, normal ) ) );
			tangentSpaceFragPosition = tbn * worldPosition;
			tangentSpaceViewPosition = tbn * cameraPosition.xyz();
		}

		void OutFragmentSurface::computeTangentSpace( sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan
			, sdw::Vec3 const & bin )
		{
			normal = nml;
			tangent = tan;
			bitangent = bin;
			auto tbn = m_writer.declLocale( "tbn"
				, transpose( mat3( tangent, bitangent, normal ) ) );
			tangentSpaceFragPosition = tbn * worldPosition;
			tangentSpaceViewPosition = tbn * cameraPosition.xyz();
		}

		//*****************************************************************************************

		InFragmentSurface::InFragmentSurface( sdw::ShaderWriter & writer
			, ShaderFlags shaderFlags
			, bool hasTextures )
			: FragmentSurface{ writer
				, writer.declInput< sdw::Vec3 >( "frgWorldPosition"
					, SceneRenderPass::VertexOutputs::WorldPositionLocation )
				, writer.declInput< sdw::Vec3 >( "frgViewPosition"
					, SceneRenderPass::VertexOutputs::ViewPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eViewSpace ) )
				, writer.declInput< sdw::Vec3 >( "frgCurPosition"
					, SceneRenderPass::VertexOutputs::CurPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eVelocity ) )
				, writer.declInput< sdw::Vec3 >( "frgPrvPosition"
					, SceneRenderPass::VertexOutputs::PrvPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eVelocity ) )
				, writer.declInput< sdw::Vec3 >( "frgTangentSpaceFragPosition"
					, SceneRenderPass::VertexOutputs::TangentSpaceFragPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declInput< sdw::Vec3 >( "frgTangentSpaceViewPosition"
					, SceneRenderPass::VertexOutputs::TangentSpaceViewPositionLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declInput< sdw::Vec3 >( "frgNormal"
					, SceneRenderPass::VertexOutputs::NormalLocation
					, checkFlag( shaderFlags, ShaderFlag::eNormal ) )
				, writer.declInput< sdw::Vec3 >( "frgTangent"
					, SceneRenderPass::VertexOutputs::TangentLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declInput< sdw::Vec3 >( "frgBitangent"
					, SceneRenderPass::VertexOutputs::BitangentLocation
					, checkFlag( shaderFlags, ShaderFlag::eTangentSpace ) )
				, writer.declInput< sdw::Vec3 >( "frgTexture"
					, SceneRenderPass::VertexOutputs::TextureLocation
					, hasTextures )
				, writer.declInput< sdw::UInt >( "frgInstance"
					, SceneRenderPass::VertexOutputs::InstanceLocation )
				, writer.declInput< sdw::UInt >( "frgMaterial"
					, SceneRenderPass::VertexOutputs::MaterialLocation ) }
		{
		}

		sdw::Vec2 InFragmentSurface::getVelocity()const
		{
			return ( curPosition.xy() / curPosition.z() ) - ( prvPosition.xy() / prvPosition.z() );
		}

		//*****************************************************************************************
	}
}
