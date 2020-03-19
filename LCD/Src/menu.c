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
#include "data_type.h"




extern xScreenSetObject xGeneratorMenu;
extern xScreenSetObject xEngineMenu;



 static xScreenSetObject *pCurrMenu;


#include "menu_data.c"








 static xScreenObjet * pCurObject=NULL;
static uint8_t CurObjectIndex=0;

 void xInputScreenKeyCallBack(xScreenSetObject * menu, char key)
 {
	 xScreenObjet * pObjects =menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets;
	 uint8_t ActiveObjectCount=0;
	 switch (key)
	 		{
	 	 	 	case KEY_STOP:
	 	 	 		pCurObject->GetDtaFunction(INC, NULL,pCurObject->DataID);
	 	 	 		break;
	 	 	 	case KEY_START:
	 	 	 		pCurObject->GetDtaFunction(DEC, NULL,pCurObject->DataID);
	 	 	 		break;
	 			case KEY_UP:
	 				if (menu->pHomeMenu[menu->pCurrIndex].pCurrIndex <  menu->pHomeMenu[menu->pCurrIndex].pMaxIndex)
	 				{
	 					menu->pHomeMenu[menu->pCurrIndex].pCurrIndex++;
	 					for (uint8_t i=0;i<MAX_SCREEN_OBJECT;i++) //Проверяем есть ли на экране динамические объекты
	 					{
	 						if (pObjects[i].xType == INPUT_HW_DATA)
	 						{
	 							ActiveObjectCount++;
	 							if (ActiveObjectCount ==(menu->pHomeMenu[menu->pCurrIndex].pCurrIndex-1))
	 						    {
	 								pObjects[i].ObjectParamert[3]=0;
	 					        }
	 							if (ActiveObjectCount == menu->pHomeMenu[menu->pCurrIndex].pCurrIndex)
	 						    {
	 							    pObjects[i].ObjectParamert[3]=1;
	 							    pCurObject =&pObjects[i];
	 							    break;
	 							}
	 						}
	 					}
	 				}
	 				break;
	 			case KEY_DOWN:
	 				if (menu->pHomeMenu[menu->pCurrIndex].pCurrIndex > 1)
	 				{
	 					menu->pHomeMenu[menu->pCurrIndex].pCurrIndex--;
	 					for (uint8_t i=0;i<MAX_SCREEN_OBJECT;i++) //Проверяем есть ли на экране динамические объекты
	 					{
	 						 	if (pObjects[i].xType == INPUT_HW_DATA)
	 						 	{
	 						 			ActiveObjectCount++;
	 						 			if (ActiveObjectCount ==(menu->pHomeMenu[menu->pCurrIndex].pCurrIndex+1))
	 						 			{
	 						 				pObjects[i].ObjectParamert[3]=0;
	 						 				break;
	 						 			}
	 						 			if (ActiveObjectCount == menu->pHomeMenu[menu->pCurrIndex].pCurrIndex)
	 						 		    {
	 						 			    pObjects[i].ObjectParamert[3]=1;
	 						 			    pCurObject = &pObjects[i];
	 						 			}
	 						   }
	 					}
	 				}
	 				break;
	 			case KEY_AUTO:
	 				//Если на экране есть объект с редактируемым полем, то оправлем команду на запись текущего значения
	 				 pCurObject->GetDtaFunction(SAVE, NULL,pCurObject->DataID);
	 				//Если на экране только один объект редактирования или его вообще нет, то выполняются дейтсвия по выходу из экрана
	 				if ( menu->pHomeMenu[menu->pCurrIndex].pMaxIndex>1) break;
	 			case KEY_EXIT:
	         	   //Выходи из активного экрана, для этого меняем его статус и также освобождаем редактируемый объект
	         	    menu->pHomeMenu[menu->pCurrIndex].xScreenStatus=NOT_ACTIVE;
	         	    pCurObject->ObjectParamert[3]=0;
	         	    if ( menu->pHomeMenu[menu->pCurrIndex].pMaxIndex) pCurObject->GetDtaFunction(ESC, NULL);
	         	    break;
	 			default:
	         	break;
	 		}
 }




