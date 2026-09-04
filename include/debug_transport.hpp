/******************************************************************************
 * @file        debug_transport.hpp
 * @brief       Debug transport interface.
 *
 *              Defines the transport interface used by the debug logger.
 *              The transport can be implemented using USB CDC (Virtual COM),
 *              UART, putchar, or any other output mechanism.
 *
 *              The actual transfer implementation must be provided by the
 *              application/platform port.
 *
 * @author      Sarath S
 * @copyright   MIT License
 ******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>

namespace debug {

namespace transport {
/**
 * @brief Transport error codes.
 */
enum class error : int {
  success = 0,
  null_pointer = -1,
  invalid_arg = -2,
  invalid_length = -3,
  transfer_error = -100
};

/**
 * @brief USBCDC/UART/Putchar debug transport.
 *
 * Uses the application-initialized USBCDC/UART/Putchar stack
 * for debug output.
 */
class usb_cdc {
public:
  /** Initialize the transport. */
  static error init();

  /** Deinitialize the transport. */
  static error deinit();

  /**
   * @brief Write data to USBCDC/UART/Putchar.
   *
   * @param data Data buffer.
   * @param length Number of bytes.
   *
   * @return Number of bytes written, or a negative error code.
   */
  static error write(const std::uint8_t *data, std::uint32_t length);
};
} // namespace transport

} // namespace debug
