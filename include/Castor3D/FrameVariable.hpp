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
#ifndef ___C3D_FrameVariable___
#define ___C3D_FrameVariable___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! The different supported variable counts
	/*!
	Can be 1, 2, 3, 4 variables, 2, 3, 4 dimensions matrices
	*/
	typedef enum
	{	eVARIABLE_TYPE_ONE		//!< One variable
	,	eVARIABLE_TYPE_VEC1		//!< 1 variable array
	,	eVARIABLE_TYPE_VEC2		//!< 2 variables array
	,	eVARIABLE_TYPE_VEC3		//!< 3 variables array
	,	eVARIABLE_TYPE_VEC4		//!< 4 variables array
	,	eVARIABLE_TYPE_MAT1		//!< 1x1 matrix
	,	eVARIABLE_TYPE_MAT2X2	//!< 2x2 matrix
	,	eVARIABLE_TYPE_MAT2X3	//!< 2x3 matrix
	,	eVARIABLE_TYPE_MAT2X4	//!< 2x4 matrix
	,	eVARIABLE_TYPE_MAT3X2	//!< 3x2 matrix
	,	eVARIABLE_TYPE_MAT3X3	//!< 3x3 matrix
	,	eVARIABLE_TYPE_MAT3X4	//!< 3x4 matrix
	,	eVARIABLE_TYPE_MAT4X2	//!< 4x2 matrix
	,	eVARIABLE_TYPE_MAT4X3	//!< 4x3 matrix
	,	eVARIABLE_TYPE_MAT4X4	//!< 4x4 matrix
	,	eVARIABLE_TYPE_COUNT
	}	eVARIABLE_TYPE;
	//! Shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class C3D_API FrameVariable
	{
	public:

	protected:
		String m_strName;		//!< The variable name as it appears in the shader program
		bool m_bChanged;
		size_t m_uiOcc;
		StringArray m_strValue;	//!< The value of the variable,
		ShaderObjectBase * m_pObject;
		ShaderProgramBase * m_pProgram;

	public:
		/**
		 * Constructor
		 */
		FrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc=1)
			:	m_bChanged( true)
			,	m_uiOcc( p_uiOcc)
			,	m_pObject( p_pObject)
			,	m_pProgram( p_pProgram)
		{
			m_strValue.resize( m_uiOcc);
		}
		/**
		 * Copy constructor
		 */
		FrameVariable( const FrameVariable & p_rVariable)
			:	m_strName( p_rVariable.m_strName)
			,	m_uiOcc( p_rVariable.m_uiOcc)
			,	m_pObject( p_rVariable.m_pObject)
			,	m_pProgram( p_rVariable.m_pProgram)
		{
			m_strValue.resize( m_uiOcc);
		}
		/**
		 * Destructor
		 */
		virtual ~FrameVariable()
		{
		}
		/**
		 * Assignes and activate the frame variable
		 */
		virtual void Apply()=0;
		/**@name Accessors */
		//@{
		/**
		 * Gives the count type of the variable
		 *@return The count type
		 */
		virtual eVARIABLE_TYPE GetType()const=0;
		/**
		 * Gives the type of the variable (essentially bool, int or real)
		 *@return The type of the variable
		 */
		virtual String GetSubType()=0;
		/**
		 * Binds this variable to it's program or object, depending of implementation (Glsl, Hlsl or Cg)
		 */
		virtual void Bind()=0;
		/**
		 * Gives the name of the variable, as it appears in the shader program
		 *@return The variable name
		 */
		inline String GetName()const { return m_strName; }
		/**
		 * Defines the name of the variable, as it appears in the shader program
		 *@param p_strName : [in] The variable name
		 */
		virtual void SetName( String const & p_strName) { m_strName = p_strName; }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_uiIndex : [in] The index of the value
		 *@param p_strValue : [in] The string containing the value
		 */
		virtual void SetValue( String const & p_strValue, size_t p_uiIndex=0)=0;

		inline void SetStrValue	( String const & p_strValue, size_t p_uiIndex=0)	{ CASTOR_ASSERT( p_uiIndex < m_uiOcc);m_strValue[p_uiIndex] = p_strValue;m_bChanged = true; }
		inline void SetChanged	( bool p_bVal=true)									{ m_bChanged = p_bVal; }

		inline ShaderProgramBase *	GetProgram	()const						{ return m_pProgram; }
		inline String				GetStrValue	( size_t p_uiIndex=0)const	{ CASTOR_ASSERT( p_uiIndex < m_uiOcc);return m_strValue[p_uiIndex]; }
		//@}
	};

	template <size_t Rows, size_t Columns> struct MatrixVariableTyper
	{
		enum
		{
			Value = eVARIABLE_TYPE_MAT1
		};
	};

	template <size_t Count> struct PointVariableTyper
	{
		enum
		{
			Value = eVARIABLE_TYPE_VEC1
		};
	};

	//! Variable type shader variable representation
	/*!
	This is a variable which is given to a shader program during it's execution.
	It has a variable type, so it can be derived in int, bool or real (essentially)
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class TFrameVariable : public FrameVariable
	{
	protected:
		typedef Policy<T> policy;

	public:
		/**
		 * Constructor
		 */
		TFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		template <typename Ty>
		TFrameVariable( const TFrameVariable<Ty> & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~TFrameVariable();
		/**@name Accessors */
		//@{
		/**
		 *@return The type of the variable (int, bool, real)
		 */
		inline String	GetSubType()	{ return typeid( T).name(); }
		inline void SetStrValue	( String const & p_strValue, size_t p_uiIndex=0)	{ FrameVariable::SetStrValue( p_strValue, p_uiIndex); }
		inline void SetChanged	( bool p_bVal=true)									{ FrameVariable::SetChanged( p_bVal); }
		//@}
	};

	//! Single variable typed variable
	/*!
	This is a single variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class OneFrameVariable : public TFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_tValue;	//!< The single value of the variable

	public:
		/**
		 * Constructor
		 */
		OneFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		template <typename Ty>
		OneFrameVariable( const OneFrameVariable<Ty> & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~OneFrameVariable();
		/**@name Accessors */
		//@{
		inline T &					operator []	( size_t p_uiIndex)			{ return m_tValue[p_uiIndex]; }
		inline T const &			operator []	( size_t p_uiIndex)const	{ return m_tValue[p_uiIndex]; }
		inline T &					GetValue	( size_t p_uiIndex=0)		{ return m_tValue[p_uiIndex]; }
		inline T const &			GetValue	( size_t p_uiIndex=0)const	{ return m_tValue[p_uiIndex]; }
		inline eVARIABLE_TYPE	GetType		()const						{ return eVARIABLE_TYPE_ONE; }
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( String const & p_strValue, size_t p_uiIndex=0);
		/**
		 * Defines the value of the variable, from a single value
		 *@param p_tValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( T const & p_tValue, size_t p_uiIndex=0);
		//@}
	};

	//! Array of N values variable typed variable
	/*!
	This is an array of N values variable with a variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Count>
	class PointFrameVariable : public TFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_pValues;
		Point<T, Count> * m_ptValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		PointFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		template <typename Ty>
		PointFrameVariable( const PointFrameVariable<Ty, Count> & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~PointFrameVariable();
		/**@name Accessors */
		//@{
		inline Point<T, Count> &		operator []	( size_t p_uiIndex)			{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> const &	operator []	( size_t p_uiIndex)const	{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> &		GetValue	( size_t p_uiIndex=0)		{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> const &	GetValue	( size_t p_uiIndex=0)const	{ return m_ptValue[p_uiIndex]; }
		inline eVARIABLE_TYPE		GetType		()const;
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( String const & p_strValue, size_t p_uiIndex=0);
		/**
		 * Defines the value of the variable, from a Point2D
		 *@param p_ptValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( Point<T, Count> const & p_ptValue, size_t p_uiIndex=0);
		//@}
	};

	//! N dimensions matrix of variable type variable
	/*!
	This is a N dimensions matrix variable with variable type
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T, size_t Rows, size_t Columns>
	class MatrixFrameVariable : public TFrameVariable<T>
	{
	protected:
		typedef Policy<T> policy;

	public:
		T * m_pValues;
		Matrix <T, Rows, Columns> * m_mtxValue;	//!< The value of the variable

	public:
		/**
		 * Constructor
		 */
		MatrixFrameVariable( ShaderProgramBase * p_pProgram, ShaderObjectBase * p_pObject, size_t p_uiOcc=1);
		/**
		 * Copy constructor
		 */
		template <typename Ty>
		MatrixFrameVariable( const MatrixFrameVariable<Ty, Rows, Columns> & p_rVariable);
		/**
		 * Destructor
		 */
		virtual ~MatrixFrameVariable();
		/**@name Accessors */
		//@{
		inline Matrix <T, Rows, Rows> &			operator []	( size_t p_uiIndex)			{ return m_mtxValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	operator []	( size_t p_uiIndex)const	{ return m_mtxValue[p_uiIndex]; }
		inline Matrix <T, Rows, Rows> &			GetValue	( size_t p_uiIndex=0)		{ return m_mtxValue[p_uiIndex]; }
		inline const Matrix <T, Rows, Rows> &	GetValue	( size_t p_uiIndex=0)const	{ return m_mtxValue[p_uiIndex]; }
		inline eVARIABLE_TYPE				GetType		()const;
		/**
		 * Defines the value of the variable, from a string
		 *@param p_strValue : [in] The string containing the value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( String const & p_strValue, size_t p_uiIndex=0);
		/**
		 * Defines the value of the variable, from a Matrix2
		 *@param p_mValue : [in] The new value
		 *@param p_uiIndex : [in] The index of the value
		 */
		inline void SetValue( Matrix<T, Rows, Columns> const & p_mtxValue, size_t p_uiIndex=0);
		//@}
	};

#include "FrameVariable.inl"
}

#endif
