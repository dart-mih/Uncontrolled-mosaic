# Uncontrolled-mosaic

Состояние на 27.05.2022: работа над основной частью завершена.

## Описание структуры проекта (параметры функций и что они под собой подразумевают содержатся внутри проектных файлов):

1) В папке Shoots_normalization находится все необходимое для нормализации изображений (и сами результаты нормализации):
    - Структуры данных для хранения информации о изображениях и камере (PhotoInf и CameraInf в PhotoAndCameraInf.h).
    - Функции считывания информации об изображениях и камере из специальных файлов (getInfoAboutPhotos и getInfoAboutCamera в PhotoAndCameraInfGetFunc.h), а также вывода считанной информации (printInfoAboutPhotos и getInfoAboutCamera в PhotoAndCameraInfGetFunc.h).
    - Функции необходимые для получения матриц преобразования изображений для нормализации (Отдельно матрица 2d->3d преобразования - getMatrix2dto3d и комбинация всех остальных - getRotationMatrix3dTo2d в Normalization_function.h).
    - Функция поворота картинки (rotateImage в Normalization_function.h).
    - Получение примерного расстояния до картинки в пикселях для нормализации (getNormalizationDistance в Normalization_function.h).
    - Функция полной нормализации изображения (normalizeShot в Normalization_function.h).
    - В папке src находятся исходные фотографии для наложения, а также файлы содержащие информацию о камере и изображениях.
    - В папке result находятся уже нормализованные изображения.

2) В папке Overlay_algorithms находятся реализованные алгоритмы наложения, которые ищут относительное расположение двух фотографий друг относительно друга.
    - Алгоритм наложения с помощью попиксельного сравнения, исходя из маршрута дрона (pixelCompareAlg в PixelCompareAlg.h).
    - Алгоритм определения позиций фотографий с помощью GPS (justGPSalg в JustGPSalg.h).
    - Все необходимые подзадачи для работы алгоритма определения позиций фотографий с помощью GPS: нахождение позиции камеры на фотографии после нормализации и нахождение относительного расстояния между позициями камер на кадрах (findPositionOfCameraAfterNormalization и getApproxRelativeDistOfPhotos соответственно в JustGPSalg.h).
    - Модификации алгоритма попиксельно сравнения, одна из которых работает с черно-белыми вариантами исходных изображений, а вторая c изображениями в цветовой системе hsv (greyscaleCompareAlg и hsvCompareAlg в ColorChangeCompareAlg.h соответственно) {показывают результаты хуже стандартного алгоритма}.
    - Алгоритм определения позиции с помощью GPS и дальнейшего уточнения положения изображения с помощью pixelCompareAlg (compareAndGPSalg в CompareAndGPSalg.h).

3) В папке Results_output располагаются txt, содержащие информацию о времени работы и найденных относительных позициях фотографиий алгоритмами. Названия файлов читаются так: первая цифра - номер алгоритма (1 для justGPSalg и 2 для pixelCompareAlg), вторая цифра - использованный алгоритм совмещения фотографий в объединенное изображение (о них чуть далее), третья цифра (если есть) - какая "часть" фотографий обрабатывалась алгоритмом.

4) В папке Post_processing располагается cpp файл, который способен обрабатывать информацию о работе алгоритмов в Results_output и выводить значение метрик сравнения алгоритмов. Так же в той же папке располагается GetInfoFromOutput.h, содержащий функции парсинга формируемых в результате работы алгоритмов файлов .txt.

5) В корневой папке проекта располагается файл, который запускает алгоритм нормализации, алгоритмы наложения, совмещения и позволяет контроллировать основные параметры (Uncontrolled_mosaic.cpp, о его использовании и параметрах будет чуть далее, в разделе *запуск*), так же здесь расположен файл, содержащий 2 алгоритма совмещения фотографий по найденным позициям в единый холст (combinePhotos и combinePhotosOptimized в combinePhotosAlgs.h). Первый (combinePhotos) созраняет как можно больше информации о картинках, записывая их полностью на полотно. Второй (combinePhotosOptimized) оптимизирует запись изображений, сокращая размеры фрагментов для записи, слегка проигрывая в количестве сохраняемой информации, но выигрывая во времени.

## Запуск:

Для запуска проекта необходимо установить opencv и компилятор c++. Подключить opencv к проекту и скомпилировать файл Uncontrolled_mosaic.cpp вместе со всеми дополнительными библиотеками .h, которые были рассмотрены выше и содержат необходимые алгоритмы, нормализации и т.д.

Внутри основного файла проекта Uncontrolled_mosaic.cpp (в самом начале функции main) располагаются параметры, которые позволяют менять формат его работы:

    - choosen_alg - выбор алгоритма наложения фотографий (1 - justGPSalg, 2 - pixelCompareAlg)
    - choosen_combine_photos_func - как совмещать фотографии по позициям (1 - combinePhotos, 2 - combinePhotosOptimized).
    - normalize_images - запускать ли алгоритм нормализации изображений или нет (0 или 1 соответственно, если изображения уже нормализованы и расположены в папке Shoots_normalization/result, то стоит указать 0, так как алгоритм просто дважды сделает одно и то же).
    - write_in_file - если 1, то вся информация по времени нахождения расположения картинок/наложения их на полотно будет записана в файл, если 0, то в консоль.
    - path_to_file_to_write_info - путь, по которому будет записана информация о работе алгоритма, если флаг write_in_file поставлен в 1.
    - num_photos - количество фотографий в наборе.
    - num_first_broken_photos - количество первых фотографий, не подходящих под наложение. (В комбинации с num_photos позволяет задавать конкретные промежутки изображений для совмещения в формате - (num_first_broken_photos, num_photos]).
