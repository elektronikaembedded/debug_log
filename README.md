# debug_log

A small C++ debug logging module for embedded projects.

I started this project because I wanted a simple logging module that I could drop into different embedded projects without rewriting the logging code every time.

The main idea is to keep the **logger separate from the hardware transport**.

For example, the logger shouldn't have to know whether the output is going through USB CDC or UART. The project decides that.

## How it works

From the application side, I want logging to be as simple as:

```cpp
project::log::init();

project::log::info("System started");
```

The application doesn't need to worry about the underlying transport.

The project configuration decides what is used underneath:

```cpp
struct system_config
{
    using debug_transport = debug::transport::usb_cdc;
    using debug_config    = project_debug_log_config;
};

namespace project
{
    using log = debug::logger<
        system_config::debug_transport,
        system_config::debug_config>;
}
```

So if I later change from USB CDC to UART, the application code doesn't need to change.

## The basic idea

The module is split into a few simple parts:

```text
debug_log
   │
   ├── Logger
   │
   ├── Transport
   │      ├── USB CDC
   │      ├── UART
   │      └── Other transports
   │
   └── Configuration
```

The logger takes care of the logging itself.

The transport takes care of sending the data somewhere.

The project configuration ties everything together.

## Project structure

For example, in an STM32 Cube project I use it like this:

```text
Project
│
├── Modules/
│   └── debug_log/
│       ├── debug_log.hpp
│       ├── debug_transport.hpp
│       ├── debug_impl.hpp
│       └── ...
│
├── Ports/
│   └── debug_transport_usb_cdc.cpp
│
├── Configs/
│   └── debug_log_config.hpp
│
└── Project/
    └── system_config.hpp
```

The important thing here is that the core module doesn't contain STM32-specific transport code.

The STM32-specific part lives in the `Ports` directory.

## Adding another transport

If I want to use UART instead of USB CDC, I can add another transport implementation:

```text
Ports/
├── debug_transport_usb_cdc.cpp
└── debug_transport_uart.cpp
```

and select it from the project configuration.

The application can still use:

```cpp
project::log::info("Hello");
```

without knowing how the message is actually transmitted.

## USB CDC

I'm currently using USB CDC as one of the transports.

There is also a template available:

```text
debug_transport_usb_cdc.cpp.template
```

The template provides the basic structure, while the actual USB CDC transfer is implemented by the target project.

This keeps the logging module independent of the particular STM32 USB implementation.



while the hardware-specific details stay outside the logger itself.


## License

MIT License

Copyright © Sarath S
