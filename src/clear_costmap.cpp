#include <ros/ros.h>                        // ROSヘッダファイル
#include <std_srvs/SetBool.h>                // サービスヘッダファイル
#include <std_srvs/Trigger.h>                // サービスヘッダファイル
#include <std_srvs/Empty.h>                // サービスヘッダファイル
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float32.h>

double wait_t,last_moved_time_; 
ros::ServiceClient clear_costmaps_srv_;  
//ros::Publisher initial_pose_pub; 
ros::Subscriber swiching_sub,cmd_vel_sub;


void mode(double t)
    {
        double time = ros::Time::now().toSec();
        if(time - last_moved_time_ > t) {
                            ROS_WARN("Clear_costmap");
                            std_srvs::Empty empty;
                            clear_costmaps_srv_.call(empty);
        }
        else
            ROS_INFO("move_now!?");
                         
    }
void Cmd_vel_Callback(const geometry_msgs::Twist& cmd_vel_msg){
    if(cmd_vel_msg.linear.x > -0.001 && cmd_vel_msg.linear.x < 0.001  &&
    cmd_vel_msg.linear.y > -0.001 && cmd_vel_msg.linear.y < 0.001   &&
    cmd_vel_msg.linear.z > -0.001 && cmd_vel_msg.linear.z < 0.001   &&
    cmd_vel_msg.angular.x > -0.001 && cmd_vel_msg.angular.x < 0.001 &&
    cmd_vel_msg.angular.y > -0.001 && cmd_vel_msg.angular.y < 0.001 &&
    cmd_vel_msg.angular.z > -0.001 && cmd_vel_msg.angular.z < 0.001){
    
    ROS_INFO("command velocity all zero");
  }
  else{
    last_moved_time_ = ros::Time::now().toSec();
  }
  mode(100);
}
int main(int argc, char **argv) {
    ros::init(argc, argv, "clear_costmap");
    ros::NodeHandle nh;
    ROS_INFO("ready!"); 
    cmd_vel_sub = nh.subscribe("/nav_vel", 10, &Cmd_vel_Callback);//subscribe topic "swiching" from waypoint_nav
    clear_costmaps_srv_ = nh.serviceClient<std_srvs::Empty>("/move_base/clear_costmaps");
    ros::spin();                           
  return 0;
}   

