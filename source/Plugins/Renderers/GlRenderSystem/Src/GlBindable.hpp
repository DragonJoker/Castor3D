/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/ )

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_BINDABLE_H___
#define ___GL_BINDABLE_H___

#include "GlObject.hpp"

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
			}

			return l_return;
		}

		/** Binds the object on GPU
		@return
			false if not bound successfully
		*/
		inline bool Bind()
		{
			REQUIRE( IsValid() );
			return m_binder( GetGlName() );
		}

		/** Unbinds the object on GPU
		*/
		inline void Unbind()
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
