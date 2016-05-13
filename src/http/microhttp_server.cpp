#include "microhttp_server.hpp"
#include "defines.hpp"
#include "date.hpp"
#include "url.hpp"
#include "helper.hpp"
#include "application.hpp"
#include "routing.hpp"
#include "logger.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <microhttpd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "hash.hpp"

/**
 * Invalid method page.
 */
#define METHOD_ERROR "<html><head><title>Illegal request</title></head><body>Cannot Handle Request.</body></html>"


struct PX_PostDataProcessor {
    char * buf;
    size_t buf_size;
    size_t cur_size;
};

PX_PostDataProcessor * PX_create_post_data_processor(size_t buffersize) {
    PX_PostDataProcessor *data_processor = (PX_PostDataProcessor*)malloc(sizeof(struct PX_PostDataProcessor));
    data_processor->cur_size = 0;
    if (buffersize) {
        data_processor->buf = (char*)malloc(sizeof(char)*buffersize);
        if (!data_processor->buf) {
            puts("Error (re)allocating memory");
            exit(1);
        }
        data_processor->buf_size = buffersize;
        
    } else {
        data_processor->buf = NULL;
        data_processor->buf_size = 0;
    }
    return data_processor;
}

int PX_add_post_data(struct PX_PostDataProcessor *pdp, const char* post_data, size_t post_data_len) {
    if (pdp) {
        size_t needed = post_data_len + pdp->cur_size;
        long remaining = pdp->buf_size - needed;
        if (remaining < 0) {
            char * additional = (char *)realloc((void *)pdp->buf, sizeof(char)*needed);
            if (!additional) {
                puts("Error (re)allocating memory");
                exit(1);
            }
            pdp->buf = additional;
            pdp->buf_size = needed;
        }
        for (size_t i=0; i<post_data_len; ++i) {
            pdp->buf[pdp->cur_size+i] = post_data[i];
        }
        pdp->cur_size += post_data_len;
        return MHD_YES;
    }
    return MHD_NO;
}


int PX_destroy_post_data_processor (struct PX_PostDataProcessor *pdp) {
    if (pdp) {
        if (pdp->buf) {
            string data = string(pdp->buf, pdp->cur_size);
            LogDebug("%s", data.c_str());
            free(pdp->buf);
        }
        free(pdp);
    }
    return MHD_YES;
}

struct Request;

//typedef int (*ResponseHandler)(struct Request*,
//    const char *mime,
//    px::Hash_p post_data,
//    struct MHD_Connection *connection);

/**
 * Data kept per request.
 */
struct Request {
    struct PX_PostDataProcessor *pdp;
    const px::routing_entry *routing_infos;
    px::URL_p url;
    HTTP_Method method;
};

static int send_error_response(unsigned int code, struct MHD_Connection *connection) {
    struct MHD_Response *response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, code, response);
    MHD_destroy_response (response);
    return ret;
}

static int not_found_action(struct Request*, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    struct MHD_Response *response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response (response);
    return ret;
}

static int send_not_modified(struct MHD_Connection *connection) {
    struct MHD_Response *response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_MODIFIED, response);
    MHD_destroy_response (response);
    return ret;
}

static int send_response(void* buffer, size_t size, const char* mime, unsigned int status_code, struct MHD_Connection *connection, const px::Hash_p additional_headers = nullptr) {
//    LogDebug("sending response data: %d %s %d", (int)size, mime, status_code);
    int ret;
    struct MHD_Response *response;
    response = MHD_create_response_from_buffer (size, buffer, MHD_RESPMEM_MUST_COPY);
    if (NULL == response) {
        return MHD_NO;
    }
    ret = MHD_queue_response(connection, status_code, response);
    
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, mime);
    
    if (additional_headers) {
        additional_headers->each_pair([&](const string &k, const px::Value_p &v) {
//            LogDebug("%s %s", k.c_str(), v->string_value().c_str());
            MHD_add_response_header(response, k.c_str(), v->string_value().c_str());
        });
    }
    
    MHD_destroy_response(response);
    return ret;
}

