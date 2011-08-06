/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_ShaderProgram___
#define ___C3D_ShaderProgram___

#include "Prerequisites.hpp"
#include "RenderSystem.hpp"
#include "FrameVariable.hpp"
#include "ShaderObject.hpp"

namespace Castor3D
{
	template <class Ty> struct FrameVariableCreator;
	template <class Ty> struct ShaderObjectCreator;
	/*!
	Base shader program implementation, used to share functions through the three supported shader languages (Glsl, Hlsl and Cg)
	\author Sylvain DOREMUS
	\version 0.6.1.2
	\date 24/01/2011
	*/
	class C3D_API ShaderProgramBase
	{
		template <class Ty> friend struct FrameVariableCreator;
		template <class Ty> friend struct ShaderObjectCreator;

	public:
		typedef enum
		{	eSHADER_LANGUAGE_GLSL
		,	eSHADER_LANGUAGE_HLSL
		,	eSHADER_LANGUAGE_CG
		,	eSHADER_LANGUAGE_COUNT
		}	eSHADER_LANGUAGE;

	protected:
		bool									m_bLinked;	//!< Tells if the program is linked
		bool									m_bError;
		eSHADER_LANGUAGE						m_eType;
		bool									m_bEnabled;	//!< Tells whether or not the shader is enabled
		ShaderObjectPtr							m_pShaders[eSHADER_TYPE_COUNT];
		Path									m_pathFile;
		FrameVariablePtrList					m_listFrameVariables;

		shared_ptr <Point4fFrameVariable>		m_pAmbients;
		shared_ptr <Point4fFrameVariable>		m_pDiffuses;
		shared_ptr <Point4fFrameVariable>		m_pSpeculars;
		shared_ptr <Point4fFrameVariable>		m_pPositions;
		shared_ptr <Point3fFrameVariable>		m_pAttenuations;
		shared_ptr <Matrix4x4fFrameVariable>	m_pOrientations;
		shared_ptr <OneFloatFrameVariable>		m_pExponents;
		shared_ptr <OneFloatFrameVariable>		m_pCutOffs;

		std::set <int>							m_setFreeLights;

		shared_ptr <Point4fFrameVariable> 		m_pAmbient;
		shared_ptr <Point4fFrameVariable> 		m_pDiffuse;
		shared_ptr <Point4fFrameVariable> 		m_pEmissive;
		shared_ptr <Point4fFrameVariable> 		m_pSpecular;
		shared_ptr <OneFloatFrameVariable>		m_pShininess;
		bool									m_bMatChanged;

		shared_ptr < Point4fFrameVariable> 		m_pAmbientLight;
		
	public:
		/**
		 * Constructor
		 */
		ShaderProgramBase( eSHADER_LANGUAGE p_eType, const StringArray & p_shaderFiles);
		/**
		 * Constructor
		 */
		ShaderProgramBase( eSHADER_LANGUAGE p_eType);
		/**
		 * Destructor
		 */
		virtual ~ShaderProgramBase();
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Creates a shader object
		 */
		void CreateObject( eSHADER_TYPE p_eType);
		/**
		 * Begins the program
		 */
		virtual void Begin();
		/**
		 * Ends the program
		 */
		virtual void End();
		/**
		 * Sets the file for the given program
		 */
		virtual void SetProgramFile( String const & p_strFile, eSHADER_TYPE p_eType);
		/**
		 * Sets the text for the given program
		 */
		virtual void SetProgramText( String const & p_strFile, eSHADER_TYPE p_eType);
		/**
		 * Retrieves the shader program wanted, creates it if null
		 */
		ShaderObjectPtr GetProgram( eSHADER_TYPE p_eType);
		/**
		 * Sets all objects file
		 */
		virtual void SetFile( Path const & p_path);
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link();
		/**
		 * Returns a free light index and assigns it
		 */
		virtual int AssignLight();
		/**
		 * Frees a given light index
		 */
		virtual void FreeLight( int p_iIndex);
		/**
		 * Sets the ambient light
		 */
		virtual void SetAmbientLight( const Point4f & p_crColour);
		/**
		 * Sets ambient component of the light at given index
		 */
		virtual void SetLightAmbient( int p_iIndex, const Point4f & p_crColour);
		/**
		 * Sets diffuse component of the light at given index
		 */
		virtual void SetLightDiffuse( int p_iIndex, const Point4f & p_crColour);
		/**
		 * Sets specular component of the light at given index
		 */
		virtual void SetLightSpecular( int p_iIndex, const Point4f & p_crColour);
		/**
		 * Sets the position of the light at given index
		 */
		virtual void SetLightPosition( int p_iIndex, const Point4f & p_ptPosition);
		/**
		 * Sets the orientation of the light at given index
		 */
		virtual void SetLightOrientation( int p_iIndex, Matrix4x4r const & p_mtxOrientation);
		/**
		 * Sets the attenuation components of the light at given index (spot and point lights only)
		 */
		virtual void SetLightAttenuation( int p_iIndex, const Point3f & p_fAtt);
		/**
		 * Sets the exponent of the light at given index (spotlight only)
		 */
		virtual void SetLightExponent( int p_iIndex, float p_fExp);
		/**
		 * Sets the cutoff angle of the light at given index (spotlight only)
		 */
		virtual void SetLightCutOff( int p_iIndex, float p_fCut);
		/**
		 * Sets the ambient component of the current material
		 */
		virtual void SetMaterialAmbient( const Point4f & p_crColour);
		/**
		 * Sets the diffuse component of the current material
		 */
		virtual void SetMaterialDiffuse( const Point4f & p_crColour);
		/**
		 * Sets the specular component of the current material
		 */
		virtual void SetMaterialSpecular( const Point4f & p_crColour);
		/**
		 * Sets the emissive component of the current material
		 */
		virtual void SetMaterialEmissive( const Point4f & p_crColour);
		/**
		 * Sets the shininess of the current material
		 */
		virtual void SetMaterialShininess( float p_fShine);

