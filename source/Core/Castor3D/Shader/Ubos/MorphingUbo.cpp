#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		MorphTargetData::MorphTargetData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, morphPosition{ this->getMember< sdw::Vec4 >( "morphPosition", true ) }
			, morphNormal{ this->getMember< sdw::Vec4 >( "morphNormal", true ) }
			, morphTangent{ this->getMember< sdw::Vec4 >( "morphTangent", true ) }
			, morphBitangent{ this->getMember< sdw::Vec4 >( "morphBitangent", true ) }
			, morphTexture0{ this->getMember< sdw::Vec4 >( "morphTexcoord0", true ) }
			, morphTexture1{ this->getMember< sdw::Vec4 >( "morphTexcoord1", true ) }
			, morphTexture2{ this->getMember< sdw::Vec4 >( "morphTexcoord2", true ) }
			, morphTexture3{ this->getMember< sdw::Vec4 >( "morphTexcoord3", true ) }
			, morphColour{ this->getMember< sdw::Vec4 >( "morphColour", true ) }
		{
		}

		ast::type::BaseStructPtr MorphTargetData::makeType( ast::type::TypesCache & cache
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
				result->declMember( "morphBitangent", ast::type::Kind::eVec4F
					, ast::type::NotArray
					, checkFlag( morphFlags, MorphFlag::eBitangents ) );
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

		void MorphTargetData::morph( sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec4 & bit
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
				, tan
				, uvw0
				, uvw1
				, uvw2
				, uvw3
				, col
				, weight );
			bit.xyz() += morphBitangent.xyz() * weight;
		}

		//*****************************************************************************************

		MorphTargetsData::MorphTargetsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_data{ getMemberArray< MorphTargetData >( "targets" ) }
		{
		}

		ast::type::BaseStructPtr MorphTargetsData::makeType( ast::type::TypesCache & cache
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

		MorphingWeightsData::MorphingWeightsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, m_limits{ getMember< sdw::UVec4 >( "limits" ) }
			, m_indices{ getMemberArray< sdw::UVec4 >( "indices" ) }
			, m_weights{ getMemberArray< sdw::Vec4 >( "weights" ) }
			, morphTargetsCount{ m_limits.x() }
		{
		}

		ast::type::BaseStructPtr MorphingWeightsData::makeType( ast::type::TypesCache & cache )
		{
			auto result = cache.getStruct( ast::type::MemoryLayout::eStd140
				, "C3D_MorphingWeightsIndices" );

			if ( result->empty() )
			{
				result->declMember( "limits"
					, ast::type::Kind::eVec4U
					, ast::type::NotArray );
				result->declMember( "indices"
					, ast::type::Kind::eVec4U
					, MaxMorphTargets );
				result->declMember( "weights"
					, ast::type::Kind::eVec4F
					, MaxMorphTargets );
			}

			return result;
		}

		void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
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
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
				target.morph( pos
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			ROF;
		}

		void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
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
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
				target.morph( pos
					, nml
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			ROF;
		}

		void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
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
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
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
			ROF;
		}

		void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec4 & bit
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
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
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
				target.morph( pos
					, nml
					, tan
					, bit
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			ROF;
		}

		void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
		{
			if ( !targets.isEnabled() )
			{
				return;
			}

			auto & writer = *getWriter();
			auto morphTargets = writer.declLocale( "morphTargets"
				, targets[vertexId] );
			auto morphWeight = writer.declLocale( "morphWeight"
				, 0.0_f );
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
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
			ROF;
		}

		void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec4 & bit
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col )const
		{
			if ( !targets.isEnabled() )
			{
				return;
			}

			auto & writer = *getWriter();
			auto morphTargets = writer.declLocale( "morphTargets"
				, targets[vertexId] );
			auto morphWeight = writer.declLocale( "morphWeight"
				, 0.0_f );
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );

			FOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
			{
				morphWeight = weight( mphIndex );
				morphIndex = index( mphIndex );
				auto target = writer.declLocale( "morphTarget"
					, morphTargets[morphIndex] );
				target.morph( pos
					, nml
					, tan
					, bit
					, uvw0
					, uvw1
					, uvw2
					, uvw3
					, col
					, morphWeight );
			}
			ROF;
		}

		void MorphingWeightsData::morphNoAnim( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt vertexId
			, sdw::Vec4 & pos
			, sdw::Vec4 & nml
			, sdw::Vec4 & tan
			, sdw::Vec4 & bit
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

			auto & writer = findWriterMandat( vertexId, pos, nml, tan, uvw0, uvw1, uvw2, col );
			auto morphTargets = writer.declLocale( "morphTargets"
				, targets[vertexId] );
			auto morphWeight = writer.declLocale( "morphWeight"
				, 0.0_f );
			auto morphIndex = writer.declLocale( "morphIndex"
				, 0_u );
			morphWeight = 1.0_f;
			morphIndex = 0_u;
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( pos
				, nml
				, tan
				, bit
				, uvw0
				, uvw1
				, uvw2
				, uvw3
				, col
				, morphWeight );
		}

		//*****************************************************************************************
	}
}
