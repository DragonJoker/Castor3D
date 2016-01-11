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
#ifndef ___GL_OBJECT_H___
#define ___GL_OBJECT_H___

#include "GlHolder.hpp"

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
			, m_glName( eGL_INVALID_INDEX )
			, m_typeName( p_typeName )
		{
		}

		/** Destructor
		*/
		inline ~Object()
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
			}

			return l_return;
		}

		/** Destroys the object on GPU, if valid.
		 */
		inline void Destroy()
		{
			if ( IsValid() )
			{
				glUntrack( Holder::GetOpenGl(), this );
				DoDestroy();
			}
			else
			{
				m_glName = eGL_INVALID_INDEX;
			}
		}

		/** Tells if the object is created
		@return
			The status
		*/
		inline bool IsValid()const
		{
			return m_glName != eGL_INVALID_INDEX;
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
			return m_glName != eGL_INVALID_INDEX;
		}

		/** Effectively destroys the object on GPU.
		 */
		inline void DoDestroy()
		{
			DestroyerHelper< DestroyFunction >::Apply( m_destroyer, m_glName );
			m_glName = eGL_INVALID_INDEX;
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
	};
}

#define IMPLEMENT_GL_OBJECT()

#endif
