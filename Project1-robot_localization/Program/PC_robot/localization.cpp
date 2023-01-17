#include <opencv2/opencv.hpp>

#include "ros/ros.h"
#include "ros/package.h"
#include "angles/angles.h"
#include "cv_bridge/cv_bridge.h"
#include "image_transport/image_transport.h"

#include "geometry_msgs/Pose2D.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/Float32.h" //bojar

#include "iris_its/VisionLine.h"
#include "iris_its/mcl.h"

#include <stdexcept>
#include <string>
#include <math.h>
#include <random>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define SEND_PORT 8787

#define TO_RAD M_PI/180.0
#define TO_DEG 180.0/M_PI

using namespace cv;
using namespace std;

//socket
struct sockaddr_in remaddr;	/* send address */
socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
int fd;				/* our socket */
unsigned char buf_tx[10];

// -------- Global Variable
MCL mcl;
std::vector<MCL::SensorData> mclSensorData;
struct RobotPose
{
    float x = 0;
    float y = 0;
    float theta = 0;
};
RobotPose robotReal;
RobotPose robotEstimation;
RobotPose robotWeightedEstimation;
RobotPose robotOdo;

int res_x = 600;
int res_y = 900;
float pos_x, pos_y, theta;
float x_before, y_before, w_before;
float dx =0, dy=0, dw=0;
float vx, vy, vw;
//////////bojar///
short int status_init_partikel; //untuk inisialisai partikel
short int status_init_weight; //untuk inisialisai partikel
short int status_data_amcl; //
short int status_mode;
float compas_robot = 90.0;
float kompas_robot; //untuk coba-coba
geometry_msgs::Pose2D msg_amcl;
geometry_msgs::Pose2D msg_amcloffset;
geometry_msgs::Pose2D msg_odomoffset;

// -------- Publisher
image_transport::Publisher pub_debuger;
ros::Publisher pub_amcl;
ros::Publisher pub_amcloffset;

// -------- Timer, Subscriber, Publisher
ros::Timer tim_1Khz;
ros::Timer tim_50hz;
ros::Subscriber sub_vision_line;
ros::Subscriber sub_odometry_buffer;
ros::Subscriber sub_velocity;
ros::Subscriber sub_kompas; //bojar

// -------- Image Variable
string mapPath = ros::package::getPath("iris_vision") + "/Regional69.png";
Mat matDebuger = Mat::zeros(Size(900, 600), CV_8UC3);

// -------- Prototype
void updateOdometry( float x, float y, float w);

void drawField(RobotPose r1, RobotPose r2, float robot_compas){
    matDebuger = imread(mapPath);
    rotate(matDebuger, matDebuger, ROTATE_90_CLOCKWISE);

    int drawx, drawy;
    int xline, yline;
    int offset = 100;

    // ---------- Draw Particles [BLACK]
    for(auto p : mcl.getParticles()){
        drawx = (int)MCL::x(p) + offset;
        drawy = (int)MCL::y(p) + offset;
        circle(matDebuger, Point(drawx, drawy), 7, Scalar(0, 0, 0), 2);
    }

    // ---------- Draw Robot Real [RED]
    drawx = (int)r1.y + offset;
    drawy = (int)r1.x + offset;
    circle(matDebuger, Point(drawx, drawy), 20, Scalar(0, 0, 255), 2);
    xline = 20 * cos(angles::from_degrees(r1.theta-90));
    yline = 20 * -sin(angles::from_degrees(r1.theta-90));
    line(matDebuger, Point(drawx, drawy), Point(drawx + xline, drawy + yline), Scalar(0, 0, 255), 2);

    // ---------- Draw Robot Weighted Estimation [BLUE]
    drawx = (int)r2.x + offset;
    drawy = (int)r2.y + offset;
    circle(matDebuger, Point(drawx, drawy), 20, Scalar(255, 0, 0), 2);
    xline = 20 * cos(angles::from_degrees(r2.theta-90));
    yline = 20 * -sin(angles::from_degrees(r2.theta-90));
    line(matDebuger, Point(drawx, drawy), Point(drawx + xline, drawy + yline), Scalar(255, 0, 0), 2);

    // ---------- Draw Robot compas heading real and with error [Blue]
    drawx = 60;
    drawy = 760;
    circle(matDebuger, Point(drawx, drawy), 30, Scalar(0, 0, 255), 2);
    putText(matDebuger, "N", Point(drawx+33, drawy+8), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255), 2, LINE_AA);
    ///////compas/////
    xline = 30 * cos(angles::from_degrees(robot_compas-90));
    yline = 30 * -sin(angles::from_degrees(robot_compas-90));
    line(matDebuger, Point(drawx, drawy), Point(drawx + xline, drawy + yline), Scalar(255, 0, 0), 2);

    resize(matDebuger, matDebuger, Size(0, 0), 0.5, 0.5);
    
    // imshow("Field of Localization", matDebuger);
    // waitKey(20);

    sensor_msgs::ImagePtr msg_localization = cv_bridge::CvImage(std_msgs::Header(), "bgr8", matDebuger).toImageMsg();
    pub_debuger.publish(msg_localization);
}

