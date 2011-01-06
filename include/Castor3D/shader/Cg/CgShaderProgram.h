#ifndef ___C3D_CgShaderProgram___
#define ___C3D_CgShaderProgram___

#include "../ShaderProgram.h"

namespace Castor3D
{
	//! CgShader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderProgram : public ShaderProgram
	{
	protected:
		CGcontext m_cgContext;	//!< Cg context, used for programs creation

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		CgShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		CgShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~CgShaderProgram();
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;

		inline CGcontext			GetContext					()const						 			{ return m_cgContext; }
		inline CgShaderObjectPtr	GetVertexProgram			()const						 			{ return static_pointer_cast<CgShaderObject>( m_pShaders[eVertexShader]); }
		inline CgShaderObjectPtr	GetFragmentProgram			()const						 			{ return static_pointer_cast<CgShaderObject>( m_pShaders[ePixelShader]); }
		inline CgShaderObjectPtr	GetGeometryProgram			()const						 			{ return static_pointer_cast<CgShaderObject>( m_pShaders[eGeometryShader]); }
		inline CgShaderObjectPtr	GetProgram					( eSHADER_PROGRAM_TYPE p_eType)const	{ return static_pointer_cast<CgShaderObject>( m_pShaders[p_eType]);}
	};
}

#endif