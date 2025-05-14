#!/usr/bin/env python3

import argparse
import sys
from rclpy.serialization import deserialize_message
import sqlite3
import yaml
import os
import numpy as np
from array import array
from rosidl_runtime_py.utilities import get_message
from rosidl_runtime_py.import_message import import_message_from_namespaced_type
from vtr_tactic_msgs.msg import OdometryResult
from vtr_common_msgs.msg import LieGroupTransform




class ROS2BagExtractor:
    def __init__(self, bag_path):
        """Initialize the ROS2 bag extractor with the path to the bag file"""
        self.bag_path = bag_path
        self.db_path = os.path.join(bag_path, "odometry_result_0.db3")
        print(self.db_path)
        self.metadata_path = os.path.join(bag_path, "metadata.yaml")
        
        # Check if the paths exist
        if not os.path.exists(self.db_path):
            print(f"Error: Database file not found at {self.db_path}")
            sys.exit(1)
        
        if not os.path.exists(self.metadata_path):
            print(f"Error: Metadata file not found at {self.metadata_path}")
            sys.exit(1)
        
        # Load metadata
        with open(self.metadata_path, 'r') as f:
            self.metadata = yaml.safe_load(f)
        
        # Connect to the database
        self.conn = sqlite3.connect(self.db_path)
        self.cursor = self.conn.cursor()
        
        # Get available topics
        self.topics = self._get_topics()
        
    def _get_topics(self):
        topics = {}
        self.cursor.execute("SELECT id, name, type FROM topics")
        for topic_id, topic_name, topic_type in self.cursor.fetchall():
            topics[topic_id] = {
                'name': topic_name,
                'type': topic_type
            }
        return topics
    

    def extract_messages(self, topic_id=None, topic_name=None):
        if topic_id is None and topic_name is not None:
            topic_id = None
            for tid, data in self.topics.items():
                if data['name'] == topic_name:
                    topic_id = tid
                    break
            
            if topic_id is None:
                print(f"Error: Topic '{topic_name}' not found in the bag file")
                return []
        
        # Get the topic type
        topic_type = self.topics[topic_id]['type']
        
        # Import the message type
        try:
            msg_type = OdometryResult #import_message_from_namespaced_type(topic_type)
        except (AttributeError, ModuleNotFoundError, ImportError) as e:
            print(f"Error importing message type {topic_type}: {e}")
            print("Make sure you have the appropriate ROS2 message packages installed")
            return []
        
        # Query to get messages
        query = "SELECT timestamp, data FROM messages WHERE topic_id = ?"
        
        self.cursor.execute(query, (topic_id,))
        messages = []
        
        for timestamp, data in self.cursor.fetchall():
            try:
                msg = deserialize_message(data, msg_type)
                messages.append({
                    'timestamp': timestamp,
                    'data': msg
                })
            except Exception as e:
                print(f"Error deserializing message: {e}")
        
        return messages
    


def store_messages_to_file(messages, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    print("-----")
    
    # Iterate through the messages
    for i, msg in enumerate(messages[:5]):
        # Convert to numpy array if it's not already (handles array.array objects)
        data=msg['data']
        transform = data.t_world_robot.xi

        if isinstance(transform, array):
            matrix = np.array(transform)
            print("tis a matrix")
        # Filename with index
        filename = os.path.join(output_dir, f"odom__{i:04d}.txt")
        
        # # Save the matrix
        np.savetxt(filename, matrix)
        
        print(f"Saved matrix {i} to {filename}")




def main():
    bag_directory = "/home/adam/Desktop/CurrentBranch/temp/vision/graph/data/odometry_result"
    extractor = ROS2BagExtractor(bag_directory)
    
    try:    
        messages = extractor.extract_messages(topic_name="odometry_result")
        
        print(f"Found {len(messages)} messages")
        for i, msg in enumerate(messages[:5]):  # Print first 5 messages
            print(f"\nMessage {i+1}:")
            print(f"Data: {msg['data']}")
        
        if len(messages) > 10:
            print(f"\n... and {len(messages) - 10} more messages")

        #store_messages_to_file(messages,"/home/adam/Desktop/CurrentBranch/src/main/src/vtr_db_extractor/odom_poses")


    finally:
        if extractor.conn:
            extractor.conn.close()


if __name__ == "__main__":
    main()