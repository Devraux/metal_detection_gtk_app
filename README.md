### Metal Detection App

The Metal Detection App is designed to work with the Metal Detection Vehicle, available in another one of my repositories with the same name. The goal of this application is to present the data collected by the metal detection vehicle in a convenient and readable format. The app is written with multi-threading to operate in a pseudo-real-time manner. The multi-threading is handled using the pthread library. 

The graphical part of the application was developed using the GTK3 and Cairo frameworks.

#### Vehicle Control
Vehicle control is as follows:
- **W** – Move forward
- **A** – Turn left
- **D** – Turn right
- **S** – Move backward

The application allows the user to track the vehicle's location in real-time (displayed as a larger red dot) as well as the locations where the metal detection system has found metals (smaller black dots). 

The collected data can be saved by clicking the "Save and Exit" button. The saved data will be stored in the project folder under the file name "detections.txt". 

The app also includes additional features:
- Monitoring the connection status with the vehicle (to check if the connection is lost).
- Access to the inertial mapping data (implemented on the vehicle) and GPS mapping data (also implemented on the vehicle). Each detection point is linked to both mapping methods, allowing for precise localization of potential metal detections.

#### Desktop application looks
![przykład dzialania mapa](https://github.com/user-attachments/assets/01a88951-3ff1-42ea-82ec-06b63fb4c1b5)
