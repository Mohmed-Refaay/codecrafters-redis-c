#ifndef WRITER_H
#define  WRITER_H

void write_simple_string(int fd, char *str);
void write_bulk_string(int fd, char *str, int len);
void write_error(int fd, char *error_msg);


#endif