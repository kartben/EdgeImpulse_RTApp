# Edge Impulse on Azure Sphere + Azure RTOS

This application is an example of Edge Impulse running on Azure RTOS (ThreadX) on an Azure Sphere's RTCore (Cortex-M4F).

## Structure of the Azure RTOS application

The application has been designed to showcase how one can leverage a real-time OS such as [Azure RTOS](https://azure.microsoft.com/services/rtos/) to build a system that can safely and accurately sample sensor data at a fixed frequency while, at the same time, running a compute-heavy machine learning workload.

The configuration of the application is derived from the parameters of the "impulse" that's been trained and exported from Edge Impulse. For example, the sampling thread will fire every `EI_CLASSIFIER_INTERVAL_MS` millisecond.

The following Azure RTOS Threads services are being used:

- [`thread_0`](src/demo_threadx.c#L90) is logging information about the system. It is running an infinite loop "logInfo() ; sleep(1s) ; logInfo() ; etc.".
- `mutex_0` is a mutex used for providing mutually exclusive access to the `sensor_readings` buffer used for storing sensor data.
- [`timer_0`](src/demo_threadx.c#L157) is firing every `EI_CLASSIFIER_INTERVAL_MS` millisecond and putting random/simulated data in a `sensor_readings` buffer.
- [`thread_1`](src/demo_threadx.c#L116) is running the Edge Impulse classifier using the `sensor_readings` buffer. It captures a copy of this buffer just before running the impulse, using `mutex_0` to guarantee safe access to the buffer.

What's more, since Edge Impulse relies on libraries that dynamically allocate memory, a wrapper (see [`malloc_threadx.h`](src/malloc_threadx.h)) that redefines the `malloc()`/`calloc()`/`free()` primitives has been added. It uses ThreadX's byte pool APIs and, by default, reserves 40KB in the byte pool.

## Getting started

Place the `AzureSphereRTCoreToolchainCandCXX.cmake` into the `C:\Program Files (x86)\Microsoft Azure Sphere SDK\CMakeFiles` directory or as appropriate for your Azure Sphere SDK installation.

In order to quickly test with your own model, you should be able to overwrite all the files from this project with the contents of the ZIP file you get from Edge Impulse when exporting as "C++ Library".

![](assets/edge-impulse-deploy.png)

If this is the first time you are developing a real-time capable application for Azure Sphere, you will want to make sure to follow the [instructions](https://docs.microsoft.com/en-us/azure-sphere/install/qs-real-time-application) to properly setup your development environment, including a GNU Arm Embedded Toolchain. The application has been tested with the GNU Arm Embedded Toolchain 9 2020-q2-update.

__Note:__ Compile the RTApp in Release configuration so it fits in memory. It seems that only a few tweaks would be needed to actually fit in memory, but that exercise is left to the reader!

__Note 2:__ This sample app will soon get updated with an example HL App (i.e. high-level application, running on the Azure Sphere core that has access to Internet for e.g. IoT communications) to show how the inference results can be sent to Azure IoT. In a nutshell, and if you're interested in exploring on your own in the meantime, read the guidance on how to [implement communication between high-level and real-time cores](https://docs.microsoft.com/en-us/azure-sphere/app-development/inter-app-communication).

## Prerequisites

1. [Seeed MT3620 Development Kit](https://aka.ms/azurespheredevkits) or other hardware that implements the [MT3620 Reference Development Board (RDB)](https://docs.microsoft.com/azure-sphere/hardware/mt3620-reference-board-design) design.
1. A breakout board and USB-to-serial adapter (for example, [FTDI Friend](https://www.digikey.com/catalog/en/partgroup/ftdi-friend/60311)) to connect the real-time core UART to a USB port on your PC. 
1. A terminal emulator (such as Telnet or [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/) to display the output.
