/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_ATTRIBUTE_H___
#define ___C3DGLES3_ATTRIBUTE_H___

#include "Mesh/GlES3AttributeBase.hpp"

namespace GlES3Render
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlES3AttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Count >
	class GlES3VecAttribute
		: public GlES3AttributeBase
	{
	public:
		GlES3VecAttribute( OpenGlES3 & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName, uint32_t p_divisor );
		virtual ~GlES3VecAttribute();
		void Bind( bool p_bNormalised = false )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlES3AttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	class GlES3MatAttribute
		: public GlES3AttributeBase
	{
	public:
		GlES3MatAttribute( OpenGlES3 & p_gl, Castor3D::ShaderProgram const & p_program, uint32_t p_stride, Castor::String const & p_attributeName );
		virtual ~GlES3MatAttribute();
		void Bind( bool p_bNormalised = false )override;
	};
}

#include "GlES3Attribute.inl"

#endif
