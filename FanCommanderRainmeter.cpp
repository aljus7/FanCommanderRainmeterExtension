#include <Windows.h>
#include <string>
#include <wchar.h>
#include <atomic>

#include "FanCommanderApi.h"
#include "API/RainmeterAPI.h"

// Global reference counter across all measures using this plugin DLL
static std::atomic<int> g_refCount(0);

struct MeasureData
{
    int fanIndex = 0;

    enum class ValueType
    {
        RPM,
        FilteredPWM,
        UnfilteredPWM
    };

    ValueType valueType = ValueType::RPM;
};

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
    *data = new MeasureData();

    // If this is the first measure initializing, connect to the API
    if (g_refCount.fetch_add(1) == 0)
    {
        Connect();
    }
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
    MeasureData* measure = (MeasureData*)data;

    measure->fanIndex = RmReadInt(rm, L"FanIndex", 0);

    LPCWSTR valueType = RmReadString(rm, L"Value", L"RPM");

    if (_wcsicmp(valueType, L"RPM") == 0)
    {
        measure->valueType = MeasureData::ValueType::RPM;
    }
    else if (_wcsicmp(valueType, L"FilteredPWM") == 0)
    {
        measure->valueType = MeasureData::ValueType::FilteredPWM;
    }
    else if (_wcsicmp(valueType, L"UnfilteredPWM") == 0)
    {
        measure->valueType = MeasureData::ValueType::UnfilteredPWM;
    }
    else
    {
        measure->valueType = MeasureData::ValueType::RPM;
    }

    *maxValue = 5000.0;
}

PLUGIN_EXPORT double Update(void* data)
{
    MeasureData* measure = (MeasureData*)data;

    switch (measure->valueType)
    {
        case MeasureData::ValueType::RPM:
            return (double)GetFanRpm(measure->fanIndex);

        case MeasureData::ValueType::FilteredPWM:
            return (double)GetFilteredPwm(measure->fanIndex);

        case MeasureData::ValueType::UnfilteredPWM:
            return (double)GetUnfilteredPwm(measure->fanIndex);
    }

    return 0.0;
}

PLUGIN_EXPORT void Finalize(void* data)
{
    delete (MeasureData*)data;

    // If this was the last active measure, safely disconnect
    if (g_refCount.fetch_sub(1) == 1)
    {
        Disconnect();
    }
}