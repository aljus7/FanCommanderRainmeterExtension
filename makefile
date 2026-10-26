RAINMETER_PLUGIN = output\FanCommanderRainmeter.dll

ALL: $(RAINMETER_PLUGIN)

$(RAINMETER_PLUGIN):
	if not exist output mkdir output
	cl /LD /std:c++17 /DUNICODE /D_UNICODE /Fe"$(RAINMETER_PLUGIN)" FanCommanderRainmeter.cpp fanCommanderApi.lib API\x64\Rainmeter.lib