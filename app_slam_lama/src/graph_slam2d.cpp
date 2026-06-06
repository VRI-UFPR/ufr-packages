
#include <lama/graph_slam2d.h>
#include <lama/pose3d.h>
#include <lama/sdm/occupancy_map.h>
#include <lama/image.h>
#include <lama/print.h>
#include <Eigen/StdVector>
#include <ufr.h>

using namespace lama;

#ifndef ROBOT_TOPIC_SCAN
#define ROBOT_TOPIC_SCAN "@new ros_humble @coder ros_humble:laser_scan @topic scan @frame_id laser_frame"
#endif

#ifndef ROBOT_TOPIC_ODOM
#define ROBOT_TOPIC_ODOM "@new ros_noetic @coder ros_noetic:pose2d @topic /odom"
#endif


Pose3D getSensorPose(std::string frame_id) {
    return Pose3D();
}

bool getOdometry(Pose2D& odom) {
    odom = Pose2D(0,0,0);
    return true;
}

typedef struct {
    float angle_min;
    float angle_max;
    float angle_increment;
    float time_increment;
    float scan_time;
    float range_min;
    float range_max;
    float ranges[2048];
} Laser;

int main() {
    link_t laser_sub = ufr_subscriber(ROBOT_TOPIC_SCAN);
    link_t odom_sub = ufr_subscriber(ROBOT_TOPIC_ODOM);

    //
    GraphSlam2D::Options options;
    GraphSlam2D* slam2d_ = new GraphSlam2D(options);

    //
    double tmp = 0.0;
    Vector2d pos(0,0); 
    Pose2D prior(pos, tmp);
    slam2d_->Init(prior);

    const float min_range_ = 0.15;
    const float max_range_ = 12.0;
    const int beam_step_ = 1;

    float odom_x, odom_y, odom_th;
    float scan_angle_min;
    float scan_angle_max;
    float scan_angle_inc;
    float scan_time_inc;
    float scan_range_min;
    float scan_range_max;
    float scan_time;
    int scan_ranges_size;
    float scan_ranges[4096];

    Pose3D sensor_origin(0,0,0,0,0,0);
    printf("Main loop\n");

    // Main Loop
    while ( ufr_loop_ok() ) {
        if ( ufr_recv_async(&laser_sub) == UFR_OK ) {
            ufr_get(&laser_sub, "ff", &scan_angle_min, &scan_angle_max);
            ufr_get(&laser_sub, "ff", &scan_angle_inc, &scan_time_inc);
            ufr_get(&laser_sub, "f", &scan_time);
            ufr_get(&laser_sub, "ff", &scan_range_min, &scan_range_max);
            scan_ranges_size = ufr_get_af32(&laser_sub, scan_ranges, 4096);
        }

        if ( ufr_recv_async(&odom_sub) == UFR_OK ) {
            ufr_get(&odom_sub, "fff", &odom_x, &odom_y, &odom_th);
        }

        continue;

        // Check if the updated is needed
        Pose2D odom(odom_x, odom_y, odom_th);
        const bool update = slam2d_->enoughMotion(odom);
        if ( !update ) {
            continue;
        }

        //
        float max_range;
        if (max_range_ == 0.0 || max_range_ > scan_range_max)
            max_range = scan_range_max;
        else
            max_range = max_range_;

        //
        float min_range;
        if (min_range_ == 0 || min_range_ < scan_range_min)
            min_range = scan_range_min;
        else
            min_range = min_range_;

        //
        PointCloudXYZ::Ptr cloud(new PointCloudXYZ);
        cloud->sensor_origin_ = sensor_origin.xyz();
        cloud->sensor_orientation_ = Quaterniond(sensor_origin.state.so3().matrix());

        //
        cloud->points.reserve(scan_ranges_size);
        for(size_t i = 0; i < scan_ranges_size; i += beam_step_){
            const double range = scan_ranges[i];

            if (not std::isfinite(range))
                continue;

            if (range >= max_range || range <= min_range)
                continue;
            
            const double x = range * std::cos(scan_angle_min+(i*scan_angle_inc));
            const double y = range * std::sin(scan_angle_min+(i*scan_angle_inc));
            cloud->points.push_back( Eigen::Vector3d(x,y,0) );
        }

        //
        double timestamp = 0;
        slam2d_->update(cloud, odom, timestamp);
        Pose2D pose = slam2d_->getPose();
    }

    // end
    ufr_close(&laser_sub);
    return 0;
}