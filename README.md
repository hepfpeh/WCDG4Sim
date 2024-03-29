# Hunapu-0.3 README

Proyecto de simulación del sistema de detección de particulas generadas por rayos cósmicos en la atmósfera, dentro del marco de la colaboración LAGO (http://lagoproject.org/).

La simulación consta de dos partes:
## 1. **WCDtankSim**
Simulación en Geant4 (10.3) del tanque de prueba de la USAC. Este proyecto está basado en los ejemplos B1, B4, B5, OpNovice, LXe y el trabajo de Amilcar roca. Esta simulación contiene:
* *Geometría del tanque WCD*
  * El "mundo" es un cubo de aire.
  * Se utiliza un tubo de tyvek solido que es llenado con un tubo de agua de dimensiones tales que el resultado es una capa de tyvek rodeando el agua.
  * PMT construido de vidrio (pyrex), aluminio y vacio.
* *Procesos físicos*
  * Transporte
  * Decaimiento
  * Electromagnéticos
  * fotones ópticos (emisión Cherenkov)
* *Generación y detección*
  * Generación de partículas con trayectorias aleatorias (todas pasan por el tanque)
  * Conteo de fotones opticos que alcanzan el PMT.
* *Salida de datos*
  * Los datos generados por la simulación se escriben a un archivo .root. Estos son:
	* Energía de la partícula primaria.
	* Angulo de incidencia respecto al eje z del tanque.
	* Indica si la trayectoria es "vertical".
	* Energía depositada en el agua.
	* Distancia viajada por la partícula primaria dentro del agua.
	* Conteo de fotones que alcanzan el PMT.
* *Pendientes de implementar*
  * Material plástico del tanque.
  * Distribución de energía de la partícula incidente.
# 2. **ElecSim**
Simulación de la electrónica de adquisición. Esta utiliza librerías ROOT.
 * Construcción numérica del pulso de carga de salida del PMT a partir de los datos de incidencia de fotones en el PMT generados por WCDtankSim.
 * Construcción numérica del pulso de voltaje generado en una electrónica equivalente.
 * Digitalización del pulso.
 * Construcción del histograma de máximos de los pulsos digitalizados.
 * *Pendiente de implementar*
   * Ruido en la señal.
   * Capacidad de especificar parámetros como: ganancia del PMT, resistesia y capacitancia del circuito equivalente, voltaje de referencia, bits del ADC y nivel de threshold a partir de línea de comandos o un archivo de configuración.


Héctor Pérez (hector@ecfm.usac.edu.gt)  
Laboratorio de instrumentación  
Instituto de investigación  
Escuela de ciencias Físicas y Matemáticas  
USAC  
