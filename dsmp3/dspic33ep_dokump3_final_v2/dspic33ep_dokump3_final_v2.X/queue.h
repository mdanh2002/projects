// Microcontrollers: Interrupt-safe ring buffers
// https://www.downtowndougbrown.com/2013/01/microcontrollers-interrupt-safe-ring-buffers/?unapproved=438825&moderation-hash=29ddc91db397310e57ba63dd4243a315#comment-438825

#define USE_EDS
#define QUEUE_CAPACITY   3840

#define queue_post_t unsigned int
#define queue_val_t  signed int

void queue_clear_left();
char queue_put_left(queue_val_t data);
char queue_get_left(queue_val_t *data);
queue_post_t queue_get_len_left();

void queue_clear_right();
char queue_put_right(queue_val_t data);
char queue_get_right(queue_val_t *data);
queue_post_t queue_get_len_right();