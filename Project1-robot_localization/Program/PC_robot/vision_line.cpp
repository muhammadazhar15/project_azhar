#include "opencv2/opencv.hpp"

#include "image_transport/image_transport.h"
#include "boost/thread/mutex.hpp"
#include "cv_bridge/cv_bridge.h"
#include "angles/angles.h"
#include "ros/package.h"
#include "ros/ros.h"

#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Pose2D.h"
#include "std_msgs/UInt16MultiArray.h"

#include "iris_its/HsvThresholdInit.h"
#include "iris_its/HlsThresholdInit.h"
#include "iris_its/VisionLine.h"
#include "iris_its/config.h"
#include "iris_its/mcl.h"

#include <random>

short int res_x = 360;
short int res_y = 640;
double scan_res = 2.5;

short int offset_x;
short int offset_y;
short int r_cam;
short int scan_radius;


short int center_x;
short int center_y;

using namespace std;
using namespace cv;

//-----Timer
ros::Timer tim_50hz;

//-----Subscriber
image_transport::Subscriber sub_frame_bgr;
image_transport::Subscriber sub_frame_hsv;

ros::Subscriber sub_odometry_buffer;
ros::Subscriber sub_velocity;
ros::Subscriber sub_hsv_threshold;
ros::Subscriber sub_hls_threshold;
ros::Subscriber sub_final_field;

//-----Publisher
image_transport::Publisher pub_raw_threshold;
image_transport::Publisher pub_final_threshold;
image_transport::Publisher pub_display_out;
image_transport::Publisher pub_debuger;

ros::Publisher pub_vision_line;

//-----Service
ros::ServiceServer ser_hsv_threshold;
ros::ServiceServer ser_hls_threshold;

//-----Mutex
boost::mutex mutex_frame_bgr;
boost::mutex mutex_frame_hsv;
boost::mutex mutex_final_field;

//-----Threshold
unsigned short int hsv_threshold[6];
unsigned short int hls_threshold[6];
vector<double> regresi;
double white_l,white_h;


//-----Container
Mat frame_hsv = Mat::zeros(Size(res_x, res_y), CV_8UC3);
Mat frame_bgr = Mat::zeros(Size(res_x, res_y), CV_8UC3);

//-----Container
Mat line_threshold = Mat::zeros(Size(res_x, res_y), CV_8UC1);
Mat raw_threshold = Mat::zeros(Size(res_x, res_y), CV_8UC1);
Mat final_threshold = Mat::zeros(Size(res_x, res_y), CV_8UC1);
Mat final_field = Mat::zeros(Size(res_x, res_y), CV_8UC1);
Mat line_gray;

//-----Container
Mat display_in = Mat::zeros(Size(res_x, res_y), CV_8UC3);
Mat display_out = Mat::zeros(Size(res_x, res_y), CV_8UC3);

Mat matDebuger = Mat::zeros(Size(res_x, res_y), CV_8UC3);

random_device rd;
mt19937 gen(rd());
normal_distribution<double> xrg(-3.0,3.0), yrg(-3.0,3.0), angle_rg(-3.0,3.0);

std::vector<MCL::SensorData> mclSensorData;
iris_its::VisionLine visionLineMsg;

//-----Odometry
float pos_x, pos_y, theta;
float vx, vy, vw;

//-----Prototype
float pixel_to_cm(float _pixel);
void loadConfig();
void saveConfig();

/**
 * @brief Olah gambar dan deteksi garis putih
 * @return line_gray : Hasil final threshold garis putih
 */
