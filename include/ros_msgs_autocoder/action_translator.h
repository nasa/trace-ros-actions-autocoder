/**
 * Copyright 2016-2025 California Institute of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROS_MSGS_AUTOCODER_ACTION_TRANSLATOR_H_
#define ROS_MSGS_AUTOCODER_ACTION_TRANSLATOR_H_

#include <actionlib/action_definition.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <unordered_map>

using PropertyMap = std::unordered_map<std::string, std::string>;

namespace trace {
namespace ros_msgs_autocoder {

template <class ActionType> class ActionTranslator {
public:
  ACTION_DEFINITION(ActionType);

  virtual Goal ToGoal(const PropertyMap &properties) = 0;
  virtual PropertyMap FromResult(const Result &result) = 0;
};

} // namespace ros_msgs_autocoder
} // namespace trace

#endif