#include "microhttp_server.hpp"
#include "stl.hpp"
#include "rsa.hpp"
#include "date.hpp"
#include "url.hpp"
#include "helper.hpp"
#include "application_server.hpp"
#include "json_serializer.hpp"
#include "routing.hpp"

#include "logger.hpp"
#include <unordered_map>
#include <stdlib.h>
#include <stdio.h>
#include <microhttpd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

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

typedef int (*ResponseHandler)(struct Request*,
    const char *mime,
    px::Hash_p post_data,
    struct MHD_Connection *connection);

/**
 * Entry we generate for each page served.
 */
struct Action {
    const px::routing_entry *routing_infos;
    
    /**
     * Handler to call to generate response.
     */
    ResponseHandler handler;
};

typedef enum : unsigned short {
    HTTP_MethodGet,
    HTTP_MethodPost,
    HTTP_MethodUnknown,
} HTTP_Method;

/**
 * Data kept per request.
 */
struct Request {
    struct PX_PostDataProcessor *pdp;
    struct Action *action;
    px::URL_p url;
    px::UserSession_p session;
    HTTP_Method method;
};


static int send_error_response(unsigned int code, struct MHD_Connection *connection) {
    struct MHD_Response *response = MHD_create_response_from_buffer (1, (void *)" ", MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, code, response);
    MHD_destroy_response (response);
    return ret;
}

static int not_found_action(struct Request*, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    struct MHD_Response *response = MHD_create_response_from_buffer (1, (void *)" ", MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response (response);
    return ret;
}

static int send_response(void* buffer, size_t size, const char* mime, unsigned int status_code, struct MHD_Connection *connection) {
    int ret;
    struct MHD_Response *response;
    response = MHD_create_response_from_buffer (size, buffer, MHD_RESPMEM_MUST_COPY);
    if (NULL == response) {
        return MHD_NO;
    }
    ret = MHD_queue_response(connection, status_code, response);
    
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_ENCODING, mime);
    
    MHD_destroy_response(response);
    return ret;
}

