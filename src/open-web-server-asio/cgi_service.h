#ifndef CGI_SERVICE_H
#define CGI_SERVICE_H

#include "client_request.h"
#include <QProcess>

class CgiService
{
public:
    CgiService();

    static bool execute_(ClientRequest &client_request);

private:
    static QProcess qproccess_;

};

#endif // CGI_SERVICE_H
