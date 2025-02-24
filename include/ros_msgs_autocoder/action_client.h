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

#ifndef ROS_MSGS_AUTOCODER_CONNECTOR_ACTION_CLIENT_H_
#define ROS_MSGS_AUTOCODER_CONNECTOR_ACTION_CLIENT_H_

#include "ros_msgs_autocoder/service_task_interface.h"
#include "ros_msgs_autocoder/action_translator.h"

#include <actionlib/action_definition.h>
#include <actionlib/client/simple_action_client.h>

#include <string>
#include <unordered_map>

namespace trace {
namespace ros_msgs_autocoder {

template <class ActionType, class ActionTranslatorType>
class ActionClient : public trace::ServiceTaskInterface {
public:
  ACTION_DEFINITION(ActionType);

  ActionClient(const std::string &action_topic);

  virtual void Connect() override;

  virtual std::future<trace::Outcome>
  SendCommand(const std::string &service_task_uuid,
              const PropertyMap &properties) override;

  virtual void AbortCommand(const std::string &service_task_uuid) override;

protected:
  void OnActionDoneCallback(const ::actionlib::SimpleClientGoalState &state,
                            const ResultConstPtr &result,
                            const std::string &service_task_uuid);

  std::unordered_map<std::string, std::promise<trace::Outcome>>
      result_promises_;
  ::actionlib::SimpleActionClient<ActionType> actionlib_client_;
  ActionTranslatorType action_translator_;
  std::string action_topic_;
};

} // namespace ros_msgs_autocoder
} // namespace trace

#endif