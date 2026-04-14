# plotjuggler-json-parser

A small PlotJuggler parser plugin prototype for ROS2 `std_msgs/msg/String` messages that contain JSON.

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

- supports only ROS2 `std_msgs/msg/String`
- expects a top-level JSON object
- extracts only numeric leaf values
- ignores strings, bools, arrays, and nulls
- allows dynamic fields
- limits created series to 200 per topic

## Status

Initial scaffold only. Build integration with upstream PlotJuggler / plotjuggler-ros-plugins still needs to be validated.
