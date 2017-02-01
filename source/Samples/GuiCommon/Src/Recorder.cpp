#include "Recorder.hpp"

#include <emmintrin.h>

#pragma warning( push )
#	pragma warning( disable:4996 )
#	include <wx/filedlg.h>
#	include <wx/msgdlg.h>
#pragma warning( pop )

#if defined( GUICOMMON_RECORDS )

namespace libffmpeg
{
	extern "C"
	{
#	include <libavutil/avutil.h>
#	include <libavutil/error.h>
#	include <libavutil/opt.h>
#	include <libavutil/common.h>
#	include <libavutil/imgutils.h>
#	include <libavutil/mathematics.h>
#	include <libavutil/samplefmt.h>
#	include <libavcodec/avcodec.h>
#	include <libavformat/avformat.h>
#	include <libswscale/swscale.h>
//#	include <libswresample/swresample.h>
#	if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(54, 6, 0)
#	include <libavutil/imgutils.h>
#	endif
	}

#ifndef AV_ERROR_MAX_STRING_SIZE
	static constexpr size_t AV_ERROR_MAX_STRING_SIZE = 64;

	static inline char * av_make_error_string( char * errbuf, size_t errbuf_size, int errnum )
	{
		av_strerror( errnum, errbuf, errbuf_size );
		return errbuf;
	}

#	undef PixelFormat
#endif

	void CheckError( int p_error, char const * const p_action )
	{
		if ( p_error < 0 )
		{
			char l_err[AV_ERROR_MAX_STRING_SIZE] { 0 };
			std::stringstream l_stream;
			l_stream << ( char const * )_( "Failure on:" ).mb_str( wxConvUTF8 ) << "\n";
			l_stream << "\t" << ( char const * )wxGetTranslation( p_action ).mb_str( wxConvUTF8 );
			l_stream << av_make_error_string( l_err, AV_ERROR_MAX_STRING_SIZE, p_error );

			throw std::runtime_error( l_stream.str() );
		}
	}
}

