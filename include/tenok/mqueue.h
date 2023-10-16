/**
 * @file
 */
#ifndef __MQUEUE_H__
#define __MQUEUE_H__

#include <fs/fs.h>
#include <mm/mpool.h>
#include <kernel/ipc.h>

typedef int mqd_t;

struct mq_attr {
    int mq_flags;   /* flags: 0 or O_NONBLOCK */
    int mq_maxmsg;  /* max number of the messages in the queue */
    int mq_msgsize; /* byte size of the message */
    int mq_curmsgs; /* number of the messages currently in the queue */
};

struct msg_queue {
    char name[FILE_NAME_LEN_MAX];
    struct mq_attr attr;
    pipe_t *pipe;
};

/**
 * @brief  Retrieve atributes of the message queue referred to by the message queue
           descriptor mqdes
 * @param  mqdes: The message queue descriptor to provide.
 * @param  newattr: The new attributes to provide.
 * @param  oldattr: The memory space for storing old attributes.
 * @retval int: 0 on sucess and nonzero error number on error.
 */
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);

/**
 * @brief  Modify attributes of the message queue referred to by the message queue
           descriptor mqdes
 * @param  mqdes: The message queue descriptor to provide.
 * @param  attr: The memory space for retrieving message queue attributes.
 * @retval int: 0 on sucess and nonzero error number on error.
 */
int mq_setattr(mqd_t mqdes, const struct mq_attr *newattr,
               struct mq_attr *oldattr);

/**
 * @brief  Create a new POSIX message queue or opens an existing queue
 * @param  name: The name of the message queue.
 * @param  oflag: The flags that specifies the control the operation of the call.
           Currently only support O_NONBLOCK.
 * @param  attr: The attribute object for setting the message queue.
 * @retval mqd_t: The message queue descriptor to return.
 */
mqd_t mq_open(const char *name, int oflag, struct mq_attr *attr);

/**
 * @brief  Close the message queue descriptor mqdes
 * @param  mqdes: The message queue descriptor to provide.
 * @retval int: 0 on sucess and nonzero error number on error.
 */
int mq_close(mqd_t mqdes);

/**
 * @brief  Remove the oldest message from the message queue referred to by the message
           queue descriptor mqdes and places it in the buffer pointed to by msg_ptr
 * @param  mqdes: The message queue descriptor to provide.
 * @param  msg_ptr: The buffer to provide for storing the received message.
 * @param  msg_len: The length of the buffer pointed to by msg_ptr.
 * @param  msg_prio: Not implemented.
 * @retval ssize_t: The number of bytes in the received message.
 */
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);

/**
 * @brief  Add the message pointed to by msg_ptr to the message queue referred to by
           the message queue descriptor mqdes
 * @param  mqdes: The message queue descriptor to provide.
 * @param  msg_ptr: The message to provide for sending to the message queue.
 * @param  msg_len: The length of the message pointed to by msg_ptr.
 * @param  msg_prio: Not implemented.
 * @retval int: 0 on sucess and nonzero error number on error.
 */
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);

#endif
