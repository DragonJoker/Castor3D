#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		castor::String const MorphTargetData::BufferName = cuT( "MorphTargets" );
		castor::String const MorphTargetData::DataName = cuT( "c3d_morphTargets" );

		MorphTargetData::MorphTargetData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, morphPosition{ this->getMember< sdw::Vec4 >( "morphPosition", true ) }
			, morphNormal{ this->getMember< sdw::Vec4 >( "morphNormal", true ) }
			, morphTangent{ this->getMember< sdw::Vec4 >( "morphTangent", true ) }
			, morphTexture0{ this->getMember< sdw::Vec4 >( "morphTexcoord0", true ) }
			, morphTexture1{ this->getMember< sdw::Vec4 >( "morphTexcoord1", true ) }
			, morphTexture2{ this->getMember< sdw::Vec4 >( "morphTexcoord2", true ) }
			, morphTexture3{ this->getMember< sdw::Vec4 >( "morphTexcoord3", true ) }
			, morphColour{ this->getMember< sdw::Vec4 >( "morphColour", true ) }
		{
		}

		ast::type::StructPtr MorphTargetData::makeType( ast::type::TypesCache & cache
			, MorphFlags morphFlags )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphTargetData" );

			if ( result->empty() )
			{
				result->declMember( "morphPosition", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::ePositions ) );
				result->declMember( "morphNormal", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eNormals ) );
				result->declMember( "morphTangent", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eTangents ) );
				result->declMember( "morphTexcoord0", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eTexcoords0 ) );
				result->declMember( "morphTexcoord1", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eTexcoords1 ) );
				result->declMember( "morphTexcoord2", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eTexcoords2 ) );
				result->declMember( "morphTexcoord3", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eTexcoords3 ) );
				result->declMember( "morphColour", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eColours ) );
			}

			return result;
		}

		void MorphTargetData::morph( sdw::Vec4 & pos
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col
			, sdw::Float const & weight )const
		{
			if ( !isEnabled() )
			{
				return;
			}

			pos.xyz() += morphPosition.xyz() * weight;
			uvw0 += morphTexture0.xyz() * weight;
			uvw1 += morphTexture1.xyz() * weight;
			uvw2 += morphTexture2.xyz() * weight;
			uvw3 += morphTexture3.xyz() * weight;
			col += morphColour.xyz() * weight;
		}

		void MorphTargetData::morph( sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col
			, sdw::Float const & weight )const
		{
			if ( !isEnabled() )
			{
				return;
			}

			morph( pos
				, uvw0
				, uvw1
				, uvw2
				, uvw3
				, col
				, weight );
			nml.xyz() += morphNormal.xyz() * weight;
		}

		void MorphTargetData::morph( sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col
			, sdw::Float const & weight )const
		{
			if ( !isEnabled() )
			{
				return;
			}

			morph( pos
				, nml
				, uvw0
				, uvw1
				, uvw2
				, uvw3
				, col
				, weight );
			tan.xyz() += morphTangent.xyz() * weight;
		}

		//*****************************************************************************************

		MorphTargetsData::MorphTargetsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_data{ getMemberArray< MorphTargetData >( "targets" ) }
		{
		}

		ast::type::StructPtr MorphTargetsData::makeType( ast::type::TypesCache & cache
			, MorphFlags morphFlags )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphTargetsData" );

			if ( result->empty() )
			{
				result->declMember( "targets"
					, MorphTargetData::makeType( cache, morphFlags )
					, MaxMorphTargets );
			}

			return result;
		}

		//*****************************************************************************************

		castor::String const MorphingWeightsData::BufferName = cuT( "MorphingWeights" );
		castor::String const MorphingWeightsData::DataName = cuT( "c3d_morphingWeightsData" );

		MorphingWeightsData::MorphingWeightsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_data{ getMember< sdw::Vec4 >( "weights" ) }
		{
		}

		ast::type::BaseStructPtr MorphingWeightsData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphingWeightsData" );

			if ( result->empty() )
			{
				result->declMember( "weights", ast::type::Kind::eVec4F );
			}

			return result;
		}

		//*****************************************************************************************

		MorphingWeightsDataArray::MorphingWeightsDataArray( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_data{ getMemberArray< MorphingWeightsData >( "weights" ) }
		{
		}

		ast::type::StructPtr MorphingWeightsDataArray::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphingWeightsDataArray" );

			if ( result->empty() )
			{
				result->declMember( "weights"
					, MorphingWeightsData::makeType( cache )
					, MaxMorphTargets );
			}

			return result;
		}

		//*****************************************************************************************
	}

	void morph( sdw::Array< shader::MorphTargetsData > const & targets
		, shader::MorphingWeightsDataArray const & weights
		, sdw::UInt vertexId
		, sdw::UInt morphTargetsCount
		, sdw::Vec4 & pos
		, sdw::Vec3 & uvw0
		, sdw::Vec3 & uvw1
		, sdw::Vec3 & uvw2
		, sdw::Vec3 & uvw3
		, sdw::Vec3 & col )
	{
		if ( !targets.isEnabled() )
		{
			return;
		}

		auto & writer = *targets.getWriter();
		auto morphTargets = writer.declLocale( "morphTargets"
			, targets[vertexId] );
		auto morphWeight = writer.declLocale( "morphWeight"
			, 0.0_f );

		FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weights[mphIndex];

			IF( writer, morphWeight != 0.0_f )
			{
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[mphIndex] );
				target.morph( pos
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			FI;
		}
		ROF;
	}

	void morph( sdw::Array< shader::MorphTargetsData > const & targets
		, shader::MorphingWeightsDataArray const & weights
		, sdw::UInt vertexId
		, sdw::UInt morphTargetsCount
		, sdw::Vec4 & pos
		, sdw::Vec4 & nml
		, sdw::Vec3 & uvw0
		, sdw::Vec3 & uvw1
		, sdw::Vec3 & uvw2
		, sdw::Vec3 & uvw3
		, sdw::Vec3 & col )
	{
		if ( !targets.isEnabled() )
		{
			return;
		}

		auto & writer = *targets.getWriter();
		auto morphTargets = writer.declLocale( "morphTargets"
			, targets[vertexId] );
		auto morphWeight = writer.declLocale( "morphWeight"
			, 0.0_f );

		FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weights[mphIndex];

			IF( writer, morphWeight != 0.0_f )
			{
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[mphIndex] );
				target.morph( pos
					, nml
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			FI;
		}
		ROF;
	}

	void morph( sdw::Array< shader::MorphTargetsData > const & targets
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
		, sdw::Vec3 & col )
	{
		if ( !targets.isEnabled() )
		{
			return;
		}

		auto & writer = *targets.getWriter();
		auto morphTargets = writer.declLocale( "morphTargets"
			, targets[vertexId] );
		auto morphWeight = writer.declLocale( "morphWeight"
			, 0.0_f );

		FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weights[mphIndex];

			IF( writer, morphWeight != 0.0_f )
			{
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[mphIndex] );
				target.morph( pos
					, nml
					, tan
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			FI;
		}
		ROF;
	}

	//*********************************************************************************************
}
