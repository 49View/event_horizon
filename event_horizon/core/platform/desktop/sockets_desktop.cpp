//
// Created by Dado on 29/04/2018.
//

#include <iostream>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include "sio_client.h"
#include "../../http/webclient.h"
#include "core/util.h"
#include <sio_message.h>

namespace Socket {

    sio::client handler;
    std::mutex _lock;
    std::condition_variable_any _cond;
    static bool connect_finish = false;

    void lock() { _lock.lock(); }
    void unlock() { _lock.unlock(); }

    void onConnected() {
        _lock.lock();
        _cond.notify_all();
        Socket::connect_finish = true;
        _lock.unlock();
    }

    void onClose( [[maybe_unused]] sio::client::close_reason const& reason ) {
        std::cout << "sio closed " << std::endl;
    }

    void onFail() {
        std::cout << "sio failed " << std::endl;
    }

    void startClient( const std::string& _host ) {
        handler.set_open_listener( std::bind( onConnected ));
        handler.set_close_listener( std::bind( onClose, std::placeholders::_1 ));
        handler.set_fail_listener( std::bind( onFail ));
        handler.connect( _host );
        _lock.lock();
        if ( !connect_finish ) {
            _cond.wait( _lock );
        }

        handler.socket()->on( "message",
            sio::socket::event_listener_aux( [&]( std::string const& name, sio::message::ptr const& data,
                                                  bool isAck, sio::message::list& ack_resp ) {
                                                      _lock.lock();
                                                      ASSERT( data->get_flag() == sio::message::flag::flag_string );
                                                      onMessage( data->get_string() );
                                                      _lock.unlock();
                                                  }
            ));

        _lock.unlock();
    }

    void removeClient() {
        if ( connect_finish ) {
            _lock.lock();
            handler.sync_close();
            handler.clear_con_listeners();
            _lock.unlock();
        }
    }
}
