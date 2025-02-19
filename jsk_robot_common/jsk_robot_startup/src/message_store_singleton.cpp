// -*- mode: C++ -*-
/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2017, JSK Lab
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the JSK Lab nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
/*
 * message_store_singleton.cpp
 * Author:  <furushchev@jsk.imi.i.u-tokyo.ac.jp>
 */

#include <jsk_robot_startup/message_store_singleton.h>

namespace jsk_robot_startup
{
namespace lifelog
{
  mongodb_store::MessageStoreProxy* MessageStoreSingleton::getInstance(
      const std::string& collection,
      const std::string& database,
      const std::string& prefix) {
    boost::mutex::scoped_lock lock(mutex_);
    std::string key = prefix + database + "/" + collection;
    if (instances_.count(key) == 0) {
      instances_[key] = new mongodb_store::MessageStoreProxy(nh_, collection, database, prefix);
    }
    return instances_[key];
  }

  void MessageStoreSingleton::destroy() {
    for (M_Proxy::iterator it = instances_.begin(); it != instances_.end(); ++it)
    {
      if (it->second) delete it->second;
    }
  }

MessageStoreSingleton::M_Proxy MessageStoreSingleton::instances_;
boost::mutex MessageStoreSingleton::mutex_;
ros::NodeHandle MessageStoreSingleton::nh_;
} // lifelog
} // jsk_robot_startup
