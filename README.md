# Tenok

[Tenok](https://github.com/shengwen-tw/tenok) is an experimental real-time operating system for Robotics and Internet of Things (IoT) inspired by [rtenv+](https://github.com/embedded2014/rtenv-plus) and [Piko/RT](https://github.com/PikoRT/pikoRT).

The Amis people are an indigenous tribe that originated in Taiwan, and the term "tenok" in their language means "kernel."

## Features

* POSIX interface
* Dual mode: Kernel mode / User mode
* Task and Thread (Task resembles UNIX process as a group of threads)
* Linux-like design: Wait queue, kfifo, and Tasklet (Softirq)
* Synchronization: Spinlock, Mutex, and Semaphore
* Inter-Process Communication (IPC): FIFO (Named pipe) and Message queue
* Asynchronous signals
* Software timer
* Event waiting of file descriptors with `poll()`
* Floating-point Unit (FPU) support
* Built-in Shell
* Simple rootfs and romfs
* Real-time plotting and customizable debug messaging using metalanguage
* Integration with MAVLink communication protocol
* Software-in-the-loop (SIL) simulation with Gazebo simulator

## Tools

* **msggen**: convert user-defined metalanguage messages into C codes and YAML files
* **rtplot**: For on-board data real-time plotting, where the message definitions are loaded from the auto-generated YAML files
* **gazebo_bridge**: Message forwarding between `tenok` (serial) and Gazebo simulator (TCP/IP)

## Getting Started

* [Developement Tools Setup](./docs/1-environment_setup.md)
* [Build and Run the Tenok](./docs/2-build_and_run.md)
* [Run Tenok with Gazebo Simulator](./docs/3-gazebo.md)
* [Interact with Tenok Shell](./docs/4-shell.md)

## Resources 

* [Full API List](https://tenok-rtos.github.io/globals_func.html)
* [Doxygen Page](https://tenok-rtos.github.io/index.html)
* [Presentation at COSCUP 2023](https://drive.google.com/file/d/1p8YJVPVwFAEknMXPbXzjj0y0p5qcqT2T/view?fbclid=IwAR1kYbiMB8bbCdlgW6ffHRBong7hNtJ8uCeVU4Qi5HvZ3G3srwhKPasPLEg)

## Supported Platforms

### ARM Cortex-M4F

* [STM32F4DISCOVERY](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) (STM32F407VG)
  - Select by enabling `include platform/stm32f4disc.mk` in the Makefile
  - UART1 (console): PA9 (TX), PB7 (RX)
  - UART3 (debug-link): PC10 (TX), PC11 (RX)

* [32F429IDISCOVERY](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) (STM32F429ZI)
  - Select by enabling `include platform/stm32f429disc.mk` in the Makefile
  - UART1 (console): PA9 (TX), PB7 (RX)
  - UART3 (debug-link): PC10 (TX), PC11 (RX)

* QEMU Emulation of [netduinoplus2](https://www.qemu.org/docs/master/system/arm/stm32.html) (STM32F405RGT6)
  - Select by enabling `include platform/qemu.mk` in the Makefile

## License

`Tenok` is released under the BSD 2-Clause License, for detailed information please read [LICENSE](https://github.com/shengwen-tw/neo-rtenv/blob/master/LICENSE).

## Related Projects
1. [rtenv](https://github.com/embedded2014/rtenv) / [rtenv+](https://github.com/embedded2014/rtenv-plus)
2. [mini-arm-os](https://github.com/jserv/mini-arm-os)
3. [Piko/RT](https://github.com/PikoRT/pikoRT)
4. [linenoise](https://github.com/antirez/linenoise)
