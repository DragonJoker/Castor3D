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
#ifndef ___C3D_KEY_FRAME_H___
#define ___C3D_KEY_FRAME_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		The class which manages key frames
	\remark		Key frames are the frames where the animation must be at a precise state
	\~french
	\brief		Classe qui gère une key frame
	\remark		Les key frames sont les frames auxquelles une animation est dans un état précis
	*/
	class KeyFrame
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_timeIndex	When the key frame starts.
		 *\param[in]	p_translate	The translation at start time.
		 *\param[in]	p_rotate	The rotation at start time.
		 *\param[in]	p_scale		The scaling at start time.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_timeIndex	Quand la key frame commence.
		 *\param[in]	p_translate	La translation au temps de début.
		 *\param[in]	p_rotate	La rotation au temps de début.
		 *\param[in]	p_scale		L'échelle au temps de début.
		 */
		KeyFrame( real p_timeIndex = 0, Castor::Point3r const & p_translate = {}, Castor::Quaternion const & p_rotate = {}, Castor::Point3r const & p_scale = {} )
			: m_timeIndex( p_timeIndex )
			, m_translate( p_translate )
			, m_rotate( p_rotate )
			, m_scale( p_scale )
		{
			Castor::matrix::set_transform( m_transform, p_translate, p_scale, p_rotate );
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~KeyFrame()
		{
		}
		/**
		 *\~english
		 *\return		The transformation matrix.
		 *\~french
		 *\return		La matrice de transformation.
		 */
		inline Castor::Matrix4x4r const & GetTransform()const
		{
			return m_transform;
		}
		/**
		 *\~english
		 *\return		The translation.
		 *\~french
		 *\return		La translation.
		 */
		inline Castor::Point3r const & GetTranslate()const
		{
			return m_translate;
		}
		/**
		 *\~english
		 *\return		The rotation.
		 *\~french
		 *\return		La rotation.
		 */
		inline Castor::Quaternion const & GetRotate()const
		{
			return m_rotate;
		}
		/**
		 *\~english
		 *\return		The scaling.
		 *\~french
		 *\return		L'échelle.
		 */
		inline Castor::Point3r const & GetScale()const
		{
			return m_scale;
		}
		/**
		 *\~english
		 *\brief		Retrieves the start time index
		 *\return		The time index
		 *\~french
		 *\brief		Récupère le temps de départ
		 *\return		Le temps
		 */
		inline real GetTimeIndex()const
		{
			return m_timeIndex;
		}

	protected:
		//!\~english	The transformation matrix at start time.
		//!\~french		La matrice de transformation à l'index de temps de début.
		Castor::Matrix4x4r m_transform;
		//!\~english	The rotation at start time.
		//!\~french		La rotation à l'index de temps de début.
		Castor::Quaternion m_rotate;
		//!\~english	The translation at start time.
		//!\~french		La translation à l'index de temps de début.
		Castor::Point3r m_translate;
		//!\~english	The scaling at start time.
		//!\~french		L'échelle à l'index de temps de début.
		Castor::Point3r m_scale;
		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		real m_timeIndex;
	};
}

#endif

