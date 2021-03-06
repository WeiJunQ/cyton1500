
	
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
	// ROS synchronization headers
	#include <message_filters/subscriber.h>
	#include <message_filters/synchronizer.h>
	#include <message_filters/sync_policies/approximate_time.h>
	#include <message_filters/time_synchronizer.h>
	// ROS headers
	#include <ros/ros.h>
	#include <sensor_msgs/Image.h>
	#include <sensor_msgs/CameraInfo.h>
	#include <sensor_msgs/PointCloud2.h>
	#include <image_transport/image_transport.h>
	#include <sensor_msgs/image_encodings.h>
	// OpenCV headers
	#include <cv_bridge/cv_bridge.h>
	#include <opencv2/imgproc/imgproc.hpp>
	#include <opencv2/highgui/highgui.hpp>
	#include <opencv2/core/core.hpp>
	#include <opencv2/features2d/features2d.hpp>
    // c++ headers
	#include <iostream>
	#include <sstream>
	#include <string>
	#include <cmath>
	#include <math.h>  
	// pcl headers
	#include <pcl_ros/point_cloud.h>
	#include <pcl/point_types.h>
	#include <pcl/filters/voxel_grid.h>
	#include <boost/foreach.hpp>
	#include <pcl/io/pcd_io.h>
	// visualization header
	#include <visualization_msgs/Marker.h>


// publisher defined
	ros::Publisher marker_pub ;
	ros::Publisher pubPos;
	// point cloud definition
	typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;
	
	// standard namespace for c++, ROS messages transport, ROS message filters, OpenCV 
	using namespace std;
	using namespace sensor_msgs;
	using namespace message_filters;
	using namespace cv;

	//names that will appear at the top of each window
	static const std::string windowName1 = "Original RGB Image";
	static const std::string windowName2 = "Original Depth Image";
	static const std::string windowName3 = "Thresholded Image with object";
	static const std::string windowName4 = "RGB image with object tracking";
	
	// Trackbar Wondow
	static const std::string trackbarWindowName = "Trackbars";
	
	//initial min and max HSV filter values.
	//these will be changed using trackbars

	/*// object 'x'.
	int H_MIN = 0;
	int H_MAX = 256;
	int S_MIN = 0;
	int S_MAX = 256;
	int V_MIN = 0;
	int V_MAX = 256;*/
	
	//// obj red.
	/*
	int H_MIN = 154;
	int H_MAX = 179;
	int S_MIN = 177;
	int S_MAX = 255;
	int V_MIN = 86;
	int V_MAX = 177;*/

	//// obj blue.
/*
	int H_MIN = 91;
	int H_MAX = 132;
	int S_MIN = 119;
	int S_MAX = 184;
	int V_MIN = 112;
	int V_MAX = 231;
	*/
	//// obj green.
	int H_MIN = 54;
	int H_MAX = 93;
	int S_MIN = 65;
	int S_MAX = 168;
	int V_MIN = 61;
	int V_MAX = 129;
	

	// Color camera parameters 
	double fx_rgb = 5.2921508098293293e+02;
	double fy_rgb = 5.2556393630057437e+02;
	double cx_rgb = 3.2894272028759258e+02;
	double cy_rgb = 2.6748068171871557e+02;
	double k1_rgb = 2.6451622333009589e-01;
	double k2_rgb = -8.3990749424620825e-01;
	double p1_rgb = -1.9922302173693159e-03;
	double p2_rgb = 1.4371995932897616e-03;
	double k3_rgb = 9.1192465078713847e-01;
	
	// Depth camera parameters 
	double fx_d = 5.9421434211923247e+02;
	double fy_d = 5.9104053696870778e+02;
	double cx_d = 3.3930780975300314e+02;
	double cy_d = 2.4273913761751615e+02;
	double k1_d = -2.6386489753128833e-01;
	double k2_d = 9.9966832163729757e-01;
	double p1_d = -7.6275862143610667e-04;
	double p2_d = 5.0350940090814270e-03;
	double k3_d = -1.3053628089976321e+00;
	
	double cx = 320.0; // center of projection
	double cy = 240.0; // center of projection
	double fx = 600.0; // focal length in pixels
	double fy = 600.0; // focal length in pixels
	
	double k_gamma[2048]; // array of 2048 elements 
	//double k_gamma[4096]; // array of 2048 elements 
	
	// Rotation matrix for depth to RGB conversion 
	double R[3][3] = { { 0.99984628826577793, 	 0.0012635359098409581,  -0.017487233004436643},
					   {-0.0014779096108364480,  0.99992385683542895,    -0.012251380107679535},
					   { 0.017470421412464927,   0.012275341476520762,    0.99977202419716948 }};
			
	// Translation matrix for depth to RGB conversion 
	double T[3][1] = { {0.019985242312092553}, {-0.00074423738761617583}, {-0.010916736334336222} };
	
	//default capture width and height
	const int FRAME_WIDTH = 640;
	const int FRAME_HEIGHT = 480;
	
	//max number of objects to be detected in frame
	const int MAX_NUM_OBJECTS=2;
	
	//minimum and maximum object area
	const int MIN_OBJECT_AREA = 20*20;
	const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;

	// 3D coord of depth points, (0,0,0) indicates no valid 3D coord
	double coords[FRAME_WIDTH*FRAME_HEIGHT*3]; // array to store 3D coordinates
	double rgb2XYZ[FRAME_WIDTH*FRAME_HEIGHT*3]; // array to store 3D coordinates of RGB points

	
	#define MIN_DEPTH_DISTANCE 0.0
	#define MAX_DEPTH_DISTANCE 10.0

	#define DEPTH_RGB_CORRECTION_X 10.0
	#define DEPTH_RGB_CORRECTION_Y 10.0


