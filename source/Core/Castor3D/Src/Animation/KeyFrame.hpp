/*
See LICENSE file in root folder
*/
#ifndef ___C3D_KEY_FRAME_H___
#define ___C3D_KEY_FRAME_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
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
		KeyFrame( castor::Milliseconds const & p_timeIndex = 0_ms
			, castor::Point3r const & p_translate = {}
			, castor::Quaternion const & p_rotate = {}
			, castor::Point3r const & p_scale = {} );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_timeIndex	When the key frame starts.
		 *\param[in]	p_transform	The transformation at start time.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_timeIndex	Quand la key frame commence.
		 *\param[in]	p_transform	La transformation au temps de début.
		 */
		KeyFrame( castor::Milliseconds const & p_timeIndex
			, castor::Matrix4x4r const & p_transform );
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
		inline castor::Matrix4x4r const & getTransform()const
		{
			return m_transform;
		}
		/**
		 *\~english
		 *\brief		Retrieves the start time index
		 *\return		The time index
		 *\~french
		 *\brief		Récupère le temps de départ
		 *\return		Le temps
		 */
		inline castor::Milliseconds const & getTimeIndex()const
		{
			return m_timeIndex;
		}

	protected:
		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		castor::Milliseconds m_timeIndex;
		//!\~english	The transformation matrix at start time.
		//!\~french		La matrice de transformation à l'index de temps de début.
		castor::Matrix4x4r m_transform{ 1.0_r };
	};
}

#endif

