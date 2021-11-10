#include <ros/ros.h>                        
#include <std_srvs/SetBool.h>                
#include <std_srvs/Trigger.h>                
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int8.h>

double wait_t=7.0;
double cov_diff;
bool ch_ler_flag=true;
bool ch_whi_flag=false;
std_msgs::Bool flag;
std_srvs::Trigger::Request req;             
std_srvs::Trigger::Response resp;           
std_srvs::SetBool::Request req_ler_str;            
std_srvs::SetBool::Request req_ler_end;            
std_srvs::SetBool::Response resp_ler;           
geometry_msgs::PoseWithCovarianceStamped pose_msg;
geometry_msgs::PoseWithCovarianceStamped amcl_pose_msg;
ros::ServiceClient Start_Wp_Client; 
ros::ServiceClient StartClient; 
ros::ServiceServer srv;
ros::Publisher initial_pose_pub;
ros::Publisher start_lea_pub;
ros::Publisher start_white_pub;
ros::Subscriber start_white_sub; 
ros::Subscriber swiching_sub;
ros::Subscriber white_suc_sub;
ros::Subscriber pose_sub;
boost::array<double , 36> old_pose;
boost::array<double , 36> current_pose;

void mode(double cov)
    {
        if (cov>=2.0&& ch_whi_flag==false)
        {
            // req_ler_str.data=true;
            ch_ler_flag=true;
            flag.data=true;
            ROS_INFO("learning_mode now!");

        }
        
        else
        {
            // req_ler_end.data=false;
            // bool start_learning = StartClient.call(req_ler_end,resp_ler); 
            //initial_pose_pub.publish(pose_msg);
            // bool  start_waypointnav = Start_Wp_Client.call(req,resp); 
            flag.data=false;
            ROS_INFO("waypoint_mode now!");
        }
        start_lea_pub.publish(flag);
    }

void Pose_Callback(const geometry_msgs::PoseWithCovarianceStamped &p)
    {
        double pose_x_cov=0;
                    // bool  start_waypointnav = Start_Wp_Clienall(req,resp);
        double pose_y_cov=0;
        double pose_xy_cov=0;
        pose_msg = p;
        // if(ch_flag)
        // {
        //     old_pose =  p.pose.covariance;
        //     ch_flag=false;
        // }
        // else
        // {
        //     current_pose = p.pose.covariance;
        //     for (int i=0; i!=current_pose.size(); i++)
        //     {
        //         cov_diff = current_pose[i] - old_pose[i];
        //     }
        pose_x_cov=pose_msg.pose.covariance[0];
        pose_y_cov=pose_msg.pose.covariance[7];
        pose_xy_cov=pose_x_cov + pose_y_cov;
        // ROS_INFO("%f", &current_pose);
        mode(pose_xy_cov);
        // old_pose = current_pose;
     
    }

void White_Callback(const std_msgs::Bool &suc)
    {
    if (suc.data==true){
       ch_whi_flag=false;
        // bool  start_learning = StartClient.call(req_ler_str,resp_ler);
      //  bool  start_waypointnav = Start_Wp_Clienall(req,resp);
       ROS_INFO("restart waypoint_mode!"); 
       //bool  start_waypointnav = Start_Wp_Client.call(req,resp);
    }
    else
        ROS_INFO("white_mode");
    }

void Start_White_Callback(const std_msgs::Bool &d)
    {
    if (d.data==true){
       ch_whi_flag=true;
       ROS_INFO("Start white_mode");
       }
    else
        ROS_INFO("way_mode");
    }

int main(int argc, char **argv) {
    ros::init(argc, argv, "swiching_by_time");        
    ros::NodeHandle nh;                    
    Start_Wp_Client = nh.serviceClient<std_srvs::Trigger>("start_wp_nav"); 
    StartClient = nh.serviceClient<std_srvs::SetBool>("learn_out");
    ROS_INFO("wait 7.0s!");
    ros::Duration(wait_t).sleep();
    ROS_INFO("ready!");
    start_lea_pub = nh.advertise<std_msgs::Bool>("start_learn", 10);
    start_white_pub = nh.advertise<std_msgs::Bool>("start_white", 10);
    pose_sub = nh.subscribe("amcl_pose", 10, &Pose_Callback);
    white_suc_sub=nh.subscribe("white_success", 10, &White_Callback);
    start_white_sub=nh.subscribe("start_white", 10, &Start_White_Callback);

    ros::spin();
    return 0;
}   