void xLineScreenKeyCallBack(xScreenSetObject * menu, char key)
{
	uint8_t index=menu->pCurrIndex;
	if (menu->pHomeMenu[index].xScreenStatus!=ACTIVE)
	{
		switch (key)
		{
			case KEY_UP:
				if (menu->pCurrIndex == menu->pMaxIndex)
					menu->pCurrIndex=0;
				else
					menu->pCurrIndex++;
				break;
			case KEY_DOWN:
				if (menu->pCurrIndex==0)
					menu->pCurrIndex=menu->pMaxIndex;
				else
					menu->pCurrIndex--;
				break;
			case KEY_AUTO:
        	//Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
        		if (menu->pHomeMenu[index].pDownScreenSet!=NULL)
        			pCurrMenu = menu->pHomeMenu[index].pDownScreenSet;
        		else
        		{
        			//Есди текущий Screen конечный, то делаем его активным, это позволит перенапрявлять назажите клавиш в обработчик экрана
        			if (menu->pHomeMenu[index].xScreenStatus==NOT_ACTIVE)
        			{
        				menu->pHomeMenu[index].xScreenStatus=ACTIVE;
        				menu->pHomeMenu[index].pCurrIndex =0;
        				//Для ускорения работы выясняем кол-во редактируемых объектов на экране, если кол-во не задана при инициализации
        				if (menu->pHomeMenu[index].pMaxIndex==0)
        				for (uint8_t i=0;i<MAX_SCREEN_OBJECT;i++)
        				{
        				  if  (menu->pHomeMenu[index].pScreenCurObjets[i].last) break;
        			      switch (menu->pHomeMenu[index].pScreenCurObjets[i].xType)
        				  {
        					  case INPUT_HW_DATA:
        						  menu->pHomeMenu[index].pMaxIndex++;
        					 	  break;
        					  default:
        					  	  break;
        				   }
        				}
        				if (menu->pHomeMenu[index].pMaxIndex>1)
        				{
        					for (uint8_t i=0;i<MAX_SCREEN_OBJECT;i++)
        					{
        					  if  (menu->pHomeMenu[index].pScreenCurObjets[i].last) break;
        					  if  (menu->pHomeMenu[index].pScreenCurObjets[i].xType ==INPUT_HW_DATA)
        					  {
        						  menu->pHomeMenu[index].pScreenCurObjets[i].ObjectParamert[3]=1;
        						  CurObjectIndex =i;
        						  pCurObject= &menu->pHomeMenu[index].pScreenCurObjets[i];
        						  break;
        					  }
        					}
        				}
        			}
        		}
        		break;
			case KEY_EXIT:
        	//Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
        	 if (menu->pHomeMenu[index].pUpScreenSet!=NULL)
        	 		pCurrMenu = menu->pHomeMenu[index].pUpScreenSet;
       		else
        	 //Есди текущий Screen конечный, то делаем его активным
       			if (menu->pHomeMenu[index].xScreenStatus==ACTIVE)
       			{
       				menu->pHomeMenu[index].xScreenStatus=NOT_ACTIVE;
       			}
        	break;
			default:
        	break;
		}

	}
	else
	{
			if (menu->pHomeMenu[index].pFunc!=NULL)

				menu->pHomeMenu[index].pFunc(menu,key);
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

static QueueHandle_t pKeyboard;

void vMenuInit(u8g2_t * temp)
{
	u8g2 = temp;
	pCurrMenu =&xGeneratorMenu;
	pKeyboard =GetKeyboardQueue();

}

#define MAX_KEY_PRESS  10
static uint8_t key;
KeyEvent TempEvent;
KeyEvent BufferEvent={0,0};
uint8_t temp_counter=0;
void vMenuTask()
{

    //Блок обработки нажатий на клавиши


	osDelay( 200U );
	temp_counter++;

	//Блок отрисовки экранов
	if (temp_counter==2)
	{
		DrawObject(pCurrMenu->pHomeMenu[pCurrMenu->pCurrIndex].pScreenCurObjets);
		LCD_Redraw();
		temp_counter=0;
	}

    if (xQueueReceive( pKeyboard, &TempEvent, 0)== pdPASS )
	{
        key=0;
        if (
        		(TempEvent.Status==MAKECODE) && (BufferEvent.Status==MAKECODE) &&
        		( ( TempEvent.KeyCode | BufferEvent.KeyCode)==0x03 )
        		)

        {
        	key=KEY_EXIT;
        }
        else
        {
        	 BufferEvent=TempEvent;
        	//Если зафиксировано нажатие клавиши
        	if (TempEvent.Status == MAKECODE)
        	{
        		switch (TempEvent.KeyCode)
        		{
        		case stop_key:
        			key = KEY_STOP;
        			break;
        		case up_key:
          	        key = KEY_UP;
                    break;
        		case down_key:
             		key = KEY_DOWN;
             		break;
        		case start_key:
        			key = KEY_START;
        			break;
        		case auto_key:
        			key = KEY_AUTO;
        			break;
        		}
        	}
        }
        if (key) pCurrMenu->pFunc(pCurrMenu,key);

	}
}

static xScreenObjet * pCurDrawScreen=NULL;
static uint8_t Blink=0;
void DrawObject( xScreenObjet * pScreenObjects)
{
   uint8_t * TEXT;
   uint8_t Insert =0;
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
		   switch (pScreenObjects[i].xType)
		   {
		   	   	   case HW_DATA:
		   	   	   case INPUT_HW_DATA:
		   	   		   	  Redraw =1;
		   	   			  break;
		   	   	   default:
				  	  break;
		   }
		   if (Redraw) break;
	   }
   }
   if (Redraw)   //Если экран нужно перерисовывать
   {

	   u8g2_ClearBuffer(u8g2);
	   for (i=0;i<MAX_SCREEN_OBJECT;i++)
	   {
		   if  (pScreenObjects[i].last) break;
		   Insert =0;
		   switch (pScreenObjects[i].xType)
		   {
	 	 	 //Если текущий объект - строка

	 	     case INPUT_HW_DATA:
	 	    	  if (pScreenObjects[i].ObjectParamert[3])
	 	    	  {
	 	    		  Insert=1;
	 	    		  if (Blink) Blink=0; else Blink=1;
	 	    	  }
	 	     case STRING:
	 	     case HW_DATA:
	 	 		 u8g2_SetFontMode(u8g2,pScreenObjects[i].ObjectParamert[0]);
	 	 		 if (!Insert)
	 	 			 u8g2_SetDrawColor(u8g2,pScreenObjects[i].ObjectParamert[1]?0:1);
	 	 		 else
	 	 			 u8g2_SetDrawColor(u8g2,Blink?0:1);
	 	 		 u8g2_DrawBox( u8g2, pScreenObjects[i].x, pScreenObjects[i].y,pScreenObjects[i].Width,pScreenObjects[i].Height);
	 	 		 if (!Insert)
	 	 			u8g2_SetDrawColor(u8g2,pScreenObjects[i].ObjectParamert[1]);
	 	 		 else
	 	 			u8g2_SetDrawColor(u8g2,Blink);
	 	 		 switch(pScreenObjects[i].xType)
	 	 		 {

	 	 		 	 case STRING:
	 	 		 		TEXT = pScreenObjects[i].pStringParametr;
	 	 		 		break;
	 	 		 	case HW_DATA:
	 	 		 	case INPUT_HW_DATA:
	 	 		 		 if (pScreenObjects[i].DataID)
	 	 		 			pScreenObjects[i].GetDtaFunction(READ, &Text,pScreenObjects[i].DataID);
	 	 		 		 else
	 	 		 			pScreenObjects[i].GetDtaFunction(READ, &Text);
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
