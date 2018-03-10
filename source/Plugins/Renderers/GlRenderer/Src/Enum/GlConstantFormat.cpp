#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlConstantFormatBaseType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";

		case gl_renderer::GL_CONSTANT_FORMAT_BASE_TYPE_INT:
			return "GL_INT";

		case gl_renderer::GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_INT:
			return "GL_UNSIGNED_INT";

		case gl_renderer::GL_CONSTANT_FORMAT_BASE_TYPE_FLOAT:
			return "GL_FLOAT";

		default:
			assert( false && "Unsupported GlConstantFormat" );
			return "GlConstantFormat_UNKNOWN";
		}
	}

	GlConstantFormatBaseType getType( renderer::ConstantFormat format )
	{
		switch ( format )
		{
		case renderer::ConstantFormat::eFloat:
		case renderer::ConstantFormat::eVec2f:
		case renderer::ConstantFormat::eVec3f:
		case renderer::ConstantFormat::eVec4f:
		case renderer::ConstantFormat::eMat2f:
		case renderer::ConstantFormat::eMat3f:
		case renderer::ConstantFormat::eMat4f:
			return GL_CONSTANT_FORMAT_BASE_TYPE_FLOAT;

		case renderer::ConstantFormat::eInt:
		case renderer::ConstantFormat::eVec2i:
		case renderer::ConstantFormat::eVec3i:
		case renderer::ConstantFormat::eVec4i:
			return GL_CONSTANT_FORMAT_BASE_TYPE_INT;

		case renderer::ConstantFormat::eUInt:
		case renderer::ConstantFormat::eVec2ui:
		case renderer::ConstantFormat::eVec3ui:
		case renderer::ConstantFormat::eVec4ui:
			return GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_INT;

		case renderer::ConstantFormat::eColour:
			return GL_CONSTANT_FORMAT_BASE_TYPE_UNSIGNED_BYTE;

		default:
			assert( false && "Unsupported ConstantFormat." );
			return GL_CONSTANT_FORMAT_BASE_TYPE_FLOAT;
		}
	}

	uint32_t getCount( renderer::ConstantFormat format )
	{
		switch ( format )
		{
		case renderer::ConstantFormat::eFloat:
		case renderer::ConstantFormat::eInt:
		case renderer::ConstantFormat::eUInt:
			return 1u;

		case renderer::ConstantFormat::eVec2f:
		case renderer::ConstantFormat::eVec2i:
		case renderer::ConstantFormat::eVec2ui:
			return 2u;

		case renderer::ConstantFormat::eVec3f:
		case renderer::ConstantFormat::eVec3i:
		case renderer::ConstantFormat::eVec3ui:
			return 3u;

		case renderer::ConstantFormat::eVec4f:
		case renderer::ConstantFormat::eVec4i:
		case renderer::ConstantFormat::eVec4ui:
		case renderer::ConstantFormat::eMat2f:
		case renderer::ConstantFormat::eColour:
			return 4u;

		case renderer::ConstantFormat::eMat3f:
			return 9u;

		case renderer::ConstantFormat::eMat4f:
			return 16u;

		default:
			assert( false && "Unsupported ConstantFormat." );
			return 4u;
		}
	}
}
