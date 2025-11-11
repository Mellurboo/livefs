#pragma once
#include <unistd.h>

/// @brief non-blocking send wrapper
/// @param client_socket client socket
/// @param data data in buffer
/// @param data_size data size
/// @param flags flags
/// @return return data sent size
ssize_t async_send(int client_socket, const void *data, size_t data_size, int flags);

/// @brief non-blocking data recieve wrapper
/// @param client_socket client socket
/// @param read_buf read in buffer
/// @param read_size read size
/// @param flags flags
/// @return success / error
ssize_t async_recv(int client_socket, void *read_buf, size_t read_size, int flags);