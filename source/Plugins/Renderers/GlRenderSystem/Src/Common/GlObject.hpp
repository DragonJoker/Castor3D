/*
See LICENSE file in root folder
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
	struct CreatorHelper< std::function< void( int, uint32_t * ) > >
	{
		typedef std::function< void( int, uint32_t * ) > CreateFunction;

		static inline uint32_t apply( CreateFunction p_creator )
		{
			uint32_t result = -1;
			p_creator( 1, &result );
			return result;
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

		static inline uint32_t apply( CreateFunction p_creator )
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
	struct DestroyerHelper< std::function< void( int, uint32_t const * ) > >
	{
		typedef std::function< void( int, uint32_t const * ) > DestroyFunction;

		static inline void apply( DestroyFunction p_destroyer, uint32_t p_glName )
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
	struct DestroyerHelper< std::function< void( uint32_t ) > >
	{
		typedef std::function< void( uint32_t ) > DestroyFunction;

		static inline void apply( DestroyFunction p_destroyer, uint32_t p_glName )
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
		inline bool create()
		{
			bool result = isValid();

			if ( !result )
			{
				result = doCreate();
			}

			if ( result )
			{
				glTrack( Holder::getOpenGl(), m_typeName, this );

#if !defined( NDEBUG )

				m_hasBeenValid = true;

#endif
			}

			return result;
		}

		/** Destroys the object on GPU, if valid.
		 */
		inline void destroy()
		{
#if !defined( NDEBUG )

			if ( m_hasBeenValid )
			{
				m_hasBeenValid = !glUntrack( Holder::getOpenGl(), this );
			}

#endif

			if ( isValid() )
			{
				doDestroy();
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
		inline bool isValid()const
		{
			return m_glName != GlInvalidIndex;
		}

		/** Tells if the object is a valid OpenGL object.
		@return
			The status.
		*/
		inline bool validate()const
		{
			return isValid() && m_validator( m_glName );
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline uint32_t const & getGlName()const
		{
			return m_glName;
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline uint32_t & getGlName()
		{
			return m_glName;
		}

		/** Retrieves the OpenGl type name.
		@return
			The name.
		*/
		inline const char * getGlTypeName()
		{
			return m_typeName;
		}

	protected:
		/** Effectively creates the object on GPU.
		@return
			false if not created successfully.
		 */
		inline bool doCreate()
		{
			m_glName = CreatorHelper< CreateFunction >::apply( m_creator );
			return m_glName != GlInvalidIndex;
		}

		/** Effectively destroys the object on GPU.
		 */
		inline void doDestroy()
		{
			DestroyerHelper< DestroyFunction >::apply( m_destroyer, m_glName );
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
