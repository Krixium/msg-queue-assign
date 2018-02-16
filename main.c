#include <stdjio.h>
#include <stdlib.h>
#include <unistd.h>

#include "msgq.h"

int main(int argc, char * argv[])
{
    int mkey;
    int id;
    struct msgbuf msg;
    struct msgbuf rcv;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: showmsg keyavl\n");
    }

    mkey = (key_t) atoi(argv[1]);
    
    // Init structures
    memset(&msg, 0, sizeof(msgbuf));
    memset(&dest, 0, sizeof(msgbuf));

    msg.mtype = 1;
    strcpy(msg.mtext, "This is a test message");

    // Open message queue
    id = open_queue(mkey);
    if (id == -1)
    {
        exit(1);
    }

    // Write
    if (send_message(id, msg); == -1)
    {
        remove_queue(id);
        exit(2);
    }

    // Read
    if (read_message(id, msg.mtype, &dest) == -1)
    {
        remove_queue(id);
        exit(3);
    }

    fprintf(stdout, "Message: %s\n", dest.mtext);

    // Close message queue
    if (remove_queue(id) == -1)
    {
        exit(4);
    }

    exit(0);
}