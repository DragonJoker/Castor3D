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
#ifndef ___C3D_SpecularGlossinessPassBuffer_H___
#define ___C3D_SpecularGlossinessPassBuffer_H___

#include "Shader/PassBuffer/PassBuffer.hpp"

#include <Design/ArrayView.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		SSBO holding the SpecularGlossinessPbrPasses data.
	\~french
	\brief		SSBO contenant les données des SpecularGlossinessPbrPass.
	*/
	class SpecularGlossinessPassBuffer
		: public PassBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API SpecularGlossinessPassBuffer( Engine & engine
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SpecularGlossinessPassBuffer();
		/**
		 *\copydoc		castor3d::PassBuffer::Visit
		 */
		C3D_API void visit( LegacyPass const & data )override;
		/**
		 *\copydoc		castor3d::PassBuffer::Visit
		 */
		C3D_API void visit( SpecularGlossinessPbrPass const & pass )override;

	public:

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		struct PassesData
		{
			castor::ArrayView< RgbaColour > diffDiv;
			castor::ArrayView< RgbaColour > specGloss;
			castor::ArrayView< RgbaColour > common;
			castor::ArrayView< RgbaColour > reflRefr;
		};

#else

		struct PassData
		{
			RgbaColour diffDiv;
			RgbaColour specGloss;
			RgbaColour common;
			RgbaColour reflRefr;
		};
		using PassesData = castor::ArrayView< PassData >;

#endif

	private:
		static constexpr uint32_t DataSize = ( sizeof( RgbColour ) * 2 )
			+ ( sizeof( float ) * 7 )
			+ ( sizeof( int ) * 2 );

	private:
		//!\~english	The specular/glossiness PBR passes data.
		//!\~french		Les données des passes PBR specular/glossiness.
		PassesData m_data;
	};
}

#endif
