/*
 * filePath.h
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef SITE_FILEPATH_H_
#define SITE_FILEPATH_H_


#define	STYLE_CSS_PATH				"/css/style.css"
#define	NOUISLIDER_CSS_PATH		"css/nouislider.min.css"
#define	MAIN_JS_PATH					"/js/main.js"
#define	NOUISLIDER_JS_PATH		"js/nouislider.min.js"

typedef enum
{
	NO_PATH,
	STYLE_CSS,
	NOUISLIDER_CSS,
	MAIN_JS,
	NOUISLIDER_JS
} HTTP_SITE_PATH;

#endif /* SITE_FILEPATH_H_ */