void cllbck_tim_50hz(const ros::TimerEvent& event)
{
    drawField(robotReal, robotWeightedEstimation, compas_robot);
    // drawField(robotOdo, robotWeightedEstimation, compas_robot);
}

void cllbck_tim_1Khz(const ros::TimerEvent& event)
{
    // ROS_INFO("status data amcl %d",status_data_amcl);
    // ROS_INFO("x:%f y:%f z:%f",pos_x,pos_y, theta);
    if(status_data_amcl == 1){
        msg_amcl.x = robotReal.x;
        msg_amcl.y = robotReal.y;
        msg_amcl.theta = robotReal.theta;
        pub_amcl.publish(msg_amcl);
        status_data_amcl = 0;
    }
    else{
        msg_amcl.x = robotReal.x*0.1 + (msg_amcloffset.x+pos_x)*0.9;
        msg_amcl.y = robotReal.y*0.1 + (msg_amcloffset.y+pos_y)*0.9;
        double w_mean_sin = (sin(robotReal.theta*TO_RAD)*0.1 + sin((msg_amcloffset.theta-theta)*TO_RAD)*0.9);
        double w_mean_cos = (cos(robotReal.theta*TO_RAD)*0.1 + cos((msg_amcloffset.theta-theta)*TO_RAD)*0.9);
        double w_mean = atan2(w_mean_sin,w_mean_cos);
        w_mean = w_mean * TO_DEG;
        msg_amcl.theta = w_mean;
        pub_amcl.publish(msg_amcl);
    }
}

