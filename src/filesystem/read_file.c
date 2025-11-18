#include <liburing.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/terminal.h>
#include <string.h>

/// @brief reads entire file using io_uring into malloc'd buffer
/// @param path file path
/// @param out_size output size
/// @return malloc'd buffer (must be freed by caller)
const char *read_file(const char *path, size_t *out_size){
    struct io_uring ring = {0};
    io_uring_queue_init(8, &ring, 0);

    int fd = open(path, O_RDONLY);
    if (fd < 0){
        perror("open");
        io_uring_queue_exit(&ring);
        return NULL;
    }

    struct stat filest;
    if (fstat(fd, &filest) != 0){
        perror("fstat");
        close(fd);
        io_uring_queue_exit(&ring);
        return NULL;
    }

    size_t size = filest.st_size;
    char *buf = malloc(size);
    if (!buf){
        perror("malloc");
        close(fd);
        io_uring_queue_exit(&ring);
        return NULL;
    }

    // submit read
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, size, 0);
    io_uring_submit(&ring);

    // wait for completion
    struct io_uring_cqe *cqe;
    io_uring_wait_cqe(&ring, &cqe);

    if (cqe->res < 0){
        fprintf(stderr, ERROR "io_uring read failed: %s\n", strerror(-cqe->res));
        io_uring_cqe_seen(&ring, cqe);
        io_uring_queue_exit(&ring);
        close(fd);
        free(buf);
        return NULL;
    }

    size_t read_len = (size_t)cqe->res;
    io_uring_cqe_seen(&ring, cqe);
    io_uring_queue_exit(&ring);
    close(fd);

    // partial read case
    if (read_len != size){
        fprintf(stderr, WARN "Short read: expected %zu bytes, got %zu\n", size, read_len);
        char *tmp = realloc(buf, read_len);
        if (tmp) buf = tmp;
    }

    if (out_size) *out_size = read_len;
    return buf;
}
