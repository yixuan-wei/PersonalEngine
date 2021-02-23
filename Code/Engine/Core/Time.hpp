//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

struct Time
{
    int sec=0;
    int min=0;
    int hour=0;
    int day=0;
    int mon=0;
    int year=0;
};

Time GetRealWorldTime();