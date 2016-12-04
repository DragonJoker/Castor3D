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
#ifndef ___GL_OBJECT_H___
#define ___GL_OBJECT_H___

#include "Common/GlHolder.hpp"

namespace GlRender
{
	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to create an object on GPU
	@remarks
		Specialization for glGenBuffers, glGenTextures, ...
	*/
	template<>
	struct CreatorHelper< std::function< bool( int, uint32_t * ) > >
	{
		typedef std::function< bool( int, uint32_t * ) > CreateFunction;

		static inline uint32_t Apply( CreateFunction p_creator )
	{
		uint32_t l_return = -1;

		if ( !p_creator( 1, &l_return ) )
		{
			l_return = -1;
		}

		return l_return;
	}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to create an object on GPU
	@remarks
		Specialization for glCreateShader
	*/
	template<>
	struct CreatorHelper< std::function< uint32_t() > >
	{
		typedef std::function< uint32_t() > CreateFunction;

		static inline uint32_t Apply( CreateFunction p_creator )
	{
		return p_creator();
	}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to destroy an object on GPU
	@remarks
		Specialization for glDeleteBuffers, glDeleteTextures, ...
	*/
	template<>
	struct DestroyerHelper< std::function< bool( int, uint32_t const * ) > >
	{
		typedef std::function< bool( int, uint32_t const * ) > DestroyFunction;

		static inline void Apply( DestroyFunction p_destroyer, uint32_t p_glName )
	{
		p_destroyer( 1, &p_glName );
	}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to destroy an object on GPU
	@remarks
		Specialization for glDeleteShader
	*/
	template<>
	struct DestroyerHelper< std::function< bool( uint32_t ) > >
	{
		typedef std::function< bool( uint32_t ) > DestroyFunction;

		static inline void Apply( DestroyFunction p_destroyer, uint32_t p_glName )
	{
		p_destroyer( p_glName );
	}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Base class for each OpenGL object (with glGen* and glDelete* functions)
	*/
	template< typename CreateFunction, typename DestroyFunction >
	class Object
		: public Holder
	{
	protected:
		typedef std::function< bool( uint32_t ) > ValidatorFunction;

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
		*/
		inline Object( OpenGl & p_openGl, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator )
			: Holder( p_openGl )
			, m_creator( p_creator )
			, m_destroyer( p_destroyer )
			, m_validator( p_validator )
			, m_glName( GlInvalidIndex )
			, m_typeName( p_typeName )
		{
		}

		/** Destructor
		*/
		~Object()
		{
		}

		/** Creates the object on GPU
		@return
			false if not created successfully
		*/
		inline bool Create()
		{
			bool l_return = IsValid();

			if ( !l_return )
			{
				l_return = DoCreate();
			}

			if ( l_return )
			{
				glTrack( Holder::GetOpenGl(), m_typeName, this );

#if !defined( NDEBUG )

				m_hasBeenValid = true;

#endif
			}

			return l_return;
		}

		/** Destroys the object on GPU, if valid.
		 */
		inline void Destroy()
		{
#if !defined( NDEBUG )

			if ( m_hasBeenValid )
			{
				m_hasBeenValid = !glUntrack( Holder::GetOpenGl(), this );
			}

#endif

			if ( IsValid() )
			{
				DoDestroy();
			}
			else
			{
				m_glName = GlInvalidIndex;
			}
		}

		/** Tells if the object is created
		@return
			The status
		*/
		inline bool IsValid()const
		{
			return m_glName != GlInvalidIndex;
		}

		/** Tells if the object is a valid OpenGL object.
		@return
			The status.
		*/
		inline bool Validate()const
		{
			return IsValid() && m_validator( m_glName );
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline uint32_t const & GetGlName()const
		{
			return m_glName;
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline uint32_t & GetGlName()
		{
			return m_glName;
		}

		/** Retrieves the OpenGl type name.
		@return
			The name.
		*/
		inline const char * GetGlTypeName()
		{
			return m_typeName;
		}

	protected:
		/** Effectively creates the object on GPU.
		@return
			false if not created successfully.
		 */
		inline bool DoCreate()
		{
			m_glName = CreatorHelper< CreateFunction >::Apply( m_creator );
			return m_glName != GlInvalidIndex;
		}

		/** Effectively destroys the object on GPU.
		 */
		inline void DoDestroy()
		{
			DestroyerHelper< DestroyFunction >::Apply( m_destroyer, m_glName );
			m_glName = GlInvalidIndex;
		}

	protected:
		//! The creation function
		CreateFunction m_creator;
		//! The destruction function
		DestroyFunction m_destroyer;
		//! The destruction function
		ValidatorFunction m_validator;
		//! The GPU object name
		uint32_t m_glName;
		//! The object type name
		const char * m_typeName;

#if !defined( NDEBUG )

		bool m_hasBeenValid{ false };

#endif
	};
}

#define IMPLEMENT_GL_OBJECT()

#endif
