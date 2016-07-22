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

#include <Graphics/Pixel.hpp>
#include <Graphics/PixelBuffer.hpp>

namespace Castor3D
{
	class Generator
	{
	protected:
		class Thread
		{
		private:
			bool m_bEnded{ true };
			bool m_bLaunched{ false };
			bool m_bStopped{ false };

		protected:
			Generator * m_parent{ nullptr };
			int m_iWidth{ 0 };
			int m_iTop{ 0 };
			int m_iBottom{ 0 };
			int m_iHeight{ 0 };
			Castor::UbPixel m_pxColour;
			uint32_t m_index{ 0u };
			std::shared_ptr< std::thread > m_pThread;
			mutable std::recursive_mutex m_mutex;

		public:
			C3D_API Thread( Generator * p_parent, uint32_t p_index, int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour );
			C3D_API virtual ~Thread();

			C3D_API void Run();
			C3D_API void Wait();
			C3D_API virtual int Entry();

			C3D_API virtual void Step() = 0;

			inline bool	IsEnded()const
			{
				return m_bEnded;
			}
			inline bool IsStopped()const
			{
				auto l_lock = Castor::make_unique_lock( m_mutex );
				return m_bStopped;
			}

			inline void Relaunch()
			{
				m_bLaunched = true;
			}
			inline void Stop()
			{
				auto l_lock = Castor::make_unique_lock( m_mutex );
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
		int m_iWidth{ 0 };
		int m_iHeight{ 0 };
		unsigned long long m_ullStep{ 0u };
		Castor::PixelBuffer m_frontBuffer;
		Castor::PixelBuffer m_backBuffer;
		Castor::UbPixel m_pxColour;
		uint32_t m_uiThreadCount{ 0u };
		bool m_bEnded{ true };
		std::vector <Thread *> m_arraySlaveThreads;
		bool m_initialised{ false };
		TextureUnit * m_pTexture{ nullptr };
		Engine * m_engine{ nullptr };

	public:
		C3D_API Generator( Engine * p_engine, int p_width, int p_height );
		C3D_API virtual ~Generator();

		C3D_API virtual bool Step();
		C3D_API virtual void SetRed( uint8_t val );
		C3D_API virtual void SetGreen( uint8_t val );
		C3D_API virtual void SetBlue( uint8_t val );
		C3D_API virtual void SwapBuffers();
		C3D_API virtual void InitialiseStep();
		C3D_API virtual void ClearAllThreads();

		C3D_API bool AllEnded();
		C3D_API void Suspend();
		C3D_API void SetSize( int p_iWidth, int p_iHeight );
		C3D_API void SetSize( Castor::Point2i const & p_size );
		C3D_API void SaveFrame();

		template <class ThreadClass>
		ThreadClass * CreateThread( int iWidth, int iTop, int iBottom, int iTotalHeight, Castor::UbPixel const & p_pxColour )
		{
			ThreadClass * l_pThread = new ThreadClass( this, m_arraySlaveThreads.size(), m_iWidth, iTop, iBottom, iTotalHeight, p_pxColour );
			m_arraySlaveThreads.push_back( l_pThread );
			return l_pThread;
		}

		inline uint8_t GetRed()const
		{
			return m_pxColour[0];
		}
		inline uint8_t GetGreen()const
		{
			return m_pxColour[1];
		}
		inline uint8_t GetBlue()const
		{
			return m_pxColour[2];
		}
		inline bool IsInitialised()const
		{
			return m_initialised;
		}
		inline Castor::PixelBuffer const * GetPixelsBuffer()const
		{
			return & m_frontBuffer;
		}
		inline Castor::PixelBuffer * GetPixelsBuffer()
		{
			return & m_frontBuffer;
		}
		inline int GetWidth()const
		{
			return m_iWidth;
		}
		inline int GetHeight()const
		{
			return m_iHeight;
		}

	protected:
		C3D_API void DoCleanup();
		uint32_t DoGetThreadsCount()
		{
			return uint32_t( m_arraySlaveThreads.size() );
		}
		C3D_API Castor::Point2i _loadImage( Castor::String const & p_strImagePath, Castor::Image & p_pImage );
		C3D_API void _subRender();
		C3D_API void _saveFrame();
	};

	extern int GetCPUCount();
}

#endif
