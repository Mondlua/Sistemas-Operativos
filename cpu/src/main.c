#include<stdlib.h>
#include<stdio.h>
#include<utils/client.h>
#include<utils/inicio.h>

int main(void){
    //int conexion_cpu_dispatch;
    //int conexion_cpu_interrupt;
    int conexion_memoria;
    char* ip_memoria;
    // char* ip_cpu;
    char* puerto_memoria;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
	char* puerto_escucha;
    int cantidad_entradas_tlb;
    char* algortitmos_tlb;
    
    t_log* cpu_log;
	t_config* cpu_config;

    cpu_log = iniciar_logger("cpu.log","cpu");
    cpu_config = iniciar_config("cpu.config");
    
	// cliente a memoria
	ip_memoria = config_get_string_value(cpu_config,"IP_MEMORIA");
	puerto_cpu_dispatch = config_get_string_value(cpu_config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(cpu_config, "PUERTO_CPU_INTERRUPT");
	puerto_memoria = config_get_string_value(cpu_config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(cpu_config, "PUERTO_ESCUCHA");

	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

return 0; 
}
/// servidor cpu de kernel 
  /*  char* puerto;
    int cpu_server;

    puerto = config_get_string_value(cpu_config ,"PUERTO_ESCUCHA_DISPATCH");
    // puerto = config_get_string_value(cpu_config ,"PUERTO_ESCUCHA_INTERRUPT");
    cpu_server = iniciar_servidor(puerto, cpu_log);
    log_info(cpu_log, "Modulo cpu lista para recibir a kernel");
    esperar_cliente(cpu_server, cpu_log);

    return 0;

}
*/