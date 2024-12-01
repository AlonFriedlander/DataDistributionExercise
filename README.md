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

4. **Git:** To clone the repository, you need Git installed.


## Get Started

To get started with the Music Rehearsal App, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/AlonFriedlander/MusicRehearsalApp.git
    ```
2. **Navigate to the main directory:**
    ```bash
    cd MusicRehearsalApp
    ```
3. **Install dependencies for both the frontend and backend:**
    * Navigate to the backend directory and install dependencies:
        ```bash
        cd backend
        npm install
        ```
    * Navigate to the frontend directory and install dependencies:
        ```bash
        cd ../frontend
        npm install
        ```
**Before continuing, make sure to set up the required environment variables as described in the Environment Variables section below.**

4. **Run the development servers:**
    * Start the backend server:
        ```bash
        cd ../backend
        npm start
        ```
    * Start the frontend server:
        ```bash
        cd ../frontend
        npm start
        ```


## Environment Variables
Ensure you set up the following environment variables before running the app:

Backend (.env in the backend directory):
```bash
PORT=5000
MONGO_URI=your_mongodb_connection_string
JWT_SECRET=your_jwt_secret
```
Frontend (.env in the frontend directory):
```bash
REACT_APP_BACKEND_URL=http://localhost:5000
```
Replace your_mongodb_connection_string and your_jwt_secret with your actual values. Ensure these .env files are excluded from version control by checking your .gitignore file.

### notes:
* **PORT:** The port where the backend server runs (default: 5000).
* **MONGO_URI:** The MongoDB connection string for the database.
* **JWT_SECRET:** A secret key used to sign and verify JWT tokens.
* **REACT_APP_BACKEND_URL:** The URL the frontend uses to communicate with the backend server.


## API Endpoints
### Authentication
* **POST /api/auth/register:** Registers a new user with a username, password, instrument, and role.
* **POST /api/auth/login:** Logs in an existing user and returns an authentication token.
* **GET /api/auth/validate-token:** Validates the user's token and returns user information.
* **GET /api/auth/validate-admin-token:** Validates the admin's token and returns admin

### Rehearsal Management
* **GET /api/rehearsal/search:** Searches for songs based on a query string.
* **POST /api/rehearsal/select:** Selects a song to be used in a live session.
* **GET /api/rehearsal/live/song:** Retrieves the current live song's data.
* **POST /api/rehearsal/admin/quit-session:** Ends the current rehearsal session.



## This app is constructed from two main projects:

* **Backend:** Handles the server-side logic, database operations, and real-time socket communication.

* **Frontend:** Provides the user interface for interacting with the app, including selecting songs and viewing lyrics.
