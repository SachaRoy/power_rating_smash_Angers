#include <iostream>

struct Date { 
    int d, m, y; 
};

const int monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // To store number of days in all months from January to Dec.
int countLeapYears(Date d); // This function counts number of leap years before the given date
int getDifference(Date dt1, Date dt2); // This function returns number of days between two given dates