static int send_response(px::Response_p response, struct MHD_Connection *connection) {
    if (!response) {
        return send_error_response(MHD_HTTP_NOT_FOUND, connection);
    } else if(response->status_code() == MHD_HTTP_NOT_MODIFIED) {
        return send_not_modified(connection);
    }
    
    px::Hash_p headers = nullptr;
    
    {
        px::Date last_modified = response->last_modified();
        
//        LogDebug("%s", last_modified.to_string().c_str());
        
        if (last_modified != px::Date::Empty()) {

            const char* value = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_IF_MODIFIED_SINCE);
            
            if (value) {
                auto d = px::Date::FromString(value);
//                LogDebug("%s %s", MHD_HTTP_HEADER_IF_MODIFIED_SINCE, d.to_string().c_str());
                
                if (d == last_modified) {
                    return send_not_modified(connection);
                }
            }
            
            if (!headers) {
                headers = px::Hash::Create();
            }
            
            headers->set(MHD_HTTP_HEADER_LAST_MODIFIED, px::Value::Create(last_modified.to_string()));
            headers->set(MHD_HTTP_HEADER_CACHE_CONTROL, px::Value::Create("max-age=0, must-revalidate"));
        }
    }
    
    if (auto resource = response->resource()) {
//        LogDebug("sending response with resource");
        // TODO: data could be improved!!!!!!!!!!!
        size_t data_length;
        auto data = resource->data(&data_length);
        if (data) {
            return send_response((void *)data, data_length, response->mime_type().c_str(), response->status_code(), connection, headers);
            
        } else {
            char * data = resource->load(&data_length);
            int ret = send_response((void *)data, data_length, response->mime_type().c_str(), response->status_code(), connection, headers);
            free(data);
            return ret;
        }
    } else {
//        LogDebug("sending response with data");
        auto data = response->data();
        return send_response((void *)data.data(), data.length(), response->mime_type().c_str(), response->status_code(), connection, headers);
    }
}

static int response_handler(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    int ret = 0;
    if (ret == 0) {
        return not_found_action(request, mime, post_data, connection);
    }
    return ret;
}

/**************************
 
 GET Actions
 
 *************************/


/**************************
 
 POST Actions
 
 *************************/

//px::Hash_p parse_post_data(const char *post_data, size_t post_data_len) {
//    string s(post_data, post_data_len);
//    string out;
////    px::Application::Instance().decode_shared(s, out);
////    return px::JSONSerializer::DeSerialize(out, nullptr)->hash_value();
//    return nullptr;
//}

/**************************
 
 Setup Server internals
 
 *************************/

static void prepare_server(const char *directory) {
    if (!px::Application::Initialize(directory)) {
        exit(1);
    }
}

// *Pre* authorize request! if the get parameters are not sufficent for authentification we will not bother parsing the post-data!
static int pre_authorize_request(const char* full_request_path, const char* method, struct Request *request, unsigned int &status_code) {
    status_code = 420;
    request->method = HTTP_MethodUnknown;
    request->routing_infos = NULL;
    
    if (!full_request_path) {
        LogDebug("no input path given");
        return MHD_NO;
    }
    
    px::URL_p url = px::URL::Create(full_request_path);
    if (url && !url->path().empty()) {
        request->url = url;
        auto& action = url->action();
        
        size_t i=0;
        
        auto routing_entry = px::RoutingTable::Instance().get(action);
        
        if (!routing_entry) {
            LogDebug("path not valid: %s", action.c_str());
            status_code = MHD_HTTP_NOT_FOUND;
            return MHD_NO;
        }
        
        if (0 != strcmp(routing_entry->method, method) ) {
            status_code = MHD_HTTP_NOT_ACCEPTABLE;
            return MHD_NO;
        } else {
            if ( 0 == strcmp (method, MHD_HTTP_METHOD_GET) ) {
                request->method = HTTP_MethodGet;
            } else if ( 0 == strcmp (method, MHD_HTTP_METHOD_POST) ) {
                request->method = HTTP_MethodPost;
            }
        }
        
        request->routing_infos = routing_entry;
        status_code = MHD_HTTP_OK;
        return MHD_YES;
    }
    return MHD_NO;
}

