#!/usr/bin/env python3

import argparse
import json
from pathlib import Path

import rosbag2_py
from rclpy.serialization import serialize_message
from std_msgs.msg import String


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate a ROS2 bag with JSON std_msgs/String messages")
    parser.add_argument("--output", type=Path, default=Path("test_bag_json_status"))
    parser.add_argument("--topic", default="/debug_state")
    parser.add_argument("--count", type=int, default=20)
    parser.add_argument("--dt", type=float, default=0.1, help="seconds between messages")
    args = parser.parse_args()

    output_uri = str(args.output)
    args.output.parent.mkdir(parents=True, exist_ok=True)

    writer = rosbag2_py.SequentialWriter()
    storage_options = rosbag2_py.StorageOptions(uri=output_uri, storage_id="sqlite3")
    converter_options = rosbag2_py.ConverterOptions(
        input_serialization_format="cdr",
        output_serialization_format="cdr",
    )
    writer.open(storage_options, converter_options)

    topic_info = rosbag2_py.TopicMetadata(
        name=args.topic,
        type="std_msgs/msg/String",
        serialization_format="cdr",
        offered_qos_profiles="",
    )
    writer.create_topic(topic_info)

    base_ns = 1_700_000_000_000_000_000
    step_ns = int(args.dt * 1e9)

    for i in range(args.count):
        payload = {
            "tracked_num": 100 - i,
            "init_error": 1e-7 * (i + 1),
            "tracker": {
                "inlier_ratio": max(0.0, 0.95 - 0.01 * i),
                "num_keyframes": i // 2,
            },
            "loop": {
                "candidates": i % 4,
                "score": 0.5 + 0.02 * i,
            },
            "stage": "tracking" if i < args.count - 3 else "relocalizing",
        }
        msg = String()
        msg.data = json.dumps(payload, separators=(",", ":"))
        writer.write(args.topic, serialize_message(msg), base_ns + i * step_ns)

    print(f"Wrote {args.count} messages to {output_uri}")


if __name__ == "__main__":
    main()
