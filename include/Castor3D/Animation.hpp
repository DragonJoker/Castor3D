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
#ifndef ___C3D_Animation___
#define ___C3D_Animation___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! Animation manager class C3D_API
	/*!
	The class C3D_API which handles an Animation, its name, length, key frames ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated
	\todo Write and Read functions.
	*/
	class C3D_API Animation : public Serialisable, public MemoryTraced<Animation>
	{
	public:
		//! Animation State Enum
		/*!
		The enumeration which defines all the states of an animation : playing, stopped, paused
		*/
		typedef enum
		{	eSTATE_PLAYING	//!< Playing animation state
		,	eSTATE_STOPPED	//!< Stopped animation state
		,	eSTATE_PAUSED	//!< Paused animation state
		,	eSTATE_COUNT
		}	eSTATE;

	protected:
		KeyFramePtrRealMap m_mapKeyFrames;				//!< The key frames
		String m_strName;								//!< The animation name
		real m_rLength;									//!< The animation length
		KeyFramePtrRealMap::iterator m_itLastKeyFrame;	//!< The iterator on the last met key frame
		real m_rCurrentTime;							//!< The current playing time
		eSTATE m_eState;								//!< The current state of the animation
		real m_rWeight;									//!< The animation weight
		real m_rScale;									//!< The animation scale
		bool m_bLooped;									//!< Tells whether or not the animation is looped
		Scene * m_pScene;
		mutable KeyFramePtrArray m_arrayKeyFrames;		//!< key frames array, mutable because used only for serialisation

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		Animation();
		/**
		 * Constructor, initialises the animation with the name given in parameter
		 *@param p_strName : [in] The name of the animation
		 */
		Animation( Scene * p_pScene, String const & p_strName=cuEmptyString);
		/**
		 * Destructor, deletes all the key frames
		 */
		~Animation();
		//@}

		/**
		 * Creates a key frame and adds it to the list
		 *@param p_rFrom : [in] The starting time
		 *@param p_rTo : [in] The ending time
		 */
		KeyFramePtr AddKeyFrame( real p_rFrom, real p_rTo);
		/**
		 * Creates a key frame and adds it to the list
		 *@param p_rFrom : [in] The starting time
		 *@param p_rTo : [in] The ending time
		 */
		KeyFramePtr AddKeyFrame( const KeyFrame & p_keyFrame);
		/**
		 * Deletes the key frame at time index p_time
		 *@param p_rTime : [in] The time index
		 */
		void RemoveKeyFrame( real p_rTime);
		/**
		 * Updates the animation, updates the key frame at the good time index
		 *@param p_rTslf : [in] The time since the last frame
		 */
		void Update( real p_rTslf);
		/**
		 * Plays the animation
		 */
		void Play();
		/**
		 * Pauses the animation
		 */
		void Pause();
		/**
		 * Stops the animation
		 */
		void Stop();

		/**@name Accessors */
		//@{
		inline String const &						GetName				()const	{ return m_strName; }
		inline real									GetLength			()const	{ return m_rLength; }
		inline eSTATE								GetState			()const	{ return m_eState; }
		inline real									GetScale			()const	{ return m_rScale; }
		inline real									GetWeight			()const	{ return m_rWeight; }
		inline bool									IsLooped			()const	{ return m_bLooped; }
		inline Scene *								GetScene			()const	{ return m_pScene; }
		inline size_t								GetKeyFramesCount	()const	{ return m_mapKeyFrames.size(); }
		inline KeyFramePtrRealMap::iterator			Begin				()		{ return m_mapKeyFrames.begin(); }
		inline KeyFramePtrRealMap::const_iterator	Begin				()const	{ return m_mapKeyFrames.begin(); }
		inline KeyFramePtrRealMap::const_iterator	End					()const	{ return m_mapKeyFrames.end(); }

		inline void	SetLength	( real p_rLength)	{ m_rLength = p_rLength; }
		inline void	SetScale	( real p_rScale)	{ m_rScale = p_rScale; }
		inline void	SetWeight	( real p_rWeight)	{ m_rWeight = p_rWeight; }
		inline void SetLooped	( bool p_bLooped)	{ m_bLooped = p_bLooped; }
		//@}

		DECLARE_SERIALISE_MAP()
		DECLARE_PRE_SERIALISE()
		DECLARE_POST_SERIALISE()
		DECLARE_PRE_UNSERIALISE()
		DECLARE_POST_UNSERIALISE()
	};
}

#endif
