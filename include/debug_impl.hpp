/**
 * @file        debug_impl.hpp
 * @brief       Internal implementation of the debug logging module.
 *
 *              Contains the template method implementations.
 */

#pragma once

#include <cstdio>

namespace debug
{

/*===========================================================================
 * Static members
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
LogLevel Logger<Transport, Config, Os>::currentLevel = LogLevel::Debug;

template <typename Transport, typename Config, typename Os>
std::uint8_t Logger<Transport, Config, Os>::buffer[Config::kBufferSize] = {};

template <typename Transport, typename Config, typename Os>
std::uint32_t Logger<Transport, Config, Os>::sequenceNumber = 0U;

template <typename Transport, typename Config, typename Os>
bool Logger<Transport, Config, Os>::initialized = false;

/*===========================================================================
 * Initialization
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::init()
{
    if constexpr (!Config::kEnable)
    {
        initialized = false;
        return;
    }

    Transport::init();
    Os::init();

    initialized = true;
}

/*===========================================================================
 * Log level
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::setLevel(LogLevel level)
{
    if constexpr (!Config::kEnable)
    {
        (void)level;
        return;
    }

    currentLevel = level;
}

template <typename Transport, typename Config, typename Os>
LogLevel Logger<Transport, Config, Os>::getLevel()
{
    return currentLevel;
}

/*===========================================================================
 * Error
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::error(const char* fmt, ...)
{
    if constexpr (!Config::kEnable)
    {
        (void)fmt;
        return;
    }

    if (nullptr == fmt)
    {
        return;
    }

    va_list args;

    va_start(args, fmt);
    log(OpStatus::None, LogLevel::Error, fmt, args);
    va_end(args);
}

/*===========================================================================
 * Warning
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::warn(const char* fmt, ...)
{
    if constexpr (!Config::kEnable)
    {
        (void)fmt;
        return;
    }

    if (nullptr == fmt)
    {
        return;
    }

    va_list args;

    va_start(args, fmt);
    log(OpStatus::None, LogLevel::Warn, fmt, args);
    va_end(args);
}

/*===========================================================================
 * Information
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::info(const char* fmt, ...)
{
    if constexpr (!Config::kEnable)
    {
        (void)fmt;
        return;
    }

    if (nullptr == fmt)
    {
        return;
    }

    va_list args;

    va_start(args, fmt);
    log(OpStatus::None, LogLevel::Info, fmt, args);
    va_end(args);
}

/*===========================================================================
 * Debug
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::debug(const char* fmt, ...)
{
    if constexpr (!Config::kEnable)
    {
        (void)fmt;
        return;
    }

    if (nullptr == fmt)
    {
        return;
    }

    va_list args;

    va_start(args, fmt);
    log(OpStatus::None, LogLevel::Debug, fmt, args);
    va_end(args);
}

/*===========================================================================
 * Operation status
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::status(
    OpStatus status,
    const char* fmt,
    ...)
{
    if constexpr (!Config::kEnable)
    {
        (void)status;
        (void)fmt;
        return;
    }

    if (nullptr == fmt)
    {
        return;
    }

    LogLevel level = LogLevel::Info;

    switch (status)
    {
    case OpStatus::Ok:
    case OpStatus::None:
        level = LogLevel::Info;
        break;

    case OpStatus::Failed:
    case OpStatus::Timeout:
        level = LogLevel::Error;
        break;

    case OpStatus::Skipped:
        level = LogLevel::Warn;
        break;

    default:
        break;
    }

    va_list args;

    va_start(args, fmt);
    log(status, level, fmt, args);
    va_end(args);
}

/*===========================================================================
 * Log implementation
 *===========================================================================*/

template <typename Transport, typename Config, typename Os>
void Logger<Transport, Config, Os>::log(
    OpStatus status,
    LogLevel level,
    const char* fmt,
    va_list args)
{
    if constexpr (!Config::kEnable)
    {
        (void)status;
        (void)level;
        (void)fmt;
        (void)args;
        return;
    }

    if ((nullptr == fmt) || (false == initialized))
    {
        return;
    }

    /*
     * Filter messages according to the configured level.
     *
     * Lower numeric value = higher priority.
     */
    if (static_cast<std::uint8_t>(level) >
        static_cast<std::uint8_t>(currentLevel))
    {
        return;
    }

    /*
     * The buffer is shared between logger calls, therefore the lock
     * must cover both formatting and transmission.
     */
    Os::lock();

    std::uint32_t position = 0U;

    /*
     * Helper used to append formatted data safely.
     */
    auto append = [&position](
        const char* format,
        auto... values)
    {
        if (position >= Config::kBufferSize)
        {
            return;
        }

        const std::uint32_t remaining =
            Config::kBufferSize - position;

        const int ret =
            std::snprintf(
                reinterpret_cast<char*>(&Logger::buffer[position]),
                remaining,
                format,
                values...);

        if (ret <= 0)
        {
            return;
        }

        const std::uint32_t written =
            static_cast<std::uint32_t>(ret);

        if (written >= remaining)
        {
            position = Config::kBufferSize - 1U;
        }
        else
        {
            position += written;
        }
    };

    /* Sequence number */

    if constexpr (Config::kSequenceNumber)
    {
        ++sequenceNumber;

        append(
            "[%05lu]",
            static_cast<unsigned long>(sequenceNumber));
    }

    /* Timestamp */

    if constexpr (Config::kTimeDateInfo)
    {
        const std::uint32_t timestamp =
            Os::getTimestamp();

        append(
            "[%lu]",
            static_cast<unsigned long>(timestamp));
    }

    /* Thread information */

    if constexpr (Config::kThreadInfo)
    {
        const char* threadName = Os::getThreadName();

        if (nullptr == threadName)
        {
            threadName = "MAIN";
        }

        append("[%s]", threadName);
    }

    /* Log level */

    const char* levelName = "LOG";

    switch (level)
    {
    case LogLevel::Error:
        levelName = "ERROR";
        break;

    case LogLevel::Warn:
        levelName = "WARN";
        break;

    case LogLevel::Info:
        levelName = "INFO";
        break;

    case LogLevel::Debug:
        levelName = "DEBUG";
        break;

    default:
        break;
    }

    if constexpr (Config::kLevelName)
    {
        append("[%s] ", levelName);
    }

    /* User message */

    if (position < Config::kBufferSize)
    {
        const std::uint32_t remaining =
            Config::kBufferSize - position;

        const int ret =
            std::vsnprintf(
                reinterpret_cast<char*>(&buffer[position]),
                remaining,
                fmt,
                args);

        if (ret > 0)
        {
            const std::uint32_t written =
                static_cast<std::uint32_t>(ret);

            if (written >= remaining)
            {
                position = Config::kBufferSize - 1U;
            }
            else
            {
                position += written;
            }
        }
    }

    /* Operation status */

    if (status != OpStatus::None)
    {
        const char* statusName = "[OK]";

        switch (status)
        {
        case OpStatus::Ok:
            statusName = "[OK]";
            break;

        case OpStatus::Failed:
            statusName = "[FAIL]";
            break;

        case OpStatus::Timeout:
            statusName = "[TIMEOUT]";
            break;

        case OpStatus::Skipped:
            statusName = "[SKIP]";
            break;

        default:
            break;
        }

        append(" %s", statusName);
    }

    /* Line termination */

    if (position + 2U < Config::kBufferSize)
    {
        buffer[position++] = '\r';
        buffer[position++] = '\n';
        buffer[position] = '\0';
    }
    else if (Config::kBufferSize > 1U)
    {
        buffer[Config::kBufferSize - 2U] = '\r';
        buffer[Config::kBufferSize - 1U] = '\n';
    }

    /* Write the data to the transport layer */

    Transport::write(buffer, position);

    Os::unlock();
}

} // namespace debug

