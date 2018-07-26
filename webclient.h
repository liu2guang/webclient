/*
 * File      : webclient.h
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-05-05     Bernard      the first version
 * 2013-06-10     Bernard      fix the slow speed issue when download file.
 * 2015-11-14     aozima       add content_length_remainder.
 * 2017-12-23     aozima       update gethostbyname to getaddrinfo.
 * 2018-01-04     aozima       add ipv6 address support.
 * 2018-07-26     chenyong     modify log information
 */

#ifndef __WEBCLIENT_H__
#define __WEBCLIENT_H__

#include <rtthread.h>

#ifdef WEBCLIENT_USING_TLS
#include <tls_client.h>
#endif

#undef DBG_SECTION_NAME
#undef DBG_LEVEL
#undef DBG_COLOR
#undef DBG_ENABLE

#define DBG_ENABLE
#define DBG_SECTION_NAME     "WEB "
#ifdef WEBCLIENT_DEBUG
#define DBG_LEVEL            DBG_LOG
#else
#define DBG_LEVEL            DBG_INFO
#endif /* WEBCLIENT_DEBUG */
#define DBG_COLOR
#include <rtdbg.h>

#ifndef web_malloc
#define web_malloc           rt_malloc
#endif

#ifndef web_calloc
#define web_calloc           rt_calloc
#endif

#ifndef web_realloc
#define web_realloc          rt_realloc
#endif

#ifndef web_free
#define web_free             rt_free
#endif

#ifndef WEBCLIENT_HEADER_BUFSZ
#define WEBCLIENT_HEADER_BUFSZ         4096
#endif

#ifndef WEBCLIENT_RESPONSE_BUFSZ
#define WEBCLIENT_RESPONSE_BUFSZ       4096
#endif

#if defined(WEBCLIENT_USING_TLS) && !defined(WEBCLIENT_TLS_READ_BUFFER)
#define WEBCLIENT_TLS_READ_BUFFER      4096
#endif

enum WEBCLIENT_STATUS
{
    WEBCLIENT_OK,
    WEBCLIENT_ERROR,
    WEBCLIENT_TIMEOUT,
    WEBCLIENT_NOMEM,
    WEBCLIENT_NOSOCKET,
    WEBCLIENT_NOBUFFER,
    WEBCLIENT_CONNECT_FAILED,
    WEBCLIENT_DISCONNECT,
    WEBCLIENT_FILE_ERROR,

};

enum WEBCLIENT_METHOD
{
    WEBCLIENT_USER_METHOD,
    WEBCLIENT_GET,
    WEBCLIENT_POST,
};

struct webclient_session
{
    /* the session socket */
    int socket;
    /* the response code of HTTP request */
    int response;

    /* transfer encoding */
    char *transfer_encoding;
    int chunk_sz;
    int chunk_offset;

    /* content_type of HTTP response */
    char *content_type;
    /* content_length of HTTP response */
    int  content_length;

    /* last modified timestamp of resource */
    char *last_modified;

    /* location */
    char *location;

    /* server host */
    char *host;
    /* HTTP request */
    char *request;

    /* position of reading */
    unsigned int position;

    /* remainder of content reading */
    size_t content_length_remainder;
    
#ifdef WEBCLIENT_USING_TLS
        /* mbedtls connect session */
        MbedTLSSession *tls_session;
#endif
};

struct webclient_session *webclient_open(const char *URI);
struct webclient_session *webclient_open_position(const char *URI, int position);
struct webclient_session *webclient_open_header(const char *URI, int method, const char *header, size_t header_sz);
int webclient_close(struct webclient_session *session);

int webclient_set_timeout(struct webclient_session *session, int millisecond);

int webclient_read(struct webclient_session *session, unsigned char *buffer, size_t size);
int webclient_write(struct webclient_session *session, const unsigned char *buffer, size_t size);

int webclient_send_header(struct webclient_session *session, int method,
                          const char *header, size_t header_sz);
int webclient_connect(struct webclient_session *session, const char *URI);
int webclient_handle_response(struct webclient_session *session);

/* hight level APIs for HTTP client */
int webclient_response(struct webclient_session *session, void **response);
struct webclient_session *webclient_open_custom(const char *URI, int method,
        const char *header, size_t header_sz,
        const char *data, size_t data_sz);

int webclient_transfer(const char *URI, const char *header, size_t header_sz,
                       const char *data, size_t data_sz,
                       char *result, size_t result_sz);


#ifdef RT_USING_DFS
int webclient_get_file(const char *URI, const char *filename);
int webclient_post_file(const char *URI, const char *filename, const char *form_data);
#endif

#endif
