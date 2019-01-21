#include "audio_video_stream.hpp"

#include <core/util.h>
#include <core/streaming_mediator.hpp>

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C" {
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

class AudioVideoStream::AudioVideoStreamImpl {
public:
	AudioVideoStreamImpl( StreamingMediator& mediator ) : mediator( mediator ) {}
	int main_decode( const std::string& tname );
	void advanceFrame();
	const std::string& Name() const { return name; }

protected:
	std::string name;
	AVFormatContext *fmt_ctx = NULL;
	AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
	int width, height;
	enum AVPixelFormat pix_fmt;
	AVStream *video_stream = NULL, *audio_stream = NULL;

	uint8_t *video_dst_data[4] = { NULL };
	int      video_dst_linesize[4];

	int video_stream_idx = -1, audio_stream_idx = -1;
	AVFrame *frame = NULL;
	AVPacket pkt;

	/* Enable or disable frame reference counting. You are not supposed to support
	* both paths in your application but pick the one most appropriate to your
	* needs. Look for the use of refcount in this example to see what are the
	* differences of API usage between them. */
	int refcount = 0;
	bool loaded = false;

	StreamingMediator& mediator;

protected:
	int decode_packet( int *got_frame, int cached );
	int open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type );
};

int AudioVideoStream::AudioVideoStreamImpl::decode_packet( int *got_frame, int /*cached*/ ) {
	int ret = 0;
	int decoded = pkt.size;

	*got_frame = 0;

	if ( pkt.stream_index == video_stream_idx ) {
		/* decode video frame */
		do {
            ret = avcodec_send_packet( video_dec_ctx, &pkt );
            if (ret == AVERROR(EAGAIN)) {
                ret = 0;
            } else if ( ret == AVERROR_EOF) {
                ret = -1;
                break;
            }
		} while ( ret != 0 );

        ret = avcodec_receive_frame(video_dec_ctx, frame);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            return ret;
        if (ret >= 0)
            *got_frame = 1;

		if ( *got_frame ) {
			if ( frame->width != width || frame->height != height ||
				 frame->format != pix_fmt ) {
				/* To handle this change, one could call av_image_alloc again and
				* decode the following frames into another rawvideo file. */
				//fprintf(stderr, "Error: Width, height and pixel format have to be "
				//	"constant in a rawvideo file, but the width, height or "
				//	"pixel format of the input video changed:\n"
				//	"old: width = %d, height = %d, format = %s\n"
				//	"new: width = %d, height = %d, format = %s\n",
				//	width, height, av_get_pix_fmt_name(pix_fmt),
				//	frame->width, frame->height,
				//	av_get_pix_fmt_name(frame->format));
				return -1;
			}

			//printf("video_frame%s n:%d coded_n:%d\n",
			//	cached ? "(cached)" : "",
			//	video_frame_count++, frame->coded_picture_number);

			/* copy decoded frame to destination buffer:
			* this is required since rawvideo expects non aligned data */
			av_image_copy( video_dst_data, video_dst_linesize,
				(const uint8_t **)( frame->data ), frame->linesize,
						   pix_fmt, width, height );
			mediator.push( name + "_y", (const uint8_t *)video_dst_data[0]);
			mediator.push( name + "_u", (const uint8_t *)video_dst_data[1]);
			mediator.push( name + "_v", (const uint8_t *)video_dst_data[2]);

			/* write to rawvideo file */
//			fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
		}
	} else if ( pkt.stream_index == audio_stream_idx ) {
		/* decode audio frame */
		// ### uncomment end of next line to re-introduce video player 
		ret = -1; //avcodec_decode_audio4( audio_dec_ctx, frame, got_frame, &pkt );
		if ( ret < 0 ) {
			//			fprintf(stderr, "Error decoding audio frame (%s)\n", av_err2str(ret));
			return ret;
		} 
		/* Some audio decoders decode only part of the packet, and have to be
		* called again with the remainder of the packet data.
		* Sample: fate-suite/lossless-audio/luckynight-partial.shn
		* Also, some decoders might over-read the packet. */
		decoded = FFMIN( ret, pkt.size );

		if ( *got_frame ) {
			//size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format));
			//printf("audio_frame%s n:%d nb_samples:%d pts:%s\n",
			//	cached ? "(cached)" : "",
			//	audio_frame_count++, frame->nb_samples,
			//	av_ts2timestr(frame->pts, &audio_dec_ctx->time_base));

			/* Write the raw audio data samples of the first plane. This works
			* fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
			* most audio decoders output planar audio, which uses a separate
			* plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
			* In other words, this code will write only the first audio channel
			* in these cases.
			* You should use libswresample or libavfilter to convert the frame
			* to packed data. */
			//			fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);
		}
	}

	/* If we use frame reference counting, we own the data and need
	* to de-reference it when we don't use it anymore */
//	if ( *got_frame && refcount )
//		av_frame_unref( frame );

	return decoded;
}

