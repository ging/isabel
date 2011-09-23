/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 *	isabel_openvpn
 *
 * @author David Prieto Ruiz
 * @version 0.2
 * 
 *	Ejecutable que reune todas las funciones necesarias para adaptar el uso de openvpn a isabel.
 *	Desarrollado en C para poder fijar suid=1 y ejecutarse como root, dado que openvpn e iptables requieren permisos de superusuario.
 * 
 *	Permite un único servidor openvpn y múltiples clientes. 
 *	
 *	Utilidades:
 *	- isabel_openvpn server	ARGS				Lanza el servidor openvpn.
 *	- isabel_openvpn server_up					Configura iptables para limitar el acceso en los puertos no deseados.
 *	- isabel_openvpn server_down				Restaura la configuración de iptables.
 *	- isabel_openvpn client	ARGS				Lanza un cliente openvpn.
 *	- isabel_openvpn client_up INTERFACE		Configura iptables para limitar el acceso en los puertos no deseados.
 *	- isabel_openvpn client_down INTERFACE		Restaura la configuración de iptables.
 *	- isabel_openvpn test_UDP_port PORT			Comprueba que si un puerto UDP está siendo usado.
 *	- isabel_openvpn add_alternative_port ARGS	Añade una regla iptables para que el servidor de tuneles escuche en un puerto alternativo
 *	- isabel_openvpn server_status				Informa del estado del servidor de túneles
 *	- isabel_openvpn clients_status				Informa de los clientes de túneles lanzados.
 *	- isabel_openvpn status						Proporciona información de los parámetros de red relacionados con los túneles de isabel.
 * 	- isabel_openvpn clean_up					Limpieza general de todo lo relacionado con túneles
 * 
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 *	Variable global utilizada en las ejecuciones en modo server y client.
 *	En ella se guarda el pid de openvpn para matarlo en caso de recibirse SIGTERM
 */
int pid_hijo = 0;



/*
 * Estructura con la información del fichero de estado del servidor de túneles.
 */
struct serverInfo
{
	char INTERFACE [10];
	char PRIVATE_IP [20];
	char STATUS [10];
	char PID [10];
};


/*
 * Estructura con la información del fichero de estado de un cliente de tuneles
 */
struct clientInfo
{
	char INTERFACE [10];
	char LOCAL_PRIVATE_IP [20];
	char SERVER_PRIVATE_IP [20];
	char SERVER_PUBLIC_IP [20];
	char NAME [100];
	char STATUS [10];
	char PID [10];
};


/*
 * Devuelve la informacion del servidor de tuneles
 */
struct serverInfo
getServerInfo(){
	struct serverInfo s;
	FILE *flujo;
	char key [20];
	char value [20];
	char cmd [1024];
	
	//Se crea un flujo de lectura del archivo de intercambio
	if ( (flujo = fopen("/tmp/serverTunnelStatus","r")) == NULL ){
		sprintf(s.STATUS,"STOPPED");
		return s;
	}
	
	//Escanea cada linea del fichero (una direccion y un puerto en cada linea) y elimina la regla de iptables correspondiente a cada uno
	while(fscanf(flujo, "%s %s" ,key,value)!=EOF){
		if (strcmp(key,"INTERFACE")==0){
			sprintf(s.INTERFACE,value);
		}
		if (strcmp(key,"PRIVATE_IP")==0){
			sprintf(s.PRIVATE_IP,value);
		}
		if (strcmp(key,"PID")==0){
			sprintf(s.PID,value);
		}
		if (strcmp(key,"STATUS")==0){
			sprintf(s.STATUS,value);
		}
	}
	//Comprobamos que los datos son reales
	if(strcmp(s.STATUS,"STARTED")==0){
		sprintf(cmd,"/sbin/ifconfig %s | grep %s > /dev/null 2>&1",s.INTERFACE,s.PRIVATE_IP);
    	if(system(cmd)!=0){
    		sprintf(s.STATUS,"STOPPED");
    	}
	}else{
		sprintf(s.STATUS,"STARTED");
	}
	
	return s;
}


