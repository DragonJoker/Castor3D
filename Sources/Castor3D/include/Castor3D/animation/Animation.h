/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

namespace Castor3D
{
	//! Animation manager class
	/*!
	The class which handles an Animation, its name, length, key frames ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\todo Review all the animation system because it's not clear, not optimised, and not good enough to be validated
	\todo Write and Read functions.
	*/
	class CS3D_API Animation
	{
	public:
		//! Animation State Enum
		/*!
		The enumeration which defines all the states of an animation : playing, stopped, paused
		*/
		typedef enum
		{
			AnimationPlaying,	//!< Playing animation state
			AnimationStopped,	//!< Stopped animation state
			AnimationPaused		//!< Paused animation state

		} eSTATE;

	protected:
		KeyFramePtrRealMap m_keyFrames;					//!< The key frames
		String m_name;									//!< The animation name
		real m_length;									//!< The animation length
		KeyFramePtrRealMap::iterator m_lastKeyFrameIt;	//!< The iterator on the last met key frame
		real m_currentTime;								//!< The current playing time
		eSTATE m_state;									//!< The current state of the animation
		real m_weight;									//!< The animation weight
		real m_scale;									//!< The animation scale
		bool m_looped;									//!< Tells whether or not the animation is looped

	public:
		/**
		 * Constructor, initialises the animation with the name given in parameter
		 *@param p_name : [in] The name of the animation
		 */
		Animation( const String & p_name);
		/**
		 * Destructor, deletes all the key frames
		 */
		~Animation();
		/**
		 * Creates a key frame and adds it to the list
		 *@param p_from : [in] The starting time
		 *@param p_to : [in] The ending time
		 */
		KeyFramePtr AddKeyFrame( real p_from, real p_to);
		/**
		 * Deletes the key frame at time index p_time
		 *@param p_time : [in] The time index
		 */
		void RemoveKeyFrame( real p_time);
		/**
		 * Writes the animation in a file
		 *@param p_file : [in] The file to write in
		 */
		bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the animation from a file
		 @param p_file : [in] The file to read from
		 */
		bool Read( Castor::Utils::File & p_file);
		/**
		 * Updates the animation, updates the key frame at the good time index
		 */
		void Update( real p_tslf);
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

	public:
		/**@name Accessors */
		//@{
		/**
		 * Returns the animation name
		 *@return the animation name
		 */
		inline const String &	GetName		()const { return m_name; }
		/**
		 * Returns the animation length
		 *@return the animation length
		 */
		inline real				GetLength	()const { return m_length; }
		/**
		 * Returns the animation state
		 *@return the animation state
		 */
		inline eSTATE			GetState	()const { return m_state; }
		/**
		 * Returns the animation scale
		 *@return the animation scale
		 */
		inline real				GetScale	()const { return m_scale; }
		/**
		 * Returns the animation weight
		 *@return the animation weight
		 */
		inline real				GetWeight	()const { return m_weight; }
		/**
		 * Returns whether or not the animation is looped
		 *@return whether or not the animation is looped
		 */
		inline bool				IsLooped	()const { return m_looped; }
		/**
		 * Sets the animation scale
		 *@param p_scale : [in] The new animation scale
		 */
		inline void	SetScale	( real p_scale)		{ m_scale = p_scale; }
		/**
		 * Sets the animation weight
		 *@param p_weight : [in] The new animation weight
		 */
		inline void	SetWeight	( real p_weight)	{ m_weight = p_weight; }
		/**
		 * Sets the animation loop status
		 *@param p_looped : [in] The new animation loop status
		 */
		inline void SetLooped	( bool p_looped)	{ m_looped = p_looped; }
		//@}
	};
}

#endif

