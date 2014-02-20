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
#ifndef ___Gl_PrecompiledHeader___
#define ___Gl_PrecompiledHeader___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include "Module_GlRender.hpp"

#define C3DGL_LIMIT_TO_2_1	0
#define C3DGL_USE_TBO		0

namespace GlRender
{
	namespace GLSL
	{
		class KeywordsBase
		{
		protected:
			Castor::String m_strVersion;
			Castor::String m_strAttribute;
			Castor::String m_strIn;
			Castor::String m_strOut;
			Castor::String m_strPixelOut;
			Castor::String m_strPixelOutputName;
			Castor::String m_strGSOutPositionName;
			Castor::String m_strGSOutNormalName;
			Castor::String m_strGSOutTangentName;
			Castor::String m_strGSOutBitangentName;
			Castor::String m_strGSOutDiffuseName;
			Castor::String m_strGSOutSpecularName;
			Castor::String m_strGSOutPositionDecl;
			Castor::String m_strGSOutNormalDecl;
			Castor::String m_strGSOutTangentDecl;
			Castor::String m_strGSOutBitangentDecl;
			Castor::String m_strGSOutDiffuseDecl;
			Castor::String m_strGSOutSpecularDecl;

		public:
			inline Castor::String const & GetVersion()const { return m_strVersion; }
			inline Castor::String const & GetIn()const { return m_strIn; }
			inline Castor::String const & GetOut()const { return m_strOut; }
			inline Castor::String const & GetPixelOut()const { return m_strPixelOut; }
			inline Castor::String const & GetPixelOutputName()const { return m_strPixelOutputName; }
			inline Castor::String const & GetGSOutPositionName()const { return m_strGSOutPositionName; }
			inline Castor::String const & GetGSOutNormalName()const { return m_strGSOutNormalName; }
			inline Castor::String const & GetGSOutTangentName()const { return m_strGSOutTangentName; }
			inline Castor::String const & GetGSOutBitangentName()const { return m_strGSOutBitangentName; }
			inline Castor::String const & GetGSOutDiffuseName()const { return m_strGSOutDiffuseName; }
			inline Castor::String const & GetGSOutSpecularName()const { return m_strGSOutSpecularName; }
			inline Castor::String const & GetGSOutPositionDecl()const { return m_strGSOutPositionDecl; }
			inline Castor::String const & GetGSOutNormalDecl()const { return m_strGSOutNormalDecl; }
			inline Castor::String const & GetGSOutTangentDecl()const { return m_strGSOutTangentDecl; }
			inline Castor::String const & GetGSOutBitangentDecl()const { return m_strGSOutBitangentDecl; }
			inline Castor::String const & GetGSOutDiffuseDecl()const { return m_strGSOutDiffuseDecl; }
			inline Castor::String const & GetGSOutSpecularDecl()const { return m_strGSOutSpecularDecl; }
			virtual Castor::String GetLayout( uint32_t p_uiIndex )const=0;
			virtual Castor::String GetAttribute( uint32_t p_uiIndex )const { return GetLayout( p_uiIndex ) + m_strAttribute; }
		};

		template< int Version, class Enable=void > class Keywords;

		template< int Version > class Keywords< Version, typename std::enable_if< (Version <= 120) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strAttribute			= cuT( "attribute" );
				m_strIn					= cuT( "varying" );
				m_strOut				= cuT( "varying" );
				m_strPixelOutputName	= cuT( "gl_FragColor" );
				m_strGSOutPositionName	= cuT( "gl_FragData[0]" );
				m_strGSOutNormalName	= cuT( "gl_FragData[1]" );
				m_strGSOutTangentName	= cuT( "gl_FragData[2]" );
				m_strGSOutBitangentName	= cuT( "gl_FragData[3]" );
				m_strGSOutDiffuseName	= cuT( "gl_FragData[4]" );
				m_strGSOutSpecularName	= cuT( "gl_FragData[5]" );
			}

			virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_uiIndex ) )const { return cuT( "" ); }
		};

		template< int Version > class Keywords< Version, typename std::enable_if< (Version > 120) && (Version < 330) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strVersion			= cuT( "#version " ) + Castor::str_utils::to_string( Version ) + cuT( "\n" );
				m_strAttribute			= cuT( "in" );
				m_strIn					= cuT( "in" );
				m_strOut				= cuT( "out" );
				m_strPixelOut			= cuT( "out	vec4	pxl_v4FragColor;\n" );
				m_strPixelOutputName	= cuT( "pxl_v4FragColor" );
				m_strGSOutPositionName	= cuT( "gl_FragData[0]" );
				m_strGSOutDiffuseName	= cuT( "gl_FragData[1]" );
				m_strGSOutNormalName	= cuT( "gl_FragData[2]" );
				m_strGSOutTangentName	= cuT( "gl_FragData[3]" );
				m_strGSOutBitangentName	= cuT( "gl_FragData[4]" );
				m_strGSOutSpecularName	= cuT( "gl_FragData[5]" );
			}

			virtual Castor::String GetLayout( uint32_t CU_PARAM_UNUSED( p_uiIndex ) )const { return cuT( "" ); }
		};

		template< int Version > class Keywords< Version, typename std::enable_if< (Version >= 330) >::type > : public KeywordsBase
		{
		public:
			Keywords()
			{
				m_strVersion			= cuT( "#version " ) + Castor::str_utils::to_string( Version ) + cuT( "\n" );
				m_strAttribute			= cuT( "in" );
				m_strIn					= cuT( "in" );
				m_strOut				= cuT( "out" );
				m_strPixelOut			= cuT( "out	vec4	pxl_v4FragColor;\n" );
				m_strPixelOutputName	= cuT( "pxl_v4FragColor" );
				m_strGSOutPositionName	= cuT( "out_c3dPosition" );
				m_strGSOutNormalName	= cuT( "out_c3dNormals" );
				m_strGSOutTangentName	= cuT( "out_c3dTangent" );
				m_strGSOutBitangentName	= cuT( "out_c3dBitangent" );
				m_strGSOutDiffuseName	= cuT( "out_c3dDiffuse" );
				m_strGSOutSpecularName	= cuT( "out_c3dSpecular" );
				m_strGSOutPositionDecl	= GetLayout( 0 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutPositionName	+ cuT( ";\n" );
				m_strGSOutDiffuseDecl	= GetLayout( 1 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutDiffuseName	+ cuT( ";\n" );
				m_strGSOutNormalDecl	= GetLayout( 2 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutNormalName	+ cuT( ";\n" );
				m_strGSOutTangentDecl	= GetLayout( 3 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutTangentName	+ cuT( ";\n" );
				m_strGSOutBitangentDecl	= GetLayout( 4 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutBitangentName	+ cuT( ";\n" );
				m_strGSOutSpecularDecl	= GetLayout( 5 ) + m_strOut + cuT( " vec4 " ) + m_strGSOutSpecularName	+ cuT( ";\n" );
			}

			virtual Castor::String GetLayout( uint32_t p_uiIndex )const { return cuT( "layout( location=" ) + Castor::str_utils::to_string( p_uiIndex ) + cuT( " ) " ); }
		};

		class ConstantsBase
		{
		public:
			virtual Castor::String Matrices()=0;
			virtual Castor::String Scene()=0;
			virtual Castor::String Pass()=0;
			static void Replace( Castor::String & p_strSource );
		};

		class ConstantsStd : public ConstantsBase
		{
		public:
			virtual Castor::String Matrices();
			virtual Castor::String Scene();
			virtual Castor::String Pass();
		};

		class ConstantsUbo : public ConstantsBase
		{
		public:
			virtual Castor::String Matrices();
			virtual Castor::String Scene();
			virtual Castor::String Pass();
		};

		static ConstantsStd	constantsStd;
		static ConstantsUbo	constantsUbo;
	}
}

#pragma warning( pop )

#endif
