/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InstantUploadData_H___
#define ___C3D_InstantUploadData_H___

#include "Castor3D/Buffer/UploadData.hpp"

#include <ashespp/Sync/Fence.hpp>

namespace castor3d
{
	template< UploadDataT UploaderT >
	class InstantUploadDataT
	{
	public:
		InstantUploadDataT( InstantUploadDataT const & ) = delete;
		InstantUploadDataT( InstantUploadDataT && )noexcept = delete;
		InstantUploadDataT & operator=( InstantUploadDataT const & ) = delete;
		InstantUploadDataT & operator=( InstantUploadDataT && ) = delete;
		template< typename ... ParamsT >
		InstantUploadDataT( ashes::Queue const & queue
			, ParamsT && ... params )
			: m_upload{ castor::forward< ParamsT >( params )... }
			, m_queue{ queue }
		{
			m_upload.begin();
		}

		~InstantUploadDataT()
		{
			UploadData & upload = m_upload;
			RenderDevice const & device = upload.getDevice();
			auto fence = device->createFence( castor::toUtf8( upload.getName() ) );
			upload.process();
			upload.end( m_queue, fence.get(), castor::Milliseconds{ ashes::MaxTimeout } );
		}

		operator UploaderT & ()
		{
			return m_upload;
		}

		UploaderT * operator->()
		{
			return &m_upload;
		}

	private:
		UploaderT m_upload{};
		ashes::Queue const & m_queue;
	};
}

#endif
