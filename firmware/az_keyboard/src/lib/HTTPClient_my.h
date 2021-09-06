#ifndef HTTPClient_my_h
#define HTTPClient_my_h

#include "Arduino.h"
#include "FS.h"
#include "SPIFFS.h"

#include "../../az_common.h"


#include <HTTPClient.h>

class HTTPClient_my: public HTTPClient
{
	public:
		int sendPostFile(char *file_path); // ファイルをPOSTで送る
};


#endif