/*
 * Devuelve la informacion de un cliente a partir del nombre de archivo.
 */
struct clientInfo
getClientInfo(const char *path){
	struct clientInfo c;
	FILE *flujo;
	char key [20];
	char value [20];
	char cmd [1024];
	
	//Se crea un flujo de lectura del archivo de intercambio
	if ( (flujo = fopen(path,"r")) == NULL )
	{
		sprintf(c.STATUS,"STOPPED");
		return c;
	}
	
	//Escanea cada linea del fichero (una direccion y un puerto en cada linea) y elimina la regla de iptables correspondiente a cada uno
	while(fscanf(flujo, "%s %s" ,key,value)!=EOF){
		if (strcmp(key,"INTERFACE")==0){
			sprintf(c.INTERFACE,value);
		}
		if (strcmp(key,"LOCAL_PRIVATE_IP")==0){
			sprintf(c.LOCAL_PRIVATE_IP,value);
		}
		if (strcmp(key,"SERVER_PRIVATE_IP")==0){
			sprintf(c.SERVER_PRIVATE_IP,value);
		}
		if (strcmp(key,"SERVER_PUBLIC_IP")==0){
			sprintf(c.SERVER_PUBLIC_IP,value);
		}
		if (strcmp(key,"NAME")==0){
			sprintf(c.NAME,value);
		}
		if (strcmp(key,"PID")==0){
			sprintf(c.PID,value);
		}
	}
	
	//Comprobamos que los datos son reales
	sprintf(cmd,"/sbin/ifconfig %s | grep %s > /dev/null 2>&1",c.INTERFACE,c.LOCAL_PRIVATE_IP);
    if(system(cmd)==0){
    		sprintf(c.STATUS,"STARTED");
    }else{
		sprintf(c.STATUS,"STOPPED");
	}
	
	return c;
}


/*
 *	Método invocado al recibir la señal SIGTERM en los modos server y client
 *	Mata openvpn y termina isabel_openvpn
 */
void
handle(int sig)
{
  int resultado;
  if (pid_hijo==0){
	printf("pid_hijo=0!!!!!!!!!!");
  	return;
  }
  if ((resultado=kill(pid_hijo,SIGTERM))==0 ){
	printf("isabel_openvpn (pid %i) killed openvpn correctly and ends\n",getpid());
	char cmd [1024];
    sprintf(cmd,"/bin/rm -rf /tmp/serverTunnelStatus");
    system (cmd);
	exit(0);
  }else{
    	printf("isabel_openvpn couldn't kill openvpn");
	exit(1);
  }
}


/*
 * Método que añade las reglas iptables que limitan el acceso al servidor de tuneles
 */
int
server_protection_up()
{
	char cmd [1024];
	
	struct serverInfo s = getServerInfo();
    if(strcmp(s.STATUS,"STARTED") != 0) {
    	return 1;
    }
			
	// Añade reglas que limitan el acceso a los puertos TCP que no son utilizados por isabel
	sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p tcp --dport 53000:53023 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p tcp --sport 53000:53023 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j DROP -p tcp -i %s",s.INTERFACE);
    system (cmd);

	// Añade reglas que limitan el acceso a los puertos UDP que no son utilizados por isabel
    sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p udp --dport  53000:53032 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p udp --sport  53000:53032 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j DROP -p udp -i %s",s.INTERFACE);
    system (cmd);
    
    return 0;
}


/*
 * Método que eliminan las reglas iptables que limitan el acceso al servidor de tuneles
 */
