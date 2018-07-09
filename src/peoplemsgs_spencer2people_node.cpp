#include <ros/ros.h>
#include <people_msgs/People.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <tf/tf.h>
#include <spencer_tracking_msgs/TrackedPersons.h>


class people_msg_transformer_aware
{
private:
  ros::NodeHandle n_;
  ros::Subscriber sub_;
  ros::Publisher pub_;
  ros::Publisher pub_marker_;
  
public:
  people_msg_transformer_aware()
  {
    
  }
  ~people_msg_transformer_aware()
  {
    
  }
  
  void init()
  {
    sub_ = n_.subscribe("/spencer/perception/tracked_persons_confirmed_by_HOG_or_upper_body", 1000, &people_msg_transformer_aware::callback, this);
    pub_ = n_.advertise<people_msgs::People>("people", 1);
    pub_marker_ = n_.advertise<visualization_msgs::MarkerArray>("people_viz", 1);
  }
  
  void callback(const spencer_tracking_msgs::TrackedPersons::ConstPtr& msg)
  {
    people_msgs::People people;
    visualization_msgs::MarkerArray people_marker_array;
    people.header = msg->header;
    
    int size = msg->tracks.size();
//     people.people.size = size;
    for(int i=0;i<size;i++)
    {
      spencer_tracking_msgs::TrackedPerson people_spencer = msg->tracks[i];
      people_msgs::Person person;
      person.velocity.x = people_spencer.twist.twist.linear.x;
      person.velocity.y = people_spencer.twist.twist.linear.y;
      person.position.x = people_spencer.pose.pose.position.x;
      person.position.y = people_spencer.pose.pose.position.y;
      people.people.push_back(person);
      
      visualization_msgs::Marker people_marker;
      people_marker.header.frame_id = "odom";
      people_marker.ns = "model";
      people_marker.type = visualization_msgs::Marker::MESH_RESOURCE;
      people_marker.action = visualization_msgs::Marker::MODIFY;
      people_marker.mesh_resource = "package://social_nav_simulation/gazebo/models/human/meshes/walking.dae";
      people_marker.mesh_use_embedded_materials = true;
      people_marker.scale.x = 1.0;
      people_marker.scale.y = 1.0;
      people_marker.scale.z = 1.0;
      
      people_marker.pose.position.x = person.position.x;
      people_marker.pose.position.y = person.position.y;
      people_marker.pose.orientation = people_spencer.pose.pose.orientation;
      people_marker_array.markers.push_back(people_marker);
    }
    

    pub_.publish(people);
    pub_marker_.publish(people_marker_array);
//     visualize_people(people_upo);
    
  }
};



int main(int argc, char ** argv)
{
  ros::init(argc, argv, "peoplemsgs_spencer2people");
  
  people_msg_transformer_aware trans;
  trans.init();
  
  ros::spin();
  
  return 0;
}