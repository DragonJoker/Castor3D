/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_BINDABLE_H___
#define ___C3DGLES3_BINDABLE_H___

#include "Common/GlES3Object.hpp"

namespace GlES3Render
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
		@param[in] p_openGlES3
			The OpenGlES3 instance
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
		inline Bindable( OpenGlES3 & p_openGlES3, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator, BindFunction p_binder )
			: ObjectType( p_openGlES3, p_typeName, p_creator, p_destroyer, p_validator )
			, m_binder( p_binder )
			, m_unbinder( p_binder )
		{
		}
		/** Constructor
		@param[in] p_openGlES3
			The OpenGlES3 instance
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
		inline Bindable( OpenGlES3 & p_openGlES3, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator, BindFunction p_binder, UnbindFunction p_unbinder )
			: ObjectType( p_openGlES3, p_typeName, p_creator, p_destroyer, p_validator )
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
						Bind();
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
				glTrack( GetOpenGlES3(), GetGlES3TypeName(), this );

#if !defined( NDEBUG )

				ObjectType::m_hasBeenValid = true;

#endif
			}

			return l_return;
		}

		/** Binds the object on GPU
		@return
			false if not bound successfully
		*/
		inline void Bind()const
		{
			REQUIRE( IsValid() );
			m_binder( GetGlES3Name() );
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
		using ObjectType::GetGlES3Name;
		using ObjectType::Validate;
		using ObjectType::GetGlES3TypeName;
		using Holder::GetOpenGlES3;

	private:
		//! The binding function
		BindFunction m_binder;
		//! The unbinding function
		UnbindFunction m_unbinder;
	};
}

#endif
