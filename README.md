# Elevation Mapping cupy

## Installation Notes (ubuntu 20.04, ROS noetic)

* Installed CUDA 11.7 (Nvidia Driver Version 515.43.04):
  ```
  wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
  sudo mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
  wget https://developer.download.nvidia.com/compute/cuda/11.7.0/local_installers/cuda-repo-ubuntu2004-11-7-local_11.7.0-515.43.04-1_amd64.deb
  sudo dpkg -i cuda-repo-ubuntu2004-11-7-local_11.7.0-515.43.04-1_amd64.deb
  sudo cp /var/cuda-repo-ubuntu2004-11-7-local/cuda-*-keyring.gpg /usr/share/keyrings/
  sudo apt-get update
  sudo apt-get -y install cuda
  ```
  
  Added CUDA path to bashrc:
  ```
  # set PATH for cuda 11.7 installation
  if [ -d "/usr/local/cuda-11.7/bin/" ]; then
      export PATH=/usr/local/cuda-11.7/bin${PATH:+:${PATH}}
      export LD_LIBRARY_PATH=/usr/local/cuda-11.7/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
  fi
  ```
  
* Installed cupy-cuda117:
  ```
  pip3 install cupy-cuda117
  ```
  
  Added python (pip3) path to launch file:
  ```
  <env name="PYTHONPATH" value="<path_to_your_install>/lib/python3.8/site-packages:$(env PYTHONPATH)" />
  ```
  
* Installed torch & torchvision:
  ```
  pip3 install torch torchvision
  ```
  
* Installed python dependencies:
  ```
  pip3 install -r requirements.txt
  ```
  
* Installed ROS dependencies:
  ```
  sudo apt install ros-noetic-pybind11-catkin
  sudo apt install ros-noetic-grid-map-core ros-melodic-grid-map-msgs
  sudo apt install ros-noetic-ros-numpy
  ```

* Installed Detectron2:
  ```
  pip3 install 'git+https://github.com/facebookresearch/detectron2.git'
  ```
  
Refs:
* [How to install Cuda 11.4 on ubuntu 18.04(or 20.04)](https://medium.com/@anarmammadli/how-to-install-cuda-11-4-on-ubuntu-18-04-or-20-04-63f3dee2099)
* [Build Detectron2 from Source](https://detectron2.readthedocs.io/en/latest/tutorials/install.html)

![python tests](https://github.com/leggedrobotics/elevation_mapping_semantic_cupy/actions/workflows/python-tests.yml/badge.svg)

## Overview

This is a ROS package for elevation mapping on GPU. The elevation mapping code is written in python and uses cupy for
GPU computation. The
plane segmentation is done independently and runs on CPU. When the plane segmentation is generated, local convex
approximations of the
terrain can be efficiently generated.
![screenshot](doc/main_repo.png)
![gif](doc/convex_approximation.gif)

## Citing

> Takahiro Miki, Lorenz Wellhausen, Ruben Grandia, Fabian Jenelten, Timon Homberger, Marco Hutter  
> Elevation Mapping for Locomotion and Navigation using GPU  [arXiv](https://arxiv.org/abs/2204.12876)

```
@misc{https://doi.org/10.48550/arxiv.2204.12876,
  doi = {10.48550/ARXIV.2204.12876},
  url = {https://arxiv.org/abs/2204.12876},
  author = {Miki, Takahiro and Wellhausen, Lorenz and Grandia, Ruben and Jenelten, Fabian and Homberger, Timon and Hutter, Marco},
  keywords = {Robotics (cs.RO), FOS: Computer and information sciences, FOS: Computer and information sciences},
  title = {Elevation Mapping for Locomotion and Navigation using GPU},
  publisher = {arXiv},
  year = {2022},
  copyright = {arXiv.org perpetual, non-exclusive license}
}
```

## Quick instructions to run:

For the lonomy bag:

CPP wrapper:

```zsh
    roslaunch elevation_mapping_cupy lonomy_semantic_elevation_single.launch use_sim_true:=true
    rosbag play --clock ~/bags/good_working_wine_field_zed_topcom_rtk_person_9_2022-07-15-14-37-05.bag 
```

Python wrapper:

````zsh
    python -m elevation_mapping_cupy.elevation_mapping_ros
    roslaunch elevation_mapping_cupy pointcloud.launch use_sim_time:=true
    rosbag play --clock ~/bags/good_working_wine_field_zed_topcom_rtk_person_9_2022-07-15-14-37-05.bag
````

For the anymal bag:

```zsh
    roslaunch elevation_mapping_cupy anymal_semantic_elevation_single.launch use_sim_time:=true
    rosbag play --clock ~/bags/anymal_coyote_2022-12-11-20-01-46.bag
```


