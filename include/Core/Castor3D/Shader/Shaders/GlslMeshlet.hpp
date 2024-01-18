/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMeshlet_H___
#define ___C3D_GlslMeshlet_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Limits.hpp"

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	struct Meshlet
		: public sdw::StructInstanceHelperT< "C3D_Meshlet"
			, sdw::type::MemoryLayout::eStd430
			, sdw::UIntArrayField< "vertices", MaxMeshletVertexCount >
			, sdw::UInt8ArrayField< "indices", MaxMeshletTriangleCount * 3u >
			, sdw::UIntField< "vertexCount" >
			, sdw::UIntField< "triangleCount" >
			, sdw::UIntField< "meshletIndex" > >
	{
		Meshlet( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled  }
		{
		}

		auto vertices()const { return getMember< "vertices" >(); }
		auto indices()const { return getMember< "indices" >(); }
		auto vertexCount()const { return getMember< "vertexCount" >(); }
		auto triangleCount()const { return getMember< "triangleCount" >(); }
		auto meshletIndex()const { return getMember< "meshletIndex" >(); }
	};
}

#endif
