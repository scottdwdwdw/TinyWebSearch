#ifndef HTTP_ERROR_H
#define HTTP_ERROR_H

#include <sys/stat.h>
#include <time.h>
#include "http_data_structure.h"
#include "http_out_filter.h"
#include "debug.h"

using namespace std;






class HttpError
{
public:
	HttpError(){}
	int RequireCheck(HttpCommandLine&command_line,HttpHeader&header,HttpOut&out,struct stat&stat_buf);
	//int DynamicServerCheck(HttpCommandLine&command_line,HttpHeader&header,HttpOut&out,struct stat&stat_buf);

	string ClientError(int state);
	//string GetErrorMessage()const {return error_message;}

private:
	string error_message;
};


#endif