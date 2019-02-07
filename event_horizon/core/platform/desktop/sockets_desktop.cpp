#include <memory>

//
// Created by Dado on 29/04/2018.
//

#include <unordered_map>
#include <thread>
#include <chrono>
#include <condition_variable>
#include "../../http/webclient.h"
#include "core/util.h"

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>


typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef client::connection_ptr connection_ptr;
typedef std::chrono::duration<int,std::micro> dur_type;


namespace Socket {

    void startClientInternal();
    void reconnect();

    std::shared_ptr<client> m_endpoint;
    std::atomic<bool> g_isConnected = false;
    std::atomic<bool> g_isConnecting = false;
    uint64_t connectionTries = 0;
    std::string g_host;
    std::string g_hostname;
    websocketpp::connection_hdl connectionHandle;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_reconnectThread;

    std::chrono::high_resolution_clock::time_point m_start;
    std::chrono::high_resolution_clock::time_point m_socket_init;
    std::chrono::high_resolution_clock::time_point m_tls_init;
    std::chrono::high_resolution_clock::time_point m_open;
    std::chrono::high_resolution_clock::time_point m_message;
    std::chrono::high_resolution_clock::time_point m_close;

    context_ptr on_tls_init( [[maybe_unused]] const char * hostname, websocketpp::connection_hdl) {
        context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);


            ctx->set_verify_mode(boost::asio::ssl::verify_none);
            // ### NDDado: reenable those two lines to enable native client certificate verification
//            ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));
//            ctx->load_verify_file("ca-chain.cert.pem");
        } catch (std::exception& e) {
            LOGRS( e.what() );
        }
        return ctx;
    }

    void on_fail(websocketpp::connection_hdl hdl) {
        g_isConnecting = false;
        g_isConnected = false;
        client::connection_ptr con = m_endpoint->get_con_from_hdl(hdl);
        LOGRS( "[WEB-SOCKET][CONNECT-FAIL] #" << connectionTries << " to " << g_host );
//      LOGRS( "Fail handler" <<  con->get_state() <<  con->get_local_close_code() <<  con->get_local_close_reason()<< con->get_remote_close_code() << con->get_remote_close_reason() << con->get_ec() << " - " << con->get_ec().message() );
        if (!m_reconnectThread) m_reconnectThread = std::make_shared<websocketpp::lib::thread>( reconnect );
    }

    void on_open(websocketpp::connection_hdl hdl) {
        LOGRS( "[WEB-SOCKET][OK-CONNECT] #" << connectionTries << " to " << g_host );
        m_open = std::chrono::high_resolution_clock::now();
        g_isConnected = true;
        g_isConnecting = false;
        connectionHandle = hdl;
        // Start the ASIO io_service run loop
        if ( m_reconnectThread ) m_reconnectThread->join();
    }

    void reconnect() {
        while ( !g_isConnected  ) {
            sleep(5);
            if ( !g_isConnecting ) startClientInternal();
        }
    }

    void on_close([[maybe_unused]] websocketpp::connection_hdl hdl) {
        m_close = std::chrono::high_resolution_clock::now();
        LOGRS( "[WEB-SOCKET][CLOSE] " << std::chrono::duration_cast<dur_type>(m_close-m_start).count() );
        g_isConnected = false;
        m_reconnectThread = std::make_shared<websocketpp::lib::thread>( reconnect );
    }

    void on_message( [[maybe_unused]] websocketpp::connection_hdl hdl, message_ptr msg) {
        onMessage( msg->get_payload() );
    }

    void startClientInternal() {
        try {
            if ( g_isConnected ) return;
            g_isConnecting = true;
            connectionTries++;

            m_endpoint = std::make_shared<client>();
            m_endpoint->set_access_channels(websocketpp::log::alevel::none);
            m_endpoint->set_error_channels(websocketpp::log::elevel::none);

            // Initialize ASIO
            m_endpoint->init_asio();

            // Register our handlers
            m_endpoint->set_tls_init_handler(bind(&on_tls_init, g_hostname.c_str(), ::_1));
            m_endpoint->set_message_handler(bind(&on_message,::_1,::_2));
            m_endpoint->set_open_handler(bind(&on_open,::_1));
            m_endpoint->set_close_handler(bind(&on_close,::_1));
            m_endpoint->set_fail_handler(bind(&on_fail,::_1));

            websocketpp::lib::error_code ec;
            client::connection_ptr con = m_endpoint->get_connection(g_host, ec);

            if (ec) {
                m_endpoint->get_alog().write(websocketpp::log::alevel::app,ec.message());
                return;
            }

            m_endpoint->connect(con);
            m_endpoint->start_perpetual();
            m_start = std::chrono::high_resolution_clock::now();
            if ( m_thread ) m_thread->join();
            m_thread = std::make_shared<websocketpp::lib::thread>( &client::run, m_endpoint.get() );
        } catch (websocketpp::exception const & e) {
            LOGRS( e.what() );
        } catch (std::exception const & e) {
            LOGRS( e.what() );
        } catch (...) {
            LOGRS( "other exception");
        }
    }

    void emitImpl( const std::string& _message ) {
        websocketpp::lib::error_code ec;

        m_endpoint->send( connectionHandle, _message, websocketpp::frame::opcode::text, ec);
        if (ec) {
            LOGRS("Echo failed because: " << ec.message());
        }
    }

    void startClient( const std::string& _host ) {
        g_host = _host;
        g_hostname = Url{g_host}.hostOnly();

        startClientInternal();
    }

    void close() {
        websocketpp::lib::error_code ec;
        m_endpoint->close(connectionHandle, websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection " << ec.message() << std::endl;
        }
        m_endpoint->stop_perpetual();
        m_endpoint->stop();
        m_thread->join();
    }

}
