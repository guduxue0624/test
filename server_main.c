#include <stdio.h>
#include "websocket_common.h"
#include "server_base.h"
#include "kw_ws_server.h"

int main(int argc, char const *argv[])
{
    server_start(8000,(void *)&kw_server_callBack);
    return 0;
}
