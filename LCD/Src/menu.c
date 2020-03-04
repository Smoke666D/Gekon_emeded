/*
 * menu.c
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */

#include "menu.h"
#include "main.h"
#include "lcd.h"
#include "u8g2.h"
#include "keyboard.h"





extern xScreenSetObject xGeneratorMenu;
extern xScreenSetObject xEngineMenu;



 static xScreenSetObject *pCurrMenu;


#include "menu_data.c"










 void xInputScreenKeyCallBack(xScreenSetObject * menu, char key)
 {

 }




void xLineScreenKeyCallBack(xScreenSetObject * menu, char key)
{
	uint8_t index=menu->pCurrIndex;
	if (menu->pHomeMenu[index].xScreenStatus==NOT_ACTIVE)
	{
		switch (key)
		{
			case KEY_RIGTH:
				if (menu->pCurrIndex == menu->pMaxIndex)
					menu->pCurrIndex=0;
				else
					menu->pCurrIndex++;
				break;
			case KEY_LEFT:
				if (menu->pCurrIndex==0)
					menu->pCurrIndex=menu->pMaxIndex;
				else
					menu->pCurrIndex--;
				break;
			case KEY_ENTER:
        	//Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
        		if (menu->pHomeMenu[index].pDownScreenSet!=NULL)
        			pCurrMenu = menu->pHomeMenu[index].pDownScreenSet;
        		else
        	//Есди текущий Screen конечный, то делаем его активным
        		   menu->pHomeMenu[index].xScreenStatus=ACTIVE;
        		break;
			case KEY_EXIT:
        	//Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
        	 if (menu->pHomeMenu[index].pUpScreenSet!=NULL)
        	 		pCurrMenu = menu->pHomeMenu[index].pUpScreenSet;
       		else
        	 //Есди текущий Screen конечный, то делаем его активным
        	    menu->pHomeMenu[index].xScreenStatus=NOT_ACTIVE;
        	break;
			default:
        	break;
		}

	}
	else
	{
			if (menu->pHomeMenu[index].pFunc!=NULL)
				menu->pHomeMenu[index].pFunc(&menu->pHomeMenu[index],key);
	}


}

//Обявдение объека-карусели экранов верхнего уровня
xScreenSetObject xGeneratorMenu=
{
		xScreensLev1,
		9,
		0,
		&xLineScreenKeyCallBack,

};

xScreenSetObject xEngineMenu=
{
		xScreensLev2_1,
		6,
		0,
		&xLineScreenKeyCallBack,
};



u8g2_t * u8g2;
#define XRESULURION 128
#define YRESOLUTION 64
#define FONT_TYPE u8g2_font_6x13_t_cyrillic
#define FONT_X_SIZE 6
#define FONT_TYPE_NUMBER u8g2_font_5x8_mf
#define FONT_TYPE4 u8g2_font_6x13_t_cyrillic
#define FONT_X_SIZE4 6
#define FONT_TYPE_NUMBER4 u8g2_font_5x8_mf


void InitMenu()
{



}

void DrawMenu(uint8_t temp, uint8_t * data)
{
  for (uint8_t i=0;i<MAX_HEADER_STRING_SIZE;i++)
  {

	 data[i]=HEADERSTRINGS[temp][i];
	 if (HEADERSTRINGS[temp][i]==0) break;
  }

}


void vMenuInit(u8g2_t * temp)
{
	u8g2 = temp;
	pCurrMenu =&xGeneratorMenu;
}

static uint8_t index=0;
static uint8_t key[10]={1,2,1,2,1,2,1,2,1,2};

void vMenuTask()
{

    //Блок обработки нажатий на клавиши
    pCurrMenu->pFunc(pCurrMenu,key[index]);

	//Блок отрисовки экранов
	DrawObject(pCurrMenu->pHomeMenu[pCurrMenu->pCurrIndex].pScreenCurObjets);
	index++;
	if (index>=10) index=0;
}

static xScreenObjet * pCurDrawScreen=NULL;

