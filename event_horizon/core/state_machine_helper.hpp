//
// Created by Dado on 2019-04-19.
//

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#ifndef BOOST_NO_AUTO_PTR
#define BOOST_NO_AUTO_PTR
#endif
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#pragma GCC diagnostic pop

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;
#define sm_not template <class FSM, class Event> void no_transition(Event const& e, FSM&, int state) {}

