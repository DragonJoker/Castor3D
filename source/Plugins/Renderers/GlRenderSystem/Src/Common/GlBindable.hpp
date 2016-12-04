/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GL_BINDABLE_H___
#define ___GL_BINDABLE_H___

#include "Common/GlObject.hpp"

namespace GlRender
{
	/*!
	@author
		Sylvain DOREMUS
	@date
		01/12/2015
	@version
		0.8.0
	@brief
		Base class for each OpenGL bindable object (with glBind* or glBind* / glUnbind* functions)
	*/
	template< typename CreateFunction, typename DestroyFunction,
			  typename BindFunction, typename UnbindFunction = BindFunction >
	class Bindable
		: private Object< CreateFunction, DestroyFunction >
	{
		using ObjectType = Object< CreateFunction, DestroyFunction >;

	public:
		/** Constructor
		@param[in] p_openGl
			The OpenGl instance
		@param[in] p_typeName
			The object type name
		@param[in] p_creator
			The creation function
		@param[in] p_destroyer
			The destruction function
		@param[in] p_validator
			The validation function
		@param[in] p_binder
			The binding function
		*/
		template< typename ValidatorFunction >
		inline Bindable( OpenGl & p_openGl, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator, BindFunction p_binder )
			: ObjectType( p_openGl, p_typeName, p_creator, p_destroyer, p_validator )
			, m_binder( p_binder )
			, m_unbinder( p_binder )
		{
		}
		/** Constructor
		@param[in] p_openGl
			The OpenGl instance
		@param[in] p_typeName
			The object type name
		@param[in] p_creator
			The creation function
		@param[in] p_destroyer
			The destruction function
		@param[in] p_validator
			The validation function
		@param[in] p_binder
			The binding function
		@param[in] p_unbinder
			The binding function
		*/
		template< typename ValidatorFunction >
		inline Bindable( OpenGl & p_openGl, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator, BindFunction p_binder, UnbindFunction p_unbinder )
			: ObjectType( p_openGl, p_typeName, p_creator, p_destroyer, p_validator )
			, m_binder( p_binder )
			, m_unbinder( p_unbinder )
		{
		}

		/** Destructor
		*/
		inline ~Bindable()
		{
		}

		/** Creates the object on GPU
		@return
			false if not created successfully
		*/
		inline bool Create( bool p_prebind = true )
		{
			bool l_return = IsValid();

			if ( !l_return )
			{
				l_return = ObjectType::DoCreate();

				if ( l_return && p_prebind )
				{
					if ( l_return )
					{
						l_return = Bind();
					}

					if ( l_return )
					{
						Unbind();
					}

					l_return = IsValid() && Validate();

					if ( !l_return )
					{
						ObjectType::DoDestroy();
					}
				}
			}

			if ( l_return )
			{
				glTrack( GetOpenGl(), GetGlTypeName(), this );

#if !defined( NDEBUG )

				m_hasBeenValid = true;

#endif
			}

			return l_return;
		}

		/** Binds the object on GPU
		@return
			false if not bound successfully
		*/
		inline bool Bind()const
		{
			REQUIRE( IsValid() );
			return m_binder( GetGlName() );
		}

		/** Unbinds the object on GPU
		*/
		inline void Unbind()const
		{
			m_unbinder( 0u );
		}

	public:
		using ObjectType::Destroy;
		using ObjectType::IsValid;
		using ObjectType::GetGlName;
		using ObjectType::Validate;
		using ObjectType::GetGlTypeName;
		using Holder::GetOpenGl;

	private:
		//! The binding function
		BindFunction m_binder;
		//! The unbinding function
		UnbindFunction m_unbinder;
	};
}

#endif
