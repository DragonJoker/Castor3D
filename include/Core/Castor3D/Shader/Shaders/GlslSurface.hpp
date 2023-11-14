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
	struct SurfaceBase
		: public sdw::StructInstance
	{
		C3D_API SurfaceBase( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		sdw::Vec3 clipPosition;
		sdw::Vec4 viewPosition;
		sdw::Vec4 worldPosition;
		sdw::Vec3 normal;

	protected:
		C3D_API static void fillType( sdw::type::BaseStruct & type );
		C3D_API static void fillIOType( sdw::type::IOStruct & type
			, PipelineFlags const & flags
			, uint32_t & index );
		C3D_API static void fillType( sdw::type::BaseStruct & type
			, PipelineFlags const & flags );
		C3D_API static void fillInit( sdw::expr::ExprList & init
			, sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal );
		C3D_API static void fillInit( sdw::expr::ExprList & init
			, PipelineFlags const & flags
			, sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal );
	};

	struct Surface
		: public SurfaceBase
	{
		C3D_API Surface( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API Surface( sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
		C3D_API Surface( sdw::Vec3 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
		C3D_API Surface( sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal );
		C3D_API Surface( sdw::Vec3 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal );
		C3D_API Surface( sdw::Vec3 world
			, sdw::Vec3 normal );
		SDW_DeclStructInstance( C3D_API, Surface );

		C3D_API static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );
		C3D_API static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, PipelineFlags const & flags );

		sdw::Vec3 texCoord;

	protected:
		C3D_API static void fillType( sdw::type::BaseStruct & type );
		C3D_API static void fillType( sdw::type::BaseStruct & type
			, PipelineFlags const & flags );
		C3D_API static void fillInit( sdw::expr::ExprList & init
			, sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
		C3D_API static void fillInit( sdw::expr::ExprList & init
			, PipelineFlags const & flags
			, sdw::Vec3 clip
			, sdw::Vec4 view
			, sdw::Vec4 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
	};

	struct RasterizerSurfaceBase
		: public SurfaceBase
	{
		C3D_API RasterizerSurfaceBase( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		// Vertex shader side
		C3D_API void computeVelocity( CameraData const & cameraData
			, sdw::Vec4 & curPos
			, sdw::Vec4 & prvPos );
		C3D_API void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec4 const & tan );
		C3D_API void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Mat3 const & mtx
			, sdw::Vec3 const & nml
			, sdw::Vec4 const & tan );
		C3D_API void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Mat3 const & mtx
			, sdw::Vec3 const & nml
			, sdw::Vec4 const & tan
			, sdw::Vec3 const & bin );
		C3D_API void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec4 const & tan
			, sdw::Vec3 const & bin );

		// Fragment shader side
		C3D_API sdw::Vec2 getVelocity()const;

		sdw::Vec4 curPosition;
		sdw::Vec4 prvPosition;
		sdw::Vec3 tangentSpaceFragPosition;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec4 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 colour;
		sdw::Array< sdw::Vec4 > passMultipliers;
		sdw::UInt nodeId;
		sdw::UInt vertexId;

	protected:
		C3D_API static void fillIOType( sdw::type::IOStruct & type
			, PassShaders const & shaders
			, PipelineFlags const & flags
			, uint32_t & index );
		C3D_API static void fillType( sdw::type::BaseStruct & type
			, PassShaders const & shaders
			, PipelineFlags const & flags );
		C3D_API static void fillType( sdw::type::BaseStruct & type );
	};

	template< sdw::var::Flag FlagT >
	struct BillboardSurfaceT
		: public sdw::StructInstance
	{
		BillboardSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , BillboardSurfaceT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		sdw::Vec4 position;
		sdw::Vec2 texture0;
		sdw::Vec3 center;
	};

	template< typename TexcoordT, sdw::var::Flag FlagT >
	struct RasterizerSurfaceT
		: public RasterizerSurfaceBase
	{
	public:
		RasterizerSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		RasterizerSurfaceT( sdw::Vec3 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal
			, TexcoordT texCoord );
		template< sdw::var::Flag FlagU >
		RasterizerSurfaceT( RasterizerSurfaceT< TexcoordT, FlagU > const & rhs );

		SDW_DeclStructInstance( , RasterizerSurfaceT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, PassShaders const & shaders
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, PassShaders const & shaders
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

	public:
		TexcoordT texture0;
		TexcoordT texture1;
		TexcoordT texture2;
		TexcoordT texture3;
	};

	template< sdw::var::Flag FlagT >
	struct VoxelSurfaceT
		: shader::SurfaceBase
	{
		VoxelSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true );

		SDW_DeclStructInstance( , VoxelSurfaceT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

		sdw::Vec3 texture0;
		sdw::Vec3 texture1;
		sdw::Vec3 texture2;
		sdw::Vec3 texture3;
		sdw::Vec3 colour;
		sdw::UInt nodeId;
		sdw::Array< sdw::Vec4 > passMultipliers;
	};
}

#include "GlslSurface.inl"

#endif
