/******************************************************************************
 * @file        debug_log.hpp
 * @brief       Lightweight debug logging interface.
 *
 * @author      Sarath S
 * @copyright   MIT License
 ******************************************************************************/
#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

namespace debug {

/**
 * @brief Default logger configuration.
 */
struct default_config {
  static constexpr bool enable = true;
  static constexpr bool time_date_info = true;
  static constexpr bool level_name = true;
  static constexpr bool sequence_number = true;
  static constexpr bool thread_info = true;
  static constexpr std::uint32_t buffer_size = 128U;
};

/**
 * @brief Default OS configuration(RTOS/Bare-metal).
 */
struct default_os {
  static void init() {
    /**
     * @brief Init placeholder.
     */
  }

  static void deinit() {
    /**
     * @brief DeInit placeholder.
     */
  }

  static void lock() {
    /**
     * @brief Thread safety locking, use mutex or semaphore.
     */
  }

  static void unlock() {
    /**
     * @brief Thread safety unlocking, use mutex or semaphore.
     */
  }
  /**
   * @brief Map the time stamp.
   */
  static std::uint32_t get_timestamp() { return 0U; }

  /**
   * @brief Default thread name.
   */
  static const char *get_thread_name() { return "MAIN"; }
};

/**
 * @brief Logger severity level.
 */
enum class log_level : std::uint8_t { error = 0U, warn, info, debug };

/**
 * @brief Logger application operation status messages.
 */

enum class op_status : std::uint8_t { ok = 0, failed, skipped, timeout, none};

/**
 * @brief Lightweight logger.
 *
 * @tparam Transport Output transport interface.
 * @tparam Config Logger configuration.
 */
template <typename Transport, typename Config = default_config,
          typename OS = default_os>
class logger {
public:
  /** Initialize the logger. */
  static void init();

  /** Set the minimum logging level. */
  static void set_level(log_level level);

  /** Get the current logging level. */
  static log_level get_level();

  /** Log an error message. */
  static void error(const char *fmt, ...);

  /** Log a warning message. */
  static void warn(const char *fmt, ...);

  /** Log an informational message. */
  static void info(const char *fmt, ...);

  /** Log a debug message. */
  static void debug(const char *fmt, ...);

  /** Log a application operation status message. */
  static void status(op_status status, const char *fmt, ...);


private:
  static void log(op_status status, log_level level, const char *fmt, va_list args);

  static log_level current_level;

  static std::uint8_t buffer[Config::buffer_size];

  static bool initialized;
};

} // namespace debug

#include "debug_impl.hpp"

/******************************************************************************
 * End of file
 ******************************************************************************/
