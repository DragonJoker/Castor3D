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

namespace castor3d
{
	namespace shader
	{
		struct Surface
			: public sdw::StructInstance
		{
			C3D_API Surface( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API Surface & operator=( Surface const & rhs );

			C3D_API void create( sdw::Vec2 clip
				, sdw::Vec3 view
				, sdw::Vec3 world
				, sdw::Vec3 normal );
			C3D_API void create( sdw::Vec2 clip
				, sdw::Vec3 view
				, sdw::Vec3 world
				, sdw::Vec3 normal
				, sdw::Vec3 texCoord );
			C3D_API void create( sdw::Vec3 world
				, sdw::Vec3 normal );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Vec2 clipPosition;
			sdw::Vec3 viewPosition;
			sdw::Vec3 worldPosition;
			sdw::Vec3 worldNormal;
			sdw::Vec3 texCoord;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		struct VertexSurface
		{
			C3D_API VertexSurface( sdw::ShaderWriter & writer
				, ProgramFlags programFlags
				, ShaderFlags shaderFlags
				, bool hasTextures );

			C3D_API void morph( MorphingData const & morphing
				, sdw::Vec4 & pos
				, sdw::Vec3 & uvw )const;
			C3D_API void morph( MorphingData const & morphing
				, sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec3 & uvw )const;
			C3D_API void morph( MorphingData const & morphing
				, sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec4 & tan
				, sdw::Vec3 & uvw )const;

			sdw::Vec4 position;
			sdw::Vec3 normal;
			sdw::Vec3 tangent;
			sdw::Vec3 texture;
			sdw::IVec4 boneIds0;
			sdw::IVec4 boneIds1;
			sdw::Vec4 boneWeights0;
			sdw::Vec4 boneWeights1;
			sdw::Mat4 transform;
			sdw::Int material;
			sdw::Vec4 position2;
			sdw::Vec3 normal2;
			sdw::Vec3 tangent2;
			sdw::Vec3 texture2;
		};

		struct FragmentSurface
		{
		protected:
			FragmentSurface( sdw::ShaderWriter & writer
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
				, sdw::UInt material );

		protected:
			sdw::ShaderWriter & m_writer;

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
			sdw::Vec3 texture;
			sdw::UInt instance;
			sdw::UInt material;
		};

		struct OutFragmentSurface
			: public FragmentSurface
		{
			C3D_API OutFragmentSurface( sdw::ShaderWriter & writer
				, ShaderFlags shaderFlags
				, bool hasTextures );

			// Vertex shader side
			C3D_API void computeVelocity( MatrixData const & matrixData
				, sdw::Vec4 & curPos
				, sdw::Vec4 & prvPos );
			C3D_API void computeTangentSpace( ProgramFlags programFlags
				, sdw::Vec3 const & cameraPosition
				, sdw::Mat3 const & mtx
				, sdw::Vec4 const & nml
				, sdw::Vec4 const & tan );
			C3D_API void computeTangentSpace( sdw::Vec3 const & cameraPosition
				, sdw::Vec3 const & nml
				, sdw::Vec3 const & tan
				, sdw::Vec3 const & bin );
		};

		struct InFragmentSurface
			: public FragmentSurface
		{
			C3D_API InFragmentSurface( sdw::ShaderWriter & writer
				, ShaderFlags shaderFlags
				, bool hasTextures );

			// Vertex shader side
			C3D_API void computeVelocity( MatrixData const & matrixData
				, sdw::Vec4 & curPos
				, sdw::Vec4 & prvPos );
			C3D_API void computeTangentSpace( ProgramFlags programFlags
				, sdw::Vec3 const & cameraPosition
				, sdw::Mat3 const & mtx
				, sdw::Vec4 const & nml
				, sdw::Vec4 const & tan );
			C3D_API void computeTangentSpace( sdw::Vec3 const & cameraPosition
				, sdw::Vec3 const & nml
				, sdw::Vec3 const & tan
				, sdw::Vec3 const & bin );

			// Fragment shader side
			C3D_API sdw::Vec2 getVelocity()const;
		};
	}
}

#endif