void cllbck_vision_line(const iris_its::VisionLine::ConstPtr& msg)
{
    // ROS_INFO("x:%f  y:%f  z:%f", robotReal.x,robotReal.y,robotReal.theta);
    // ROS_INFO("Kompas = %f", compas_robot);
    // ROS_INFO("%f", mcl.p_maxWeight);
    // ROS_INFO("n_data = %d", mcl.n_dataSensor);
    // robotOdo.x = msg->pose.x;
    // robotOdo.y = msg->pose.y;
    // robotOdo.theta = 90-msg->pose.theta;

    ////inisialisasi lokasi partikel/////
    if(status_init_partikel == 0){
        mcl.initParticles(compas_robot);
        status_init_partikel = 1;
    }

    mclSensorData.clear();
    for(auto point : msg->mclSensorData)
    {
        MCL::SensorData data( point.x, point.y );
        mclSensorData.push_back( data );
    }
    // ROS_INFO("[VisionLine] Received %lu sensor data", mclSensorData.size());
    // ROS_INFO("%f %f %f ", dx, dy,dw);

    updateOdometry(msg->pose.x, msg->pose.y, msg->pose.theta);

    ///////-------hanya untuk ambil data mode odmetry dan omnivision
    if(status_init_weight == 0){
        mcl.updateMotion(-(double)dy, -(double)dx, (double)dw);
        mcl.updateSensor( mclSensorData, compas_robot );
        auto weighted_estimation = mcl.weighted_estimation();
        // ------ drawField variable
        robotWeightedEstimation.x = std::get<0>(weighted_estimation);
        robotWeightedEstimation.y = std::get<1>(weighted_estimation);
        robotWeightedEstimation.theta = std::get<2>(weighted_estimation);
        if(mcl.p_maxWeight > 0.1){
            robotReal.x = robotWeightedEstimation.y;
            robotReal.y = robotWeightedEstimation.x;
            robotReal.theta = robotWeightedEstimation.theta;
            //untuk offset
            msg_odomoffset.x = msg->pose.x;
            msg_odomoffset.y = msg->pose.y;
            msg_odomoffset.theta = msg->pose.theta;
            msg_amcloffset.x = robotReal.x - msg_odomoffset.x;
            msg_amcloffset.y = robotReal.y - msg_odomoffset.y;
            msg_amcloffset.theta = robotReal.theta + msg_odomoffset.theta;
            pub_amcloffset.publish(msg_amcloffset);
            status_init_weight = 1;
        }
        
    }
    else{
        mcl.updateMotion(-(double)dy, -(double)dx, (double)dw);
        mcl.updateSensor( mclSensorData, compas_robot );

        auto weighted_estimation = mcl.weighted_estimation();
        // ------ drawField variable
        robotWeightedEstimation.x = std::get<0>(weighted_estimation);
        robotWeightedEstimation.y = std::get<1>(weighted_estimation);
        robotWeightedEstimation.theta = std::get<2>(weighted_estimation);

        if((mcl.p_maxWeight > 10 && mcl.n_dataSensor > 130 && abs(msg->vx)<15 && abs(msg->vy)<15 && abs(msg->vw)<5 && robotReal.x>0 && robotReal.x<600 && robotReal.y>0 && robotReal.y<900)){
            if(abs((robotReal.x - dx)-robotWeightedEstimation.y)>250 || abs((robotReal.y - dy)-robotWeightedEstimation.x)>250 || abs((robotReal.theta)-robotWeightedEstimation.theta)>90 ){
                double cek_odo = mcl.weightPosition(mclSensorData, robotReal.y - dy, robotReal.x - dx, robotReal.theta);
                double cek_amcl = mcl.weightPosition(mclSensorData, robotWeightedEstimation.x, robotWeightedEstimation.y, robotWeightedEstimation.theta);
                // ROS_INFO("odo:%f mcl:%f", cek_odo,cek_amcl);
                if(cek_odo > cek_amcl || cek_amcl < 20){
                    robotReal.x = (msg_amcloffset.x) + msg->pose.x;
                    robotReal.y = (msg_amcloffset.y) + msg->pose.y;
                    robotReal.theta = (msg_amcloffset.theta) - msg->pose.theta;
                    mcl.status_kidnap = 1;
                    status_mode = 1;
                }
                else{
                    robotReal.x = robotWeightedEstimation.y;
                    robotReal.y = robotWeightedEstimation.x;
                    robotReal.theta = robotWeightedEstimation.theta;
                    //untuk offset
                    msg_odomoffset.x = msg->pose.x;
                    msg_odomoffset.y = msg->pose.y;
                    msg_odomoffset.theta = msg->pose.theta;
                    msg_amcloffset.x = robotReal.x - msg_odomoffset.x;
                    msg_amcloffset.y = robotReal.y - msg_odomoffset.y;
                    msg_amcloffset.theta = robotReal.theta + msg_odomoffset.theta;
                    pub_amcloffset.publish(msg_amcloffset);
                    status_mode = 0;
                }
            }
            else{
                robotReal.x = 0.9*((msg_amcloffset.x) + msg->pose.x) + 0.1*robotWeightedEstimation.y;
                robotReal.y = 0.9*((msg_amcloffset.y) + msg->pose.y) + 0.1*robotWeightedEstimation.x;
                robotReal.theta = (msg_amcloffset.theta) - msg->pose.theta;
                double w_mean_sin = (sin(robotReal.theta*TO_RAD)*0.9 + sin(robotWeightedEstimation.theta*TO_RAD)*0.1);
                double w_mean_cos = (cos(robotReal.theta*TO_RAD)*0.9 + cos(robotWeightedEstimation.theta*TO_RAD)*0.1);
                double w_mean = atan2(w_mean_sin,w_mean_cos);
                w_mean = w_mean * TO_DEG;
                robotReal.theta = w_mean;
                ///untuk offset
                msg_odomoffset.x = msg->pose.x;
                msg_odomoffset.y = msg->pose.y;
                msg_odomoffset.theta = msg->pose.theta;
                msg_amcloffset.x = robotReal.x - msg_odomoffset.x;
                msg_amcloffset.y = robotReal.y - msg_odomoffset.y;
                msg_amcloffset.theta = robotReal.theta + msg_odomoffset.theta;
                pub_amcloffset.publish(msg_amcloffset);
                status_mode = 0;
            } 
        }
        else{
            // robotReal.x = robotReal.x - dx;
            // robotReal.y = robotReal.y - dy;
            // robotReal.theta = robotReal.theta;
            robotReal.x = (msg_amcloffset.x) + msg->pose.x;
            robotReal.y = (msg_amcloffset.y) + msg->pose.y;
            robotReal.theta = (msg_amcloffset.theta) - msg->pose.theta;
            status_mode = 1;
        }
    }
    //// ROS_INFO("theta amcl %f", robotWeightedEstimation.theta);
    //// ROS_INFO("x:%f y:%f theta:%f",(msg_amcloffset.x-msg_odomoffset.x) + msg->pose.x,(msg_amcloffset.y-msg_odomoffset.y) + msg->pose.y,(msg_amcloffset.theta+msg_odomoffset.theta) - msg->pose.theta);
    //// ROS_INFO("%f", msg->pose.theta);

    // ///--------hanya untuk ambil data mode odometry
    // static short int status_offset_offset; //offset luknut
    // if(status_offset_offset == 0){
    //     msg_amcloffset.x = pos_x - 14;
    //     msg_amcloffset.y = pos_y + 14;
    //     msg_amcloffset.theta = robotReal.theta;
    //     pub_amcloffset.publish(msg_amcloffset);
    //     status_offset_offset = 1;
    // }
    // robotReal.x = pos_x;
    // robotReal.y = pos_y;
    // robotReal.theta = -theta;

    // ////////--------hanya untuk ambil data mode omnivision
    // mcl.updateMotion(-(double)dy, -(double)dx, (double)dw);
    // mcl.updateSensor( mclSensorData, compas_robot );
    // auto weighted_estimation = mcl.weighted_estimation();
    // // ------ drawField variable
    // robotWeightedEstimation.x = std::get<0>(weighted_estimation);
    // robotWeightedEstimation.y = std::get<1>(weighted_estimation);
    // robotWeightedEstimation.theta = std::get<2>(weighted_estimation);
    // robotReal.x = robotWeightedEstimation.y;
    // robotReal.y = robotWeightedEstimation.x;
    // robotReal.theta = robotWeightedEstimation.theta;
    // msg_odomoffset.x = msg->pose.x;
    // msg_odomoffset.y = msg->pose.y;
    // msg_odomoffset.theta = msg->pose.theta;
    // msg_amcloffset.x = robotReal.x - msg_odomoffset.x;
    // msg_amcloffset.y = robotReal.y - msg_odomoffset.y;
    // msg_amcloffset.theta = robotReal.theta + msg_odomoffset.theta;
    // pub_amcloffset.publish(msg_amcloffset);


    // ------ Publish AMCL Value
    status_data_amcl = 1;
    ///untuk data kamera atas
    memcpy(buf_tx, &robotReal.x, 4);
	memcpy(buf_tx + 4, &robotReal.y, 4);
    memcpy(buf_tx + 8, &status_mode, 2);
	sendto(fd, (void*)buf_tx, sizeof(buf_tx), 0, (struct sockaddr *)&remaddr, addrlen);
}

