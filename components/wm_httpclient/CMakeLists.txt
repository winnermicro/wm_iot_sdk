if(CONFIG_COMPONENT_HTTP_CLIENT_ENABLED)
    list(APPEND ADD_INCLUDE "include"
                            "http/include"
                            )

    list(APPEND ADD_SRCS    "http/src/HTTPClient.c"
                            "http/src/HTTPClientAuth.c"
                            "http/src/HTTPClientString.c"
                            "http/src/HTTPClientWrapper.c"
                            "http/src/HTTPSClientCert.c"
                            "src/wm_http_client.c"
                            )

if(CONFIG_WM_HTTP2_CLIENT_ENABLED)
    list(APPEND ADD_DEFINITIONS -DHAVE_CONFIG_H)

    list(APPEND ADD_INCLUDE "http2/nghttp2/lib"
                            "http2/nghttp2/lib/includes/"
                            "http2/nghttp2/src"
                            "http2/port"
                            )
    list(APPEND ADD_SRCS    "http2/nghttp2/lib/nghttp2_buf.c"
                            "http2/nghttp2/lib/nghttp2_callbacks.c"
                            "http2/nghttp2/lib/nghttp2_debug.c"
                            "http2/nghttp2/lib/nghttp2_extpri.c"
                            "http2/nghttp2/lib/nghttp2_frame.c"
                            "http2/nghttp2/lib/nghttp2_hd.c"
                            "http2/nghttp2/lib/nghttp2_hd_huffman.c"
                            "http2/nghttp2/lib/nghttp2_hd_huffman_data.c"
                            "http2/nghttp2/lib/nghttp2_helper.c"
                            "http2/nghttp2/lib/nghttp2_http.c"
                            "http2/nghttp2/lib/nghttp2_map.c"
                            "http2/nghttp2/lib/nghttp2_mem.c"
                            "http2/nghttp2/lib/nghttp2_alpn.c"
                            "http2/nghttp2/lib/nghttp2_option.c"
                            "http2/nghttp2/lib/nghttp2_outbound_item.c"
                            "http2/nghttp2/lib/nghttp2_pq.c"
                            "http2/nghttp2/lib/nghttp2_priority_spec.c"
                            "http2/nghttp2/lib/nghttp2_queue.c"
                            "http2/nghttp2/lib/nghttp2_ratelim.c"
                            "http2/nghttp2/lib/nghttp2_rcbuf.c"
                            "http2/nghttp2/lib/nghttp2_session.c"
                            "http2/nghttp2/lib/nghttp2_stream.c"
                            "http2/nghttp2/lib/nghttp2_submit.c"
                            "http2/nghttp2/lib/nghttp2_time.c"
                            "http2/nghttp2/lib/nghttp2_version.c"
                            "http2/nghttp2/lib/sfparse.c"
                            "src/wm_http2_client.c"
                            )
    endif()

    register_component()
endif()