static int send_data_response(struct Request* request, px::Value_p data, unsigned int status_code, struct MHD_Connection *connection) {
    string json;
    px::JSONSerializer::Serialize(data, json, nullptr);
    px::RSA rsa(request->session->get_public_key(true));
    
    string encrypted_json;
    rsa.encrypt(json, encrypted_json);
    return send_response((void *)encrypted_json.c_str(), encrypted_json.length(), request->action->routing_infos->mime_type, status_code, connection);
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
static int user_meta_infos(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    string user_id = request->session->get_user_id(true).to_string();
    if (!user_id.empty()) {
        auto info = px::ApplicationServer::Server().load_user_meta_info(user_id);
        send_data_response(request, _V(info), MHD_HTTP_OK, connection);
    }
    return send_error_response(420, connection);
}

static int list(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    string list_id = request->url->param(Px::Request::Param::ListID);
    if (!list_id.empty()) {
        auto list = px::ApplicationServer::Server().load_list(list_id);
        send_data_response(request, _V(list), MHD_HTTP_OK, connection);
    }
    return send_error_response(420, connection);
}

static int list_dif(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    string user_id = request->session->get_user_id(true).to_string();
    if (!user_id.empty()) {
        long modified = 0;
        string t = request->url->param(Px::Request::Param::LastModified);
        if (!t.empty()) {
            modified = std::stol(t);
        }
        auto dif = px::ApplicationServer::Server().load_list_dif(user_id, px::Date(modified));
        send_data_response(request, _V(dif), MHD_HTTP_OK, connection);
    }
    return send_error_response(420, connection);
}

/**************************
 
 POST Actions
 
 *************************/

px::Hash_p parse_post_data(const char *post_data, size_t post_data_len) {
    string s(post_data, post_data_len);
    string out;
    px::Application::Instance().decode_shared(s, out);
    return px::JSONSerializer::DeSerialize(out, nullptr)->hash_value();
}

// TODO: add proper error-logs and handling
// TODO: add authentication, provide user_session to server calls etc.!!
static int list_modifications(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    if (post_data) {
        auto info = px::ApplicationServer::Server().insert_list_modifications(post_data->get(Px::Request::Param::ListID)->string_value(), post_data->get(Px::Request::Param::Modifications)->array_value(), nullptr);
        send_data_response(request, _V(info), MHD_HTTP_OK, connection);
    }
    return send_error_response(420, connection);
}

static int user_modifications(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    if (post_data) {
        auto info = px::ApplicationServer::Server().insert_info_modifications(post_data->get(Px::Request::Param::ListID)->string_value(), post_data->get(Px::Request::Param::Modifications)->array_value(), nullptr);
        send_data_response(request, _V(info), MHD_HTTP_OK, connection);
    }
    return send_error_response(420, connection);
}

static int signin(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    if (post_data) {
        
        auto session = px::ApplicationServer::Server().create_session(post_data->get(Px::Request::Param::Email)->string_value(),
                                                                      post_data->get(Px::Request::Param::Password)->string_value(),
                                                                      post_data->get(Px::Request::Param::PublicKey)->string_value());
        if (session) {
            request->session = session;
            send_data_response(request, _V(session), MHD_HTTP_OK, connection);
        }
    }
    return not_found_action(request, mime, post_data, connection);
}

static int signup(struct Request* request, const char *mime, px::Hash_p post_data, struct MHD_Connection *connection) {
    if (post_data) {
        auto session = px::ApplicationServer::Server().signup(post_data->get(Px::Request::Param::Email)->string_value(),
                                                              post_data->get(Px::Request::Param::Password)->string_value(),
                                                              post_data->get(Px::Request::Param::PublicKey)->string_value());
        if (session) {
            request->session = session;
            send_data_response(request, _V(session), MHD_HTTP_OK, connection);
        }
    }
    return send_error_response(420, connection);
}

/**************************
 
 Routing Table
 
 *************************/

static struct Action actions[] = {
    { &px::RoutingEntry::GetUserMetaInfo, &user_meta_infos },
    { &px::RoutingEntry::GetList, &list },
    { &px::RoutingEntry::GetListDif, &list_dif },
    
    { &px::RoutingEntry::PostListModifications, &list_modifications },
    { &px::RoutingEntry::PostUserModifications, &user_modifications },

    { &px::RoutingEntry::Signin, &signin },
    { &px::RoutingEntry::Signup, &signup },
    
    { NULL, &not_found_action } /* 404 */
};

static size_t max_action_length = 0;

static void prepare_server() {
    static std::once_flag f;
    std::call_once(f, [] {
        size_t i=0;
        while ( (actions[i].routing_infos ) ) {
            size_t len = strlen(actions[i].routing_infos->path);
            if (len>max_action_length) {
                max_action_length = len;
            }
            i++;
        }
    });
    
    px::Error_p err = nullptr;
    px::Application::Initialize<px::ApplicationServer>(options, &err);
    if (err) {
        LogError("%s", err->description().c_str());
    }
    exit(1);
}

// *Pre* authorize request! if the get parameters are not sufficent for authentification we will not bother parsing the post-data!
static int pre_authorize_request(const char* encoded_path, const char* method, struct Request *request, unsigned int &status_code) {
    
    status_code = 420;
    request->method = HTTP_MethodUnknown;
    request->action = NULL;
    
    // first convert encoded_path from base64
    // -> than to rsa-encryption
    // -> validate time
    // -> validate action
    // -> validate token
    size_t length = strlen(encoded_path);
    if (!length) {
        LogDebug("no input path given");
        return MHD_NO;
    }
    
    px::URL_p url = px::URL::CreateEncoded(encoded_path);
    if (url && !url->path().empty()) {
        request->url = url;
        auto& action = url->path();
        
        size_t i=0;
        while ( (actions[i].routing_infos != NULL) ) {
            if (0 == strcmp(actions[i].routing_infos->path, action.c_str()) ) {
                request->action = &actions[i];
                if (0 != strcmp((const char *)actions[i].routing_infos->method, method) ) {
                    status_code = MHD_HTTP_METHOD_NOT_ACCEPTABLE;
                    return MHD_NO;
                } else {
                    if ( 0 == strcmp (method, MHD_HTTP_METHOD_GET) ) {
                        request->method = HTTP_MethodGet;
                    } else if ( 0 == strcmp (method, MHD_HTTP_METHOD_POST) ) {
                        request->method = HTTP_MethodPost;
                    }
                }
                break;
            }
            i++;
        }
        
        if (request->action == NULL) {
            LogDebug("unknown action %s", url->path().c_str());
            return MHD_NO;
        }
        
        url->each_param([](const string &k, const string &v) {
            LogDebug("%s : %s", k.c_str(), v.c_str());
        });

        
        auto time_param = url->param(Px::Request::Param::Time);
        if (time_param.empty()) {
            LogDebug("no time");
            status_code = 420;
            return MHD_NO;
        }
        
        long time = std::stol(time_param);
        px::Date d = px::Date(time);
        px::Date now = px::Date();
        px::Date start = now.advance_minutes(-10);
        px::Date end = now.advance_minutes(10);
        
        if (!((start <= d) && (d <= end))) {
            status_code = 420;
            LogDebug("time not valid");
            return MHD_NO;
        }
        
        if (request->action->routing_infos->auth_required) {
            auto token = url->param(Px::Request::Param::AuthToken);
            if (!token.empty()) {
                auto session = px::ApplicationServer::Server().load_session(token);
                if (session) {
                    request->session = session;
                }
                status_code = MHD_HTTP_OK;
                return MHD_YES;
            }
            LogDebug("no auth token");
            status_code = MHD_HTTP_FORBIDDEN;
        } else {
            status_code = MHD_HTTP_OK;
            return MHD_YES;
        }
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
    if (request->method != HTTP_MethodUnknown && request->action) {
        px::Hash_p json = nullptr;
        if (request->pdp) {
            json = parse_post_data(request->pdp->buf, request->pdp->cur_size);
        }
        struct Action action = *request->action;
        return action.handler(request, action.routing_infos->mime_type, json, connection);
    }
    
    /* unsupported HTTP method */
    return send_error_response(MHD_HTTP_METHOD_NOT_ACCEPTABLE, connection);
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


int startup_server(short port) {
    ignore_sigpipe();
    prepare_server();
    
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
