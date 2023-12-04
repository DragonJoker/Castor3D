/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureAnimation_H___
#define ___GLSL_TextureAnimation_H___

#include "GlslBuffer.hpp"
#include "GlslTextureTransform.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		class TextureAnimations
			: public BufferT< TextureTransformData >
		{
		public:
			C3D_API TextureAnimations( sdw::ShaderWriter & writer
				, uint32_t binding
				, uint32_t set 
				, bool enable = true );

			TextureTransformData getTextureAnimation( sdw::UInt const & index )const
			{
				return BufferT< TextureTransformData >::getData( index - 1u );
			}
		};
	}
}

#endif
