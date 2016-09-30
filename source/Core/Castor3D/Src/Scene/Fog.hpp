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
#ifndef ___C3D_FOG_H___
#define ___C3D_FOG_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	enum class FogType
	{
		Disabled,
		Linear,
		Exponential,
		SquaredExponential,
		Coloured,
		Ground,
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		Fog implementation.
	\~french
	\brief		Implémentation de brouillard.
	*/
	class Fog
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API Fog();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~Fog();
		/**
		 *\~english
		 *\brief		Fills the frame variable buffer with the fog's parameters.
		 *\param[in]	p_camera	The scene's camera.
		 *\~french
		 *\brief		Remplit le tampon de variables shader avec les paramètres du brouillard.
		 *\param[in]	p_camera	La caméra de la scène.
		 */
		C3D_API void FillShader( FrameVariableBuffer const & p_buffer )const;
		/**
		 *\~english
		 *\return		The fog's density.
		 *\~french
		 *\return		La densité du brouillard.
		 */
		inline real GetDensity()const
		{
			return m_density;
		}
		/**
		 *\~english
		 *\brief		Sets the fog's density.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la densité du brouillard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetDensity( real p_value )
		{
			m_density = p_value;
		}
		/**
		 *\~english
		 *\return		The fog type.
		 *\~french
		 *\return		Le type de brouillard.
		 */
		inline FogType GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the fog type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le type de brouillard.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetType( FogType p_value )
		{
			m_type = p_value;
		}

	private:
		//!\~english	The fog type.
		//!\~french		Le type de brouillard.
		FogType m_type{ FogType::Disabled };
		//!\~english	The fog's density.
		//!\~french		La densité du brouillard.
		real m_density{ 0.0_r };
	};
}

#endif