double x = 0;
double y = 0;
double z = 0;
	// enable depth registration with-->rosrun rqt_reconfigure rqt_reconfigure(if registered data required)  
	
void createTrackbars()
{
	//create window for trackbars
	namedWindow(trackbarWindowName,0);
	
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN");
	sprintf( TrackbarName, "H_MAX");
	sprintf( TrackbarName, "S_MIN");
	sprintf( TrackbarName, "S_MAX");
	sprintf( TrackbarName, "V_MIN");
	sprintf( TrackbarName, "V_MAX");
	
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, 179);
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, 179);
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, 255);
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, 255);
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, 255);
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, 255);
}

string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}


void drawObject(int x, int y,Mat &frame)
{
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}


void trackFilteredObject(int &u, int &v, bool &ballfound, Mat threshold, Mat &cameraFeed)
{
	cv::Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS)
        {
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			 {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea)
                {
					u = moment.m10/area;
					v = moment.m01/area;
					objectFound = true;
					ballfound = true;
					refArea = area;
				}
				else objectFound = false;
			  }
			  
        	//let user know you found an object
			if(objectFound ==true)
			{
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(u,v,cameraFeed);
			}
		}
		else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}


void callback(const ImageConstPtr& image_rgb, const sensor_msgs::PointCloud2::ConstPtr& pCloud)
{
	// Solve all of perception here...
	cv::Mat image_color = cv_bridge::toCvCopy(image_rgb)->image;
		
	cv::imshow(windowName1,image_color);

	

	// new cloud formation 
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	
	// new cloud from old cloud. (just copy )
	pcl::fromROSMsg (*pCloud, *cloud);
	
	// new image frame. to get size of point cloud
	cv::Mat imageFrame;
	imageFrame = cv::Mat(cloud->height, cloud->width, CV_8UC3); 
	
	for (int h=0; h<imageFrame.rows; h++) 
    {
        for (int w=0; w<imageFrame.cols; w++) 
        {
            pcl::PointXYZ point = cloud->at(w, h);
        }
    }
		

	createTrackbars();

	//some boolean variables for different functionality within this program
    bool trackObjects = false;
    bool useMorphOps = false;
   
	cv::Mat cameraFeed;
	//matrix storage for HSV image
	cv::Mat HSV;
	//matrix storage for binary threshold image
	cv::Mat threshold;
	
	//cv::Mat depthMap;

	//x and y values for the location of the object
	int u=0, v=0;
	bool ballfound = false;
	image_color.copyTo(cameraFeed);
	//image_depth.copyTo(depthMap);
	
	//convert frame from BGR to HSV colorspace
	cv::cvtColor(cameraFeed,HSV,CV_BGR2HSV);
	
	//filter HSV image between values and store filtered image to threshold matrix
	cv::inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
	
	cv::Mat erodeElement1 = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	cv::Mat dilateElement1 = getStructuringElement( MORPH_RECT,Size(8,8));
	
	cv::erode(threshold,threshold,erodeElement1);
	cv::erode(threshold,threshold,erodeElement1);
		
	cv::dilate(threshold,threshold,dilateElement1);
	cv::dilate(threshold,threshold,dilateElement1);
		
	cv::imshow(windowName3,threshold);
    
	
	trackFilteredObject(u,v,ballfound,threshold,cameraFeed);
    
   	//show frames 
	cv::imshow(windowName4,cameraFeed); // image processing and data processing ends here 
	
	// raw depth value from RGB pixel coordinate
	int i = (u) + (v)*cloud->width;
	
	// 3D coordinates from point cloud using depth value
	float x3Ddepth = (float)cloud->points[i].x;
	float y3Ddepth = (float)cloud->points[i].y;
	float z3Ddepth = (float)cloud->points[i].z;
	
	// printing of coordinates in DEPTH camera frame
	//cout <<"x, y, z---in Depth frame -->> "<< x3Ddepth<<" "<<y3Ddepth<<" "<<z3Ddepth<<endl;  
	
	// 3D coordinates from depth frame to RGB frame
	float x3Drgb = (x3Ddepth*R[0][0])+(y3Ddepth*R[0][1])+(z3Ddepth*R[0][2])+T[0][0];
	float y3Drgb = (x3Ddepth*R[1][0])+(y3Ddepth*R[1][1])+(z3Ddepth*R[1][2])+T[1][0];
	float z3Drgb = (x3Ddepth*R[2][0])+(y3Ddepth*R[2][1])+(z3Ddepth*R[2][2])+T[2][0];

/**	
	if (u>20 && u<460 && v>20 && v<620 && z3Drgb != 0 && ballfound)
	{
		printf("\n%3d - Flying ball - %6f %6f %6f", x3Drgb, y3Drgb, z3Drgb);
	}	
	else
	{
		printf("\n ball not found in ROI");
	}
*/
	
	bool calculateTrajectory=false;
	geometry_msgs::PoseStamped Pos;

	/* EE header stamp and frame id*/
		Pos.header.stamp = ros::Time::now();
		Pos.header.frame_id = "/base4";

	int n;
	vector<float> trajCoords[n][3];
	
	// position
	x=x3Drgb;
	y=y3Drgb;
	z=z3Drgb;
	
	if(std::isnan(x))
	{
		printf("\n object not found in ROI");
		Pos.pose.position.x=9.0;
		Pos.pose.position.y=9.0;
		Pos.pose.position.z=9.0;
		pubPos.publish(Pos);
	}
	else if (u>20 && u<460 && v>20 && v<620 && z3Drgb != 0 && ballfound) //  && !(std::isnan(x))
	{
		Pos.pose.position.x=(-z+1.58);
		Pos.pose.position.y=(-x-0.565);
		Pos.pose.position.z=(y+0.788);
		printf("\n publishing position");
		printf("\n%3d - object - %6f %6f %6f", x3Drgb, y3Drgb, z3Drgb);
		calculateTrajectory=true;
		pubPos.publish(Pos);
	}
	else
	{
Pos.pose.position.x=9.0;
		Pos.pose.position.y=9.0;
		Pos.pose.position.z=9.0;
		pubPos.publish(Pos);
		printf("\n something is wrong \n");
	}

	/**

	// %%%%%%%%%%%%%%%   ball movement visualization in RVIZ  %%%%%%%%%%%%%%%%%%%
	// Visualization marker
	visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "/camera_rgb_frame";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = visualization_msgs::Marker::SPHERE;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    
    
    
    
    
    //marker.pose.position.x =zKinect;
    //marker.pose.position.y =yKinect; 
    //marker.pose.position.z =xKinect; 
    
    marker.pose.position.x =xKinect;
    marker.pose.position.y =yKinect; 
    marker.pose.position.z =zKinect; 
    

     // not required for spherical shape
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();
    
    marker_pub.publish(marker);
    */
    
  
    cv::waitKey(1);
}


int main(int argc, char** argv) {
	
	ros::init(argc, argv, "objdetgreen");

	ros::NodeHandle nh;
	
	ROS_INFO("Starting ");
	
	message_filters::Subscriber<Image> RGB_sub(nh, "/camera/rgb/image_color", 1);
	message_filters::Subscriber<sensor_msgs::PointCloud2> PointCloud_sub(nh, "/camera/depth/points", 1);

	typedef sync_policies::ApproximateTime<Image, sensor_msgs::PointCloud2> MySyncPolicy;
	
	// ApproximateTime takes a queue size as its constructor argument, hence MySyncPolicy(10)
	Synchronizer<MySyncPolicy> sync(MySyncPolicy(10), RGB_sub, PointCloud_sub);
	sync.registerCallback(boost::bind(&callback, _1, _2));

    //marker_pub = nh.advertise<visualization_msgs::Marker>("ball_rviz", 1);
    pubPos = nh.advertise<geometry_msgs::PoseStamped>("pos_green", 1); 
	
	ros::spin();
	return 0;
}
