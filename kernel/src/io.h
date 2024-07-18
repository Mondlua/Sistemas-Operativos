#ifndef IO_H
#define IO_H

#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include <utils/io_operation.h>

instruccion_params* deserializar_io_gen_sleep_con_interfaz(t_buffer_ins* buffer);
instruccion_params* deserializar_io_stdin_stdout_con_interfaz(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_create_delete_con_interfaz(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_truncate_con_interfaz(t_buffer_ins* buffer);
instruccion_params* deserializar_io_fs_write_read_con_interfaz(t_buffer_ins* buffer);


#endif