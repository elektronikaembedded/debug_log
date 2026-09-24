/**
 * @file        debug_transport.hpp
 * @brief       Debug transport interface definitions.
 *
 *              This header defines the common error codes used by
 *              debug transport implementations.
 *
 * Template usage:
 *              Transport implementations are provided as templates in
 *              the Port/ folder.
 *
 *              To add a new transport:
 *                1. Copy the required *.hpp.template and *.cpp.template
 *                   files from the Port/ folder.
 *                2. Rename them for the target transport.
 *                3. Implement init(), deinit(), and write().
 *                4. Add the transport type to the logger configuration.
 *
 *              Example:
 *                  Port/
 *                    debug_transport_uart.hpp.template
 *                    debug_transport_uart.cpp.template
 *
 *                  Application:
 *                    debug_transport_uart.hpp
 *                    debug_transport_uart.cpp
 *
 *              The transport implementation must provide:
 *
 *                  static Error init();
 *                  static Error deinit();
 *                  static Error write(const std::uint8_t* data,
 *                                     std::uint32_t length);
 */

#pragma once

#include <cstdint>

namespace debug {
namespace transport {

/**
 * @brief Debug transport error codes.
 */
enum class DebugError : std::int8_t {
  Success = 0,
  NullPointer = -1,
  InvalidArg = -2,
  InvalidLength = -3,
  TransferError = -100
};

} // namespace transport
} // namespace debug
