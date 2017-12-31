/*
See LICENSE file in root folder
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
		inline bool create( bool p_prebind = true )
		{
			bool result = isValid();

			if ( !result )
			{
				result = ObjectType::doCreate();

				if ( result && p_prebind )
				{
					if ( result )
					{
						bind();
						unbind();
					}

					result = isValid() && validate();

					if ( !result )
					{
						ObjectType::doDestroy();
					}
				}
			}

			if ( result )
			{
				glTrack( getOpenGl(), getGlTypeName(), this );

#if !defined( NDEBUG )

				ObjectType::m_hasBeenValid = true;

#endif
			}

			return result;
		}

		/** Binds the object on GPU
		@return
			false if not bound successfully
		*/
		inline void bind()const
		{
			REQUIRE( isValid() );
			m_binder( getGlName() );
		}

		/** Unbinds the object on GPU
		*/
		inline void unbind()const
		{
			m_unbinder( 0u );
		}

	public:
		using ObjectType::destroy;
		using ObjectType::isValid;
		using ObjectType::getGlName;
		using ObjectType::validate;
		using ObjectType::getGlTypeName;
		using Holder::getOpenGl;

	private:
		//! The binding function
		BindFunction m_binder;
		//! The unbinding function
		UnbindFunction m_unbinder;
	};
}

#endif