void olahVisionLine()
{
    mutex_frame_hsv.lock();
    if (hsv_threshold[0] > hsv_threshold[1])
    {
        Mat dstA, dstB;
        inRange(frame_hsv, Scalar(0, hsv_threshold[2], hsv_threshold[4]), Scalar(hsv_threshold[1], hsv_threshold[3], hsv_threshold[5]), dstA);
        inRange(frame_hsv, Scalar(hsv_threshold[0], hsv_threshold[2], hsv_threshold[4]), Scalar(180, hsv_threshold[3], hsv_threshold[5]), dstB);
        bitwise_or(dstA, dstB, line_threshold);
    }
    else
    {
        inRange(frame_hsv, Scalar(hsv_threshold[0], hsv_threshold[2], hsv_threshold[4]), Scalar(hsv_threshold[1], hsv_threshold[3], hsv_threshold[5]), line_threshold);
    }
    mutex_frame_hsv.unlock();

    erode(line_threshold, line_threshold, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
    dilate(line_threshold, line_threshold, getStructuringElement(MORPH_ELLIPSE, Size(4, 4)));
    circle(line_threshold, Point(center_x, center_y), r_cam, Scalar(255), -1); //Mengabaikan lingkaran tengah
    circle(line_threshold, Point(center_x, center_y), 333, Scalar(0), r_cam);  //Mengabaikan lingkaran pinggir

    raw_threshold = line_threshold.clone();

    mutex_frame_bgr.lock();
    display_in = frame_bgr.clone();
    mutex_frame_bgr.unlock();
    line_threshold = Scalar(0);
    // imwrite("/home/iris//IRIS_RodaTiga/raw_image.jpg", display_in);

    // Contours and Hulls
    vector<vector<Point> > line_contour;
    vector<vector<Point> > line_hull;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    // Mencari kontur lapangan
    findContours(raw_threshold, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
    line_contour.resize(1);
    line_hull.resize(1);

    // Memilih kontur dengan luasan lebih dari 5000pixel^2
    // Erik ganti menjadi 1000
    for (int i = 0; i < contours.size(); i++)
        if (contourArea(contours[i]) > 5000)
            line_contour[0].insert(line_contour[0].end(), contours[i].begin(), contours[i].end());

    // Membuat convex hull dari gabungan contour yang sudah dipilih
    convexHull(line_contour[0], line_hull[0]);

    drawContours(line_threshold, line_hull, 0, Scalar(255), -1);
    drawContours(display_in, line_hull, 0, Scalar(0, 0, 255), 2);
    // line_threshold = Area kontur lapangan

    // Segmentasi garis dengan threshold grayscale biasa dan
    // menambahkan hasil threshold garis ke hasil segmentasi lapangan

    mutex_frame_bgr.lock();
    // cvtColor(frame_bgr, line_gray, CV_BGR2GRAY);
    cvtColor(frame_bgr, line_gray, CV_BGR2HLS);
    mutex_frame_bgr.unlock();

    // Makin tinggi makin sempit
    // inRange(line_gray,white_l,white_h,line_gray);

    // threshold(line_gray, line_gray, white_l, 255, THRESH_BINARY);
    inRange(line_gray, Scalar(hls_threshold[0], hls_threshold[2], hls_threshold[4]),Scalar(hls_threshold[1], hls_threshold[3], hls_threshold[5]), line_gray);
    dilate(line_gray, line_gray, getStructuringElement(MORPH_ELLIPSE, Size(2, 2)));
    
    circle(line_gray, Point(center_x, center_y), r_cam, Scalar(0), -1);  //Mengabaikan lingkaran tengah
    circle(line_gray, Point(center_x, center_y), 333, Scalar(0), r_cam); //Mengabaikan lingkaran pinggir

    // imwrite("/home/iris//IRIS_RodaTiga/line_detect.jpg", line_gray);
    // imwrite("/home/iris//IRIS_RodaTiga/field_detect.jpg", line_threshold);
    bitwise_and(line_threshold, line_gray, line_gray);
    // imwrite("/home/iris//IRIS_RodaTiga/final_threshold.jpg", line_gray);
    // imshow("field", line_threshold);
    // imshow("line", line_gray);
    // waitKey(5);
}

void cllbck_tim_50hz(const ros::TimerEvent &event)
{
    visionLineMsg.status = 0;
    matDebuger = Mat::zeros(Size(res_x, res_y), CV_8UC3);
    mclSensorData.clear();
    visionLineMsg.mclSensorData.clear();

    olahVisionLine();
    // flip(line_gray, line_gray, 1);

    // Menghitung jarak halangan dari pusat robot
    for (int angle = 0; angle < 360; angle+=scan_res)
    {
        int pixel = 0;
        int pixel_x = center_x;
        int pixel_y = center_y;
        int statusPutih = 0;
        int cnt_putih = 0;

        while (pixel_x >= 0 && pixel_y >= 0 && pixel_x < res_x && pixel_y < res_y && pixel <= scan_radius )
        {
            // Menemukan garis putih
            if (line_gray.at<unsigned char>(Point(pixel_x, pixel_y)) == 255 && statusPutih == 0  )
            {
                // -- Jarak robot ke garis putih Sudah di REGRESI supaya lapangan jadi kotak 
                float jarakInCm = pixel_to_cm(pixel);

                // -- Menghitung jarak x dari pusat robot
                float jarak_x = jarakInCm * cos(angles::from_degrees((float)angle + 0 )) + 0;
                // float jarak_x = jarakInCm * cos(angles::from_degrees((float)angle + angle_rg(gen) )) + xrg(gen);

                // -- Menghitung jarak y dari pusat robot
                float jarak_y = jarakInCm * sin(angles::from_degrees((float)angle + 0 )) + 0;
                // float jarak_y = jarakInCm * sin(angles::from_degrees((float)angle + angle_rg(gen) )) + yrg(gen);


                // -- Titik putih dengan posisi relative terhadap robot
                mclSensorData.push_back( MCL::SensorData( jarak_x, jarak_y) );

                // Berhenti pada loop sudut ini setelah menemukan garis putih
                statusPutih = 1;
                // break;

            }
            // Menemukan garis putih (LAGI)
            else if(line_gray.at<unsigned char>(Point(pixel_x, pixel_y)) == 255)
            {
                // do nothing
                statusPutih = 1;
            }
            // Nemu bukan putih
            else{
                // statusPutih = 0;
                if(cnt_putih > 5){
                    statusPutih = 0;
                    cnt_putih = 0;
                }
                else{
                    cnt_putih++;
                }
            }

            // Menentukan koordinat pindai selanjutnya
            pixel+=1;
            pixel_x = center_x + pixel * cos(angles::from_degrees((float)angle));
            pixel_y = center_y - pixel * sin(angles::from_degrees((float)angle));
        }

    }

    visionLineMsg.status = 1;

    for(auto point : mclSensorData)
    {
        geometry_msgs::Point p;
        p.x = point.first;
        p.y = point.second;
        p.z = 0;
        visionLineMsg.mclSensorData.push_back( p );

        circle(matDebuger, Point(res_x/2 + point.first, res_y/2 - point.second), 3, Scalar(255, 255, 255), -1);
    }

    visionLineMsg.status = 2;

    visionLineMsg.vx = vx;
    visionLineMsg.vy = vy;
    visionLineMsg.vw = vw;
    visionLineMsg.pose.x = pos_x;
    visionLineMsg.pose.y = pos_y;
    visionLineMsg.pose.theta = theta;

    // Jika tidak nemu garis, gausah dikirim
    if(visionLineMsg.mclSensorData.size())
        pub_vision_line.publish(visionLineMsg);

    // Draw Robot constanly at center with theta = 0
    circle(matDebuger, Point(res_x/2 , res_y/2), 10, Scalar(0, 0, 255), 2);
    line(matDebuger, Point(res_x/2, res_y/2), Point(res_x/2, res_y/2 - 20), Scalar(0, 0, 255), 2);

    // imshow("matDebuger", matDebuger);
    // imwrite("/home/iris//IRIS_RodaTiga/data_image.jpg", matDebuger);
    // waitKey(5);
    // rotate(matDebuger, matDebuger, ROTATE_180);
    resize(matDebuger, matDebuger, Size(0, 0), 0.5, 0.5);

    sensor_msgs::ImagePtr msg_vision_line = cv_bridge::CvImage(std_msgs::Header(), "bgr8", matDebuger).toImageMsg();
    pub_debuger.publish(msg_vision_line);

}

void cllbck_sub_frame_bgr(const sensor_msgs::ImageConstPtr &msg)
{
    mutex_frame_bgr.lock();
    frame_bgr = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
    mutex_frame_bgr.unlock();
}

void cllbck_sub_frame_hsv(const sensor_msgs::ImageConstPtr &msg)
{
    mutex_frame_hsv.lock();
    frame_hsv = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
    mutex_frame_hsv.unlock();
}

void cllbck_sub_odometry_buffer(const geometry_msgs::Pose2DConstPtr &msg)
{
    pos_x = msg->x;
    pos_y = msg->y;
    theta = msg->theta;
}

void cllbck_sub_hls_threshold(const std_msgs::UInt16MultiArrayConstPtr &msg)
{
    // Menyimpan nilai threshold HLS Garis
    // pada file konfigurasi pengolahan citra
    for (int i = 0; i < 6; i++)
        hls_threshold[i] = msg->data[i];
    
    saveConfig();

    // ROS_INFO("%d %d %d %d %d %d", hls_threshold[0], hls_threshold[1], hls_threshold[2], hls_threshold[3], hls_threshold[4], hls_threshold[5]);
}

bool cllbck_ser_hls_threshold(iris_its::HlsThresholdInit::Request &req, iris_its::HlsThresholdInit::Response &res)
{
    // Menyesuaikan nilai threshold HLS Garis
    // pada GUI dan program pengolahan citra
    for (int i = 0; i < 6; i++)
        res.hls_threshold.push_back(hls_threshold[i]);

    return true;
}

void cllbck_sub_velocity(const geometry_msgs::TwistConstPtr &msg)
{
    vx = msg->linear.x;
    vy = msg->linear.y;
    vw = msg->angular.z;
}

float pixel_to_cm(float _pixel)
{
    // Nilai regresi diambil dari config/vision.yaml
    double result = 0;
    for (int i = 0; i < regresi.size(); i++)
        result += (regresi[i] * pow(_pixel, (double)i));

    if (_pixel < r_cam)
    {
        // return 40;
        return 50; // Special for SIM

    }
    return result;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "vision_line");

    ros::NodeHandle NH;
    image_transport::ImageTransport IT(NH);
    ros::MultiThreadedSpinner MTS;

    loadConfig();

    // // prototipe
    //  namedWindow("track", WINDOW_AUTOSIZE);
    //  createTrackbar("Hlow", "track", &Hlow, 180);
    //  createTrackbar("Hhigh", "track", &Hhigh, 180);
    //  createTrackbar("Llow", "track", &Llow, 255);
    //  createTrackbar("Lhigh", "track", &Lhigh, 255);
    //  createTrackbar("Slow", "track", &Slow, 255);
    //  createTrackbar("Shigh", "track", &Shigh, 255);
     

    //-----Timer
    // tim_50hz = NH.createTimer(ros::Duration(0.02), cllbck_tim_50hz);
    tim_50hz = NH.createTimer(ros::Duration(0.001), cllbck_tim_50hz);

    //-----Subscriber
    sub_frame_bgr = IT.subscribe("vision_frame_bgr", 16, cllbck_sub_frame_bgr);
    sub_frame_hsv = IT.subscribe("vision_frame_hsv", 16, cllbck_sub_frame_hsv);
    sub_odometry_buffer = NH.subscribe("stm2pc/odometry_buffer", 16, cllbck_sub_odometry_buffer);
    sub_velocity = NH.subscribe("pc2stm/velocity", 16, cllbck_sub_velocity);
    sub_hls_threshold = NH.subscribe("vision_line/hls_threshold", 16, cllbck_sub_hls_threshold);

    //-----Publisher
    pub_raw_threshold = IT.advertise("vision_line/raw_threshold", 16);
    pub_final_threshold = IT.advertise("vision_line/final_threshold", 16);
    pub_display_out = IT.advertise("vision_line/display_out", 16);
    pub_debuger = IT.advertise("vision_line/debuger", 16);

    pub_vision_line = NH.advertise<iris_its::VisionLine>("vision_line/sensorData", 16);

    //-----Service
    // ser_hsv_threshold = NH.advertiseService("vision_field/hsv_threshold_init", cllbck_ser_hsv_threshold);
    ser_hls_threshold = NH.advertiseService("vision_line/hls_threshold_init", cllbck_ser_hls_threshold);

    ros::spin();
}

void loadConfig()
{
    Config config;
    config.load("color/field.yaml");
    config.parseKeyValue("hue_min", &hsv_threshold[0]);
    config.parseKeyValue("hue_max", &hsv_threshold[1]);
    config.parseKeyValue("saturation_min", &hsv_threshold[2]);
    config.parseKeyValue("saturation_max", &hsv_threshold[3]);
    config.parseKeyValue("value_min", &hsv_threshold[4]);
    config.parseKeyValue("value_max", &hsv_threshold[5]);

    config.load("color/linehls.yaml");
    config.parseKeyValue("hue_low", &hls_threshold[0]);
    config.parseKeyValue("hue_high", &hls_threshold[1]);
    config.parseKeyValue("light_low", &hls_threshold[2]);
    config.parseKeyValue("light_high", &hls_threshold[3]);
    config.parseKeyValue("satur_low", &hls_threshold[4]);
    config.parseKeyValue("satur_high", &hls_threshold[5]);

    config.load("color/line.yaml");
    config.parseKeyValue("white_l", &white_l);
    config.parseKeyValue("white_h", &white_h);

    config.load("vision.yaml");
    config.parseMapBegin("transform");
    config.parseKeyValue("offset_x", &offset_x);
    config.parseKeyValue("offset_y", &offset_y);
    config.parseKeyValue("r_cam", &r_cam);
    config.parseKeyValue("scan_radius", &scan_radius);
    config.parseMapEnd();

    center_x = (res_x / 2) + offset_x;
    center_y = (res_y / 2) + offset_y;

    config.parseMapBegin("line_to_cm");

    uint16_t max_orde;
    config.parseKeyValue("max_orde", &max_orde);
    regresi.clear();
    for (int i = 0; i <= max_orde; i++)
    {
        string index = to_string(i);
        string base = "orde_";
        double reg_value;
        config.parseKeyValue(base + index, &reg_value);
        regresi.push_back(reg_value);
    }

}

void saveConfig()
{
    Config config;

    config.emitKeyValue("hue_low", hls_threshold[0]);
    config.emitKeyValue("hue_high", hls_threshold[1]);
    config.emitKeyValue("light_low", hls_threshold[2]);
    config.emitKeyValue("light_high", hls_threshold[3]);
    config.emitKeyValue("satur_low", hls_threshold[4]);
    config.emitKeyValue("satur_high", hls_threshold[5]);

    config.save("color/linehls.yaml");
}