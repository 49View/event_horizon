//
// Created by Dado on 11/01/2018.
//

#include "callback_dependency.h"
#include "util.h"
#include "file_manager.h"

dep_map g_deps;

std::string cbToString( uint8_p&& _data ) {
    return std::string( reinterpret_cast<const char*>(_data.first.get()), _data.second);
}

namespace DependencyHandler {

    dep_map& dependencies() {
        return g_deps;
    };

    bool needsDependencies( [[maybe_unused]] const std::string& _key ) {
        return true;
    }

    void update() {
        // Two steps here;

        // 1st this is the FileManager callback queue
        // it's quite concrete and just deals with loading data in
        for ( auto& q : FM::callbacksDataMap ) {
            auto qd = q.second->cbData;
            // Check if our callbacks have returned from loading (either successfully or not)
            // (IsComplete just checks for callback has reached the end of it's life, we do not know at this point
            // if the file has been loaded correctly or failed)
            if ( !q.second->isComplete()) {
                // Now if the file has been loaded correctly then execute the file callback related to that file
                if ( qd->hasFinishedLoading() ) {
                    bool cbStatus = q.second->executeCallback(qd->status);
                    // Also tag the status as "it has passed the loading stage, was it successful or not? in the
                    // callback ?
                    qd->status = cbStatus ? DependencyStatus::CallbackSuccessfullyExectuted
                                          : DependencyStatus::CallbackFailedOnExectution;
                    FM::callbacksDataMapExecuted[q.first] = q.second;
                }
                // If it has failed to laod then just leave the status as it is (LoadingFailed)
            }
        }
        for ( auto it = FM::callbacksDataMap.begin(); it != FM::callbacksDataMap.end();) {
            if ( ( *it ).second->isComplete() ) { it = FM::callbacksDataMap.erase( it ); } else { ++it; }
        }

        // 2nd This is a list of all dependencies file loads might have
        // An entry contains a list of dependencies key (string at the moment)
        for ( auto& v : DH::dependencies()) {
            // This complete return true only if every loading from the dependency list has been completed either
            // successfull or not, and once it has it will call the callback for the dependency. At that point the
            // entry will be tag as completed and won't enter this if case again
            if ( !v->isComplete()) {
                bool isComplete = true; // just means it has passed all the loading stage
                bool hasCompletedSuccessfully = true; // it means it has passed all the loading stages and callbacks
                // correctly
                for ( const auto& d : v->deps() ) {
                    auto it = FM::callbacksDataMapExecuted.find( d );
                    if ( it == FM::callbacksDataMapExecuted.end()) {
                        isComplete = false;
                        break;
                    } else {
                        hasCompletedSuccessfully &= it->second->hasCompletedSuccessfully();
                    }
                }
                if ( isComplete ) {
                    // this will execute even if there have been errors loading any resource because we can handle it
                    // inside the callback, so it's the callback job to decide how to handle the failures, but
                    // default the base class will just not execute the callback internally
                    v->execute( hasCompletedSuccessfully );
                    // Ok we break the cycle because execute can invalidate the iterator hence crashing
                    // TODO: maybe a restart instead of a "break and continue on next frame" ?
                    break;
                }
            }
        }

        DH::dependencies().erase( remove_if( DH::dependencies().begin(), DH::dependencies().end(), []( const auto& v)
                -> bool { return v->isComplete(); } ), DH::dependencies().end() );
    }

};
