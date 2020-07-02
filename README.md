# Edge Impulse on Azure Sphere

This application is an example of Edge Impulse running on an Azure Sphere's RTCore (Cortex-M4F).

Note: Compile the RTApp in Release configuration so it fits in memory. It seems that only a few tweaks would be needed to actually fit in memory, but that exercise is left to the reader!

Place the <b>AzureSphereRTCoreToolchainCXX.cmake</b> into the <b>C:\Program Files (x86)\Microsoft Azure Sphere SDK\CMakeFiles </b>
directory or as appropriate for your Azure Sphere SDK installation.

## Prerequisites

1. [Seeed MT3620 Development Kit](https://aka.ms/azurespheredevkits) or other hardware that implements the [MT3620 Reference Development Board (RDB)](https://docs.microsoft.com/azure-sphere/hardware/mt3620-reference-board-design) design.
1. A breakout board and USB-to-serial adapter (for example, [FTDI Friend](https://www.digikey.com/catalog/en/partgroup/ftdi-friend/60311)) to connect the real-time core UART to a USB port on your PC. 
1. A terminal emulator (such as Telnet or [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/) to display the output.
