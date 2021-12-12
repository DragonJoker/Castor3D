/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSurface_H___
#define ___C3D_GlslSurface_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
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

		void create( sdw::Vec2 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal );
		void create( sdw::Vec2 clip
			, sdw::Vec3 view
			, sdw::Vec3 world
			, sdw::Vec3 normal
			, sdw::Vec3 texCoord );
		void create( sdw::Vec3 world
			, sdw::Vec3 normal );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
		static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		sdw::Vec2 clipPosition;
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
			, ProgramFlags programFlags
			, ShaderFlags shaderFlags
			, FilteredTextureFlags textureFlags
			, PassFlags passFlags
			, bool hasTextures );

		void morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec3 & uvw )const;
		void morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec3 & uvw )const;
		void morph( MorphingData const & morphing
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec3 & uvw )const;

		sdw::Vec4 position;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 texture0;
		sdw::Vec3 texture1;
		sdw::IVec4 boneIds0;
		sdw::IVec4 boneIds1;
		sdw::Vec4 boneWeights0;
		sdw::Vec4 boneWeights1;
		sdw::Mat4 transform;
		sdw::Int material;
		sdw::Int nodeId;
		sdw::Vec4 position2;
		sdw::Vec3 normal2;
		sdw::Vec3 tangent2;
		sdw::Vec3 texture2;
	};

	using InVertexSurface = VertexSurfaceT< ast::var::Flag::eShaderInput >;

	template< ast::var::Flag FlagT >
	struct FragmentSurfaceT
		: public sdw::StructInstance
	{
	public:
		FragmentSurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , FragmentSurfaceT );

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
			, ProgramFlags programFlags
			, ShaderFlags shaderFlags
			, FilteredTextureFlags textureFlags
			, PassFlags passFlags
			, bool hasTextures );

		// Vertex shader side
		void computeVelocity( MatrixData const & matrixData
			, sdw::Vec4 & curPos
			, sdw::Vec4 & prvPos );
		void computeTangentSpace( ProgramFlags programFlags
			, sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Mat3 const & mtx
			, sdw::Vec4 const & nml
			, sdw::Vec4 const & tan );
		void computeTangentSpace( sdw::Vec3 const & cameraPosition
			, sdw::Vec3 const & worldPos
			, sdw::Vec3 const & nml
			, sdw::Vec3 const & tan
			, sdw::Vec3 const & bin );

		// Fragment shader side
		sdw::Vec2 getVelocity()const;

	public:
		sdw::Vec3 worldPosition;
		sdw::Vec3 viewPosition;
		sdw::Vec3 curPosition;
		sdw::Vec3 prvPosition;
		sdw::Vec3 tangentSpaceFragPosition;
		sdw::Vec3 tangentSpaceViewPosition;
		sdw::Vec3 normal;
		sdw::Vec3 tangent;
		sdw::Vec3 bitangent;
		sdw::Vec3 texture0;
		sdw::Vec3 texture1;
		sdw::UInt instance;
		sdw::UInt material;
		sdw::Int nodeId;
	};

	using OutVertexSurface = FragmentSurfaceT< ast::var::Flag::eShaderOutput >;
	using InFragmentSurface = FragmentSurfaceT< ast::var::Flag::eShaderInput >;
}

#include "GlslSurface.inl"

#endif
