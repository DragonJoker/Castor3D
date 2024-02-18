/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSurface_H___
#define ___C3D_GlslSurface_H___

#include "SdwModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Shaders/GlslDerivativeValue.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d::shader
{
	template< typename Position3T, typename Position4T, typename NormalT >
	struct SurfaceBaseT
		: public sdw::StructInstance
	{
		SurfaceBaseT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		sdw::Vec3 clipPosition;
		Position4T viewPosition;
		Position4T worldPosition;
		NormalT normal;

	protected:
		static void fillType( sdw::type::BaseStruct & type );
		static void fillIOType( sdw::type::IOStruct & type
			, PipelineFlags const & flags
			, uint32_t & index );
		static void fillType( sdw::type::BaseStruct & type
			, PipelineFlags const & flags );
		static void fillInit( sdw::expr::ExprList & init
			, sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal );
		static void fillInit( sdw::expr::ExprList & init
			, PipelineFlags const & flags
			, sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal );

		static Position3T makePosition3( sdw::Vec3 const & in )
			requires std::is_same_v< Position3T, sdw::Vec3 >
		{
			return in;
		}

		static Position3T makePosition3( sdw::Vec3 const & in )
			requires std::is_same_v< Position3T, DerivVec3 >
		{
			return derivVec3( in );
		}

		static Position3T makePosition3( sdw::Float const & in )
		{
			return makePosition3( vec3( in ) );
		}

		static Position4T makePosition4( sdw::Vec4 const & in )
			requires std::is_same_v< Position4T, sdw::Vec4 >
		{
			return in;
		}

		static Position4T makePosition4( sdw::Vec4 const & in )
			requires std::is_same_v< Position4T, DerivVec4 >
		{
			return derivVec4( in );
		}

		static Position4T makePosition4( sdw::Vec3 const & rgb
			, sdw::Float const & a )
			requires std::is_same_v< Position4T, sdw::Vec4 >
		{
			return vec4( rgb, a );
		}

		static Position4T makePosition4( DerivVec3 const & rgb
			, DerivFloat const & a )
			requires std::is_same_v< Position4T, DerivVec4 >
		{
			return derivVec4( rgb, a );
		}

		static Position4T makePosition4( sdw::Vec3 const & in )
			requires std::is_same_v< Position4T, sdw::Vec4 >
		{
			return vec4( in, 1.0_f );
		}

		static Position4T makePosition4( DerivVec3 const & in )
			requires std::is_same_v< Position4T, DerivVec4 >
		{
			return derivVec4( in, 1.0_f );
		}

		static Position4T makePosition4( sdw::Float const & in )
		{
			return makePosition4( vec4( in ) );
		}

		static NormalT makeNormal( sdw::Vec3 const & in )
			requires std::is_same_v< NormalT, sdw::Vec3 >
		{
			return in;
		}

		static NormalT makeNormal( sdw::Vec3 const & in )
			requires std::is_same_v< NormalT, DerivVec3 >
		{
			return derivVec3( in );
		}

		static NormalT makeNormal( sdw::Float const & in )
		{
			return makeNormal( vec3( in ) );
		}
	};

	template< typename Position3T, typename Position4T, typename NormalT >
	struct SurfaceT
		: public SurfaceBaseT< Position3T, Position4T, NormalT >
	{
		SurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		SurfaceT( sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal
			, sdw::Vec3 texCoord );
		SurfaceT( sdw::Vec3 clip
			, Position3T view
			, Position3T  world
			, NormalT normal
			, sdw::Vec3 texCoord );
		SurfaceT( sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal );
		SurfaceT( sdw::Vec3 clip
			, Position3T view
			, Position3T world
			, NormalT normal );
		SurfaceT( NormalT world
			, NormalT normal );
		SDW_DeclStructInstance( , SurfaceT );

		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, PipelineFlags const & flags );

		sdw::Vec3 texCoord;

	protected:
		static void fillType( sdw::type::BaseStruct & type );
		static void fillType( sdw::type::BaseStruct & type
			, PipelineFlags const & flags );
		static void fillInit( sdw::expr::ExprList & init
			, sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal
			, sdw::Vec3 texCoord );
		static void fillInit( sdw::expr::ExprList & init
			, PipelineFlags const & flags
			, sdw::Vec3 clip
			, Position4T view
			, Position4T world
			, NormalT normal
			, sdw::Vec3 texCoord );
	};

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	struct RasterizerSurfaceBaseT
		: public SurfaceBaseT< Position3T, Position4T, Normal3T >
	{
		RasterizerSurfaceBaseT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		// Vertex shader side
		void computeVelocity( CameraData const & cameraData
			, Position4T & curPos
			, Position4T & prvPos );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, Position3T const & worldPos
			, Normal3T const & nml
			, Normal4T const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, Position3T const & worldPos
			, sdw::Mat3 const & mtx
			, Normal3T const & nml
			, Normal4T const & tan );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, Position3T const & worldPos
			, sdw::Mat3 const & mtx
			, Normal3T const & nml
			, Normal4T const & tan
			, Normal3T const & bin );
		void computeTangentSpace( PipelineFlags const & flags
			, sdw::Vec3 const & cameraPosition
			, Position3T const & worldPos
			, Normal3T const & nml
			, Normal4T const & tan
			, Normal3T const & bin );

		// Fragment shader side
		sdw::Vec2 getVelocity()const;

		Position4T curPosition;
		Position4T prvPosition;
		Position3T tangentSpaceFragPosition;
		sdw::Vec3 tangentSpaceViewPosition;
		Normal4T tangent;
		Normal3T bitangent;
		sdw::Vec3 colour;
		sdw::Array< sdw::Vec4 > passMultipliers;
		sdw::UInt nodeId;
		sdw::UInt vertexId;
		sdw::UInt meshletId;

	protected:
		static void fillIOType( sdw::type::IOStruct & type
			, SubmeshShaders const & submeshShaders
			, PassShaders const & passShaders
			, PipelineFlags const & flags
			, uint32_t & index );
		static void fillType( sdw::type::BaseStruct & type
			, SubmeshShaders const & submeshShaders
			, PassShaders const & passShaders
			, PipelineFlags const & flags );
		static void fillType( sdw::type::BaseStruct & type
			, SubmeshShaders const & submeshShaders );
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

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, sdw::var::Flag FlagT >
	struct RasterizerSurfaceT
		: public RasterizerSurfaceBaseT< Position3T, Position4T, Normal3T, Normal4T >
	{
	public:
		RasterizerSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		RasterizerSurfaceT( sdw::Vec3 clip
			, Position3T view
			, Position3T world
			, Normal3T normal
			, TexcoordT texCoord );
		template< sdw::var::Flag FlagU >
		RasterizerSurfaceT( RasterizerSurfaceT< Position3T, Position4T, Normal3T, Normal4T, TexcoordT, FlagU > const & rhs );

		SDW_DeclStructInstance( , RasterizerSurfaceT );

		static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
			, sdw::EntryPoint entryPoint
			, SubmeshShaders const & submeshShaders
			, PassShaders const & passShaders
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, SubmeshShaders const & submeshShaders
			, PassShaders const & passShaders
			, PipelineFlags const & flags );
		static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
			, SubmeshShaders const & submeshShaders );

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
