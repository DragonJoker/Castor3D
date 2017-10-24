/*
See LICENSE file in root folder
*/
#ifndef ___GL_ATTRIBUTE_H___
#define ___GL_ATTRIBUTE_H___

#include "Mesh/GlAttributeBase.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlAttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Count >
	class GlVecAttribute
		: public GlAttributeBase
	{
	public:
		GlVecAttribute( OpenGl & p_gl, castor3d::ShaderProgram const & p_program, uint32_t p_stride, castor::String const & p_attributeName, uint32_t p_divisor );
		virtual ~GlVecAttribute();
		void bind( bool p_bNormalised = false )override;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\brief		Derived from GlAttribsBase, allows to specify attribute type and count
	*/
	template< typename T, uint32_t Columns, uint32_t Rows >
	class GlMatAttribute
		: public GlAttributeBase
	{
	public:
		GlMatAttribute( OpenGl & p_gl, castor3d::ShaderProgram const & p_program, uint32_t p_stride, castor::String const & p_attributeName );
		virtual ~GlMatAttribute();
		void bind( bool p_bNormalised = false )override;
	};
}

#include "GlAttribute.inl"

#endif