#endif

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GuiCommon
{
	namespace
	{
#if defined( GUICOMMON_RECORDS )
#	if !defined( MAKEFOURCC )
#		if BYTE_ORDER == BIG_ENDIAN
#			define MAKEFOURCC( a, b, c, d ) ( ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 ) | ( uint32_t( c ) <<  8 ) | ( uint32_t( d ) <<  0 ) )
#		else
#			define MAKEFOURCC( a, b, c, d ) ( ( uint32_t( a ) <<  0 ) | ( uint32_t( b ) <<  8 ) | ( uint32_t( c ) << 16 ) | ( uint32_t( d ) << 24 ) )
#		endif
#	endif

		using clock = std::chrono::high_resolution_clock;
		using time_point = clock::time_point;

		class RecorderImplBase
			: public Recorder::IRecorderImpl
		{
		public:
			virtual bool UpdateTime()
			{
				auto l_now = clock::now();
				uint64_t l_timeDiff = std::chrono::duration_cast< std::chrono::milliseconds >( l_now - m_saved ).count();

				if ( m_recordedCount )
				{
					m_recordedTime += l_timeDiff;
				}

				return DoUpdateTime( l_timeDiff );
			}

			virtual bool StartRecord( Size const & p_size, int p_wantedFPS )
			{
				bool l_result = false;
				m_wantedFPS = p_wantedFPS;
				m_recordedCount = 0;
				m_recordedTime = 0;
				wxString l_strWildcard = _( "Supported Video files" );
				l_strWildcard += wxT( "(*.avi;*.mkv)|*.avi;*.mkv" );
				wxFileDialog l_dialog( nullptr, _( "Please choose a video file name" ), wxEmptyString, wxEmptyString, l_strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

				if ( l_dialog.ShowModal() == wxID_OK )
				{
					wxString l_strFileName = l_dialog.GetPath();

					try
					{
						DoStartRecord( p_size, l_strFileName );
						l_result = IsRecording();
					}
					catch ( std::bad_alloc & )
					{
					}
					catch ( std::exception & exc )
					{
						wxString l_strMsg = _( "Can't start recording file" );
						l_strMsg += wxT( " " );
						l_strMsg += l_strFileName;
						l_strMsg += wxT( ":\n" );
						l_strMsg += wxString( exc.what(), wxMBConvLibc() );
						l_strMsg += wxT( ")" );
						throw std::runtime_error( string::string_cast< char >( make_String( l_strMsg ) ) );
					}
				}

				return l_result;
			}

			virtual bool RecordFrame( PxBufferBaseSPtr p_buffer )
			{
				bool l_result = false;
				DoRecordFrame( p_buffer );
				m_saved = clock::now();
				m_recordedCount++;
				l_result = true;
				return l_result;
			}

		protected:
			virtual bool DoStartRecord( Size const & p_size, wxString const & p_name ) = 0;
			virtual bool DoUpdateTime( uint64_t p_uiTimeDiff ) = 0;
			virtual void DoRecordFrame( PxBufferBaseSPtr p_buffer ) = 0;

		protected:
			time_point m_saved;
			uint32_t m_recordedCount{ 0 };
			uint64_t m_recordedTime{ 0 };
			int m_wantedFPS{ 0 };
		};

		class FFmpegWriter
		{
		public:
			bool IsValid()
			{
				return m_formatContext && m_stream && m_codec;
			}

			void FillPacket( libffmpeg::AVPacket & p_packet )
			{
				p_packet.stream_index = m_formatContext->streams[0]->index;
			}

			void Write( libffmpeg::AVPacket & p_packet )
			{
				auto l_timestamp = p_packet.pts;

				if ( p_packet.pts != ( 0x8000000000000000LL ) )
				{
					l_timestamp = av_rescale_q( l_timestamp, m_codecContext->time_base, m_formatContext->streams[0]->time_base );
				}

				p_packet.pts = l_timestamp;

				if ( !( p_packet.pts % 10 ) )
				{
					p_packet.flags |= AV_PKT_FLAG_KEY;
				}

				libffmpeg::av_interleaved_write_frame( m_formatContext, &p_packet );
			}

			void GetFormat( wxString const & p_name )
			{
				m_outputFormat = libffmpeg::av_guess_format( nullptr, p_name.char_str().data(), nullptr );

				if ( !m_outputFormat )
				{
					m_outputFormat = libffmpeg::av_guess_format( "mpeg", nullptr, nullptr );
				}

				if ( !m_outputFormat )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not deduce output format" ) ).mb_str( wxConvUTF8 ) );
				}

				m_codec = libffmpeg::avcodec_find_encoder( m_outputFormat->video_codec );

				if ( !m_codec )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not find codec" ) ).mb_str( wxConvUTF8 ) );
				}

				m_formatContext = libffmpeg::avformat_alloc_context();

				if ( !m_formatContext )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not find format context" ) ).mb_str( wxConvUTF8 ) );
				}

				m_formatContext->oformat = m_outputFormat;
				m_formatContext->video_codec_id = m_outputFormat->video_codec;
				snprintf( m_formatContext->filename, sizeof( m_formatContext->filename ), "%s", p_name.char_str().data() );

				m_stream = libffmpeg::avformat_new_stream( m_formatContext, m_codec );

				if ( !m_stream )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not allocate stream" ) ).mb_str( wxConvUTF8 ) );
				}
			}

			bool Open( wxString const & p_name )
			{
#if LIBAVFORMAT_VERSION_MAJOR >= 57 && LIBAVFORMAT_VERSION_MINOR >= 34
				libffmpeg::CheckError( avcodec_parameters_from_context( m_stream->codecpar, m_codecContext )
									   , "Setting codec parameters from context" );
#else
				m_stream->codec = m_codecContext;
#endif

				if ( m_formatContext->oformat->flags & AVFMT_GLOBALHEADER )
				{
					m_codecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
				}

				libffmpeg::CheckError( avcodec_open2( m_codecContext, m_codec, nullptr )
									   , "Codec opening" );

				libffmpeg::CheckError( libffmpeg::avio_open( &m_formatContext->pb, p_name.char_str().data(), AVIO_FLAG_WRITE )
									   , "Stream opening" );

				libffmpeg::CheckError( libffmpeg::avformat_write_header( m_formatContext, nullptr )
									   , "Writing format header to stream" );

				return true;
			}

			void Close( bool p_wasRecording )
			{
				if ( p_wasRecording )
				{
					libffmpeg::av_write_trailer( m_formatContext );
				}

				if ( m_formatContext )
				{
					if ( m_formatContext->pb )
					{
						libffmpeg::avio_close( m_formatContext->pb );
					}

					libffmpeg::avformat_free_context( m_formatContext );
					m_formatContext = nullptr;
				}
			}

		protected:
			void DoAllocateFrame()
			{
				// Allocate the encoded frame.
				m_frame = libffmpeg::av_frame_alloc();

				if ( !m_frame )
				{
					throw std::runtime_error( ( char const * )wxString( _( "Could not allocate encoded video frame" ) ).mb_str( wxConvUTF8 ) );
				}

				// Allocate the encoded raw picture.
				libffmpeg::CheckError( libffmpeg::av_image_alloc( m_frame->data, m_frame->linesize, m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt, 1 )
									   , "Encoded picture buffer allocation" );
			}

		protected:
			libffmpeg::AVCodec * m_codec{ nullptr };
			libffmpeg::AVCodecContext * m_codecContext{ nullptr };
			libffmpeg::AVFrame * m_frame{ nullptr };
			libffmpeg::AVOutputFormat * m_outputFormat{ nullptr };
			libffmpeg::AVFormatContext * m_formatContext{ nullptr };
			libffmpeg::AVStream * m_stream{ nullptr };
		};

		class RecorderImpl
			: public RecorderImplBase
			, private FFmpegWriter
		{
		public:
			RecorderImpl()
			{
				libffmpeg::av_register_all();
			}

			virtual ~RecorderImpl()
			{
			}

			void AllocateContext( libffmpeg::AVCodec * p_codec, libffmpeg::AVCodecID p_id )
			{
			}

			virtual bool IsRecording()
			{
				return IsValid() && m_frame;
			}

			virtual void StopRecord()
			{
				if ( IsRecording() )
				{
					// We first write remaining frames.
					libffmpeg::AVPacket l_pkt = { 0 };
					libffmpeg::av_init_packet( &l_pkt );
					int l_gotOutput = 1;

					while ( l_gotOutput )
					{
						int iRet = libffmpeg::avcodec_encode_video2( m_codecContext, &l_pkt, nullptr, &l_gotOutput );

						if ( iRet >= 0 && l_gotOutput )
						{
							Write( l_pkt );
						}
					}
				}

				Close( IsRecording() );

				if ( m_swsContext )
				{
					libffmpeg::sws_freeContext( m_swsContext );
					m_swsContext = nullptr;
				}

				if ( m_codecContext )
				{
					libffmpeg::avcodec_close( m_codecContext );
					libffmpeg::av_free( m_codecContext );
					m_codecContext = nullptr;
				}

				if ( m_frame )
				{
					libffmpeg::av_freep( &m_frame->data[0] );
					libffmpeg::av_frame_free( &m_frame );
					m_frame = nullptr;
				}
			}

		private:
			virtual bool DoUpdateTime( uint64_t ptimeDiff )
			{
				return ptimeDiff >= 1000 / m_wantedFPS;
			}

			virtual bool DoStartRecord( Size const & p_size, wxString const & p_name )
			{
				wxSize l_size( p_size.width(), p_size.height() );
				GetFormat( p_name );

				try
				{
					m_codecContext = libffmpeg::avcodec_alloc_context3( m_codec );

					if ( !m_codecContext )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not allocate video codec context" ) ).mb_str( wxConvUTF8 ) );
					}

					// Sample parameters
					m_codecContext->bit_rate = m_bitRate;
					// Resolution
					m_codecContext->width = l_size.x;
					m_codecContext->height = l_size.y;
					// Frames per second
					m_codecContext->time_base = { 1, m_wantedFPS };
					// Emit one intra frame every thirty frames
					m_codecContext->gop_size = m_wantedFPS;
					m_codecContext->max_b_frames = 1;
					m_codecContext->pix_fmt = m_pixelFmt;

					//// x264 specifics
					libffmpeg::av_opt_set( m_codecContext->priv_data, "preset", "slow", AV_OPT_SEARCH_CHILDREN );
					libffmpeg::av_opt_set( m_codecContext->priv_data, "profile", "high", AV_OPT_SEARCH_CHILDREN );
					libffmpeg::av_opt_set( m_codecContext->priv_data, "level", "4.1", AV_OPT_SEARCH_CHILDREN );

					DoAllocateFrame();

					// Retrieve the scaling and encoding context.
					m_swsContext = libffmpeg::sws_getContext( m_codecContext->width, m_codecContext->height, libffmpeg::AV_PIX_FMT_RGBA,
															  m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt,
															  SWS_POINT, nullptr, nullptr, nullptr );

					if ( !m_swsContext )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not initialise conversion context" ) ).mb_str( wxConvUTF8 ) );
					}

					if ( !Open( p_name ) )
					{
						throw std::runtime_error( ( char const * )wxString( _( "Could not open file" ) ).mb_str( wxConvUTF8 ) );
					}
				}
				catch ( std::exception & )
				{
					StopRecord();
					throw;
				}

				return true;
			}

			virtual void DoRecordFrame( PxBufferBaseSPtr p_buffer )
			{
				if ( IsRecording() )
				{
					try
					{
						auto l_buffer = p_buffer->const_ptr();
						int l_lineSize[8] = { m_codecContext->width * 4, 0, 0, 0, 0, 0, 0, 0 };
						auto l_outputHeight = libffmpeg::sws_scale( m_swsContext, &l_buffer, l_lineSize, 0, p_buffer->dimensions().height(), m_frame->data, m_frame->linesize );
						libffmpeg::AVPacket l_packet{ 0 };
						libffmpeg::av_init_packet( &l_packet );
						std::vector< uint8_t > l_outbuf( l_packet.size );
						l_packet.pts = m_recordedCount;
						l_packet.dts = m_recordedCount;
#	if LIBAVUTIL_VERSION_INT > AV_VERSION_INT(54, 6, 0)
						l_packet.size = libffmpeg::av_image_get_buffer_size( m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height, 1 );
#	else
						l_packet.size = libffmpeg::avpicture_get_size( m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height );
#	endif
						l_packet.data = l_outbuf.data();
						FillPacket( l_packet );
						m_frame->pts = m_recordedCount++;
						int l_gotOutput = 0;

						libffmpeg::CheckError( libffmpeg::avcodec_encode_video2( m_codecContext, &l_packet, m_frame, &l_gotOutput )
											   , "Frame encoding" );

						if ( l_gotOutput )
						{
							Write( l_packet );
						}
					}
					catch ( std::exception & )
					{
						StopRecord();
						throw;
					}
				}
			}

		private:
			int const m_bitRate{ 600000 };
			libffmpeg::AVPixelFormat const m_pixelFmt{ libffmpeg::AV_PIX_FMT_YUV420P };// or PIX_FMT_YUV420P
			libffmpeg::SwsContext * m_swsContext{ nullptr };
			std::array< ByteArray, AV_NUM_DATA_POINTERS > m_frameBuffers;
		};

#else

		class RecorderImpl
			: public Recorder::IRecorderImpl
		{
		public:
			RecorderImpl()
			{
			}

			virtual ~RecorderImpl()
			{
			}

			virtual bool StartRecord( Size const & p_size, int p_wantedFPS )
			{
				return true;
			}

			virtual bool IsRecording()
			{
				return false;
			}

			virtual bool UpdateTime()
			{
				return false;
			}

			virtual bool RecordFrame( PxBufferBaseSPtr )
			{
				return true;
			}

			virtual void StopRecord()
			{
			}
		};

#endif
	}

	//*************************************************************************************************

	Recorder::Recorder()
		: m_impl( std::make_unique< RecorderImpl >() )
	{
	}

	Recorder::~Recorder()
	{
		m_impl.reset();
	}
}
