# xray-tracing

#### Версия от 14.02.2017

1. Расчет угла поворота плоскости детектора - выдает совсем неверные
числа,  Это угол между направлением от центра детектора на точку в
фокусе кристалла и нормалью к поверхности детектора ( = перпендикуляр
к линии, соединяющей центр детектора и центр кривизны кристалла)
**Изменено в соответствии с требованием**

2. Выдача значения FRO angle - должно быть  = 180-FRO(current value).
И желательно выдавать его в log сразу, как и остальные параметры
(сейчас оно появляется только после trace)
**Добавлено выведение FRO в Ouput log до начала расчета**

3. Терминологически - ныненшняя величина Scattering angle на самом
деле является Bragg angle,  а Incident angle  = 90 - Bragg.  Надо
поправить и выдавать оба значения.
**Названия заменены на Bragg angle и Incident angle**

4. Из графика дисперсионной схемы следует, что лучи для разных длин
волн не пересекаются в меридиональной плоскости после отражения от
кристалла (между кристаллом и детектором). Фокусное расстояние в
меридиональной плоскости равно (R/2)*cosFi (!) - должны пересекаться! 
**Исправлено.**

5. 
Порядки отражения - в целом, работает так, как надо, пользоваться
удобно. НО - если задать в качестве Main order какой-то старший
порядок, а в качестве Addit. order - первый порядок, то этот 1 порядок
не отображается на схеме. Аналогично, при таких установках вообще не
генерятся additional lines.
Т.е. нужно, чтобы и в таблицу линий, и на график выводились линии во
всех порядках, указанных в двух окнах Main и Additional, и убирать
дубликаты. **Предположительно исправлено**
Кроме того, будет правильно ввести ограничение - генеривать для
порядков только те линии, получившаяся длина волны которых (табличная
\* порядок) не превышает максимального из введеных 2d кристалла (линии
с большей длиной волны не отразятся ни при каких условиях, можно не
показывать) **Линни длины больше 2d не генерируются по нажатию Addit. order. В ручном 
режиме их добавить можно.**

6. В таблице результатов трейсинга для каждой спектральной линии
столбец X-coord  - значения выдаются со знаком минус. Надо
инвертировать, чтобы X-coord по знаку соответствовала положению линий
на графике внизу. **Столбец X-coord инвертирован**

7. Поясни, плз, содержание колонки Captured [n]. Это число лучей с
данной длиной волны, попавших во всю апертуру кристалла? Или число
лучей, излученных в телесный угол Source solid angle? Если последнее,
то лучше переобозвать колонку как Emitted. **Captured - это число лучей с данной длиной 
волны, попавших во всю апертуру красталла. Число лучей излеченных в телесный 
 угол это Rays\*Intensity.** 

8. На графике - Selected lines, в режиме Display all желательно также
отображать Lmin и Lmax. **Добавлено отобрадение Lmin и Lmax  в режиме Display All**

9. Кнопка Generate waves - по-моему, можно удалить. Вводили ее для
отладки, не могу придумать, когда этот функционал может понадобиться
при реальных расчетах. **Убрана кнопка Generate waves**

10. Кнопки Del. additional lines и Clear all lines лучше поменять
местами, чтобы Clear all была нижней, логически последней в ряду.
**Порядок изменен**

11. Надписи на кнопках здесь же - надо все-таки выровнять, т.е. начало
надписей сделать с равным отступом от левой границы. В крайнем случае
просто сделать центрирование надписи по полю. **Надписи выровнены по центру**

12. При поиске линии в базе пробел считается содержательным а не
служебным символом. Лучше считать, что введенные в запрос символ
пробела соответстсвует логическому элементу AND - то есть должен
вестить поиск по записям, содержащим оба параметра. Например, запрос
вида "Al He" должен возвращать только линии Al с обозначением He либо
в названии линии, либо в поле изоэлектронной последовательности.
Сейчас же такой запрос вообще ничего не возвращает.**Исправлено. Пробел опять 
является содержательным символом. Операция OR представлена символом ";".**

13. Добавить в финальный релиз директорию Par
**Добавлено**

14. Можно ли переместить файл с базой линий в ту же директорию, что и
файлы с кривыми отражения (чтоб народ не лазил по служебным папкам, а
заходил только в одну)?
И можно ли переименовать эту папку из example в Input? Замечание не
принципиальное - если требует много усилий, можно проигнорировать.
**Переименовано. Файл с БД помещен в директорию Input. Добавлена возможность выбора
файла с базой данных. По умолчанию берется из Input/trans-db.txt**

15. Сделать отдельный депозитарий с финальным релизом, чтобы люди
могли скачать только его, а не весь проект.
**Сделано.**
