/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSurface_H___
#define ___C3D_GlslSurface_H___

#include "SdwModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	template< ast::var::Flag FlagT >
	struct SurfaceT
		: public sdw::StructInstance
	{
		SurfaceT( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( , SurfaceT );

		void create( sdw::Vec3 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal );
		void create( sdw::Vec3 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
		void create( sdw::Vec3 world
			, sdw::Vec3 normal );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
		static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		sdw::Vec3 clipPosition;
		sdw::Vec3 viewPosition;
		sdw::Vec3 worldPosition;
		sdw::Vec3 worldNormal;
		sdw::Vec3 texCoord;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	template< ast::var::Flag FlagT >
	struct VertexSurfaceT
		: public sdw::StructInstance
	{
		VertexSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , VertexSurfaceT );

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
			, PipelineFlags const & flags );
		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		// Base
		sdw::Vec4 position;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 texture0;
		sdw::Vec3 texture1;
		sdw::Vec3 texture2;
		sdw::Vec3 texture3;
		sdw::Vec3 colour;
		sdw::UVec4 passMasks;
		// Velocity
		sdw::Vec3 velocity;
		// Instantiation
		sdw::UVec4 objectIds;
	};

	using VertexSurface = VertexSurfaceT< ast::var::Flag::eNone >;
	Writer_Parameter( VertexSurface );

	template< typename TexcoordT, ast::var::Flag FlagT >
	struct RasterizerSurfaceT
		: public sdw::StructInstance
	{
	public:
		RasterizerSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , RasterizerSurfaceT );

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
			, PipelineFlags const & flags );
		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		// Vertex shader side
		void computeVelocity( MatrixData const & matrixData
			, sdw::Vec4 & curPos
			, sdw::Vec4 & prvPos );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Mat3 const & mtx
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan
			, sdw::Vec3 const & bin );

		// Fragment shader side
		sdw::Vec2 getVelocity()const;

	public:
		sdw::Vec4 worldPosition;
		sdw::Vec4 viewPosition;
		sdw::Vec4 curPosition;
		sdw::Vec4 prvPosition;
		sdw::Vec3 tangentSpaceFragPosition;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 bitangent;
		TexcoordT texture0;
		TexcoordT texture1;
		TexcoordT texture2;
		TexcoordT texture3;
		sdw::Vec3 colour;
		sdw::Array< sdw::Vec4 > passMultipliers;
		sdw::UInt nodeId;
		sdw::UInt vertexId;
	};

	template< typename PositionT >
	struct ComputeSurfaceT
	{
	public:
		ComputeSurfaceT( sdw::ShaderWriter & writer
			, PipelineFlags const & flags
			, uint32_t stride
			, uint32_t binding
			, uint32_t set );

		// Vertex shader side
		void computeVelocity( MatrixData const & matrixData
			, sdw::Vec4 & curPos
			, sdw::Vec4 & prvPos );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Mat3 const & mtx
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan
			, sdw::Vec3 const & bin );

		// Fragment shader side
		sdw::Vec2 getVelocity()const;

	public:
#define DeclareSsbo( Name, Type )\
		sdw::StorageBuffer m_##Name##Buffer;\
		sdw::Array< Type > ##Name;\

		DeclareSsbo( indices, sdw::UInt );
		DeclareSsbo( positions, PositionT );
		DeclareSsbo( normals, sdw::Vec4 );
		DeclareSsbo( tangents, sdw::Vec4 );
		DeclareSsbo( texcoords0, sdw::Vec4 );
		DeclareSsbo( texcoords1, sdw::Vec4 );
		DeclareSsbo( texcoords2, sdw::Vec4 );
		DeclareSsbo( texcoords3, sdw::Vec4 );
		DeclareSsbo( colours, sdw::Vec4 );
		DeclareSsbo( velocities, sdw::Vec4 );
		DeclareSsbo( passMasks, sdw::UVec4 );
		DeclareSsbo( objectsIds, sdw::UVec4 );

#undef DeclareSsbo
	};
}

#include "GlslSurface.inl"

#endif