int AudioVideoStream::AudioVideoStreamImpl::open_codec_context( int *stream_idx,
									  AVCodecContext **dec_ctx, AVFormatContext * _fmt_ctx, enum AVMediaType type ) {
	int ret, stream_index;
	AVStream *st;
	AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = av_find_best_stream( _fmt_ctx, type, -1, -1, NULL, 0 );
	if ( ret < 0 ) {
		LOGE( "Could not find %s stream in input file '%s'\n", av_get_media_type_string( type ), name.c_str() );
		return ret;
	} else {
		stream_index = ret;
		st = _fmt_ctx->streams[stream_index];

		/* find decoder for the stream */
		dec = avcodec_find_decoder( st->codecpar->codec_id );
		if ( !dec ) {
			LOGE( "Failed to find %s codec\n", av_get_media_type_string( type ) );
			return AVERROR( EINVAL );
		}

		/* Allocate a codec context for the decoder */
		*dec_ctx = avcodec_alloc_context3( dec );
		if ( !*dec_ctx ) {
			LOGE( "Failed to allocate the %s codec context\n", av_get_media_type_string( type ) );
			return AVERROR( ENOMEM );
		}

		/* Copy codec parameters from input stream to output codec context */
		if ( ( ret = avcodec_parameters_to_context( *dec_ctx, st->codecpar ) ) < 0 ) {
			fprintf( stderr, "Failed to copy %s codec parameters to decoder context\n",
					 av_get_media_type_string( type ) );
			return ret;
		}

		/* Init the decoders, with or without reference counting */
		av_dict_set( &opts, "refcounted_frames", refcount ? "1" : "0", 0 );
		if ( ( ret = avcodec_open2( *dec_ctx, dec, &opts ) ) < 0 ) {
			fprintf( stderr, "Failed to open %s codec\n",
					 av_get_media_type_string( type ) );
			return ret;
		}
		*stream_idx = stream_index;
	}

	return 0;
}

void AudioVideoStream::AudioVideoStreamImpl::advanceFrame() {

    if ( !loaded ) return;

	int ret = 0, got_frame;
	int endOfFileOrError = 0;
	/* read frames from the file */
//	do {
		endOfFileOrError = av_read_frame( fmt_ctx, &pkt );
//		if ( endOfFileOrError >= 0 ) {
			//AVPacket orig_pkt = pkt;
//			do {
                ret = decode_packet( &got_frame, 0 );
//                if ( ret >= 0 ) {
//                    pkt.data += ret;
//                    pkt.size -= ret;
//                }
//			} while (pkt.size > 0);
			//av_packet_unref(&orig_pkt);
//			LOGR("Packet Size %d", pkt.size );
//		}
//	} while ( pkt.stream_index == audio_stream_idx && ( endOfFileOrError >= 0 ) );

	/* flush cached frames */
	//pkt.data = NULL;
	//pkt.size = 0;
	//do {
	//	decode_packet(&got_frame, 1);
	//} while (got_frame);
//
//	avcodec_free_context(&video_dec_ctx);
//	avcodec_free_context(&audio_dec_ctx);
//	avformat_close_input(&fmt_ctx);
//	av_frame_free(&frame);
//	av_free(video_dst_data[0]);
}

int AudioVideoStream::AudioVideoStreamImpl::main_decode( const std::string& tname ) {
	int ret = 0;

//	setId( tname );
	name = tname;

	/* register all formats and codecs */
	av_register_all();

    avformat_network_init();

	/* open input file, and allocate format context */
	if ( avformat_open_input( &fmt_ctx, name.c_str(), NULL, NULL ) < 0 ) {
		LOGE( "Could not open source file %s\n", name.c_str() );
		return -1;
	}

	/* retrieve stream information */
	if ( avformat_find_stream_info( fmt_ctx, NULL ) < 0 ) {
		LOGE( "Could not find stream information\n" );
        return -1;
	}

	if ( open_codec_context( &video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO ) >= 0 ) {
		video_stream = fmt_ctx->streams[video_stream_idx];

		/* allocate image where the decoded image will be put */
		width = video_dec_ctx->width;
		height = video_dec_ctx->height;
		pix_fmt = video_dec_ctx->pix_fmt;
//		std::unique_ptr<uint8_t[]> vbuffer = std::make_unique<uint8_t[]>(width*height);
//		TM.addTextureWithData( tname + "_y", std::move(vbuffer), width, height, 1 );
//		TM.addTextureWithData( tname + "_u", std::move(vbuffer), width / 2, height / 2, 1 );
//		TM.addTextureWithData( tname + "_v", std::move(vbuffer), width / 2, height / 2, 1 );
		ret = av_image_alloc( video_dst_data, video_dst_linesize,
							  width, height, pix_fmt, 1 );
		if ( ret < 0 ) {
			LOGE( "Could not allocate raw video buffer\n" );
		}
	}

	if ( open_codec_context( &audio_stream_idx, &audio_dec_ctx, fmt_ctx, AVMEDIA_TYPE_AUDIO ) >= 0 ) {
		audio_stream = fmt_ctx->streams[audio_stream_idx];
	}

	ASSERT( audio_stream || video_stream );

	frame = av_frame_alloc();
	//	ASSERT(frame);

		/* initialize packet, set data to NULL, let the demuxer fill it */
	av_init_packet( &pkt );
	pkt.data = NULL;
	pkt.size = 0;

//	double m_out_start_time = 0.0;
//	int flgs = 0;//AVSEEK_FLAG_ANY;
//	int seek_ts = 0;//(int)( m_out_start_time*( video_stream->time_base.den ) ) / ( video_stream->time_base.num );
//	if ( av_seek_frame( fmt_ctx, video_stream_idx, seek_ts, flgs ) < 0 ) {
//		LOGE("Failed to seek Video ");
//	}

    loaded = true;
	return ret < 0;
}

AudioVideoStream::AudioVideoStream( StreamingMediator& mediator ) {
	pimpl = std::make_unique<AudioVideoStream::AudioVideoStreamImpl>(mediator);
}

const std::string& AudioVideoStream::Name() const {
    return pimpl->Name();
}

int AudioVideoStream::main_decode( const std::string& tname ) {
	return pimpl->main_decode( tname );
}

void AudioVideoStream::advanceFrame() {
	pimpl->advanceFrame();
}