int
server_protection_down()
{
	char cmd [1024];
	
	struct serverInfo s = getServerInfo();
    //Ojo s.STATUS es STOPPED porque ya se ha cerrado el tunel.
    
	// Elimina reglas que limitan el acceso a los puertos TCP que no son utilizados por isabel
	sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p tcp --dport 53000:53023 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p tcp --sport 53000:53023 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j DROP -p tcp -i %s",s.INTERFACE);
    system (cmd);

	// Eliminan reglas que limitan el acceso a los puertos UDP que no son utilizados por isabel
    sprintf(cmd,"iptables -D INPUT -j ACCEPT -p udp --dport  53000:53032 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"iptables -D INPUT -j ACCEPT -p udp --sport  53000:53032 -i %s",s.INTERFACE);
    system (cmd);
    sprintf(cmd,"iptables -D INPUT -j DROP -p udp -i %s",s.INTERFACE);
    system (cmd);
    
    return 0;
}


/*
 * Método que añade las reglas iptables que limitan el acceso al cliente de tuneles
 */
void
client_protection_up(const char *dev)
{
	char cmd [1024];

	// Añade reglas que limitan el acceso a los puertos TCP que no son utilizados por isabel     
	sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p tcp --dport 53000:53023 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p tcp --sport 53000:53023 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j DROP -p tcp -i %s",dev);
    system (cmd);

	// Añade reglas que limitan el acceso a los puertos UDP que no son utilizados por isabel
	sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p udp --dport  53000:53032 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j ACCEPT -p udp --sport  53000:53032 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -A INPUT -j DROP -p udp -i %s",dev);
    system (cmd);
}


/*
 * Método que elimina las reglas iptables que limitan el acceso al cliente de tuneles
 */
void
client_protection_down(const char *dev)
{
	char cmd [1024];
	
	// Elimina reglas que limitan el acceso a los puertos TCP que no son utilizados por isabel
	sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p tcp --dport 53000:53023 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p tcp --sport 53000:53023 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j DROP -p tcp -i %s",dev);
    system (cmd);

	// Elimina reglas que limitan el acceso a los puertos UDP que no son utilizados por isabel
    sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p udp --dport  53000:53032 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j ACCEPT -p udp --sport  53000:53032 -i %s",dev);
    system (cmd);
    sprintf(cmd,"/sbin/iptables -D INPUT -j DROP -p udp -i %s",dev);
    system (cmd);
}


/*
 *	Método utilizado internamente y programado por Gabriel Huecas.
 *	Determina si hay alguna aplicación utilizando el puerto _s en el host _h
 *	Devuelve "1" si está libre y "0" si está ocupado.
 */
int
testUDPport(const char *_h, const char *_s)
{
    struct sockaddr ssaddr;
    struct addrinfo hints, *res;
    int gai_error, handle;

    if (   (  (!_h) || (strcmp(_h, "0")==0))
        && ( ((!_s) || (strcmp(_s, "0")==0)))) {
        printf("inetAddr_t::inetAddr_t:: at least one of the "
               "two parameters should be NON NULL, host or service, "
               "system halted!\n"
              );
        abort();
    }

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    if ((gai_error = getaddrinfo(_h, _s, &hints, &res))) {
        printf("inetAddr_t::ineAddr_t: getaddrinfo error:: "
               "address failed. Host: [%s] Port: [%s] Error: [%s]\n",
               _h, _s, gai_strerror(gai_error)
              );
        abort();
    }

    memset(&ssaddr, 0, sizeof(ssaddr));
    memcpy(&ssaddr, res->ai_addr, res->ai_addrlen);

    handle= socket(ssaddr.sa_family, SOCK_DGRAM, 0);

    if (handle < 0) {
        //printf("bind [%s]\n", strerror(errno));
		//printf("Error en socket\n");
		return 0;
    }

    int resbind = bind(handle, &ssaddr, res->ai_addrlen);

    if (resbind <0) {
        //printf("bind [%s]\n", strerror(errno));
	return 0;
    }

    return 1;

    // NUNCA SE HACE CLOSE
}


/*
 *	Método de uso interno.
 *	Limpia todas las sentencias añadidas a la tabla de NAT de iptables para escuchar en puertos alternativos.
 */
int
cleanAlternativePorts()
{
	FILE *flujo;
	char address[80];
	char port[80];
	char cmd [1024];
	
	//Se crea un flujo de lectura del archivo de intercambio
	if ( (flujo = fopen("/tmp/serverTunnelAlternativePorts","r")) == NULL ){
		return 1;
	}
	
	//Escanea cada linea del fichero (una direccion y un puerto en cada linea) y elimina la regla de iptables correspondiente a cada uno
	while (fscanf(flujo, "%s %s" ,address,port) != EOF) {
		sprintf(cmd,"/sbin/iptables -D PREROUTING -t nat -p udp --dport %s -d %s -j DNAT --to-destination %s:53018",port,address,address);
        if(system (cmd)==1)
        	printf("Iptables NAT rule couldn't be deleted correctly");
	}
	
	return 0;
}


/*
 * Función que comprueba el estado del servidor de tuneles de isabel.
 * Imprime info por pantalla y devuelve la palabra (Ap Sp S) con los bits a 1 si están activados
 * S	Estado del servidor de túneles
 * Sp	Estado de la protección del tunel servidor
 * Ap	Servidor escuchando en puertos alternativos (activado si al menos se escucha en un puerto alternativo)
 */
int
server_status()
{
	FILE *flujo;
	char address[80];
	char port[80];
	char cmd [1024];
	int server_tunnel;
	int server_protection;
	int alternative_ports;

	//Obtenemos la informacion del servidor
	struct serverInfo s = getServerInfo();

	//Comprobamos si esta lanzado el tunel servidor
	if(strcmp(s.STATUS,"STARTED")==0){
		printf("OpenVPN server is launched.\n");
    	server_tunnel=1;
    }else{
		printf("OpenVPN server is NOT launched.\n");
    	server_tunnel=0;
    }
    
    //Comprobamos si existen reglas iptables para tun10 (restriccion de acceso al servidor de tuneles)
    sprintf(cmd,"/sbin/iptables -L -v | /bin/grep %s > /dev/null 2>&1",s.INTERFACE);
    if(strncmp("tun",s.INTERFACE,3)==0 && system(cmd)==0){
	printf("Iptables rules to protect server tunnel interface are set up.\n");
    	server_protection=1;
    }else{
	printf("Iptables rules to protect server tunnel interface are NOT set up.\n");
    	server_protection=0;
    }
    
    //Comprobamos si se está escuchando en los puertos alternativos.
    alternative_ports=0;

	//Se crea un flujo de lectura del archivo de intercambio
	if ( (flujo = fopen("/tmp/serverTunnelAlternativePorts","r")) == NULL )
	{
		printf("Iptables rules to listen al alternative ports are NOT set up.\n");
	}else{
		//Escanea cada linea del fichero y comprueba que existe la regla de iptables correspondiente a cada uno
		while (fscanf(flujo, "%s %s" ,address,port) != EOF) {
			sprintf(cmd,"/sbin/iptables -L -t nat -n | /bin/grep DNAT | /bin/grep dpt:%s > /dev/null 2>&1",port);
	        	if(system (cmd)==0){
	        		printf("Alternative port %s is set up\n",port);
				alternative_ports = alternative_ports + 1;
			}else{
				printf("Alternative port %s is NOT set up\n",port);
			}
		}
	}

	// Activamos el flag con que haya un puerto alternativo levantado.
	if (alternative_ports > 0)
		alternative_ports=1;
    
    return server_tunnel + 2*server_protection + 4*alternative_ports;
}


/*
 * Función utilizada para filtrar los ficheros de estado de clientes al escanear el directorio donde se hayan los ficheros con información acerca de los tuneles. 
 */
#if defined(__BUILD_FOR_INTEL_DARWIN)
int client_file_filter(struct dirent *entry)
#elif defined(__BUILD_FOR_INTEL_LINUX)
int client_file_filter(const struct dirent *entry)
#else
#error "Please, define client_file_filter for your OS"
#endif
{
	if (strncmp("clientTunnelStatus",entry->d_name,18) == 0)
		return 1;
	else
		return 0;
}


/*
 * Función que comprueba el estado de los clientes de tuneles de isabel.
 * Imprime por pantalla información sobre los clientes de túneles lanzados y devuelve el número de clientes lanzados.
 */
int
client_status()
{
	struct dirent **entradas= NULL;
	struct clientInfo c;
	char path[200];
	int numeroEntradas;
	int numeroClientes = 0;
	int i;
	char cmd [1024];

	numeroEntradas = scandir ("/tmp/", &entradas, client_file_filter, NULL);

	for (i=0; i<numeroEntradas; i++)
	{
		sprintf(path,"/tmp/%s",entradas[i]->d_name);
		c = getClientInfo(path);
		
		if (strcmp(c.STATUS,"STARTED")==0){
			printf("\n-Tunnel %s using interface %s is connected to %s.\n",c.NAME,c.INTERFACE,c.SERVER_PUBLIC_IP);
			printf("\tIt's private IP is %s and the private ip addres of the server is %s.\n",c.LOCAL_PRIVATE_IP,c.SERVER_PRIVATE_IP);		
			numeroClientes ++;
			
			sprintf(cmd,"/sbin/iptables -L -v | grep %s > /dev/null 2>&1",c.INTERFACE);
			if(system (cmd)==0){
				printf("\tIptables rules to protect client tunnel interface are set up.\n");
			}else{
				printf("\tIptables rules to protect client tunnel interface are NOT set up.\n");
			}
		}
		// Liberamos la memoria que reservó scandir(), puesto que no la volvemos a utilizar 
		free (entradas[i]);
		entradas[i] = NULL;
	}
	//Liberamos la tabla entradas completa, ya que no la volvemos a utilizar.
	free (entradas);
	entradas = NULL;
	
	return numeroClientes;
}


/*
 * Elimina el fichero de estado del servidor  
 */
void
cleanServerStatusFile(){
	char cmd [1024];
    sprintf(cmd,"/bin/rm -rf /tmp/serverTunnelStatus > /dev/null 2>&1");
    system (cmd);
}


/*
 * Elimina el fichero de puertos alternativos.  
 */
void
cleanAlternativePortsFile(){
	char cmd [1024];
    sprintf(cmd,"/bin/rm -rf /tmp/serverTunnelAlternativePorts > /dev/null 2>&1");
    system (cmd);
}


/*
 * Elimina el fichero de estado del cliente
 */
int
cleanClientStatusFile(const char *dev){
	char cmd [1024];
    sprintf(cmd,"/bin/rm `/bin/grep -l %s /tmp/clientTunnelStatus* 2>/dev/null | /usr/bin/head -n 1` > /dev/null 2>&1",dev);
    return system (cmd);
}


/*
 * Función que asegura que no queda ni rastro de la ejecución de isabel_openvpn
 */
void
clean_up(){
	
	char cmd [1024];
	printf ("\nCleaning up isabel openvpn tunnels...\n");
	
	//Cerramos todos los tuneles que haya abiertos.
	sprintf(cmd,"/bin/kill `ps v -e | /bin/grep \"isabel_openvpn server\" | /usr/bin/awk -F \" \" '{ print $1 }' | /usr/bin/head -n 1` > /dev/null 2>&1");
	while(system(cmd)==0){
		sleep(1);	
	}
	
	sprintf(cmd,"/bin/kill `ps v -e | /bin/grep \"isabel_openvpn client\" | /usr/bin/awk -F \" \" '{ print $1 }' | /usr/bin/head -n 1` > /dev/null 2>&1");	
	while(system(cmd)==0){
		sleep(1);	
	}

	//Eliminamos todas las reglas iptables posibles.
	sprintf(cmd,"/sbin/iptables -D INPUT `/sbin/iptables -L -v --line-numbers | /bin/grep tcp | /bin/grep 53000:53023 | /usr/bin/awk -F \" \" '{ print $1 }' | /usr/bin/head -n 1` > /dev/null 2>&1");
	while(system(cmd)==0);
	
	sprintf(cmd,"/sbin/iptables -D INPUT `/sbin/iptables -L -v --line-numbers | /bin/grep udp | /bin/grep 53000:53032 | /usr/bin/awk -F \" \" '{ print $1 }' | /usr/bin/head -n 1` > /dev/null 2>&1");
	while(system(cmd)==0);

	sprintf(cmd,"/sbin/iptables -D INPUT `/sbin/iptables -L -v --line-numbers | /bin/grep tun | /bin/grep DROP | /usr/bin/awk -F \" \" '{ print $1 }' | /usr/bin/head -n 1` > /dev/null 2>&1");
	while(system(cmd)==0);

	cleanAlternativePorts();
	
	//Eliminamos los ficheros de sincronización.
	cleanServerStatusFile();
	while(cleanClientStatusFile("tun")==0);
	
	printf ("Done.\n");
}


/*
 *	Su funcionamiento depende de los argumentos que se le pasen:
 *
 *	- isabel_openvpn server FICHERO_CONFIGURACION FICHERO_LOGS FICHERO_STATUS USUARIO DIRECCION MASCARA FICHERO_PID
 *		- Se llama desde ServerTunnel y lanza un servidor openvpn
 *		- FICHERO_CONFIGURACION		Fichero con la configuración básica del servidor ($ISABEL_DIR/lib/tunnel/conf/server_basic.conf).
 *		- FICHERO_LOGS				Fichero donde se guardan los logs ($ISABEL_LOGS_DIR/ServerTunnel.log).
 *		- FICHERO_STATUS			Fichero donde se guarda el estado del servidor ($ISABEL_LOGS_DIR/ServerStatus.log).
 *		- USUARIO					Usuario al que se van a rebajar los privilegios cuando se lanza el tunel.
 *		- DIRECCION					Direccion de la subred utilizada (p. e. 10.0.0.0).
 *		- MASCARA					Máscara de red de la subred utilizada (p. e. 255.255.255.0).
 *		- FICHERO_PID				Fichero donde se guarda el pid de openvpn ($ISABEL_TMP_DIR/serverTunnel.pid2).
 *
 *	- isabel_openvpn server_up
 *		- Llamado desde openvpn al terminar de lanzarse el tunel
 *		- Añade las reglas de iptables para que por el tunel sólo se mande información de isabel.
 * 
 *	- isabel_openvpn server_down
 *		- Llamado desde openvpn al terminar de lanzarse el tunel
 *		- Borra las reglas de iptables añadidas para restringir el acceso a través del tunel.
 *		- Elimina las reglas de iptables que permiten utilizar puertos alternativos (función cleanAlternativePorts()).
 *
 *	- isabel_openvpn client FICHERO_CONFIGURACION FICHERO_LOGS USUARIO SERVIDOR PUERTO FICHERO_PID DEV
 *		- Se llama desde ClientTunnel y lanza un cliente openvpn
 *		- FICHERO_CONFIGURACION		Fichero con la configuración básica del cliente ($ISABEL_DIR/lib/tunnel/conf/client_basic.conf).
 *		- FICHERO_LOGS				Fichero donde se guardan los logs ($ISABEL_LOGS_DIR/ClientTunnel.log).
 *		- USUARIO					Usuario al que se van a rebajar los privilegios cuando se lanza el tunel.
 *		- SERVIDOR					Dirección del servidor de túneles al que nos vamos a conectar.
 *		- PUERTO					Puerto en el que escucha el servidor (en principio 53018).
 *		- FICHERO_PID				Fichero donde se guarda el pid de openvpn ($ISABEL_TMP_DIR/clientTunnel.pid2).
 * 		- DEV						Nombre de la interface de tuneles. 
 *
 *	- isabel_openvpn client_up
 *		- Llamado desde openvpn al cerrarse el tunel
 *		- Añade las reglas de iptables para que por el tunel sólo se mande información de isabel. 
 *
 *	- isabel_openvpn client_down
 *		- Llamado desde openvpn al cerrarse el tunel
 *		- Borra las reglas de iptables añadidas para restringir el acceso a través del tunel.
 * 
 * 	- isabel_openvpn test_UDP_port PUERTO
 *		- Comprueba si el puerto udp indicado está siendo usado por otra aplicación
 * 
 * 	- isabel_openvpn add_altenative_port DIRECCION_IP PUERTO_ALTERNATIVO PUERTO_PRINCIPAL
 * 		- Añade una regla de NAT iptables para encaminar los paquetres que llegan al PUERTO_ALTERNATIVO de la DIRECCION_IP al PUERTO_PRINCIPAL.
 * 
 * 	- isabel_openvpn server_status
 * 		- Función que comprueba el estado del servidor de tuneles de isabel.
 * 		- Imprime info por pantalla y devuelve la palabra (Ap Sp S) con los bits a 1 si están activados
 * 			S	Estado del servidor de túneles
 * 			Sp	Estado de la protección del tunel servidor
 * 			Ap	Servidor escuchando en puertos alternativos (activado si al menos se escucha en un puerto alternativo)
 *
 *	- isabel_openvpn clients_status 
 * 		- Función que comprueba el estado de los clientes de tuneles de isabel.
 * 		- Imprime por pantalla información sobre los clientes de túneles lanzados y devuelve el número de clientes lanzados.
 *
 *	- isabel_openvpn status
 * 		- Muestra en pantalla información acerca de los túneles y de la configuración de red.
 * 		- Devuelve la palabra (C Ap Sp S) con los bits a 1 si están activados donde los bits significan:
 * 			S	Estado del servidor de túneles.
 * 			Sp	Estado de la protección del tunel servidor.
 * 			Ap	Activo si el servido escucha en al menos un puerto alternativo.
 *			C	Activo si hay lanzado al menos un cliente de túneles.
 * 
 * 	- isabel_openvpn clean_up
 * 		- Limpia todo lo relacionado con isabel_openvpn
 *
 */
int
main(int argc, char *argv[])
{
  char cmd [1024];
  int res = 0;
 
  sprintf(cmd,"openvpn --help | grep -- -script-security > /dev/null 2>&1");
  int checkVersion = system (cmd);
    
  if(strcmp(argv[1],"server") == 0) {  
	/*
	 * El hijo ejecuta openvpn y el padre espera a que openvpn termine.
	 * Si el hijo (openvpn) termina, termina el padre (isabel_openvpn) también.
	 * Si el padre (isabel_openvpn) recibe una señal SIGTERM, mata al hijo (openvpn) y termina.
	 */
	int pid;
	if ((pid=fork())==0) {
          if (checkVersion==0){
          res=execl("/usr/sbin/openvpn","openvpn",
                          "--config",argv[2],
                          "--log",argv[3],
                          "--status",argv[4],
                          "--user",argv[5],
                          "--server",argv[6],argv[7],
                          "--port",argv[8],
                          "--writepid",argv[9],
                          "--script-security","2",0);
          }else{
          res=execl("/usr/sbin/openvpn","openvpn",
                          "--config",argv[2],
                          "--log",argv[3],
                          "--status",argv[4],
                          "--user",argv[5],
                          "--server",argv[6],argv[7],
                          "--port",argv[8],
                          "--writepid",argv[9],0);
          }
	}else{
		pid_hijo=pid;
		signal(SIGTERM, handle);
		int estado;
        wait(&estado);
        printf("isabel_openvpn (pid %i) ends because openvpn(pid %i) has finished\n",getpid(),pid_hijo);
	}
  

  }else if(strcmp(argv[1],"client") == 0) {  
	/*
	 * El hijo ejecuta openvpn y el padre espera a que openvpn termine.
	 * Si el hijo (openvpn) termina, termina el padre (isabel_openvpn) también.
	 * Si el padre (isabel_openvpn) recibe una señal SIGTERM, mata al hijo (openvpn) y termina.
	 */
	int pid;
        if ((pid=fork())==0) {
          if (checkVersion==0){
          res=execl("/usr/sbin/openvpn","openvpn",
                          "--config",argv[2],
                          "--log",argv[3],
                          "--user",argv[4],
                          "--remote",argv[5],argv[6],
                          "--writepid",argv[7],
                          "--dev",argv[8],
                          "--script-security","2",0);
          }else{
          res=execl("/usr/sbin/openvpn","openvpn",
                          "--config",argv[2],
                          "--log",argv[3],
                          "--user",argv[4],
                          "--remote",argv[5],argv[6],
                          "--writepid",argv[7],
                          "--dev",argv[8],0);
	 }
	}else{
		pid_hijo=pid;
    	signal(SIGTERM, handle);
        int estado;
        wait(&estado);
        printf("isabel_openvpn (pid %i) ends because openvpn (pid %i) has finished\n",getpid(),pid_hijo);
	}
  

  }else if(strcmp(argv[1],"server_up") == 0) {
	res = server_protection_up();
  
  
  }else if(strcmp(argv[1],"server_down") == 0) {
	server_protection_down();
	res = cleanAlternativePorts();
	cleanServerStatusFile();
	cleanAlternativePortsFile();
  

  }else if(strcmp(argv[1],"client_up") == 0) {
  	client_protection_up(argv[2]);
  

  }else if(strcmp(argv[1],"client_down") == 0) {
  	client_protection_down(argv[2]);
  	cleanClientStatusFile(argv[2]);
  

  }else if(strcmp(argv[1],"add_alternative_port") == 0) {
	sprintf(cmd,"/sbin/iptables -A PREROUTING -t nat -p udp --dport %s -d %s -j DNAT --to-destination %s:%s",argv[3],argv[2],argv[2],argv[4]);
    res = system (cmd);
  

  }else if(strcmp(argv[1],"test_UDP_port") == 0) {
    char *host= "127.0.0.1";
	res = testUDPport(host,argv[2]);
  

  }else if(strcmp(argv[1],"server_status") == 0) {
    printf("\nISABEL_OPENVPN SERVER STATUS:\n\n");
    res=server_status();
    printf("\nisabel_openvpn server status=%i\n\n",res);
  
  
  }else if(strcmp(argv[1],"clients_status") == 0) {
    printf("\nISABEL_OPENVPN CLIENTS STATUS:\n");
    res=client_status();
    printf("\nisabel_openvpn clients=%i\n\n",res);
  

  }else if(strcmp(argv[1],"status") == 0) {
    printf("\nISABEL_OPENVPN SERVER STATUS:\n\n");
    res=server_status();
    printf("\nisabel_openvpn server status=%i\n\n",res);
    printf("\nISABEL_OPENVPN CLIENTS STATUS:\n");
    int clients=client_status();
    printf("\nisabel_openvpn clients=%i\n\n",clients);
    printf("NETWORK CONFIGURATION:\n\n");
    sprintf(cmd,"/sbin/ifconfig");
    system (cmd);
    printf("\nIPTABLES CONFIGURATION:\n\n");
    sprintf(cmd,"/sbin/iptables -L -v -n");
    system (cmd);
    printf("\nIPTABLES NAT CONFIGURATION:\n\n");
    sprintf(cmd,"/sbin/iptables -L -v -t nat -n");
    system (cmd);
    printf("\n");
    
    if (clients>0){
    	clients=1;
    }else{
    	clients=0;	
    }
    res = res + 8*clients;
  
  
  }else if(strcmp(argv[1],"clean_up") == 0) {
    clean_up();
    res=0;
  
  
  }else{
  	printf("Invalid parameter.\n Use isabel_openvpn OPTION (ARGS).\n");
  	res = -1;
  }
  
  return res;
}

