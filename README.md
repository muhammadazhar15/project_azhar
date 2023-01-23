# [project_azhar](https://github.com/muhammadazhar15/project_azhar)
Collection of projects that have been made

# [PROJECT 1: Soccer Robot Localization Based on Sensor Fusion from Odometry and Omnivision](Project1-robot_localization)

<div align="center">
 <img src="Project1-robot_localization/Image/Robot_design/robot.JPG" height="200px">
 <img src="Project1-robot_localization/Image/Robot_design/Robot%20design.png" height="200px">
 <img src="Project1-robot_localization/Image/Robot_design/Framework%20robot.png" height="200px">
</div>
  
* This project is research to design new robot localization system for [IRIS ITS](https://iris.its.ac.id/) robots
* This localization system combines Odometry and Particle Filter through sensor fusion method
* Odometry uses 2 encoder sensors and a gyro sensor to sense robot displacement
* Particle Filter use omnivision as visual sensor to detect white line in the soccer field through image processing
* For more complete explanation about this research, you can check [this](Project1-robot_localization/Paper)

> **This is my early research particle filter with python**
> 
> https://user-images.githubusercontent.com/120243882/213215666-4653b4f7-5cf8-48bd-91ea-d05469076f68.mp4
> * if you want try this program you can check [this](Project1-robot_localization/Prototype_particle_filter)

> **This is particle filter that applied to the soccer robot**
> 
> https://user-images.githubusercontent.com/120243882/213215766-411f6838-0309-486a-b52c-df728a80d029.mp4

> **This is final localization system with senor fusion**
> 
> https://user-images.githubusercontent.com/120243882/213237570-8e828789-f15e-48b3-8e15-77b1aa8b0c28.mp4
> * The red line is the localization data generated by sensor fusion
> * This video compare sensor fusion data with original robot location in the soccer field

# [PROJECT 2: Voice Recognition System for Commands on Wheeled Soccer Robots](Project2-voice_command)

<div align="center" margin-right="100px">
 <img src="Project2-voice_command/Image/voice%20activity%20detector.JPG" height="300px">
 &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
 <img src="Project2-voice_command/Image/CNN%20system.JPG" height="300px">  
</div>

* This research conducted to give [IRIS ITS](https://iris.its.ac.id/) soccer robots some orders through human voice
* There are 2 main processes in this voice recognition system, record and recognition
* This research use CNN (Convolutional Neural Network) to recognize commands from human voice
* There are 7 commands that can be recognized ['Corner', 'Dropball', 'Goalkick', 'Kalibrasi', 'Kickoff', 'Stop', 'Tendang']
* In this research MFCC is used as feature extraction method to process data from human voice
* Data that used in CNN training process comes from 5 different people and 9 different people for test
* For more complete explanation about this research, you can check [this](Project2-voice_command/Paper)

> **Those are some visualization data from voice signal data and MFCC feature extraction results**
> 
> <img src="Project2-voice_command/Image/Data_train/azhar.png" width="120%" height="120%">

> **Voice Commands Demo for Soccer Robot**
> 
> https://user-images.githubusercontent.com/120243882/213928835-b99e419a-455b-4ef3-9824-324b2552bdfb.mp4

# [PROJECT 3: Heart Disease Detection System with ANN (Artificial Neural Network)](Project3-heart_disease_detector)

<div align="center">
 <img src="Project3-heart_disease_detector/Image/home.JPG" width="40%" height="40%">
</div>

* Research project while teaching at [MA Darussalam Jombang](https://emispendis.kemenag.go.id/dashboard/?content=madrasah&action=lbg&nss=131235170053)
* Introducing AI (Artificial Intelligence) to Madrasah students, especially about Neural Networks
* This research was conducted with students to prepare for participate in [LKIR LIPI](https://kompetisi.brin.go.id/tentang/lkir) 2021
* Heart Disease Detection System is designed to detect disease through health status and heart sounds
* Health status data comes from [Kaggle](https://www.kaggle.com/datasets/johnsmith88/heart-disease-dataset) and heart sound data comes from [here](http://www.peterjbentley.com/heartchallenge/)
* The Heart Disease Detection System is applied in the website
* For more complete explanation about this research, you can check [this](Project3-heart_disease_detector/Paper)

> **Heart Disease Detection System Demo**
> 
> https://user-images.githubusercontent.com/120243882/213934320-be85940c-d860-4ea4-b8dd-51fb7ff6c627.mp4
