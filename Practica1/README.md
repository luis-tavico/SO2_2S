# INSTALACION Y MODIFICACION DE KERNEL

# Pasos seguidos

A continuacion se detalla cada uno de los pasos seguidos para la instalacion y modificacion del kernel.

## Actualizar repositorios

Primero es necesario actualizar todos los paquetes instalados, para no tener algun problema de compatibilidad.

```
$ sudo apt update
$ sudo apt upgrade
```

## Instalar dependencias

Luego necesitamos instalar algunos paquetes necesarios para la compilacion.

```
$ sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev fakeroot dwarves
```

## Descargar y descomprimir el kernel

A continuacion, debemos descargar el codigo fuente del kernel desde el sitio web oficial [kernel.org](https://kernel.org/). Usaremos la version ```longterm``` mas reciente del kernel.

![img1](images/img1.png)

Copiamos la direccion del vinculo ```tarball```. Luego usamos este enlace para descargar y descomprimir la fuente del kernel.

```
$ wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.45.tar.xz
$ tar -xf linux-6.6.45.tar.xz
```

## Modificar el kernel

### Mensaje personalizado

Agregaremos un mensaje personalizado al registro de inicio del kernel que de la bienvenida al usuario cuando se inicie el kernel para ello primero ingresamos al siguiente directorio:

```
$ cd linux-6.6.45/init/
```

Luego buscamos el archivo ```main.c``` y agregamos la siguiente linea de codigo en el archivo.

```
pr_notice("Bienvenido a USAC Linux - Nombre: Pedro Luis Pu Tavico, Carnet: 202000562\n");
```

### Nombre personalizado

Modificaremos el nombre del kernel mostrado por uname a “USAC Linux” para ello primero ingresamos al siguiente directorio:

```
$ cd linux-6.6.45/include/linux/
```

Luego buscamos el archivo ```uts.h``` y modoficamos la siguiente linea de codigo:

```
#define UTS_SYSNAME "Linux"
```

por:

```
#define UTS_SYSNAME "USAC Linux"
```

### Llamadas al Sistema

#### Hora actual

Agregaremos una nueva llamada al sistema que devuelva la hora actual en segundos desde el epoch linux. para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/kernel/time/
```

Luego buscamos el archivo ```time.c``` y agregamos la siguiente funcion al final del archivo.

```
SYSCALL_DEFINE0(get_current_time)
{
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    return ts.tv_sec; // Retorna la hora en segundos desde el epoch
}
```

#### Tiempo actividad

Agregaremos una llamada al sistema que devuelva el tiempo de actividad del sistema en segundos desde el ultimo reinicio, para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/kernel/time/
```

Luego buscamos el archivo ```time.c``` y agregamos la siguiente funcion al final del archivo.

```
SYSCALL_DEFINE0(get_system_uptime)
{
    struct timespec64 uptime;
    get_monotonic_boottime64(&uptime);
    return uptime.tv_sec; // Retorna el tiempo de actividad en segundos
}
```

### Registrar Llamadas al Sistema

Registraremos las llamadas al sistema realizadas anteriormente en la tabla de llamadas del sistema, para ello primero ingresamos al siguiente directorio: 

```
$ cd linux-6.6.45/arch/x86/entry/syscalls/
```

Luego buscamos el archivo ```syscall_64.tbl``` y agregamos la siguientes lineas de codigo al final del archivo.

```
548 common  get_current_time    sys_get_current_time
549 common  get_system_uptime   sys_get_system_uptime
550 common  get_last_five_logs  sys_get_last_five_logs
```

Ahora agregamos las declaraciones de las nuevas llamadas al sistema, entonces ingresamos al siguiente directorio: 

```
$ cd include/linux/
```

Luego buscamos el archivo ```syscalls.h``` y agregamos la siguientes lineas de codigo al final del archivo.

```
asmlinkage long sys_get_current_time(void);
asmlinkage long sys_get_system_uptime(void);
asmlinkage long sys_get_last_five_logs(char *buffer);
```

## Configurar el kernel

Primero ingrasamos al directorio del codigo fuente.

```
$ cd linux-6.6.45
```

La configuracion del kernel se debe especificar en un archivo .config. Para no escribir este desde 0 vamos a copiar el archivo de configuracion del Linux actualmente instalado.

```
$ cp -v /boot/config-$(uname -r) .config
```

Sin embargo, este esta lleno de modulos y drivers que no necesitamos que pueden aumentar el tiempo de compilación. Por lo que utilizamos el comando ```localmodconfig``` que analiza los módulos del kernel cargados del sistema y modifica el archivo .config de modo que solo estos modulos se incluyan en la compilacion.

```
$ make localmodconfig
```

Luego tenemos que modificar el .config, ya que al copiar nuestro .config se incluyeron nuestras llaves privadas, por lo que tendremos que reliminarlas del .config.

```
$ scripts/config --disable SYSTEM_TRUSTED_KEYS
$ scripts/config --disable SYSTEM_REVOCATION_KEYS
$ scripts/config --set-str CONFIG_SYSTEM_TRUSTED_KEYS ""
$ scripts/config --set-str CONFIG_SYSTEM_REVOCATION_KEYS ""
```

## Compilar el kernel

Ahora es el momento de compilar el kernel. Para esto simplemente ejecutamos el comando:

```
$ fakeroot make -j4
```

Donde ```-j4``` es la cantidad de nucleos que se deseamos utilizar para compilar el kernel.

Utilizar ```fakeroot``` es necesario por que nos permite ejecutar el comando ```make``` en  un  entorno  donde  parece  que  se  tiene  permisos  de superusuario  para  la  manipulacion  de  ficheros.  Es necesario  para  permitir a este comando crear archivos (tar, ar, .deb etc.) con ficheros con permisos/propietarios de superusuario.

## Instalar el kernel

La instalacion se divide en dos partes: instalar los modulos del kernel e instalar el kernel mismo.

Primero se instalan los modulos del kernel ejecutando:

```
$ sudo make modules_install
```

Luego instalamos el kernel ejecutando:

```
$ sudo make install
```

Despues de eso, reiniciamos la computadora para que se complete la instalacion.

```
$ sudo reboot
```

Ahora hemos terminado de instalar el kernel con los cambios necesarios.

# Errores ocurridos

A continuacion se detalla cada uno de los errores que ocurrieron durante la instalacion y modificacion del kernel.

## Falta de memoria RAM

![img2](images/img1.png)

## Falta de espacio

Este error ocurrio debido a la falta de espacio en el dispositivo y por lo tanto el proceso de compilacion fallo.

![img3](images/img3.png)

La causa de este error se debe a que anteriormente se realizaron varias compilaciones y esto provoco que archivos temporales ocuparan demasiado espacio.

Para solucionar este error se ejecuto el siguiente comando:

```
make clean
```

Este comando se encargar de limpiar el espacio de trabajo usado.

## Otro Error

![img4](images/img1.png)