		/**
		 * Resets compilation variables to be able to compile again
		 */
		void ResetToCompile();
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables();
		/**
		 * Creates a frame variable of wanted type
		 */
		template <class Ty> shared_ptr<Ty> CreateFrameVariable( int p_iNbOcc, ShaderObjectPtr p_pObject);
		/**
		 * Creates a frame variable of wanted type
		 */
		template <class Ty> shared_ptr<Ty> CreateFrameVariable( int p_iNbOcc, eSHADER_TYPE p_eTarget);
		/**
		 * Retrieves Vertex Attribute from the program
		 */
		virtual int GetAttributeLocation( String const & p_strName)const=0;

		/**@name Accessors */
		//@{
		inline void 	Enable				()				{ m_bEnabled = true; }
		inline void 	Disable				()				{ m_bEnabled = false; }

		inline bool									IsLinked				()const { return m_bLinked; }
		inline eSHADER_LANGUAGE						GetType					()const	{ return m_eType; }
		FrameVariablePtrList::const_iterator		GetFrameVariablesBegin	()const { return m_listFrameVariables.begin(); }
		FrameVariablePtrList::iterator				GetFrameVariablesBegin	()		{ return m_listFrameVariables.begin(); }
		FrameVariablePtrList::const_iterator		GetFrameVariablesEnd	()const	{ return m_listFrameVariables.end(); }
		//@}

	protected:
		virtual void _addFrameVariable( FrameVariablePtr p_pVariable, ShaderObjectPtr p_pObject);
		virtual ShaderObjectPtr _createObject( eSHADER_TYPE p_eType)=0;
		virtual shared_ptr<OneIntFrameVariable>			_create1IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point2iFrameVariable>		_create2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point3iFrameVariable>		_create3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point4iFrameVariable>		_create4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<OneUIntFrameVariable>		_create1UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point2uiFrameVariable>		_create2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point3uiFrameVariable>		_create3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point4uiFrameVariable>		_create4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<OneFloatFrameVariable>		_create1FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point2fFrameVariable>		_create2FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point3fFrameVariable>		_create3FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point4fFrameVariable>		_create4FloatVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<OneDoubleFrameVariable>		_create1DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point2dFrameVariable>		_create2DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point3dFrameVariable>		_create3DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Point4dFrameVariable>		_create4DoubleVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x2iFrameVariable>		_create2x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x3iFrameVariable>		_create2x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x4iFrameVariable>		_create2x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x2iFrameVariable>		_create3x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x3iFrameVariable>		_create3x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x4iFrameVariable>		_create3x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x2iFrameVariable>		_create4x2IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x3iFrameVariable>		_create4x3IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x4iFrameVariable>		_create4x4IntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x2uiFrameVariable>	_create2x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x3uiFrameVariable>	_create2x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x4uiFrameVariable>	_create2x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x2uiFrameVariable>	_create3x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x3uiFrameVariable>	_create3x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x4uiFrameVariable>	_create3x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x2uiFrameVariable>	_create4x2UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x3uiFrameVariable>	_create4x3UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x4uiFrameVariable>	_create4x4UIntVariable(		int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x2fFrameVariable>		_create2x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x3fFrameVariable>		_create2x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x4fFrameVariable>		_create2x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x2fFrameVariable>		_create3x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x3fFrameVariable>		_create3x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x4fFrameVariable>		_create3x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x2fFrameVariable>		_create4x2FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x3fFrameVariable>		_create4x3FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x4fFrameVariable>		_create4x4FloatVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x2dFrameVariable>		_create2x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x3dFrameVariable>		_create2x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix2x4dFrameVariable>		_create2x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x2dFrameVariable>		_create3x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x3dFrameVariable>		_create3x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix3x4dFrameVariable>		_create3x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x2dFrameVariable>		_create4x2DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x3dFrameVariable>		_create4x3DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
		virtual shared_ptr<Matrix4x4dFrameVariable>		_create4x4DoubleVariable(	int p_iNbOcc, ShaderObjectPtr p_pObject)=0;
	};

	//! GLSL Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API GlslShaderProgram : public ShaderProgramBase, public MemoryTraced<GlslShaderProgram>
	{
	protected:
		friend class ShaderManager;

	public:
		/**
		 * Constructor
		 *@param p_shaderFiles : [in] The shader's files
		 */
		GlslShaderProgram( const StringArray & p_shaderFiles);
		GlslShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~GlslShaderProgram();
	};
	//! HLSL Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API HlslShaderProgram : public ShaderProgramBase, public MemoryTraced<HlslShaderProgram>
	{
	protected:
		friend class ShaderManager;

	public:
		/**
		 * Constructor
		 *@param p_shaderFiles : [in] The shader's files
		 */
		HlslShaderProgram( const StringArray & p_shaderFiles);
		HlslShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~HlslShaderProgram();
	};
	//! Cg Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderProgram : public ShaderProgramBase
	{
	public:
		/**
		 * Constructor
		 *@param p_shaderFiles : [in] The shader's files
		 */
		CgShaderProgram( const StringArray & p_shaderFiles);
		CgShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~CgShaderProgram();
	};

#	include "ShaderProgram.inl"
}

#endif
