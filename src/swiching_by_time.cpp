#include <ros/ros.h>                        // ROSヘッダファイル
#include <std_srvs/SetBool.h>                // サービスヘッダファイル
#include <std_srvs/Trigger.h>                // サービスヘッダファイル
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <std_msgs/Float32.h>
//servise callback
//flag == true
bool ch_flag =false;
//float pose_x,pose_y,pose_z,ori_x,ori_y,ori_z,ori_w;
double wait_t;

std_srvs::Trigger::Request req;             // リクエストの生成
std_srvs::Trigger::Response resp;           // レスポンスの生成
std_srvs::SetBool::Request req_ler_str;             // リクエストの生成
std_srvs::SetBool::Request req_ler_end;             // リクエストの生成
std_srvs::SetBool::Response resp_ler;           // レスポンスの生成
geometry_msgs::PoseWithCovarianceStamped pose_msg;
ros::ServiceClient Start_Wp_Client; 
ros::ServiceClient StartClient; 
ros::ServiceServer srv;
ros::Publisher initial_pose_pub; 
ros::Subscriber swiching_sub;
void pose_set(float pose_x,float pose_y,float ori_z,float ori_w)
    {
    pose_msg.header.stamp = ros::Time::now();
    pose_msg.header.frame_id = "map";
    pose_msg.pose.pose.position.x = pose_x;
    pose_msg.pose.pose.position.y = pose_y;
    pose_msg.pose.covariance[0] = 0.25;
    pose_msg.pose.covariance[6 * 1 + 1] = 0.25;
    pose_msg.pose.covariance[6 * 5 + 5] = 0.06853891945200942;
    //pose_msg.pose.pose.orientation.z = sin(alpha/2);
    //pose_msg.pose.pose.orientation.w = cos(alpha/2);
    pose_msg.pose.pose.orientation.z = ori_z;
    pose_msg.pose.pose.orientation.w = ori_w;
 
    } 

void mode(double t)
    {
        req_ler_str.data=true;
        bool  start_learning = StartClient.call(req_ler_str,resp_ler); //学習の開始
        ROS_INFO("learning_mode!");
        ros::Duration(t).sleep();
        req_ler_end.data=false;
        start_learning = StartClient.call(req_ler_end,resp_ler); //学習の終了
       // initial_pose_pub.publish(pose_msg);
        bool  start_waypointnav = Start_Wp_Client.call(req,resp); //way restart
        ROS_INFO("waypoint_mode!");
    }
void CallBack(const std_msgs::Float32& msg)
 {
        if(msg.data==1)
        {
        wait_t = 5.0;
        pose_set(-63.35,-105.17,0.96,0.279);
        }
        if(msg.data==2)
        {
        wait_t = 10.0;
        pose_set(1,1,0.96,0.279);
        }
    
        mode(wait_t);
 }

/* サーバー */
int main(int argc, char **argv) {
    ros::init(argc, argv, "swiching_by_time");        // ノードの初期化（ノード名を"srv_test"に設定
    ros::NodeHandle nh;                    // ノードハンドルの生成
    initial_pose_pub = nh.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 10);
    Start_Wp_Client = nh.serviceClient<std_srvs::Trigger>("start_wp_nav");  // クライアントの生成
    StartClient = nh.serviceClient<std_srvs::SetBool>("learn_out");  // クライアントの生成
    ROS_INFO("ready!");
    swiching_sub = nh.subscribe("swiching", 10, &CallBack);
    //srv = nh.advertiseService("start_call", &serviceCallBack);  // サーバー生成とコールバック関数の登録
    ros::spin();                              // リクエストの無限待ち
  return 0;
}   

