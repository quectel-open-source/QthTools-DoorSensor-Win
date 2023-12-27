#include "kernelconfig.h"

#define APP_FIRST_YEARS 2022

kernelConfig::kernelConfig()
{

}

int m_getTimeStamp(QDate date)
{
    int days = date.dayOfYear();
    int year = date.year();
    int yearDaysSum = 0;
    for(int i = APP_FIRST_YEARS;i<year;i++)
    {
        if(0 == i%4)
        {
            yearDaysSum +=366;
        }
        else
        {
            yearDaysSum +=365;
        }
    }
    return yearDaysSum+days;
}
