# ros-msgs-autocoder

See Wiki in this repo for details about how to integrate ros-msgs-autocoder with your ROS system that uses TRACE 

### Purpose ### 
Minimizes code-writing repition when using TRACE to call robotic behaviors that are implemented using ROS. 

### Implementation ### 
Given ros Action msgs as input, this package will use mako to automatically create action clients communicate with the corresponding ros Action Server. 

(Largely a copy of the action client autogeneration capabilities from europa-lander-connector)
