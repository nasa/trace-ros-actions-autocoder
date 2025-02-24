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

#include "ros_msgs_autocoder/action_client.h"
#include "autocoded_proj_files/ros_action_clients.h"

namespace trace {
namespace ros_msgs_autocoder {

template <class ActionType, class ActionTranslatorType>
ActionClient<ActionType, ActionTranslatorType>::ActionClient(
    const std::string &action_topic)
    : actionlib_client_(action_topic, true), action_topic_(action_topic) {}

template <class ActionType, class ActionTranslatorType>
void ActionClient<ActionType, ActionTranslatorType>::Connect() {
  std::cout << "Waiting for actionlib server @ " << this->action_topic_ << "..."
            << std::endl;
  this->actionlib_client_.waitForServer();
  std::cout << "...done." << std::endl;
}

template <class ActionType, class ActionTranslatorType>
std::future<trace::Outcome>
ActionClient<ActionType, ActionTranslatorType>::SendCommand(
    const std::string &service_task_uuid, const PropertyMap &properties) {

  this->result_promises_[service_task_uuid] =
      std::promise<trace::Outcome>(); // new promise

  Connect();

  this->actionlib_client_.sendGoal(
      this->action_translator_.ToGoal(properties),
      boost::bind(&ActionClient::OnActionDoneCallback, this, _1, _2,
                  service_task_uuid));

  return this->result_promises_.at(service_task_uuid).get_future();
}

template <class ActionType, class ActionTranslatorType>
void ActionClient<ActionType, ActionTranslatorType>::AbortCommand(
    const std::string &service_task_uuid) {
  this->actionlib_client_.cancelAllGoals();
}

template <class ActionType, class ActionTranslatorType>
void ActionClient<ActionType, ActionTranslatorType>::OnActionDoneCallback(
    const ::actionlib::SimpleClientGoalState &state,
    const ResultConstPtr &result, const std::string &service_task_uuid) {
  if (state == ::actionlib::SimpleClientGoalState::SUCCEEDED) {
    trace::Outcome ok(trace::StatusCode::OK,
                      "Activity completed successfully.");
    for (auto kv_pair : this->action_translator_.FromResult(*result)) {
      ok.add_result(kv_pair.first, kv_pair.second);
    }
    this->result_promises_.at(service_task_uuid).set_value(ok);
  } else if (state == ::actionlib::SimpleClientGoalState::ABORTED) {
    trace::Outcome error(trace::StatusCode::ERROR,
                         "Activity completed unsuccessfully!");
    for (auto kv_pair : this->action_translator_.FromResult(*result)) {
      error.add_result(kv_pair.first, kv_pair.second);
    }
    this->result_promises_.at(service_task_uuid).set_value(error);
  } else {
    this->result_promises_.at(service_task_uuid)
        .set_value(trace::Outcome(trace::StatusCode::CANCELLED,
                                  "Connector activity was cancelled."));
  }
}

} // namespace europa_lander
} // namespace trace

