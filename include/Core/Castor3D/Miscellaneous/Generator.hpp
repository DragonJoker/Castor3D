/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GENERATOR_H___
#define ___C3D_GENERATOR_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <CastorUtils/Graphics/Pixel.hpp>
#include <CastorUtils/Graphics/PixelBuffer.hpp>

namespace castor3d
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
			castor::UbPixel m_pxColour;
			uint32_t m_index{ 0u };
			std::shared_ptr< std::thread > m_pThread;
			mutable std::recursive_mutex m_mutex;

		public:
			C3D_API Thread( Generator * parent, uint32_t index, int iWidth, int iTop, int iBottom, int iTotalHeight, castor::UbPixel const & pxColour );
			C3D_API virtual ~Thread();

			C3D_API void run();
			C3D_API void wait();
			C3D_API virtual int Entry();

			C3D_API virtual void Step() = 0;

			inline bool	isEnded()const
			{
				return m_bEnded;
			}
			inline bool IsStopped()const
			{
				auto lock = castor::makeUniqueLock( m_mutex );
				return m_bStopped;
			}

			inline void Relaunch()
			{
				m_bLaunched = true;
			}
			inline void stop()
			{
				auto lock = castor::makeUniqueLock( m_mutex );
				m_bStopped = true;
			}
			inline void setRed( uint8_t val )
			{
				m_pxColour[0] = val;
			}
			inline void setGreen( uint8_t val )
			{
				m_pxColour[1] = val;
			}
			inline void setBlue( uint8_t val )
			{
				m_pxColour[2] = val;
			}

		private:
			static uint32_t StEntry( Generator::Thread * pThis );
		};

	protected:
		int m_iWidth{ 0 };
		int m_iHeight{ 0 };
		unsigned long long m_ullStep{ 0u };
		castor::PixelBuffer m_frontBuffer;
		castor::PixelBuffer m_backBuffer;
		castor::UbPixel m_pxColour;
		uint32_t m_uiThreadCount{ 0u };
		bool m_bEnded{ true };
		std::vector <Thread *> m_arraySlaveThreads;
		bool m_initialised{ false };
		TextureUnit * m_pTexture{ nullptr };
		Engine * m_engine{ nullptr };

	public:
		C3D_API Generator( Engine * engine, int width, int height );
		C3D_API virtual ~Generator();

		C3D_API virtual bool Step();
		C3D_API virtual void setRed( uint8_t val );
		C3D_API virtual void setGreen( uint8_t val );
		C3D_API virtual void setBlue( uint8_t val );
		C3D_API virtual void swapBuffers();
		C3D_API virtual void InitialiseStep();
		C3D_API virtual void ClearAllThreads();

		C3D_API bool AllEnded();
		C3D_API void Suspend();
		C3D_API void setSize( int iWidth, int iHeight );
		C3D_API void setSize( castor::Point2i const & size );
		C3D_API void saveFrame();

		template <class ThreadClass>
		ThreadClass * CreateThread( int iWidth, int iTop, int iBottom, int iTotalHeight, castor::UbPixel const & pxColour )
		{
			ThreadClass * pThread = new ThreadClass( this, m_arraySlaveThreads.size(), m_iWidth, iTop, iBottom, iTotalHeight, pxColour );
			m_arraySlaveThreads.push_back( pThread );
			return pThread;
		}

		inline uint8_t getRed()const
		{
			return m_pxColour[0];
		}
		inline uint8_t getGreen()const
		{
			return m_pxColour[1];
		}
		inline uint8_t getBlue()const
		{
			return m_pxColour[2];
		}
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		inline castor::PixelBuffer const * getPixelsBuffer()const
		{
			return & m_frontBuffer;
		}
		inline castor::PixelBuffer * getPixelsBuffer()
		{
			return & m_frontBuffer;
		}
		inline int getWidth()const
		{
			return m_iWidth;
		}
		inline int getHeight()const
		{
			return m_iHeight;
		}

	protected:
		C3D_API void doCleanup();
		uint32_t doGetThreadsCount()
		{
			return uint32_t( m_arraySlaveThreads.size() );
		}
		C3D_API castor::Point2i _loadImage( castor::String const & strImagePath, castor::Image & pImage );
		C3D_API void _subrender();
		C3D_API void _saveFrame();
	};

	extern int getCPUCount();
}

#endif
