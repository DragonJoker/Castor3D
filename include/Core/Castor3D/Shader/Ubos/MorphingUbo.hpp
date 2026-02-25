/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingUbo_H___
#define ___C3D_MorphingUbo_H___

#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>

namespace c3d::shader
{
	struct MorphTargetData
		: public sdw::StructInstance
	{
	public:
		SDW_DeclStructInstance( C3D_API, MorphTargetData );

		C3D_API MorphTargetData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, MorphFlags morphFlags = MorphFlags{} );

		C3D_API void morph( sdw::Vec4 const & ioPos
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol
			, sdw::Float const & weight )const;
		C3D_API void morph( sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol
			, sdw::Float const & weight )const;
		C3D_API void morph( sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol
			, sdw::Float const & weight )const;
		C3D_API void morph( sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec4 const & ioBit
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol
			, sdw::Float const & weight )const;

	public:
		sdw::Vec4 morphPosition;
		sdw::Vec4 morphNormal;
		sdw::Vec4 morphTangent;
		sdw::Vec4 morphBitangent;
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
		SDW_DeclStructInstance( C3D_API, MorphTargetsData );

		C3D_API MorphTargetsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, MorphFlags morphFlags = MorphFlags{} );

		sdw::ReturnWrapperT< MorphTargetData > operator[]( sdw::UInt const & index )const
		{
			return m_data[index];
		}

	private:
		sdw::Array< MorphTargetData > m_data;
	};

	struct MorphingWeightsData
		: public sdw::StructInstanceHelperT< "C3D_MorphingWeightsIndices"
			, sdw::type::MemoryLayout::eStd140
			, sdw::UVec4Field< "limits" >
			, sdw::UVec4ArrayField< "indices", MaxMorphTargets >
			, sdw::Vec4ArrayField< "weights", MaxMorphTargets > >
	{
	public:
		SDW_DeclStructInstance( C3D_API, MorphingWeightsData );

		C3D_API MorphingWeightsData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, m_limits{ StructInstanceHelperT::getMember< "limits" >() }
			, m_indices{ StructInstanceHelperT::getMember< "indices" >() }
			, m_weights{ StructInstanceHelperT::getMember< "weights" >() }
			, morphTargetsCount{ m_limits.x() }
		{
		}

		C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::Array< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec4 const & ioBit
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;

		C3D_API void morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;
		C3D_API void morph( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec4 const & ioBit
			, sdw::Vec3 & ioUVW0
			, sdw::Vec3 & ioUVW1
			, sdw::Vec3 & ioUVW2
			, sdw::Vec3 & ioUVW3
			, sdw::Vec3 & ioCol )const;

		C3D_API static void morphNoAnim( sdw::ArrayStorageBufferT< shader::MorphTargetsData > const & targets
			, sdw::UInt const & vertexId
			, sdw::Vec4 const & ioPos
			, sdw::Vec4 const & ioNml
			, sdw::Vec4 const & ioTan
			, sdw::Vec4 const & ioBit
			, sdw::Vec3 & uvw0
			, sdw::Vec3 & uvw1
			, sdw::Vec3 & uvw2
			, sdw::Vec3 & uvw3
			, sdw::Vec3 & col );

		sdw::UInt index( sdw::UInt const & index )const
		{
			return m_indices[index / 4_u][index % 4_u];
		}

		sdw::Float weight( sdw::UInt const & index )const
		{
			return m_weights[index / 4_u][index % 4_u];
		}

	private:
		sdw::UVec4 m_limits;
		sdw::Array< sdw::UVec4 > m_indices;
		sdw::Array< sdw::Vec4 > m_weights;

	public:
		sdw::UInt morphTargetsCount;
	};
}

#endif
