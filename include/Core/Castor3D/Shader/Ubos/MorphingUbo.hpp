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

			static ast::type::StructPtr makeType( ast::type::TypesCache & cache
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

			static ast::type::StructPtr makeType( ast::type::TypesCache & cache
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
			C3D_API MorphingWeightsData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, MorphingWeightsData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			sdw::Float operator[]( sdw::UInt const & index )const
			{
				return m_data[index];
			}

		public:
			C3D_API static castor::String const BufferName;
			C3D_API static castor::String const DataName;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Vec4 m_data;
		};

		struct MorphingWeightsDataArray
			: public sdw::StructInstance
		{
		public:
			C3D_API MorphingWeightsDataArray( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );

			SDW_DeclStructInstance( C3D_API, MorphingWeightsDataArray );

			static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

			sdw::Float operator[]( sdw::UInt const & index )const
			{
				return m_data[index / 4_u][index % 4_u];
			}

		private:
			sdw::Array< MorphingWeightsData > m_data;
		};
	}

	C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
		, shader::MorphingWeightsDataArray const & weights
		, sdw::UInt vertexId
		, sdw::UInt morphTargetsCount
		, sdw::Vec4 & pos
		, sdw::Vec3 & uvw0
		, sdw::Vec3 & uvw1
		, sdw::Vec3 & uvw2
		, sdw::Vec3 & uvw3
		, sdw::Vec3 & col );
	C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
		, shader::MorphingWeightsDataArray const & weights
		, sdw::UInt vertexId
		, sdw::UInt morphTargetsCount
		, sdw::Vec4 & pos
		, sdw::Vec4 & nml
		, sdw::Vec3 & uvw0
		, sdw::Vec3 & uvw1
		, sdw::Vec3 & uvw2
		, sdw::Vec3 & uvw3
		, sdw::Vec3 & col );
	C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
		, shader::MorphingWeightsDataArray const & weights
		, sdw::UInt vertexId
		, sdw::UInt morphTargetsCount
		, sdw::Vec4 & pos
		, sdw::Vec4 & nml
		, sdw::Vec4 & tan
		, sdw::Vec3 & uvw0
		, sdw::Vec3 & uvw1
		, sdw::Vec3 & uvw2
		, sdw::Vec3 & uvw3
		, sdw::Vec3 & col );
}

#define C3D_MorphTargets( writer, binding, set, morphFlags, programFlags )\
	sdw::Ssbo morphingBuffer{ writer\
		, castor3d::shader::MorphTargetData::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, checkFlag( programFlags, castor3d::ProgramFlag::eMorphing ) };\
	auto c3d_morphTargets = morphingBuffer.declMemberArray< castor3d::shader::MorphTargetsData >( castor3d::shader::MorphTargetData::DataName\
		, checkFlag( programFlags, castor3d::ProgramFlag::eMorphing )\
		, morphFlags );\
	morphingBuffer.end()

#define C3D_MorphingWeights( writer, binding, set, flags )\
	sdw::Ssbo morphingWeights{ writer\
		, castor3d::shader::MorphingWeightsData::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) };\
	auto c3d_morphingWeights = morphingWeights.declMemberArray< castor3d::shader::MorphingWeightsDataArray >( castor3d::shader::MorphingWeightsData::DataName\
		, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) );\
	morphingWeights.end()

#endif
