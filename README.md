<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/a7c2bccd-131b-4acd-8dc7-73c232ca1948" alt="DALLE2024-05-2201 57 33" width="300"/>

# forecast-rp2040-station

## Introduction

The forecast-rp2040-station is a project based on the Raspberry Pi RP2040 microcontroller. It aims to provide a compact and efficient meteorological station capable of monitoring and displaying various weather parameters. This project leverages the power of the RP2040 to collect data from sensors and display the information in real-time.


## Functionality

The forecast-rp2040-station offers the following features:

- **Temperature Monitoring**: Continuously tracks the ambient temperature.
- **Humidity Tracking**: Provides real-time humidity levels.
- **Pressure Measurement**: Measures atmospheric pressure.
- **Data Display**: Shows the collected data on a built-in display.
- **Data Logging**: Logs the weather data for historical analysis.
- **Connectivity**: Supports connectivity options to upload data to a cloud service.

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/ede9e482-9626-4ea4-907a-657552a753c1" alt="2024-05-19 02 01 09" width="300"/>

## How to Run the Code

To get started with the forecast-rp2040-station, follow these steps:

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/kuzlik340/forecast-rp2040-station.git
    cd forecast-rp2040-station
    ```

2. **Install Dependencies**:
    Ensure you have the necessary dependencies installed. You can do this by running:
    ```sh
    pip install -r requirements.txt
    ```

3. **Upload the Code to RP2040**:
    Use a suitable tool to upload the code to your RP2040 microcontroller. You might use tools like Thonny or command line tools.

4. **Connect the Sensors**:
    Connect the temperature, humidity, and pressure sensors to the appropriate GPIO pins on the RP2040.

5. **Run the Code**:
    Execute the main script to start the meteorological station.
    ```sh
    python main.py
    ```

6. **View Data**:
    Once the code is running, you can view the collected weather data on the display connected to the RP2040.

<img src="https://github.com/kuzlik340/forecast-rp2040-station/assets/145336491/635a64a0-a4c4-4f85-825c-b47dbc7b113a" alt="IMG_1922 (1)" width="300"/>

## Conclusion

The forecast-rp2040-station is a versatile project that demonstrates the capabilities of the Raspberry Pi RP2040 in monitoring and displaying weather data. With its range of sensors and connectivity options, it serves as an excellent tool for both educational purposes and practical applications in weather monitoring.

Feel free to contribute to this project by submitting issues or pull requests. For more details, check the documentation in the repository.