void DrawObject( xScreenObjet * pScreenObjects)
{
   uint8_t * TEXT;
   uint8_t Text[9]={' ',' ',' ',' ',' ',' ',' ',' ',' '};
   uint8_t i, x_offset,y_offset;
   uint8_t Redraw=0;
   //Проверяем, какой экран необходимо перерисовать
   if (pCurDrawScreen != pScreenObjects)  //Если экран изменился
   {
	   pCurDrawScreen = pScreenObjects;
	   Redraw=1;
   }
   else       							//Если тот же самый экран
   {
	   for (i=0;i<MAX_SCREEN_OBJECT;i++) //Проверяем есть ли на экране динамические объекты
	   {
		   if  (pScreenObjects[i].last) break;
		   if ((pScreenObjects[i].xType == DATA_STRING) ||  (pScreenObjects[i].xType == INPUT_DATA_STRING))
		   {
			Redraw =1;
			break;
		   }
	   }
   }

   if (Redraw)   //Если экран нужно перерисовывать
   {
	   u8g2_ClearBuffer(u8g2);
	   for (i=0;i<MAX_SCREEN_OBJECT;i++)
	   {
		   if  (pScreenObjects[i].last) break;
		   switch (pScreenObjects[i].xType)
		   {
	 	 	 //Если текущий объект - строка
	 	 	 case STRING:
	 	 	 case DATA_STRING:
	 	     case INPUT_DATA_STRING:
	 	 		 u8g2_SetFontMode(u8g2,pScreenObjects[i].ObjectParamert[0]);
	 	 		 if (pScreenObjects[i].ObjectParamert[0]==0)
	 	 			 u8g2_SetDrawColor(u8g2,pScreenObjects[i].ObjectParamert[0]?0:1);
	 	 		 u8g2_DrawBox( u8g2, pScreenObjects[i].x, pScreenObjects[i].y,pScreenObjects[i].Width,pScreenObjects[i].Height);
	 	 		 u8g2_SetDrawColor(u8g2,pScreenObjects[i].ObjectParamert[0]);
	 	 		 switch(pScreenObjects[i].xType)
	 	 		 {

	 	 		 	 case STRING:
	 	 		 		TEXT = pScreenObjects[i].pStringParametr;
	 	 		 		break;
	 	 		 	 case DATA_STRING:
	 	 		 	 case INPUT_DATA_STRING:
	 	 		 		 pScreenObjects[i].GetDtaFunction(READ_COMMNAD, pScreenObjects[i].DataID, &Text);
	 	 		 		 TEXT = Text;
	 	 		 		 break;
	 	 		 }
	 	 		 u8g2_SetFont( u8g2, FONT_TYPE4);
	 	 		 switch (pScreenObjects[i].ObjectParamert[2])
	 	 		 {
	 	 		 	 case RIGTH_ALIGN:
	 	 		 		 x_offset =  pScreenObjects[i].x + (pScreenObjects[i].Width - u8g2_GetUTF8Width(u8g2,TEXT))-1;
	 	 		 		 break;
	 	 		 	 case LEFT_ALIGN:
	 	 		 		 x_offset =  pScreenObjects[i].x+1;
	 	 		 		 break;
	 	 		 	 default:
	 	 		 		 x_offset =  pScreenObjects[i].x + (pScreenObjects[i].Width - u8g2_GetUTF8Width(u8g2,TEXT))/2;
	 	 		 		 break;
	 	 		 }
	 	 		 y_offset =  pScreenObjects[i].y + pScreenObjects[i].Height/2 + u8g2_GetAscent(u8g2)/2;
	 	 		 u8g2_DrawUTF8( u8g2,x_offset, y_offset, TEXT);


	 	    	 break;

	 	 	 default:
	 	 		 break;
		   }
	   }
   }
}

void GetInt(char * Data)
{
	Data[0]='0';
	Data[1]='0';
	Data[2]='1';
	Data[3]=0;
}

void GetTime(char * Data)
{
	Data[0]='1';
	Data[1]='1';
	Data[2]=':';
	Data[3]='2';
	Data[4]='0';
	Data[5]=':';
	Data[6]='5';
	Data[7]='4';
	Data[8]=0;
}
