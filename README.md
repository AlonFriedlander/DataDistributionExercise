# Data Distribution Exercise

## Overview
The **Data Distribution Exercise** is a C++-based system designed to demonstrate efficient data distribution using a publisher-subscriber model over UDP communication. The system consists of a publisher that broadcasts information about geometrical shapes (squares and circles) and subscribers that receive and process this information based on their interests.

## Features
* Publisher broadcasts data about shapes (blue square, green circle) at defined frequencies.
* Subscribers can subscribe to receive:
* Only square-related data.
* Both square and circle-related data.
* Supports multiple subscribers of each type.
* Communication is implemented over UDP.

## File Structure
#### Below is the project structure to help you understand how the codebase is organized:
```Configuration/```    
Contains configuration files, such as:
* ```Config.json```: Stores parameters like frequencies, communication settings, and other system properties.

```DataDistributionExercise/```    
Core implementation files:

* ```CommonLibrary/```: Shared utilities and classes used across the project, such as serialization/deserialization and helper functions.
* ```Publisher/```: Code for the publisher application responsible for broadcasting shape data.
* ```PublisherTest/```: Unit tests for verifying the publisher's functionality.
* ```Subscriber/```: Code for subscriber applications that receive and process the shape data.
* ```SubscriberTest/```: Unit tests for verifying subscriber functionalities.
* ```x64/Debug/```: Compiled binaries and debugging files.
* ```DataDistributionExercise.sln```: Solution file for building the project.

```external/```
Third-party dependencies, including:

* ```argparse```: Library for command-line argument parsing.
* ```json```: Library for handling JSON serialization and configuration files.


## Requirements
### Functional
1. **Publisher:**

* Publishes square data at 2 Hz and circle data at 3 Hz.
* Each shape has the following properties:
  * Color: Blue for squares, green for circles.
  * Size: Fixed size.
  * Location: Randomized latitude, longitude, and altitude.
2. **Subscribers:**
* **Square Subscriber:** Receives and prints messages about squares.
* **General Subscriber:** Receives and prints messages about both squares and circles.

3. **Communication:**
* All communication between the publisher and subscribers is over UDP.


## Prerequisites
1. C++ compiler with C++17 or later support (e.g., GCC, Clang, or MSVC).
2. CMake for building the project.
3. UDP communication requires an open port on localhost.
4. Git: To clone the repository, you need Git installed.


## Get Started


1. **Clone the repository**:
   ```bash
   git clone https://github.com/AlonFriedlander/DataDistributionExercise.git
   cd DataDistributionExercise
    ```
2. **Build the project using CMake:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
3. **Run the publisher:**
    ```bash
    ./Publisher
    ```
4. **Run one or more subscribers:**
    ```bash
    ./Subscriber --type square
    ./Subscriber --type general
    ```

## Configuration
Modify Configuration/Config.json to customize frequencies, shape properties, or other settings.
