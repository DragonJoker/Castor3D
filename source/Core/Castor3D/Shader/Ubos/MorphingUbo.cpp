#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"

#include <ShaderWriter/CompositeTypes/ArrayStorageBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d::shader
{
	//*****************************************************************************************

	MorphTargetData::MorphTargetData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, c3d::move( expr ), enabled }
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

	void MorphTargetData::morph( sdw::Vec4 const & ioPos
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol
		, sdw::Float const & weight )const
	{
		if ( !isEnabled() )
		{
			return;
		}

		ioPos.xyz() += morphPosition.xyz() * weight;
		ioUVW0 += morphTexture0.xyz() * weight;
		ioUVW1 += morphTexture1.xyz() * weight;
		ioUVW2 += morphTexture2.xyz() * weight;
		ioUVW3 += morphTexture3.xyz() * weight;
		ioCol += morphColour.xyz() * weight;
	}

	void MorphTargetData::morph( sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol
		, sdw::Float const & weight )const
	{
		if ( !isEnabled() )
		{
			return;
		}

		morph( ioPos
			, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
			, weight );
		ioNml.xyz() += morphNormal.xyz() * weight;
	}

	void MorphTargetData::morph( sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol
		, sdw::Float const & weight )const
	{
		if ( !isEnabled() )
		{
			return;
		}

		morph( ioPos, ioNml
			, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
			, weight );
		ioTan.xyz() += morphTangent.xyz() * weight;
	}

	void MorphTargetData::morph( sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec4 const & ioBit
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol
		, sdw::Float const & weight )const
	{
		if ( !isEnabled() )
		{
			return;
		}

		morph( ioPos, ioNml, ioTan
			, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
			, weight );
		ioBit.xyz() += morphBitangent.xyz() * weight;
	}

	//*****************************************************************************************

	MorphTargetsData::MorphTargetsData( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: StructInstance{ writer, c3d::move( expr ), enabled }
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

	void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml, ioTan
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::Array< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec4 const & ioBit
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml, ioTan, ioBit
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml, ioTan
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec4 const & ioBit
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )const
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

		sdwFOR( writer, sdw::UInt, mphIndex, 0_u, mphIndex < morphTargetsCount, ++mphIndex )
		{
			morphWeight = weight( mphIndex );
			morphIndex = index( mphIndex );
			auto target = writer.declLocale( "morphTarget"
				, morphTargets[morphIndex] );
			target.morph( ioPos, ioNml, ioTan, ioBit
				, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
				, morphWeight );
		}
		sdwROF
	}

	void MorphingWeightsData::morphNoAnim( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
		, sdw::UInt const & vertexId
		, sdw::Vec4 const & ioPos
		, sdw::Vec4 const & ioNml
		, sdw::Vec4 const & ioTan
		, sdw::Vec4 const & ioBit
		, sdw::Vec3 & ioUVW0
		, sdw::Vec3 & ioUVW1
		, sdw::Vec3 & ioUVW2
		, sdw::Vec3 & ioUVW3
		, sdw::Vec3 & ioCol )
	{
		if ( !targets.isEnabled() )
		{
			return;
		}

		auto & writer = findWriterMandat( vertexId, ioPos, ioNml, ioTan, ioBit, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol );
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
		target.morph( ioPos, ioNml, ioTan, ioBit
			, ioUVW0, ioUVW1, ioUVW2, ioUVW3, ioCol
			, morphWeight );
	}

	//*****************************************************************************************
}
