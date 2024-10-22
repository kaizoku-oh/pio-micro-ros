# 🚀 Micro-ROS on PlatformIO

[![GitHub release](https://img.shields.io/github/v/release/kaizoku-oh/pio-micro-ros)](https://github.com/kaizoku-oh/pio-micro-ros/releases)
[![GitHub issues](https://img.shields.io/github/issues/kaizoku-oh/pio-micro-ros)](https://github.com/kaizoku-oh/pio-micro-ros/issues)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/kaizoku-oh/pio-micro-ros/blob/main/LICENSE)

------

[![GitHub Build workflow status - nucleo_u575zi_q](https://github.com/kaizoku-oh/pio-micro-ros/workflows/build_nucleo_u575zi_q/badge.svg)](https://github.com/kaizoku-oh/pio-micro-ros/actions/workflows/build_nucleo_u575zi_q.yml)
[![GitHub Build workflow status - esp32dev](https://github.com/kaizoku-oh/pio-micro-ros/workflows/build_esp32dev/badge.svg)](https://github.com/kaizoku-oh/pio-micro-ros/actions/workflows/build_esp32dev.yml)

## 🛠️ 0. Install OS dependencies

```bash
sudo apt install -y git cmake python3-pip
```

## 📦 1. Install micro-ros PlatformIO library
```ini
; Add these in platformio.ini
board_microros_distro = humble
board_microros_transport = serial
lib_deps =
    https://github.com/micro-ROS/micro_ros_platformio
```

## 🐋 2. Run micro-ros agent docker image

```bash
# Run Micro-ROS agent (humble) in a container with access to serial device
docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:humble serial --dev /dev/ttyACM0 -v6
```

### 🔌 Using multiple serial ports with the same agent

```bash
# Run Micro-ROS agent (humble) in a container with access to multiple serial devices
docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:humble multiserial --devs "/dev/ttyUSB0 /dev/ttyACM0" -v6
```

## 🐳 3. Run ros2 docker image

```bash
# Run container in interactive mode
docker run -it --rm osrf/ros:humble-desktop

# Return a list of all the topics currently active in the system
ros2 topic list

# To see the data being published on a topic, use
ros2 topic echo /button

# To publish data onto a topic directly from the command line
ros2 topic pub /led std_msgs/UInt8 "data: 1" --once
ros2 topic pub /led std_msgs/UInt8 "data: 0" --once
```
