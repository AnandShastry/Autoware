#ifndef __DECISION_MAKER_NODE__
#define __DECISION_MAKER_NODE__

#include <unordered_map>

#include <ros/ros.h>
#include <ros/spinner.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include <random>

#include <geometry_msgs/Point.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <jsk_recognition_msgs/BoundingBoxArray.h>
#include <jsk_recognition_msgs/BoundingBoxArray.h>
#include <jsk_rviz_plugins/OverlayText.h>
#include <sensor_msgs/PointCloud2.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <tf/transform_listener.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/MarkerArray.h>

#include <autoware_msgs/CloudClusterArray.h>
#include <autoware_msgs/ConfigDecisionMaker.h>
#include <autoware_msgs/LaneArray.h>
#include <autoware_msgs/VehicleCmd.h>
#include <autoware_msgs/lane.h>
#include <autoware_msgs/lane.h>
#include <autoware_msgs/state.h>
#include <autoware_msgs/state.h>
#include <autoware_msgs/traffic_light.h>
#include <autoware_msgs/waypoint.h>
#include <vector_map/vector_map.h>

#include <amathutils_lib/amathutils.hpp>
#include <cross_road_area.hpp>
#include <decision_maker_events.hpp>
#include <decision_maker_param.hpp>
#include <state_machine_lib/state_context.hpp>

namespace decision_maker
{
using namespace vector_map;

enum class E_Lamp : int32_t
{
  LAMP_EMPTY = -1,
  LAMP_RIGHT = 1,
  LAMP_LEFT = 2,
  LAMP_HAZARD = 3
};
enum class E_Control : int32_t
{
  KEEP = -1,
  STOP = 1,
  DECELERATE = 2,
  ACCELERATE = 3,
  OTHERS = 4,
};

enum class E_ChangeFlags : int32_t
{
  STRAIGHT,
  RIGHT,
  LEFT,

  UNKNOWN = -1,
};

inline bool hasvMap(void)
{
  return true;
}

template <class T>
typename std::underlying_type<T>::type enumToInteger(T t)
{
  return static_cast<typename std::underlying_type<T>::type>(t);
}

struct AutowareStatus
{
  // planning status
  autoware_msgs::LaneArray using_lane_array;  // with wpstate
  autoware_msgs::LaneArray based_lane_array;
  autoware_msgs::lane finalwaypoints;
  int closest_waypoint;

  // vehicle status
  geometry_msgs::Pose pose;
  double velocity;  // kmph

  // control status
};

class DecisionMakerNode
{
private:
  ros::NodeHandle nh_;
  ros::NodeHandle private_nh_;
  // Publishers
  std::unordered_map<std::string, ros::Publisher> Pubs;
  // Subscribers
  std::unordered_map<std::string, ros::Subscriber> Subs;

  std::shared_ptr<ros::AsyncSpinner> spinners;

  AutowareStatus current_status_;

  // ROS Messages
  std_msgs::String state_string_msg;
  geometry_msgs::PoseStamped current_pose_;

  jsk_rviz_plugins::OverlayText state_text_msg;

  // ROS Messages(Autoware)
  std::vector<geometry_msgs::Point> inside_points_;

  std::map<std::string, bool> EventFlags;

  tf::TransformListener tflistener_baselink;

  int closest_stop_waypoint_;
  int closest_stopline_waypoint_;
  int goal_waypoint_;
  autoware_msgs::waypoint CurrentStoplineTarget_;

  double average_velocity_;
  int closest_waypoint_;
  CrossRoadArea *ClosestArea_;
  std::string CurrentStateName;
  std::string TextOffset;
  std::vector<CrossRoadArea> intersects;
  double displacement_from_path_;

  bool foundOtherVehicleForIntersectionStop_;  // In fact this should be defined as state.
  class DetectionArea
  {
  public:
    double x1, x2;
    double y1, y2;

    DetectionArea()
    {
    }
  };
  DetectionArea detectionArea_;

  bool isManualLight;

  // Param
  bool enableDisplayMarker;
  bool enableForceStateChange;
  uint32_t param_convergence_count_;
  uint32_t param_target_waypoint_;
  double param_convergence_threshold_;
  uint32_t param_stopline_target_waypoint_;
  double param_stopline_target_ratio_;
  double param_shift_width_;
  double param_crawl_velocity_;
  double param_detection_area_rate_;
  std::string param_baselink_tf_;

