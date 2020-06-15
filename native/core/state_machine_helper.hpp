//
// Created by Dado on 2019-04-19.
//

#pragma once

#include <memory>
#include <core/sml.hpp>
using namespace boost::sml;

template <typename T>
class BackEndService {
public:
    T *BackEnd() {
        return backEnd.get();
    }
    void setBackEnd( const std::shared_ptr<T>& be ) {
        backEnd = be;
    }
protected:
    std::shared_ptr<T> backEnd;
};

// This is using boost::msm with functors but we are trying to go with kris-jusiak [boost]::sml. Feeling brave.
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wall"
//#ifndef BOOST_NO_AUTO_PTR
//#define BOOST_NO_AUTO_PTR
//#endif
//#include <boost/msm/back/state_machine.hpp>
//#include <boost/msm/front/state_machine_def.hpp>
//#include <boost/msm/front/functor_row.hpp>
//#pragma GCC diagnostic pop
//
//namespace msm = boost::msm;
//namespace mpl = boost::mpl;
//using namespace msm::front;
//#define sm_not template <class FSM, class Event> void no_transition(Event const& e, FSM&, int state) {}
//
