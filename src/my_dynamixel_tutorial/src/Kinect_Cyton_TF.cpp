// General includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
// c++ headers
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
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
// pcl headers
#include <pcl/point_types.h>
#include <pcl/common/common.h>
#include <pcl/point_cloud.h>
#include <pcl/console/parse.h>
#include <pcl/console/print.h>
#include <boost/filesystem.hpp>
#include <flann/flann.h>
#include <fstream>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <boost/foreach.hpp>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/passthrough.h>
#include <pcl/common/transformation_from_correspondences.h>
#include <pcl/common/transforms.h>
#include <pcl/registration/transformation_estimation_svd.h>
#include <pcl/common/time.h> // for watch(timer)
// Message publication

// tf
#include <tf/transform_listener.h>
// visualization header
#include <visualization_msgs/Marker.h>

#include <gtest/gtest.h>
// matrix library

#include <iostream>

#include <pcl/common/common.h>
#include <pcl/common/angles.h>
#include <pcl/common/transforms.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/registration/transformation_estimation_svd.h>

using namespace std;
using namespace sensor_msgs;
using namespace message_filters;
using namespace cv;

int main(int argc, char **argv){

ros::init(argc, argv, "vision_node");
ros::NodeHandle nh;

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in (new pcl::PointCloud<pcl::PointXYZ> ());
pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out (new pcl::PointCloud<pcl::PointXYZ> ());

cloud_in->width = 5;
cloud_in->height = 1;
cloud_in->is_dense = false;
cloud_in->resize(cloud_in->width * cloud_in->height);

cloud_out->width = 5;
cloud_out->height = 1;
cloud_out->is_dense = false;
cloud_out->resize(cloud_out->width * cloud_out->height);

cloud_in->points[0].x = -0.217339;
cloud_in->points[0].y = 0.100075;
cloud_in->points[0].z = 1.417494;

cloud_in->points[1].x = 0.002359;
cloud_in->points[1].y = -0.008858;
cloud_in->points[1].z = 1.813092;

cloud_in->points[2].x = 0.048462;
cloud_in->points[2].y = -0.277439;
cloud_in->points[2].z = 1.745592;

cloud_in->points[3].x = -0.045875;
cloud_in->points[3].y = -0.112430;
cloud_in->points[3].z = 2.026028;

cloud_in->points[4].x = -0.204622;
cloud_in->points[4].y = -0.047612;
cloud_in->points[4].z = 1.988037;

//cloud_in->points[5].x = 0.025900;
//cloud_in->points[5].y = -0.070515;
//cloud_in->points[5].z = 0.767509;
/*/*/
cloud_out->points[0].x = 0.0493866;
cloud_out->points[0].y = 0.109814;
cloud_out->points[0].z = 0.767768;

cloud_out->points[1].x = 0.000536421;
cloud_out->points[1].y = -0.215016;
cloud_out->points[1].z = 0.689727;

cloud_out->points[2].x = 0.0691566;
cloud_out->points[2].y = -0.364316;
cloud_out->points[2].z = 0.462721;

cloud_out->points[3].x = -0.214802;
cloud_out->points[3].y = -0.107544;
cloud_out->points[3].z = 0.537751;

cloud_out->points[4].x = -0.20743;
cloud_out->points[4].y = 0.0897305;
cloud_out->points[4].z = 0.590681;

//cloud_out->points[5].x = -0.0395801;
//cloud_out->points[5].y = -0.0388143;
//cloud_out->points[5].z = -0.0904533;

pcl::registration::TransformationEstimationSVD<pcl::PointXYZ,pcl::PointXYZ> TESVD;
pcl::registration::TransformationEstimationSVD<pcl::PointXYZ,pcl::PointXYZ>::Matrix4 transformKC; // 4X4 matrix for storing transformation

TESVD.estimateRigidTransformation (*cloud_in,*cloud_out,transformKC);

// new point input
	double xKinect, yKinect, zKinect;
	
	// point to be calculated
	double xRobot, yRobot, zRobot;
	
	// input point initialized
	xKinect =0.004046 , yKinect =-0.2021, zKinect =1.9667 ;
	
	// point transform 
	xRobot = (xKinect*transformKC (0,0))+(yKinect*transformKC (0,1))+(zKinect*transformKC (0,2))+transformKC (0,3);
	yRobot = (xKinect*transformKC (1,0))+(yKinect*transformKC (1,1))+(zKinect*transformKC (1,2))+transformKC (1,3);
	zRobot = (xKinect*transformKC (2,0))+(yKinect*transformKC (2,1))+(zKinect*transformKC (2,2))+transformKC (2,3);
	
	// data Printing
	std::cout << "\n The input kinect Coordinate is  : \t" ;
	printf ("pointKinect-----> < %0.3f, %0.3f, %0.3f >\n", xKinect, yKinect, zKinect);
	std::cout << "\n The corresponding Robot Coordinate is  : \t" ;
	printf ("pointRobot-----> < %0.3f, %0.3f, %0.3f >\n", xRobot, yRobot, zRobot);	


std::cout <<"The Estimated Rotation and translation matrices (using getTransformation function) are : \n" << std::endl;
printf ("\n");
printf (" | %6.3f %6.3f %6.3f | \n", transformKC (0,0), transformKC (0,1), transformKC (0,2));
printf ("R = | %6.3f %6.3f %6.3f | \n",transformKC (1,0), transformKC (1,1),transformKC (1,2));
printf (" | %6.3f %6.3f %6.3f | \n", transformKC (2,0), transformKC (2,1), transformKC (2,2));
printf ("\n");
printf ("t = < %0.3f, %0.3f, %0.3f >\n",transformKC (0,3), transformKC (1,3), transformKC (2,3));

ros::spin();
return 0;
}