  // for vectormap server
  // ros::ServiceClient cross_road_cli;
  // vector_map_server::GetCrossRoad cross_road_srv;

  // initialization flags for initialized by callback
  std::mutex vMap_mutex;
  bool created_shift_lane_flag_;

  // initialization method
  void initROS();
  void initVectorMap(void);
  void initStateMsgs(void);
  bool initVectorMapClient(void);

  // looping method
  void update(void);
  void update_msgs(void);
  void update_pubsub(void);
  void displayMarker(void);

  void publishToVelocityArray();
  std::string createStateMessageText();
  int createCrossRoadAreaMarker(visualization_msgs::Marker &crossroad_marker, double scale);

  // judge method
  // in near future, these methods will be deprecate to decision_maker library
  bool isCrossRoadByVectorMapServer(const autoware_msgs::lane &lane_msg, const geometry_msgs::PoseStamped &pose_msg);
  bool isLocalizationConvergence(const geometry_msgs::Point &_current_point);
  bool handleStateCmd(const uint64_t _state_num);
  // double calcIntersectWayAngle(const CrossRoadArea& area);
  double calcIntersectWayAngle(const autoware_msgs::lane &laneinArea);

  void insertPointWithinCrossRoad(const std::vector<CrossRoadArea> &_intersects, autoware_msgs::LaneArray &lane_array);

  void setWaypointState(autoware_msgs::LaneArray &lane_array);
  double calcPosesAngleDiff(const geometry_msgs::Pose &p_from, const geometry_msgs::Pose &p_to);
  double calcPosesAngleDiffN(const geometry_msgs::Pose &p_from, const geometry_msgs::Pose &p_to);
  double getPoseAngle(const geometry_msgs::Pose &p);

  /* for planning according to state*/
  void publishStoppedLaneArray(void);
  void publishControlledLaneArray(void);
  void updateLaneWaypointsArray(void);
  void changeVelocityBasedLane(void);
  void changeVelocityLane(int dir);
  void createShiftLane(void);
  void changeShiftLane(void);
  void removeShiftLane(void);
  void setAllStoplineStop(void);

  void publishLampCmd(const E_Lamp &status);
  void publishStoplineWaypointIdx(int wp_idx);
  void publishLightColor(int status);
#if 0
  // callback by state context
  void StoplinePlanIn(int status);
  void StoplinePlanOut(int status);
  void callbackInStateObstacleAvoid(int status);
  void callbackOutStateObstacleAvoid(int status);
  void updateStateObstacleAvoid(int status);
  void updateStateSTR(int status);
  void updateStateStop(int status);
  void callbackOutStateStop(int status);
  void callbackInStateStop(int status);
  void callbackInStateAcc(int status);
  void callbackInStateDec(int status);
  void callbackInStateKeep(int status);
  void callbackOutStateLaneChange(int status);
#endif

  void tryNextState(const std::string &key);

  bool waitForEvent(const std::string &key, const bool &flag);
  bool waitForEvent(const std::string &key, const bool &flag, const double &timeout);

  bool isArrivedGoal(void);

  // current decision maker is support only lane area
  bool isVehicleOnLaneArea(void)
  {
    return true;
  }
  bool isVehicleOnFreeArea(void)
  {
    return false;
  }

  void setupStateCallback(void);

  void updateStateVehicleReady(const std::string &state_name, int status);
  void updateStateDriveReady(const std::string &state_name, int status);
  void updateStateDrive(const std::string &state_name, int status);
  void updateStateMissionComplete(const std::string &state_name, int status);

  // entry callback
  void entryInitState(const std::string &state_name, int status);
  void entrySensorInitState(const std::string &state_name, int status);
  void entryLocalizationInitState(const std::string &state_name, int status);
  void entryPlanningInitState(const std::string &state_name, int status);
  void entryVehicleInitState(const std::string &state_name, int status);
  void entryVehicleReadyState(const std::string &state_name, int status);
  void entryWaitMissionOrderState(const std::string &state_name, int status);
  void entryMissionCheckState(const std::string &state_name, int status);
  void entryDriveReadyState(const std::string &state_name, int status);
  void entryDriveState(const std::string &state_name, int status);

  // update callback
  void updateInitState(const std::string &state_name, int status);
  void updateSensorInitState(const std::string &state_name, int status);
  void updateMapInitState(const std::string &state_name, int status);
  void updateLocalizationInitState(const std::string &state_name, int status);
  void updateVehicleReadyState(const std::string &state_name, int status);
  void updateWaitMissionOrderState(const std::string &state_name, int status);

