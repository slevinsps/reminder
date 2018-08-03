#ifndef STRUCTERS_H
#define STRUCTERS_H
#include <vector>
#include <QString>

typedef struct date_time_s
{
    int day;
    int month;
    int year;
    int minute;
    int hour;
}date_time;


typedef struct Files_s
{
    std::vector <QString> files_arr;
}Files;
#endif // STRUCTERS_H

