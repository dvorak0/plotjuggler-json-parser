# plotjuggler-json-parser

A small PlotJuggler MessageParser plugin for ROS2 `std_msgs/msg/String` messages that contain JSON.

## Goal

Convert messages like:

```json
{"tracked_num": 100, "init_error": 1e-7}
```

into PlotJuggler numeric series such as:

- `tracked_num`
- `init_error`

Nested objects are flattened using dot-separated keys, for example:

```json
{"tracker": {"tracked_num": 100, "init_error": 1e-7}}
```

becomes:

- `tracker.tracked_num`
- `tracker.init_error`

## MVP behavior

- registers on the standard `ros2msg` parser path
- intercepts only ROS2 `std_msgs/msg/String`
- expects a top-level JSON object
- extracts only numeric leaf values
- ignores strings, bools, arrays, and nulls
- allows dynamic fields
- limits created series to 200 per topic

## Build status

Validated to build inside `uniflexai/tinynav:latest` on `nixos-1` with these packages installed:

- `ros-humble-plotjuggler`
- `ros-humble-plotjuggler-ros`
- `qtbase5-dev`
- `libqt5xml5`, Qt Xml dev headers
- `nlohmann-json3-dev`

Build command used:

```bash
source /opt/ros/humble/setup.bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/opt/ros/humble
cmake --build build -j2
```

Current output artifact:

- `libParserROS2StringJson.so`

## Test bag generator

A helper script is included to generate a ROS2 bag containing JSON payloads in `std_msgs/msg/String`:

```bash
python3 scripts/make_test_bag.py --output test_bag_json_status
```

Default output topic:

- `/debug_state`

The script writes JSON messages with fields such as:

- `tracked_num`
- `init_error`
- `tracker.inlier_ratio`
- `tracker.num_keyframes`
- `loop.candidates`
- `loop.score`

## Current limitations

- plugin loading was observed in PlotJuggler, but end-to-end runtime parsing is still being validated
- ROS2 `std_msgs/String` payload extraction is currently implemented manually from serialized bytes
- the plugin is currently standalone, not yet integrated into upstream `plotjuggler-ros-plugins`
