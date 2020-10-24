# esp32
esp32 drivers

idf_component_register(SRCS
	"[sources list].c"
INCLUDE_DIRS "."
REQUIRES esp32_libs)	<- https://github.com/Virviglaz/esp32.git
#PRIV_REQUIRES i2c)	<-idf drivers
