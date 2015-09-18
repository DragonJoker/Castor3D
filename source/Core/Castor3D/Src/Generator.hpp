/*
This source file is part of ProceduralGenerator (https://sourceforge.net/projects/proceduralgene/)

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
#ifndef ___C3D_GENERATOR_H___
#define ___C3D_GENERATOR_H___

#include "Castor3DPrerequisites.hpp"

#include <Pixel.hpp>
#include <PixelBuffer.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	class C3D_API Generator
	{
	protected:
		class C3D_API Thread
		{
		private:
			bool			m_bEnded;
			bool			m_bLaunched;
			bool			m_bStopped;

		protected:
			Generator				*			m_pParent;
			int									m_iWidth;
			int									m_iTop;
			int									m_iBottom;
			int									m_iHeight;
			Castor::UbPixel						m_pxColour;
			uint32_t							m_uiIndex;
			std::shared_ptr< std::thread >		m_pThread;
			mutable std::recursive_mutex		m_mutex;

		public:
			Thread( Generator * p_parent, uint32_t p_uiIndex, int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour );
			virtual ~Thread();

			void Run();
			void Wait();
			virtual int Entry();

			virtual void Step() = 0;

			inline bool	IsEnded()const
			{
				return m_bEnded;
			}
			inline bool IsStopped()const
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				return m_bStopped;
			}

			inline void Relaunch()
			{
				m_bLaunched = true;
			}
			inline void Stop()
			{
				CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
				m_bStopped = true;
			}
			inline void SetRed( uint8_t val )
			{
				m_pxColour[0] = val;
			}
			inline void SetGreen( uint8_t val )
			{
				m_pxColour[1] = val;
			}
			inline void SetBlue( uint8_t val )
			{
				m_pxColour[2] = val;
			}

		private:
			static uint32_t StEntry( Generator::Thread * p_pThis );
		};

	protected:
		int							m_iWidth;
		int							m_iHeight;
		unsigned long long			m_ullStep;
		Castor::PixelBuffer			m_frontBuffer;
		Castor::PixelBuffer			m_backBuffer;
		Castor::UbPixel				m_pxColour;
		uint32_t					m_uiThreadCount;
		bool						m_bEnded;
		std::vector <Thread *>		m_arraySlaveThreads;
		bool						m_bInitialised;
		TextureUnit			*		m_pTexture;
		Engine 		*			m_engine;

	public:
		Generator( Engine * p_engine, int p_width, int p_height );
		virtual ~Generator();

		virtual bool Step();
		virtual void SetRed( uint8_t val );
		virtual void SetGreen( uint8_t val );
		virtual void SetBlue( uint8_t val );
		virtual void SwapBuffers();
		virtual void InitialiseStep();
		virtual void ClearAllThreads();

		bool AllEnded();
		void Suspend();
		void SetSize( int p_iWidth, int p_iHeight );
		void SetSize( Castor::Point2i const & p_size );
		void SaveFrame();

		template <class ThreadClass>
		ThreadClass * CreateThread( int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour )
		{
			ThreadClass * l_pThread = new ThreadClass( this, m_arraySlaveThreads.size(), m_iWidth, iTop, iBottom, iTotalHeight, p_pxColour );
			m_arraySlaveThreads.push_back( l_pThread );
			return l_pThread;
		}

		inline uint8_t				GetRed()const
		{
			return m_pxColour[0];
		}
		inline uint8_t				GetGreen()const
		{
			return m_pxColour[1];
		}
		inline uint8_t				GetBlue()const
		{
			return m_pxColour[2];
		}
		inline bool							IsInitialised()const
		{
			return m_bInitialised;
		}
		inline Castor::PixelBuffer const *	GetPixelsBuffer()const
		{
			return & m_frontBuffer;
		}
		inline Castor::PixelBuffer 	*	GetPixelsBuffer()
		{
			return & m_frontBuffer;
		}
		inline int							GetWidth()const
		{
			return m_iWidth;
		}
		inline int							GetHeight()const
		{
			return m_iHeight;
		}

	protected:
		void DoCleanup();
		uint32_t DoGetThreadsCount()
		{
			return uint32_t( m_arraySlaveThreads.size() );
		}
		Castor::Point2i _loadImage( Castor::String const & p_strImagePath, Castor::Image & p_pImage );
		void _subRender();
		void _saveFrame();
	};

	extern int GetCPUCount();
}

#pragma warning( pop )

#endif
