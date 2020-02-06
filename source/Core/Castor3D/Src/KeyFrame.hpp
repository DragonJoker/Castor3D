﻿/*
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
#ifndef ___C3D_KEY_FRAME_H___
#define ___C3D_KEY_FRAME_H___

#include "Castor3DPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	template< typename T > class C3D_API KeyFrame
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_rTimeIndex	When the key frame starts
		 *\param[in]	p_tValue		The wanted value
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_rTimeIndex	Quand la key frame commence
		 *\param[in]	p_tValue		La valeur voulue
		 */
		KeyFrame( real p_rTimeIndex = 0, T const & p_tValue = T() )
			:	m_rTimeIndex( p_rTimeIndex )
			,	m_tValue( p_tValue )
		{
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
		 *\brief		Defines the wanted translation
		 *\param[in]	p_ptTranslate	The translation
		 *\~french
		 *\brief		Définit la translation voulue
		 *\param[in]	p_ptTranslate	La translation
		 */
		inline void SetValue( T const & p_tValue )
		{
			m_tValue = p_tValue;
		}
		/**
		 *\~english
		 *\brief		Retrieves the wanted translation
		 *\return		The translation
		 *\~french
		 *\brief		Récupère la translation voulue
		 *\return		La translation
		 */
		inline T const & GetValue()const
		{
			return m_tValue;
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
			return m_rTimeIndex;
		}
		/**
		 *\~english
		 *\brief		Defines the start time index
		 *\param[in]	p_rValue	The time index
		 *\~french
		 *\brief		Définit le temps de départ
		 *\param[in]	p_rValue	Le temps
		 */
		inline void SetTimeIndex( real p_rValue )
		{
			m_rTimeIndex = p_rValue;
		}

	protected:
		//!\~english The start time index	\~french L'index de temps de début
		real m_rTimeIndex;
		//!\~english The value at start time	\~french La valeur à l'index de temps de début
		T m_tValue;
	};
}

#pragma warning( pop )

#endif
