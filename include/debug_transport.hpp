/**
 * @file        debug_transport.hpp
 * @brief       Debug transport interface definitions.
 */

#pragma once

#include <cstdint>

namespace debug
{

namespace transport
{

/**
 * @brief Debug transport error codes.
 */
enum class Error : std::int8_t
{
    Success = 0,
    NullPointer = -1,
    InvalidArg = -2,
    InvalidLength = -3,
    TransferError = -100
};

} // namespace transport

} // namespace debug
