/**
 * @file        debug_log.hpp
 * @brief       Lightweight debug logging interface.
 */

#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

namespace debug
{

/**
 * @brief Default logger configuration.
 */
struct DefaultConfig
{
    static constexpr bool kEnable = true;
    static constexpr bool kTimeDateInfo = true;
    static constexpr bool kLevelName = true;
    static constexpr bool kSequenceNumber = true;
    static constexpr bool kThreadInfo = true;
    static constexpr std::uint32_t kBufferSize = 128U;
};

/**
 * @brief Default OS configuration.
 *
 *        Provides no-op OS functions for bare-metal applications.
 */
struct DefaultOs
{
    static void init() {}
    static void deinit() {}
    static void lock() {}
    static void unlock() {}

    static std::uint32_t getTimestamp()
    {
        return 0U;
    }

    static const char* getThreadName()
    {
        return "MAIN";
    }
};

/**
 * @brief Logger severity level.
 */
enum class LogLevel : std::uint8_t
{
    Error = 0U,
    Warn,
    Info,
    Debug
};

/**
 * @brief Application operation status.
 */
enum class OpStatus : std::uint8_t
{
    Ok = 0U,
    Failed,
    Skipped,
    Timeout,
    None
};

/**
 * @brief Lightweight logger.
 *
 * @tparam Transport Output transport interface.
 * @tparam Config    Logger configuration.
 * @tparam Os        OS abstraction.
 */
template <
    typename Transport,
    typename Config = DefaultConfig,
    typename Os = DefaultOs>
class Logger
{
public:
    /**
     * @brief Initialize the logger.
     */
    static void init();

    /**
     * @brief Set the minimum logging level.
     *
     * @param level Minimum level to be logged.
     */
    static void setLevel(LogLevel level);

    /**
     * @brief Get the current logging level.
     *
     * @return Current minimum logging level.
     */
    [[nodiscard]] static LogLevel getLevel();

    /**
     * @brief Log an error message.
     */
    static void error(const char* fmt, ...);

    /**
     * @brief Log a warning message.
     */
    static void warn(const char* fmt, ...);

    /**
     * @brief Log an informational message.
     */
    static void info(const char* fmt, ...);

    /**
     * @brief Log a debug message.
     */
    static void debug(const char* fmt, ...);

    /**
     * @brief Log an application operation status message.
     */
    static void status(OpStatus status, const char* fmt, ...);

private:
    static void log(
        OpStatus status,
        LogLevel level,
        const char* fmt,
        va_list args);

    static LogLevel currentLevel;
    static std::uint8_t buffer[Config::kBufferSize];
    static bool initialized;
    static std::uint32_t sequenceNumber;
};

} // namespace debug

#include "debug_impl.hpp"
