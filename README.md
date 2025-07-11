# Green-house-mangement-system
A simple ESP-32 based green house management system through which we can monitor temperature and humidity values and make sure they remain within a certain preset threshold.
## Introduction:
This project is used to control and monitor two individual green houses which were closeby.
The project has a very simple a nd easy to use Interface which has busttons and sliders.
## Components and Sensors:
- 2 X DHT 11/ DHT 12 (Used to monitor the temperature and humidity)
- 2 X Humidifier
- 2 X Cooler
- 2 X Pump
- 6 X Relays
- 2 x IRF520 (its a MOSFET used for controlling the speed  of the fan)

  The **Cooler** I used is a custom made one which uses evaporative cooling like the one with grass pads, a fan and  a pump as shown below.
  ![Cooler](https://github.com/user-attachments/assets/61f89217-faef-4983-a4f9-8d1725cdc38e)


  The  **humidifier** is used to increase the humidity, if you are using a bigger system you can go with a misting system.
  The **pump** is used to water the plants.

### Interface:
![Screenshot (230)](https://github.com/user-attachments/assets/09fe667d-0c9c-432a-ad27-ac2d8e7cb2a1)

  The manual and auto mode are povided in the interface the manual mode is triggered for manual control, Usually the system should run in the auto mode where the temperatue and humidity are
  maintained automatically (the temperature and humidity are maintained below the given thresholds).

## Circuit Diagram:
The complete connection to the relays is not shown as I assume you know how to connect a relay.

![Greenhouse circuit_page-0001](https://github.com/user-attachments/assets/3a8c8cea-b68f-44d2-b976-9861a01f1a8e)

## Key Learning for me:
When using external power supplies to run your components always connect the grounds of your external power supply and your microcontroller to make a common reference ground.


    