void updateOdometry( float x, float y, float w)
{
    if( x_before != 0.0 && y_before != 0.0 ){
        dx = x_before - x;
        dy = y_before - y;
        dw = w_before - w;
    }

    x_before = x;
    y_before = y;
    w_before = w;

}

void cllbck_sub_velocity(const geometry_msgs::TwistConstPtr &msg)
{
    vx = msg->linear.x;
    vy = msg->linear.y;
    vw = msg->angular.z;
}

void cllbck_sub_odometry_buffer(const geometry_msgs::Pose2DConstPtr &msg)
{
    pos_x = msg->x;
    pos_y = msg->y;
    theta = msg->theta;
}

//bojar
void cllbck_sub_kompas(const std_msgs::Float32ConstPtr &msg)
{
    kompas_robot = 180 - msg->data;
    if(kompas_robot < 0) kompas_robot = 360+kompas_robot;
    compas_robot = kompas_robot + 10.0; //offset gedung robotika +10 (+180 jika untuk pindah gawang)
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "localization");

    ros::NodeHandle NH;
    ros::MultiThreadedSpinner MTS;
    image_transport::ImageTransport IT(NH);

    /* create a UDP socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}
    // untuk send data udp
    memset((char *)&remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(SEND_PORT);
    // remaddr.sin_addr.s_addr = inet_addr("172.16.80.189");
    remaddr.sin_addr.s_addr = inet_addr("172.16.80.104");

    // -----Timer
    tim_50hz = NH.createTimer(ros::Duration(0.1), cllbck_tim_50hz);
    tim_1Khz = NH.createTimer(ros::Duration(0.001), cllbck_tim_1Khz);

    // -----Subscriber
    sub_vision_line = NH.subscribe("vision_line/sensorData", 16, cllbck_vision_line);
    sub_velocity = NH.subscribe("pc2stm/velocity", 16, cllbck_sub_velocity);
    sub_odometry_buffer = NH.subscribe("stm2pc/odometry_buffer", 16, cllbck_sub_odometry_buffer);
    sub_kompas = NH.subscribe("stm2pc/kompas", 16, cllbck_sub_kompas); //bojar

    // -----Publisher
    pub_debuger = IT.advertise("localization/debuger", 16);
    pub_amcl = NH.advertise<geometry_msgs::Pose2D>("localization/amclPose", 16);
    pub_amcloffset = NH.advertise<geometry_msgs::Pose2D>("localization/amclOffset", 16);

    MTS.spin();
    return 0;
}