# Powder-Trickler-Tank
Автоматический триклер для механических весов

## Модели для печати:

1. Непосредственно триклер: https://www.thingiverse.com/thing:4752507
2. Крепление датчиков к RCBS 505 - https://www.thingiverse.com/thing:4752516

К другим весам моделей нет не факт что будут.

Модели сделаны с минимальными зазорами под плачтик типа PETG с минимальной усадкой.
Будьте готовы вооружиться скальпелем, надфилями и цианокрилатом.

### [Схема распайки](https://github.com/fenofun/Powder-Trickler-Tank/blob/main/Scheme/Scheme.png)
### [Скетч](https://github.com/fenofun/Powder-Trickler-Tank/blob/main/frimware/Trickler_Tank.ino)

## Важные особенности

1. Оптодатчики фиксируют положение намного точнее человеческого глаза, так что повторяемость навески заметно улучшается.
Но если весы кривые сами по себе то чудес ждать нет смысла. 
2. Триклер не имеет никакого понятия что вы выставили на весах. И при первом прогоне, на максимальной скорости, оценивает время от старта до начала движения весов.
После первого "калибровочного" прогона триклер уже знает тайминг и притормаживает до начала движения весов, начиная отмерять навеску согласно заданному на весах значению
3. Я устал постоянно пересыпать порох в диспенср, триклер, а потом обратно. Поэтому голова триклера сделана съемной с резьбой под стандартную бутылку. 
4. Т.к. драйвера шагового двигателя имеют наглость сгорать - сделал импровизированный шилд и собрал все на пинах. Но смысла в этом нет.

## Реализованные функции

### Работа без подключенных датчиков 
 1. Разовое нажатие на клнопку с удержанием - минимальная скорость 
 2. Двойное нажатие с удержанием - средняя скорость 
 3. Тройное нажатие с удержанием - максимальная скорость
 4. Можно крутить руками если вдруг электричество кончилось

### Работа с подключенными датчиками
 1. Цикл работы запускается одиночным нажатием на кнопку и заканчивается звуковым сигналом. 
 2. До звукового сигнала трогать весы нельзя - триклер продолжить работать пока не будет зампнут верхний оптодатчик или нажата кнопка.
 3. При смене навески - необходимо сбросить калибровку таймера:
      - зажать кнопку на 4сек до звукового сигнала
      - переподключить питание (в текущей версии прошивки в памяти ничего не сохраняется) 
      


