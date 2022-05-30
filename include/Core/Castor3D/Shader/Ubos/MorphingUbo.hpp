/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingUbo_H___
#define ___C3D_MorphingUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>

namespace castor3d
{
	namespace shader
	{
		struct MorphTargetData
			: public sdw::StructInstance
		{
		public:
			C3D_API MorphTargetData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );

			SDW_DeclStructInstance( C3D_API, MorphTargetData );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, MorphFlags morphFlags = MorphFlags{} );

			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col
				, sdw::Float const & weight )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col
				, sdw::Float const & weight )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec4 & tan
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col
				, sdw::Float const & weight )const;

		public:
			C3D_API static castor::String const BufferName;
			C3D_API static castor::String const DataName;

		public:
			sdw::Vec4 morphPosition;
			sdw::Vec4 morphNormal;
			sdw::Vec4 morphTangent;
			sdw::Vec4 morphTexture0;
			sdw::Vec4 morphTexture1;
			sdw::Vec4 morphTexture2;
			sdw::Vec4 morphTexture3;
			sdw::Vec4 morphColour;
		};

		struct MorphTargetsData
			: public sdw::StructInstance
		{
		public:
			C3D_API MorphTargetsData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );

			SDW_DeclStructInstance( C3D_API, MorphTargetsData );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, MorphFlags morphFlags = MorphFlags{} );

			MorphTargetData operator[]( sdw::UInt const & index )const
			{
				return m_data[index];
			}

		private:
			sdw::Array< MorphTargetData > m_data;
		};

		struct MorphingWeightsData
			: public sdw::StructInstance
		{
		public:
			C3D_API MorphingWeightsData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );

			SDW_DeclStructInstance( C3D_API, MorphingWeightsData );

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
				, sdw::UInt vertexId
				, sdw::Vec4 & pos
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col )const;
			C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
				, sdw::UInt vertexId
				, sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col )const;
			C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
				, sdw::UInt vertexId
				, sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec4 & tan
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col )const;
			C3D_API void morph( sdw::ArraySsboT< shader::MorphTargetsData > const & targets
				, sdw::UInt vertexId
				, sdw::Vec4 & pos
				, sdw::Vec4 & nml
				, sdw::Vec4 & tan
				, sdw::Vec3 & uvw0
				, sdw::Vec3 & uvw1
				, sdw::Vec3 & uvw2
				, sdw::Vec3 & uvw3
				, sdw::Vec3 & col )const;

			sdw::UInt index( sdw::UInt const & index )const
			{
				return m_indices[index / 4_u][index % 4_u];
			}

			sdw::Float weight( sdw::UInt const & index )const
			{
				return m_weights[index / 4_u][index % 4_u];
			}

		public:
			C3D_API static castor::String const BufferName;
			C3D_API static castor::String const DataName;

		private:
			sdw::UVec4 m_limits;
			sdw::Array< sdw::UVec4 > m_indices;
			sdw::Array< sdw::Vec4 > m_weights;

		public:
			sdw::UInt morphTargetsCount;
		};
	}
}

#endif
