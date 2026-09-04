/******************************************************************************
 * @file        debug_impl.hpp
 * @brief       Internal implementation of the debug logging module.
 *
 * @author      Sarath S
 * @copyright   MIT License
 ******************************************************************************/

#pragma once

#include <cstdio>
#include <cstring>

namespace debug {

/*===========================================================================
 * Static members
 *===========================================================================*/

template <typename Transport, typename Config, typename OS>
log_level logger<Transport, Config, OS>::current_level = log_level::debug;

template <typename Transport, typename Config, typename OS>
std::uint8_t logger<Transport, Config, OS>::buffer[Config::buffer_size] = {};

template <typename Transport, typename Config, typename OS>
bool logger<Transport, Config, OS>::initialized = false;

/*===========================================================================
 * Private helpers
 *===========================================================================*/

/* Initialization */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::init() {
  if constexpr (!Config::enable) {
    initialized = false;
    return;
  }

  Transport::init();
  OS::init();

  initialized = true;
}

/* Log level */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::set_level(log_level level) {
  if constexpr (!Config::enable) {
    (void)level;
    return;
  }

  current_level = level;
}

template <typename Transport, typename Config, typename OS>
log_level logger<Transport, Config, OS>::get_level() {
  return current_level;
}

/* Error */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::error(const char *fmt, ...) {
  if constexpr (!Config::enable) {
    (void)fmt;
    return;
  }

  if (nullptr == fmt) {
    return;
  }

  va_list args;

  va_start(args, fmt);

  log(log_level::error, fmt, args);

  va_end(args);
}

/* Warning */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::warn(const char *fmt, ...) {
  if constexpr (!Config::enable) {
    (void)fmt;
    return;
  }

  if (nullptr == fmt) {
    return;
  }

  va_list args;

  va_start(args, fmt);

  log(log_level::warn, fmt, args);

  va_end(args);
}

/* Information */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::info(const char *fmt, ...) {
  if constexpr (!Config::enable) {
    (void)fmt;
    return;
  }

  if (nullptr == fmt) {
    return;
  }

  va_list args;

  va_start(args, fmt);

  log(log_level::info, fmt, args);

  va_end(args);
}

/* Debug */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::debug(const char *fmt, ...) {
  if constexpr (!Config::enable) {
    (void)fmt;
    return;
  }

  if (nullptr == fmt) {
    return;
  }

  va_list args;

  va_start(args, fmt);

  log(log_level::debug, fmt, args);

  va_end(args);
}

/* Log implementation */

template <typename Transport, typename Config, typename OS>
void logger<Transport, Config, OS>::log(log_level level, const char *fmt,
                                        va_list args) {
  if constexpr (!Config::enable) {
    (void)level;
    (void)fmt;
    (void)args;
    return;
  }

  if ((nullptr == fmt) || (false == initialized)) {
    return;
  }

  /*
   * Filter messages according to the configured level.
   *
   * Lower numeric value = higher priority.
   */
  if (static_cast<std::uint8_t>(level) >
      static_cast<std::uint8_t>(current_level)) {
    return;
  }

  /*
   * Protect the complete formatting and transmission operation.
   *
   * The buffer is shared between all logger calls, therefore the
   * lock must cover both formatting and writing.
   */
  OS::lock();

  std::uint32_t position = 0U;

  /*
   * Helper used to append formatted data safely.
   */
  auto append = [&position](const char *format, auto... values) {
    if (position >= Config::buffer_size) {
      return;
    }

    const std::uint32_t remaining = Config::buffer_size - position;

    const int ret =
        std::snprintf(reinterpret_cast<char *>(&logger::buffer[position]),
                      remaining, format, values...);

    if (ret <= 0) {
      return;
    }

    const std::uint32_t written = static_cast<std::uint32_t>(ret);

    if (written >= remaining) {
      position = Config::buffer_size - 1U;
    } else {
      position += written;
    }
  };

  /* Sequence number */

  if constexpr (Config::sequence_number) {
    static std::uint32_t sequence_no = 0U;

    ++sequence_no;

    append("[%05lu]", static_cast<unsigned long>(sequence_no));
  }

  /* Timestamp */

  if constexpr (Config::time_date_info) {
    const std::uint32_t timestamp = OS::get_timestamp();

    append("[%lu]", static_cast<unsigned long>(timestamp));
  }

  /* Thread info */

  if constexpr (Config::thread_info) {
    const char *thread_name = OS::get_thread_name();

    if (nullptr == thread_name) {
      thread_name = "MAIN";
    }

    append("[%s]", thread_name);
  }

  /* Log level */

  const char *level_name = "LOG";

  switch (level) {
  case log_level::error:
    level_name = "ERROR";
    break;

  case log_level::warn:
    level_name = "WARN";
    break;

  case log_level::info:
    level_name = "INFO";
    break;

  case log_level::debug:
    level_name = "DEBUG";
    break;

  default:
    break;
  }

  append("[%s] ", level_name);

  /* User message */

  if (position < Config::buffer_size) {
    const std::uint32_t remaining = Config::buffer_size - position;

    const int ret = std::vsnprintf(reinterpret_cast<char *>(&buffer[position]),
                                   remaining, fmt, args);

    if (ret > 0) {
      const std::uint32_t written = static_cast<std::uint32_t>(ret);

      if (written >= remaining) {
        position = Config::buffer_size - 1U;
      } else {
        position += written;
      }
    }
  }

  /* Line termination */

  if (position + 2U < Config::buffer_size) {
    buffer[position++] = '\r';
    buffer[position++] = '\n';
    buffer[position] = '\0';
  } else if (Config::buffer_size > 1U) {
    buffer[Config::buffer_size - 2U] = '\r';
    buffer[Config::buffer_size - 1U] = '\n';
  }

  /* Write the data to transport layer */
  Transport::write(buffer, position);

  OS::unlock();
}

} // namespace debug

/******************************************************************************
 * End of file
 ******************************************************************************/
