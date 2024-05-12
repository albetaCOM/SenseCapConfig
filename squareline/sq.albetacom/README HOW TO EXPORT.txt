UI.C -> 
	Modificar:  ui_init (void) --> ui_albetacom_init(void)
	Eliminar: lv_obj_t * ui____initial_actions0;
	Modificar: #include "ui.h" -->  #include "ui_albetacom.h"


ui.h -> Eliminar:
		LV_FONT_DECLARE(ui_font_font0);
		LV_FONT_DECLARE(ui_font_font2);
		LV_FONT_DECLARE(ui_font_font3);
		LV_FONT_DECLARE(ui_font_font4);
	Modificar void ui_init(void); --> void ui_albetacom_init(void);


Renombrar ui.c y ui.h a ui_albetacom.c / h


Copiar ui_albetacom.c y .h en el folder donde esta la ui.c del proyecto



Copiar el contenido de la carpeta /screens a la carpeta /screens del proyecto




One shot: 

Añadir en ui.h
	#include "ui_albetacom.h"
Añadir en la función ui_init del fichero ui.c:
	ui_albetacom_init();