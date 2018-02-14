#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlAttributeFormatBaseType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_ATTRIBUTE_FORMAT_BASE_TYPE_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";

		case gl_renderer::GL_ATTRIBUTE_FORMAT_BASE_TYPE_INT:
			return "GL_INT";

		case gl_renderer::GL_ATTRIBUTE_FORMAT_BASE_TYPE_UNSIGNED_INT:
			return "GL_UNSIGNED_INT";

		case gl_renderer::GL_ATTRIBUTE_FORMAT_BASE_TYPE_FLOAT:
			return "GL_FLOAT";

		default:
			assert( false && "Unsupported GlAttributeFormatBaseType" );
			return "GlAttributeFormatBaseType_UNKNOWN";
		}
	}

	GlAttributeFormatBaseType getType( renderer::AttributeFormat format )
	{
		switch ( format )
		{
		case renderer::AttributeFormat::eFloat:
		case renderer::AttributeFormat::eVec2f:
		case renderer::AttributeFormat::eVec3f:
		case renderer::AttributeFormat::eVec4f:
		case renderer::AttributeFormat::eMat2f:
		case renderer::AttributeFormat::eMat3f:
		case renderer::AttributeFormat::eMat4f:
			return GL_ATTRIBUTE_FORMAT_BASE_TYPE_FLOAT;

		case renderer::AttributeFormat::eInt:
		case renderer::AttributeFormat::eVec2i:
		case renderer::AttributeFormat::eVec3i:
		case renderer::AttributeFormat::eVec4i:
			return GL_ATTRIBUTE_FORMAT_BASE_TYPE_INT;

		case renderer::AttributeFormat::eUInt:
		case renderer::AttributeFormat::eVec2ui:
		case renderer::AttributeFormat::eVec3ui:
		case renderer::AttributeFormat::eVec4ui:
			return GL_ATTRIBUTE_FORMAT_BASE_TYPE_UNSIGNED_INT;

		case renderer::AttributeFormat::eColour:
			return GL_ATTRIBUTE_FORMAT_BASE_TYPE_UNSIGNED_BYTE;

		default:
			assert( false && "Unsupported attribute format." );
			return GL_ATTRIBUTE_FORMAT_BASE_TYPE_FLOAT;
		}
	}

	uint32_t getCount( renderer::AttributeFormat format )
	{
		switch ( format )
		{
		case renderer::AttributeFormat::eFloat:
		case renderer::AttributeFormat::eInt:
		case renderer::AttributeFormat::eUInt:
			return 1u;

		case renderer::AttributeFormat::eVec2f:
		case renderer::AttributeFormat::eVec2i:
		case renderer::AttributeFormat::eVec2ui:
			return 2u;

		case renderer::AttributeFormat::eVec3f:
		case renderer::AttributeFormat::eVec3i:
		case renderer::AttributeFormat::eVec3ui:
			return 3u;

		case renderer::AttributeFormat::eVec4f:
		case renderer::AttributeFormat::eVec4i:
		case renderer::AttributeFormat::eVec4ui:
		case renderer::AttributeFormat::eMat2f:
		case renderer::AttributeFormat::eColour:
			return 4u;

		case renderer::AttributeFormat::eMat3f:
			return 9u;

		case renderer::AttributeFormat::eMat4f:
			return 16u;

		default:
			assert( false && "Unsupported attribute format." );
			return 4u;
		}
	}
}
