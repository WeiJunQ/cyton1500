/** Headers */

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>

/* ROS synchronization headers */
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/time_synchronizer.h>

/* Message publication */
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"
#include "std_msgs/Float64.h"
#include <std_msgs/Float64.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <dynamixel_controllers/SetSpeed.h>
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseStamped.h"
#include <geometry_msgs/PointStamped.h>

/* dYnamixel */
#include "dynamixel_controllers/SetSpeed.h"
#include "dynamixel_controllers/SetComplianceSlope.h"
#include "dynamixel_msgs/JointState.h"
#include "dynamixel_msgs/MotorStateList.h"

#include <sensor_msgs/JointState.h>

#include "tf/transform_listener.h"
#include "tf/message_filter.h"
#include "message_filters/subscriber.h"


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>                                         // for in-/output
#include <string.h>                                        // strcat
#include <fcntl.h>                                         // for 'O_RDONLY' deklaration
#include <termios.h>                                       // for serial

//Include system headers
#include <cstring>
#include <iostream>
#include <cstdio>

#include <sstream>
//#include <math.h>
//#include <algorithm>

#include <vector>

#define SHM_SIZE 1024
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ioctl.h>


using namespace std;
using namespace sensor_msgs;
using namespace message_filters;
	

/* publisher declaration */
ros::Publisher CJS_D;
ros::Publisher JS_D;				
void callback(const dynamixel_msgs::JointState::ConstPtr& msg1, const dynamixel_msgs::JointState::ConstPtr& msg2, 
						const dynamixel_msgs::JointState::ConstPtr& msg3, const dynamixel_msgs::JointState::ConstPtr& msg4, 
						const dynamixel_msgs::JointState::ConstPtr& msg5, const dynamixel_msgs::JointState::ConstPtr& msg6, 
						const dynamixel_msgs::JointState::ConstPtr& msg7, const dynamixel_msgs::JointState::ConstPtr& msg8, 
						const geometry_msgs::PoseStampedConstPtr& signal){


/* array declared for msg publish */
	std_msgs::Float32MultiArray JaG_data;
/* data clear */
	JaG_data.data.clear();

/* We are publishing total 15 variables in this array
	8 joints -- (8 values)
	3d position -- (3 values, tx,ty & tz)
	3d orientatin -- (4 values, quaternion) 
	joint_speed (8values)*/
/////////
geometry_msgs::PoseStamped aa;


aa.header.stamp = ros::Time::now();
		aa.header.frame_id = "/base8";


		aa.pose.position.x = msg1->current_pos; 
		aa.pose.position.y = msg2->current_pos; 
		aa.pose.position.z = msg3->current_pos;
		aa.pose.orientation.x = msg4->current_pos;
		aa.pose.orientation.y = msg5->current_pos;
		aa.pose.orientation.z = msg6->current_pos;
		aa.pose.orientation.w = msg7->current_pos;

		JS_D.publish(aa);  
	/////////////
/* Joint position states */
	JaG_data.data.push_back(msg1->current_pos);
	JaG_data.data.push_back(msg2->current_pos);
	JaG_data.data.push_back(msg3->current_pos);
	JaG_data.data.push_back(msg4->current_pos);
	JaG_data.data.push_back(msg5->current_pos);
	JaG_data.data.push_back(msg6->current_pos);
	JaG_data.data.push_back(msg7->current_pos);
	JaG_data.data.push_back(msg8->current_pos);

/* EE Position */
	JaG_data.data.push_back(signal->pose.position.x);
	JaG_data.data.push_back(signal->pose.position.y);
	JaG_data.data.push_back(signal->pose.position.z);
	
/* EE Orientation */
	JaG_data.data.push_back(signal->pose.orientation.x);
	JaG_data.data.push_back(signal->pose.orientation.y);
	JaG_data.data.push_back(signal->pose.orientation.z);
	JaG_data.data.push_back(signal->pose.orientation.w);

/* Joint speed states*/
	JaG_data.data.push_back(msg1->velocity);
	JaG_data.data.push_back(msg2->velocity);
	JaG_data.data.push_back(msg3->velocity);
	JaG_data.data.push_back(msg4->velocity);
	JaG_data.data.push_back(msg5->velocity);
	JaG_data.data.push_back(msg6->velocity);
	JaG_data.data.push_back(msg7->velocity);
	JaG_data.data.push_back(msg8->velocity);

/* joint angles error states*/
	JaG_data.data.push_back(msg1->error);
	JaG_data.data.push_back(msg2->error);
	JaG_data.data.push_back(msg3->error);
	JaG_data.data.push_back(msg4->error);
	JaG_data.data.push_back(msg5->error);
	JaG_data.data.push_back(msg6->error);
	JaG_data.data.push_back(msg7->error);
/* all the datas are put together and published	*/

	
	CJS_D.publish(JaG_data);


}	



int main(int argc, char** argv)
{
	ros::init(argc, argv, "Cyton_JS");
	ros::NodeHandle nh;

	//sleep(0.1);
/* topic subscription */
	message_filters::Subscriber<dynamixel_msgs::JointState> joint0_sub(nh, "/shoulder_roll_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint1_sub(nh, "/shoulder_pitch_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint2_sub(nh, "/shoulder_yaw_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint3_sub(nh, "/elbow_pitch_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint4_sub(nh, "/elbow_yaw_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint5_sub(nh, "/wrist_pitch_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint6_sub(nh, "/wrist_roll_controller/state", 1); //_registered
	message_filters::Subscriber<dynamixel_msgs::JointState> joint7_sub(nh, "/gripper_controller/state", 1); //_registered
	message_filters::Subscriber<geometry_msgs::PoseStamped> EE_sub(nh, "/Tpoints", 1); //_registered
	
/* synchronization policy*/
	typedef sync_policies::ApproximateTime<dynamixel_msgs::JointState, dynamixel_msgs::JointState, 
											dynamixel_msgs::JointState, dynamixel_msgs::JointState, 
											dynamixel_msgs::JointState, dynamixel_msgs::JointState, 
											dynamixel_msgs::JointState, dynamixel_msgs::JointState, 
											geometry_msgs::PoseStamped> MySyncPolicy; 
	
/** ApproximateTime takes a queue size as its constructor argument, hence MySyncPolicy(10)*/
	Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), joint0_sub, joint1_sub, 
														joint2_sub, joint3_sub, 
														joint4_sub, joint5_sub, 
														joint6_sub, joint7_sub, 
														EE_sub);
														
	sync.registerCallback(boost::bind(&callback, _1, _2, _3, _4, _5, _6, _7, _8, _9));
	
	CJS_D = nh.advertise<std_msgs::Float32MultiArray>("/CJS_D", 1);
	//////////

	JS_D = nh.advertise<geometry_msgs::PoseStamped>("/JS_D", 1);
///////////
	ros::spin();
	
	return 0;
};

