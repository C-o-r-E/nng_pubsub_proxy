#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/protocol/pubsub0/pub.h>

#define PROXY_FRONT_URL "tcp://localhost:3327"
#define PROXY_BACK_URL "tcp://localhost:3328"

void panic_on_error(int should_panic, const char* format, ...)
{
    if (should_panic)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    nng_socket sock_front_end =   NNG_SOCKET_INITIALIZER;
    nng_socket sock_back_end =    NNG_SOCKET_INITIALIZER;
    int ret = 0;

    /*
        First we need some nng sockets. Not to be confused with network sockets
    */
    ret = nng_sub0_open_raw(&sock_front_end);
    panic_on_error(ret, "Failed to open front end socket\n");

    ret = nng_pub0_open_raw(&sock_back_end);
    panic_on_error(ret, "Failed to open back end socket\n");

    /*
        Now we need to set up a listener for each socket so that they have addresses
    */

    nng_listener front_ls = NNG_LISTENER_INITIALIZER;
    nng_listener back_ls = NNG_LISTENER_INITIALIZER;

    ret = nng_listener_create(&front_ls, sock_front_end, PROXY_FRONT_URL);
    panic_on_error(ret, "Failed to create front listener\n");

    ret = nng_listener_create(&back_ls, sock_back_end, PROXY_BACK_URL);
    panic_on_error(ret, "Failed to create back listener\n");

    ret = nng_listener_start(front_ls, 0);
    panic_on_error(ret, "Failed to start front listener\n");

    ret = nng_listener_start(back_ls, 0);
    panic_on_error(ret, "Failed to start back listener\n");

    /*
        Finally let nng do the forwarding/proxying
    */

    ret = nng_device(sock_front_end, sock_back_end);
    panic_on_error(ret, "nng_device returned %d: %s\n", ret, nng_strerror(ret));

    printf("done");
    return 0;
}