static int create_response (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr) {
    struct MHD_Response *response;
    struct Request *request;
    unsigned int i;
    request = (Request *)*ptr;
    if (NULL == request) {
        
        request = (Request*)calloc (1, sizeof (struct Request));
        *ptr = request;
        if (NULL == request) {
            fprintf (stderr, "calloc error: %s\n", strerror (errno));
            return MHD_NO;
        }
        unsigned int status_code = 0;
        
        if (!pre_authorize_request(url, method, request, status_code)) {
            LogDebug("pre_authorize_request failed");
            return send_error_response(status_code, connection);
        }

        if (request->method == HTTP_MethodPost) {
            request->pdp = PX_create_post_data_processor(65536);
            if (NULL == request->pdp) {
                fprintf (stderr, "Failed to setup post processor for `%s'\n",
                         url);
                return MHD_NO; /* internal error */
            }
        }
        return MHD_YES;
    }
    
    
    
    if (request->method == HTTP_MethodPost) {
        /* evaluate POST data */
        PX_add_post_data(request->pdp, upload_data, *upload_data_size);
        if (0 != *upload_data_size) {
            *upload_data_size = 0;
            return MHD_YES;
        }
    }
    
    if (request->method != HTTP_MethodUnknown && request->routing_infos) {
        auto controller = request->routing_infos->controller;
        if (controller) {
            px::Hash_p params = px::Hash::Create({{px::Param::ActionName , px::Value::Create(request->routing_infos->action)}, {px::Param::FileName , px::Value::Create(request->url->file())}});
            params->append(request->url->params());
            
            for (auto& p : px::Param::ValidParams()) {
                const char* value = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, p.c_str());
                if (value) {
                    params->set(p, px::Value::Create(value));
                }
            }
            
//            if (request->pdp) {
//                params->append(parse_post_data(request->pdp->buf, request->pdp->cur_size));
//            }

            auto response = controller->call(request->method, params);
            
            return send_response(response, connection);
        }
    }
    
    /* unsupported HTTP method */
    return send_error_response(MHD_HTTP_NOT_ACCEPTABLE, connection);
}


static void request_completed_callback (void *cls,
                            struct MHD_Connection *connection,
                            void **con_cls,
                            enum MHD_RequestTerminationCode toe)
{
    struct Request *request = (Request*)*con_cls;
    
    if (NULL == request) {
        return;
    }
    
    if (NULL != request->pdp) {
        PX_destroy_post_data_processor(request->pdp);
    }
    request->url = nullptr;
    free (request);
}

static void catcher (int sig) {
    
}

static void ignore_sigpipe () {
    struct sigaction oldsig;
    struct sigaction sig;
    
    sig.sa_handler = &catcher;
    sigemptyset (&sig.sa_mask);
#ifdef SA_INTERRUPT
    sig.sa_flags = SA_INTERRUPT;  /* SunOS */
#else
    sig.sa_flags = SA_RESTART;
#endif
    if (0 != sigaction (SIGPIPE, &sig, &oldsig))
        fprintf (stderr, "Failed to install SIGPIPE handler: %s\n", strerror (errno));
}


int startup_server(short port, const char *directory) {
    ignore_sigpipe();
    prepare_server(directory);
    
    LogDebug("starting server on port: %d dir: %s", port, directory);
    
    struct MHD_Daemon *d;
    struct timeval tv;
    struct timeval *tvp;
    fd_set rs;
    fd_set ws;
    fd_set es;
    MHD_socket max;
    MHD_UNSIGNED_LONG_LONG mhd_timeout;

    /* initialize PRNG */
    srand ((unsigned int) time (NULL));
    d = MHD_start_daemon (MHD_USE_DEBUG,
                          port,
                          NULL, NULL,
                          &create_response, NULL,
                          MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 15,
                          MHD_OPTION_NOTIFY_COMPLETED, &request_completed_callback, NULL,
                          MHD_OPTION_END);
    if (NULL == d)
        return 1;
    while (1)
    {
        max = 0;
        FD_ZERO (&rs);
        FD_ZERO (&ws);
        FD_ZERO (&es);
        if (MHD_YES != MHD_get_fdset (d, &rs, &ws, &es, &max))
            break; /* fatal internal error */
        if (MHD_get_timeout (d, &mhd_timeout) == MHD_YES)
        {
            tv.tv_sec = mhd_timeout / 1000;
            tv.tv_usec = (mhd_timeout - (tv.tv_sec * 1000)) * 1000;
            tvp = &tv;
        }
        else
            tvp = NULL;
        select (max + 1, &rs, &ws, &es, tvp);
        MHD_run (d);
    }
    MHD_stop_daemon (d);
    return 0;
}