  void updateMissionCheckState(const std::string &state_name, int status);
  void updateDriveReadyState(const std::string &state_name, int status);
  void updateDriveState(const std::string &state_name, int status);
  void updateLaneAreaState(const std::string &state_name, int status);
  void updateFreeAreaState(const std::string &state_name, int status);
  void updateLeftTurnState(const std::string &state_name, int status);
  void updateRightTurnState(const std::string &state_name, int status);
  void updateStoplineState(const std::string &state_name, int status);
  void updateGoState(const std::string &state_name, int status);
  void updateWaitState(const std::string &state_name, int status);
  void updateMissionCompleteState(const std::string &state_name, int status);
  void exitWaitMissionOrderState(const std::string &state_name, int status);

  // callback by topic subscribing
  void callbackFromFilteredPoints(const sensor_msgs::PointCloud2::ConstPtr &msg);

  void callbackFromCurrentVelocity(const geometry_msgs::TwistStamped &msg);
  void callbackFromCurrentPose(const geometry_msgs::PoseStamped &msg);
  void callbackFromClosestWaypoint(const std_msgs::Int32 &msg);
  void callbackFromLightColor(const ros::MessageEvent<autoware_msgs::traffic_light const> &event);
  void callbackFromLaneChangeFlag(const std_msgs::Int32 &msg);
  void callbackFromFinalWaypoint(const autoware_msgs::lane &msg);
  void callbackFromLaneWaypoint(const autoware_msgs::LaneArray &msg);
  void callbackFromTwistCmd(const geometry_msgs::TwistStamped &msg);
  void callbackFromSimPose(const geometry_msgs::PoseStamped &msg);
  void callbackFromStateCmd(const std_msgs::Int32 &msg);
  void callbackFromConfig(const autoware_msgs::ConfigDecisionMaker &msg);
  void callbackFromObjectDetector(const autoware_msgs::CloudClusterArray &msg);

  void callbackFromVectorMapArea(const vector_map_msgs::AreaArray &msg);
  void callbackFromVectorMapPoint(const vector_map_msgs::PointArray &msg);
  void callbackFromVectorMapLine(const vector_map_msgs::LineArray &msg);
  void callbackFromVectorMapCrossRoad(const vector_map_msgs::CrossRoadArray &msg);

public:
  state_machine::StateContext *ctx;
  VectorMap g_vmap;

  DecisionMakerNode(int argc, char **argv) : private_nh_("~")
  {
    enableDisplayMarker = DEFAULT_DISPLAY_FLAG;
    param_convergence_threshold_ = DEFAULT_CONVERGENCE_THRESHOLD;
    param_convergence_count_ = DEFAULT_CONVERGENCE_COUNT;
    param_target_waypoint_ = DEFAULT_TARGET_WAYPOINT;
    param_shift_width_ = DEFAULT_SHIFT_WIDTH;
    param_stopline_target_waypoint_ = DEFAULT_STOPLINE_TARGET_WAYPOINT;
    param_stopline_target_ratio_ = 0.0;
    param_crawl_velocity_ = DEFAULT_CRAWL_VELOCITY;
    param_detection_area_rate_ = DEFAULT_DETECTION_AREA_RATE;
    param_baselink_tf_ = "base_link";
    detectionArea_.x1 = DEFAULT_DETECTION_AREA_X1;
    detectionArea_.x2 = DEFAULT_DETECTION_AREA_X2;
    detectionArea_.y1 = DEFAULT_DETECTION_AREA_Y1;
    detectionArea_.y2 = DEFAULT_DETECTION_AREA_Y2;

    std::string file_name;
    private_nh_.getParam("state_file_name", file_name);

    ctx = new state_machine::StateContext(file_name);
    setupStateCallback();
    // this->initROS(argc, argv);
  }

  void init(void);

  bool isSubscribed(const std::string &topic_name)
  {
    return Subs.count(topic_name) ? true : false;
  }

  bool isEventFlagTrue(std::string key)
  {
    if (EventFlags.count(key) == 0)
    {
      EventFlags[key] = false;
    }
    return EventFlags[key];
  }

  void run(void);
  geometry_msgs::Point to_geoPoint(const vector_map_msgs::Point &vp)
  {
    geometry_msgs::Point gp;
    gp.x = vp.ly;
    gp.y = vp.bx;
    gp.z = vp.h;
    return gp;
  }
};

}  // namespace decision_maker

#endif
